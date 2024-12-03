#include<iostream>
#include<vector>
#include<limits.h>
#include"Matrix.h"
#include<ctime>

using namespace std;
using namespace Numeric_lib;

struct task {
	int number; // number of task
	bool isCloudTask;  // judge whether the task is a cloud task
	double priority; 
	int finishTimeLocal;  // finish time of the task in a core
	int finishTimeSending; // finish time of the task in sending
	int finishTimeCloud;   // finish time of the task in cloud
	int finishTimeReceiving;   // finish time of the task in receiving
	int readyTimeLocal; // earlist  time that the task can start in local core
	int readyTimeSending; // earlist  time that the task can start in sending
	int readyTimeCloud;  // earlist  time that the task can start in cloud
	int readyTimeReceiving; // earlist  time that the task can start in receiving
	int startTime; // task's actual start time
	int channel; // illustrate which channel the task operate (local core = 0,1,2, cloud=3)
	bool isExitTask;  //whether it is an exit task
	bool isEntryTask;  // whether it is an entry task
	int readyCount1;
	int readyCount2;
};

// Phase one in step one: primary assignment
void primary(vector<task>&ini, Matrix<int, 2>&ta,int t)
{
	int min;
	unsigned int i;
	unsigned int j;
	for (i = 0; i < ta.dim1(); i++)
	{
		ini[i].number = i + 1;
		min = ta(i, 0);
		for (j = 0; j < ta.dim2(); j++)
			if (ta(i, j) < min)
				min = ta(i, j);
		if (min > t)
			ini[i].isCloudTask = 1;
		else
			ini[i].isCloudTask = 0;
	}
}

// Phase two in step one: task prioritizing 
void prioritize(vector<task>&ini, Matrix<int, 2>&ta, Matrix<int, 2>&G,int t)
{
	unsigned int i;
	unsigned int j,m;
	int k ;
	double w;
	double max;
	m = ini.size() - 1;
	for (i = 0; i < ini.size(); i++)
	{
		k = 0;
		for (j = 0; j < G.dim2(); j++)
			if (G(m - i, j) == 1)
				k = k + 1;
		if (k == 0)
			ini[m - i].isExitTask = 1;
		k = 0;
		for (j = 0; j < G.dim2(); j++)
			if (G(j, m - i) == 1)
				k = k + 1;
		if(k==0)
			ini[m - i].isEntryTask = 1;
		max = 0;
		w = 0;
		if (!(ini[m - i].isCloudTask))
		{
			for (j = 0; j < ta.dim2(); j++)
				w = w + ta(m - i, j);
			w = w / 3;
		}
		else
			w = t;
		for (j = 0; j < G.dim2(); j++)
			if ((G(m - i, j) == 1) && (max < ini[j].priority))
				max = ini[j].priority;
		ini[m - i].priority = w + max;
	}
}

int find_biggest_pri(vector<task>&ini)
{
	unsigned int i;
	int max=0;
	for (i = 0; i < ini.size(); i++)
		if (ini[i].priority > ini[max].priority)
			max = i;
	return max;
}

// find the max in two numbers
int max2(int &m, int &n)
{
	if (m >= n)
		return m;
	else
		return n;
}

// if local schedule, return RTL
int d_rtl(task &vi, vector<task>&S, Matrix<int, 2>&G) 
{
	unsigned int i;
	unsigned int j;
	int max=0;
	if (S.size()!=0)
	{
		for (i = 0; i < G.dim2(); i++)
			if (G(i, vi.number - 1) == 1)
				for (j = 0; j < S.size(); j++)
					if ((S[j].number == i + 1)&&(max < max2(S[j].finishTimeLocal, S[j].finishTimeReceiving)))
						max = max2(S[j].finishTimeLocal, S[j].finishTimeReceiving);
	}
	return max;
}

// if cloud schedule, return RTWS
int d_rtws(task &vi, vector<task>&S, Matrix<int, 2>&G)
{
	unsigned int i;
	unsigned int j;
	int max=0;
	if (S.size()!=0)
	{
		for (i = 0; i < G.dim2(); i++)
			if (G(i, vi.number - 1) == 1)
				for (j = 0; j < S.size(); j++)
					if ((S[j].number == i + 1)&&(max < max2(S[j].finishTimeLocal, S[j].finishTimeSending)))
						max = max2(S[j].finishTimeLocal, S[j].finishTimeSending);
	}
	return max;
}

