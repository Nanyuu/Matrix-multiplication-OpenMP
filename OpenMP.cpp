#include<omp.h>
#include<iostream>
#include<time.h>
#include<graphics.h>
#include<conio.h>
#include<string>
#include<iomanip>

constexpr auto MAX_Length = 10;//矩阵的个数

constexpr auto Process_Length = (MAX_Length/2); //迭代基数

constexpr auto Process_Num = 1; //迭代乘数

constexpr auto Matrix_de = 1000;//矩阵的维数

constexpr auto CPU_Thread_Num = 6; //CPU的线程数目

using namespace std;

//矩阵乘法
double multi_Matrix(float*** Matix_goal, float** Matrix_multi,int thread_num,int process_multi)
{
	omp_set_num_threads(thread_num+1);

	long process_thrad = process_multi * Process_Length;


	clock_t start, end;
	start = clock();
	#pragma omp parallel for
	for (long i = 0; i < process_thrad; i++)
	{


		//矩阵乘法
		for (int j = 0; j < Matrix_de; j++)
		{

			for (int k = 0; k < Matrix_de; k++)
			{
				float sum = 0;
				for (int l = 0; l < Matrix_de; l++)
				{
					sum += Matix_goal[i][j][l] * Matrix_multi[l][k]; //矩阵乘法
				}
				//不赋值 保证矩阵基本相同
				//Matix_goal[i][j][k] = sum;
			}
		}
	}
	end = clock();

	return ((double)(end - start) / CLOCKS_PER_SEC);
}

//进度条
void process_percent(int i)
{
	cout << "进度： [";
	for (int j = 0; j <= i; j++)
	{
		cout << ">>>>";
	}
	for (int k = Process_Num - i; k > 1; k--)
	{
		cout << "----";
	}
	cout << "]" << endl;
}

//绘图
void plot(double** thread_speedup)
{
	int value_x = 100; //原点坐标x
	int value_y = 450;//原点坐标y

	int show_x = 600;//窗口大小x
	int show_y = 600;//窗口大小y

	initgraph(show_x, show_y);//初始化窗口大小

	line(-100, value_y, 800, value_y); //X轴
	line(value_x, 800, value_x, -100); //Y轴

	outtextxy(value_x-10, value_y+10, "0");
	outtextxy(show_x - 100, value_y + 30, "线程数量");
	outtextxy(value_x - 50, 30, "加速比");

	//绘制坐标系
	//50为一个坐标间隔
	for (int i = 1; i < 9; i++)
	{
		outtextxy(value_x + i * 50, value_y+10, char(i+'0'));
		outtextxy(value_x-10 , value_y - i * 50, char(i+'0'));
	}


	//绘制线
	setlinecolor(GREEN);
	for (int i = 0; i < 5; i++)
	{
		float line_start_x = value_x + (1 + i) * 50;
		float line_start_y = value_y - (thread_speedup[0][i]) * 50;
		float line_end_x = value_x + (2 + i) * 50;
		float line_end_y = value_y - (thread_speedup[0][i + 1]) * 50;

		line(line_start_x, line_start_y, line_end_x, line_end_y);
		/*		solidcircle(value_x + (1 + i) * 50, value_y - (thread_speedup[0][i]) * 50, 1)*/
	}
	if (Process_Num != 1)
	{
		//绘制线
		setlinecolor(RED);
		for (int i = 0; i < 5; i++)
		{
			float line_start_x = value_x + (1 + i) * 50;
			float line_start_y = value_y - (thread_speedup[2][i]) * 50;
			float line_end_x = value_x + (2 + i) * 50;
			float line_end_y = value_y - (thread_speedup[2][i + 1]) * 50;

			line(line_start_x, line_start_y, line_end_x, line_end_y);
			//solidcircle(value_x + (1 + i) * 50, value_y - (thread_speedup[2][i]) * 50, 1);
		}

		//绘制点
		setlinecolor(BLUE);
		for (int i = 0; i < 5; i++)
		{
			float line_start_x = value_x + (1 + i) * 50;
			float line_start_y = value_y - (thread_speedup[4][i]) * 50;
			float line_end_x = value_x + (2 + i) * 50;
			float line_end_y = value_y - (thread_speedup[4][i + 1]) * 50;

			line(line_start_x, line_start_y, line_end_x, line_end_y);
			//solidcircle(value_x + (1 + i) * 50, value_y - (thread_speedup[4][i]) * 50, 1);
		}
	}




}


void main()
{
	//定义三维数组
	float*** little_mat = new float**[MAX_Length];  //z轴
	for (long i = 0; i < MAX_Length; i++)
	{
		little_mat[i] = new float*[Matrix_de];  //y轴
		for (int j = 0; j < Matrix_de; j++)
			little_mat[i][j] = new float[Matrix_de]; //x轴
	}

	int t = 1;
	//初始化三维数组
	for (long i = 0; i < MAX_Length; i++)
		for (int j = 0; j < Matrix_de; j++)
			for (int k = 0; k < Matrix_de; k++)
				little_mat[i][j][k] = t++;

	//定义、初始化二维数组
	float** multi_mat = new float* [Matrix_de];
	for (long i = 0; i < Matrix_de; i++)
	{
		multi_mat[i] = new float[Matrix_de];
		for (int j = 0; j < Matrix_de; j++)
		{
			multi_mat[i][j] = 1;
		}
	}
	

	//创建数组 记录线程数和时间的关系
	double** thread_time = new double*[Process_Num];
	double** thread_speedup = new double* [Process_Num];
	for (int i = 0; i < Process_Num; i++)
	{
		thread_time[i] = new double[CPU_Thread_Num];
		thread_speedup[i] = new double[CPU_Thread_Num];

	}


	//执行矩阵乘法 i的大小和cpu线程数有关 k代表要处理的矩阵数量
	for (int k = 0; k < Process_Num; k++)
	{
		for (int i = 0; i < CPU_Thread_Num; i++)
		{
			thread_time[k][i] = multi_Matrix(little_mat, multi_mat, i, k+1);
			thread_speedup[k][i] = thread_time[k][0] / thread_time[k][i];
			
		}
		process_percent(k);
	}
	for (int k = 0; k < Process_Num; k++)
	{

		//cout << k+1<<"个"
		cout<<Process_Length*(1+k)<<"个"<< Matrix_de<<"*"<<Matrix_de<<"的矩阵处理："<<endl;
		cout << "线程数    处理时间     加速比"<<endl;
		for (int i = 0; i < 6; i++)
		{
			cout.setf(ios::fixed);
			cout  << i + 1 << "          " <<setprecision(3)<< thread_time[k][i];
			cout << "       " << setprecision(3) << thread_time[k][0] / thread_time[k][i] << endl;
		}

		cout << endl;
	}

	plot(thread_speedup);


	/*

	clock_t start, end;

	//开始计时
	start = clock();

	//#pragma omp parallel for
	//50个数组
	for (long i = 0; i < MAX_Length; i++)
	{
		
		//矩阵乘法
		for (int j = 0; j < Matrix_de; j++)
		{
			
			for (int k = 0; k < Matrix_de; k++)
			{
				float sum = 0;
				for (int l = 0; l < Matrix_de; l++)
				{
					sum += little_mat[i][j][l] * multi_mat[l][k]; //矩阵乘法
				}
				little_mat[i][j][k] = sum;
			}
		}
	}

	cout << "第一个数值是："<<little_mat[0][0][0]<<endl;

	end = clock();

	cout << "消耗时间：" << (double)(end - start) / (double)CLOCKS_PER_SEC << endl;
	
	*/


	system("pause");
}