// if cloud schedule, return RTC
int d_rtc(task &vi, vector<task>&S, Matrix<int, 2>&G)
{
	unsigned int i;
	unsigned int j;
	int max=vi.finishTimeSending;
	if (S.size()!=0)
	{
		for (i = 0; i < G.dim2(); i++)
			if (G(i, vi.number - 1) == 1)
				for (j = 0; j < S.size(); j++)
					if ((S[j].number == i + 1)&&(max < max2(vi.finishTimeSending, S[j].finishTimeCloud)))
						max = max2(vi.finishTimeSending, S[j].finishTimeCloud);
	}
	return max;
}

// if cloud schedule, return RTWR
int d_rtwr(task &vi)
{
	return vi.finishTimeCloud;
}

// if local schedule, return the smallest finish time
int locals(task &vi, vector<task>&S, Matrix<int, 2>&G, Matrix<int, 2>&ta)
{
	vi.readyTimeLocal = d_rtl(vi, S, G);
	unsigned int i;
	unsigned int j;
	int mint=INT_MAX;
	int ft;
	int max = 0; // find a local core's biggest finish time
	if (S.size()==0)
	{
		for (i = 0; i < ta.dim2(); i++)
		{
			ft = ta(vi.number - 1, i);
			if (mint > ft)
			{
				mint = ft;
				vi.channel = i;
			}
		}
		return mint;
	}
	for (i = 0; i < ta.dim2(); i++)
	{
		ft = vi.readyTimeLocal + ta(vi.number - 1, i);
		max = 0;
		for (j = 0; j < S.size(); j++)
			if ((S[j].channel == i) && (max < S[j].finishTimeLocal))
				max = S[j].finishTimeLocal;
		if(max>vi.readyTimeLocal)
			ft=max+ ta(vi.number - 1, i);
		if (mint > ft)
		{
			mint = ft;
			vi.channel = i;
		}
	}
	return mint;
}

// if cloud schedule, return the finish time
int clouds(task &vi, vector<task>&S, Matrix<int, 2>&G, int ts, int tc, int tr)
{
	vi.readyTimeSending = d_rtws(vi, S, G);
	unsigned int i;
	int maxs = 0;
	int t;
	int maxc = 0;
	int maxr = 0;
	int ft;
	t = ts + tc + tr;
	if (S.size()==0)
	{
		vi.finishTimeSending = ts;
		vi.readyTimeCloud = ts;
		vi.finishTimeCloud = ts + tc;
		vi.readyTimeReceiving = ts+tc;
		return t;
	}
	for(i=0;i<S.size();i++)
		if (S[i].channel == 3)
			if (maxs < S[i].finishTimeSending)
				maxs = S[i].finishTimeSending;
	if (maxs > vi.readyTimeSending)
		vi.finishTimeSending = maxs + ts;
	else
		vi.finishTimeSending = vi.readyTimeSending + ts;
	vi.readyTimeCloud = d_rtc(vi, S, G);
	for (i = 0; i < S.size(); i++)
		if (S[i].channel == 3)
			if (maxc < S[i].finishTimeCloud)
				maxc = S[i].finishTimeCloud;
	if (maxc > vi.readyTimeCloud)
		vi.finishTimeCloud = maxc + tc;
	else
		vi.finishTimeCloud = vi.readyTimeCloud + tc;
	vi.readyTimeReceiving = d_rtwr(vi);
	for (i = 0; i < S.size(); i++)
		if (S[i].channel == 3)
			if (maxr < S[i].finishTimeReceiving)
				maxr = S[i].finishTimeReceiving;
	if (maxr > vi.readyTimeReceiving)
		ft = maxr + tr;
	else
		ft = vi.readyTimeReceiving + tr;
	return ft;
}

void initials(vector<task>&S, vector<task>&ini, Matrix<int, 2>&ta, Matrix<int, 2>&G, int ts, int tc, int tr)
{
	unsigned int i;
	int t;
	int maxp; // find the max priority in each iteration of ini
	int mint;  // find the minimum finish time of local
	int anot;  // perpare for another time (cloud)
	t = ts + tc + tr;
	for (i = 0; i < G.dim1(); i++)
	{
		maxp = find_biggest_pri(ini);
		if (!ini[maxp].isCloudTask)
		{
			mint = locals(ini[maxp], S, G, ta);
			anot = clouds(ini[maxp], S, G, ts, tc, tr);
			if (anot < mint)
			{
				ini[maxp].readyTimeLocal = 0;
				ini[maxp].finishTimeLocal = 0;
				ini[maxp].channel = 3;
				ini[maxp].finishTimeReceiving = anot;
				ini[maxp].startTime = anot - t;
			}
			else
			{
				ini[maxp].finishTimeCloud = 0;
				ini[maxp].finishTimeSending = 0;
				ini[maxp].readyTimeSending = 0;
				ini[maxp].readyTimeCloud = 0;
				ini[maxp].readyTimeReceiving = 0;
				ini[maxp].finishTimeReceiving = 0;
				ini[maxp].finishTimeLocal = mint;
				ini[maxp].startTime = mint - ta(ini[maxp].number - 1, ini[maxp].channel);
			}
		}
		else
		{
			ini[maxp].finishTimeLocal = 0;
			ini[maxp].readyTimeLocal = 0;
			ini[maxp].channel = 3;
			ini[maxp].finishTimeReceiving= clouds(ini[maxp], S, G, ts, tc, tr);
			ini[maxp].startTime = ini[maxp].finishTimeReceiving - t;
		}
		S.push_back(ini[maxp]);
		ini.erase(ini.begin() + maxp);
	}
}

// return a task's finish time
int find_ft(task&vi)
{
	int max;
	max = max2(vi.finishTimeLocal, vi.finishTimeReceiving);
	return max;
 }

// print the sequence S
void prints(vector<task>&S)
{
	unsigned int i;
	int k,m;
	for (i = 0; i < S.size(); i++)
	{
		k = 1 + S[i].channel;
		m = find_ft(S[i]);
		cout << "Task" << S[i].number << ": ";
		switch (S[i].channel)
		{
		case 0:
			cout << "local core" << k << ", ";
			break;
		case 1:
			cout << "local core" << k << ", ";
			break;
		case 2:
			cout << "local core" << k << ", ";
			break;
		case 3:
			cout << "cloud" << ", ";
			break;
		default:
			break;
		}
		cout << "start time is: " << S[i].startTime << ",finish time is: "<<m<<endl;
	}
}

// return the completion time of sequence S
int find_tcom(vector<task>&S)
{
	unsigned int i;
	int max=0;
	for (i = 0; i < S.size(); i++)
		if ((S[i].isExitTask) && (max < find_ft(S[i])))
			max = find_ft(S[i]);
	return max;
}

// return the total energy of the sequence S
double find_en(vector<task>&S, int p1, int p2, int p3, double ps)
{
	unsigned int i;
	double ene=0;
	for (i = 0; i < S.size(); i++)
	{
		switch (S[i].channel)
		{
		case 0:
			ene = ene + p1 * (find_ft(S[i]) - S[i].startTime);
			break;
		case 1:
			ene = ene + p2 * (find_ft(S[i]) - S[i].startTime);
			break;
		case 2:
			ene = ene + p3 * (find_ft(S[i]) - S[i].startTime);
			break;
		case 3:
			ene = ene + ps * (S[i].finishTimeSending - S[i].startTime);
			break;
		default:
			break;
		}
	}
	return ene;
}

//compute all the ready1 in a sequence
void get_ready1(vector<task>&S, Matrix<int, 2>&G)
{
	unsigned int i, j, k;
	int m;
	for (i = 0; i < S.size(); i++)
	{
		m = 0;
		for (j = 0; j < G.dim2(); j++)
			if (G(j, S[i].number-1) == 1)
				for (k = 0; k < S.size(); k++)
					if (S[k].number == j + 1)
						m = m + 1;
		S[i].readyCount1 = m;
	}
}

//compute all the ready2 in a sequence
void get_ready2(vector<task>&S)
{
	unsigned int i, j;
	int m;
	for (i = 0; i < S.size(); i++)
	{
		m = 0;
		for (j = 0; j < S.size(); j++)
			if ((S[i].channel == S[j].channel) && (S[j].startTime < S[i].startTime))
				m = m + 1;
		S[i].readyCount2 = m;
	}
}

// local schedule task vi whose local core is confirmed
int localse(task &vi, vector<task>&SN, Matrix<int, 2>&G, Matrix<int, 2>&ta)
{
	vi.readyTimeLocal = d_rtl(vi, SN, G);
	unsigned int i;
	int ft;
	int max=0;
	if (SN.size()==0)
		ft = vi.readyTimeLocal + ta(vi.number - 1, vi.channel);
	else
	{
		for (i = 0; i < SN.size(); i++)
			if ((SN[i].channel == vi.channel) && (max < SN[i].finishTimeLocal))
				max = SN[i].finishTimeLocal;
		if(max>vi.readyTimeLocal)
			ft=max+ ta(vi.number - 1, vi.channel);
		else
			ft=vi.readyTimeLocal+ ta(vi.number - 1, vi.channel);
	}
	return ft;
}

void kernel(vector<task>&S, vector<task>&SN, int ktar, task vtar, Matrix<int, 2>&G, Matrix<int, 2>&ta,int ts, int tc, int tr)
{
	unsigned int i;
	int m;
	int t;
	t = ts + tc + tr;
	vector<task>re;
	re = S;
	for (i = 0; i < re.size(); i++)
		if (vtar.number == re[i].number)
		{
			re[i].channel = ktar;
			if (ktar == 3)
			{
				re[i].finishTimeLocal = 0;
				re[i].readyTimeLocal = 0;
			}
		}
	while (re.size()!=0)
	{
		get_ready1(re, G);
		get_ready2(re);
		m = 0;
		while ((re[m].readyCount1!=0)&&(re[m].readyCount2 != 0))
			m = m + 1;
		if (re[m].channel == 3)
		{
			re[m].finishTimeReceiving = clouds(re[m], SN, G, ts, tc, tr);
			re[m].startTime = re[m].finishTimeReceiving - t;
		}
		else
		{
			re[m].finishTimeLocal = localse(re[m], SN, G, ta);
			re[m].startTime = re[m].finishTimeLocal - ta(re[m].number - 1, re[m].channel);
		}
		SN.push_back(re[m]);
		re.erase(re.begin() + m);
	}
}

void mcc(vector<task>&S, Matrix<int, 2>&G, Matrix<int, 2>&ta, int ts, int tc, int tr,int p1, int p2, int p3, double ps, int tmax)
{
	unsigned int i, j;
	int tcom;
	int tcom2;
	int a;
	double en;
	double en1;
	double en2;
	double ratio1=0;
	double ratio2;
	vector<task>SN;
	tcom = find_tcom(S);
	en = find_en(S, p1, p2, p3, ps);
	for (i = 0; i < S.size(); i++)
	{
		a = S[i].channel;
		if (S[i].channel != 3)
		{
			for (j = 0; j < 4; j++)
			{
				if (j != a)
				{
					SN.erase(SN.begin(), SN.end());
					en1 = find_en(S, p1, p2, p3, ps);
					kernel(S, SN, j, S[i], G, ta, ts, tc, tr);
					tcom2 = find_tcom(SN);
					en2 = find_en(SN, p1, p2, p3, ps);
					if ((en2 < en1) && (tcom >= tcom2))
						S = SN;
					else if ((en2 < en1) && (tcom2 <= tmax))
					{
						ratio2 = (en - en2) / (tcom2 - tcom);
						if (ratio2 > ratio1)
						{
							ratio1 = ratio2;
							S = SN;
						}
					}
				}
			}
		}
	}
}

void outerloop(vector<task>&S, Matrix<int, 2>&G, Matrix<int, 2>&ta, int ts, int tc, int tr, int p1, int p2, int p3, double ps, int tmax)
{
	double en;
	double en1=0;
	en = find_en(S, p1, p2, p3, ps);
	while (en1<en)
	{
		en= find_en(S, p1, p2, p3, ps);
		mcc(S, G, ta, ts, tc, tr, p1, p2, p3, ps, tmax);
		en1= find_en(S, p1, p2, p3, ps);
	}
}

int main()
{
	int NUM_TASKS_GRAPH1 = 20;  // the number of tasks
	int NUM_LOCAL_CORES = 3;   // the number of local cores
	int ts1 = 3,tc1 = 1,tr1 = 1;
	unsigned int i;
	int t1;
	int tmax1 = 50;
	int PRIORITY_LOCAL_CORE_1 = 1;
	int PRIORITY_LOCAL_CORE_2 = 2;
	int PRIORITY_LOCAL_CORE_3 = 4;
        double PRIORITY_CLOUD = 0.5;
        double rt;
        clock_t start, end;
	t1 = ts1 + tc1 + tr1;
	Matrix<int, 2>G1(NUM_TASKS_GRAPH1,NUM_TASKS_GRAPH1);
	Matrix<int, 2>ta1(NUM_TASKS_GRAPH1,NUM_LOCAL_CORES);
	vector<task>ini1(NUM_TASKS_GRAPH1);
	vector<task>S1;
	/*G1(0, 1) = 1;
	G1(0, 2) = 1;
	G1(0, 3) = 1;
	G1(0, 4) = 1;
	G1(0, 5) = 1;
	G1(1, 7) = 1;
	G1(1, 8) = 1;
	G1(2, 6) = 1;
	G1(3, 7) = 1;
	G1(3, 8) = 1;
	G1(4, 8) = 1;
	G1(5, 7) = 1;
	G1(6 ,9) = 1;
	G1(7, 9) = 1;
	G1(8, 9) = 1;*/
	G1(0,5) = 1;
	G1(1,6) = 1;
	G1(2,7) = 1;
	G1(3,8) = 1;
	G1(4,8) = 1;
	G1(5,9) = 1;
	G1(5,10) = 1;
	G1(6,11) = 1;
	G1(7,11) = 1;
	G1(7,13) = 1;
	G1(8,12) = 1;
	G1(9,14) = 1;
	G1(10,14) = 1;
	G1(11,15) = 1;
	G1(12,16) = 1;
	G1(13,16) = 1;
	G1(14,17) = 1;
	G1(15,17) = 1;
	G1(16,17) = 1;
	G1(17,19) = 1;
	G1(17,18) = 1;
	
	ta1(0, 0) = 9;
	ta1(0, 1) = 7;
	ta1(0, 2) = 5;
	ta1(1, 0) = 8;
	ta1(1, 1) = 6;
	ta1(1, 2) = 5;
	ta1(2, 0) = 6;
	ta1(2, 1) = 5;
	ta1(2, 2) = 4;
	ta1(3, 0) = 7;
	ta1(3, 1) = 5;
	ta1(3, 2) = 3;
	ta1(4, 0) = 5;
	ta1(4, 1) = 4;
	ta1(4, 2) = 2;
	ta1(5, 0) = 7;
	ta1(5, 1) = 6;
	ta1(5, 2) = 4;
	ta1(6, 0) = 8;
	ta1(6, 1) = 5;
	ta1(6, 2) = 3;
	ta1(7, 0) = 6;
	ta1(7, 1) = 4;
	ta1(7, 2) = 2;
	ta1(8, 0) = 5;
	ta1(8, 1) = 3;
	ta1(8, 2) = 2;
	ta1(9, 0) = 7;
	ta1(9, 1) = 4;
	ta1(9, 2) = 2;
	ta1(10, 0) = 8;
	ta1(10, 1) = 6;
	/*ta1(10, 2) = 1;
	ta1(11, 0) = 7;
	ta1(11, 1) = 3;
	ta1(11, 2) = 2;
	ta1(12, 0) = 9;
	ta1(12, 1) = 5;
	ta1(12, 2) = 4;
	ta1(13, 0) = 5;
	ta1(13, 1) = 3;
	ta1(13, 2) = 1;
	ta1(14, 0) = 7;
	ta1(14, 1) = 3;
	ta1(14, 2) = 2;
	ta1(15, 0) = 8;
	ta1(15, 1) = 4;
	ta1(15, 2) = 2;
	ta1(16, 0) = 7;
	ta1(16, 1) = 6;
	ta1(16, 2) = 5;
	ta1(17, 0) = 9;
	ta1(17, 1) = 2;
	ta1(17, 2) = 1;
	ta1(18, 0) = 6;
	ta1(18, 1) = 3;
	ta1(18, 2) = 1;
	ta1(19, 0) = 7;
	ta1(19, 1) = 4;
	ta1(19, 2) = 2;*/


	primary(ini1, ta1, t1);
	prioritize(ini1, ta1, G1,t1);
	start = clock();
	initials(S1, ini1, ta1, G1, ts1, tc1, tr1);
	end = clock();
	cout << "Initial schedule: " << endl;
	prints(S1);
	rt = (double)(end - start) / (double)(CLOCKS_PER_SEC)*(double)(1000.000000);
	cout << " Now the total energy is: " << find_en(S1, PRIORITY_LOCAL_CORE_1, PRIORITY_LOCAL_CORE_2, PRIORITY_LOCAL_CORE_3, PRIORITY_CLOUD)<<endl;
        cout << " Now the completion time is: " << find_tcom(S1) << endl;
	cout << "Running time of initial schedule of Graph is "<<rt<<" ms"<< endl;
	start = clock();
	outerloop(S1, G1, ta1, ts1, tc1, tr1, PRIORITY_LOCAL_CORE_1, PRIORITY_LOCAL_CORE_2, PRIORITY_LOCAL_CORE_3, PRIORITY_CLOUD, tmax1);
	end = clock();
	cout << "After Task Migration: " << endl;
	prints(S1);
	rt = (double)(end - start) / (double)(CLOCKS_PER_SEC)*(double)(1000.000000);
	cout << " Now the total energy is: " << find_en(S1, PRIORITY_LOCAL_CORE_1, PRIORITY_LOCAL_CORE_2, PRIORITY_LOCAL_CORE_3, PRIORITY_CLOUD) << endl;
    cout << " Now the completion time is: " << find_tcom(S1) << endl;
	cout << "Running time of task migration of Graph is "<<rt<<" ms"<< endl;
	cout << endl;
}
