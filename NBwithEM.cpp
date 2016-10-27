#include<iostream>
#include<map>
#include<fstream>
#include<string>
#include<windows.h>
#include<set>
#include<time.h>
#include<math.h>
#include <sstream>
using namespace std;
string classnameswebkb[] = { "student","faculty","course","project" };

string classnames20news[] = { "alt.atheism","talk.politics.guns","talk.politics.mideast","talk.politics.misc",
"talk.religion.misc","soc.religion.christian","comp.sys.ibm.pc.hardware","comp.graphics",
"comp.os.ms-windows.misc","comp.sys.mac.hardware","comp.windows.x","rec.autos","rec.motorcycles",
"rec.sport.baseball","rec.sport.hockey","sci.crypt","sci.electronics",
"sci.space","sci.med","misc.forsale" };

string classnamesr8[] = { "acq","crude","earn","grain","interest","money-fx","ship","trade" };

string classnamesr52[] = { "acq","alum","bop","carcass","cocoa","coffee","copper","cotton","cpi","cpu",
"crude","dlr","earn","fuel","gas","gnp","gold","grain","heat","housing",
"income","instal-debt","interest","ipi","iron-steel","jet","jobs","lead","lei","livestock",
"lumber","meal-feed","money-fx","money-supply","nat-gas","nickel","orange","pet-chem","platinum","potato",
"reserves","retail","rubber","ship","strategic-metal","sugar","tea","tin","trade","veg-oil",
"wpi","zinc" };
string classnames[60];
int  trainingnumber;
int unlabeltrainingnumber;

double weighted = 1;


map<string, int> classindex;

map<string, int> webpages[20000];   //ÿ���ı�ʹ��һ��map<string,int>��ʾ��map��ÿ���ڵ�ĵ�һ�ؼ���Ϊ���ʣ��ڶ��ؼ���Ϊ���ʳ��ִ���
map<string, int> testwebpages[20000];//ͬ��

map<int, string> label;  //ÿ���ı���Ӧ�ı�ǩ����һ�ؼ���Ϊ���ı��ı�ţ��ڶ��ؼ���Ϊ���ı��ı�ǩ���ַ�����ʽ��
map<int, string> testlabel;//ͬ��

set<string> wholeset; //���ڴ������г��ֹ��ĵ��ʣ������ܵĵ�������,����������������˹ƽ��

map<string, double> wordforeachclass[60]; //ͳ��ÿ������е��ʳ��ֵ�������ÿ�������һ��map<string,int>��ʾ����һ�ؼ���Ϊ���ʣ��ڶ��ؼ���Ϊ�õ����ڸ�����г��ֵĴ���
double  totalwordsforeachclass[60];   //ͳ��ÿ�������ֵĵ�������

double probforeachclass[60];  //����ÿ�������������

bool fortraining[20000];  //bool��������ѡȡ���Լ�������֤
bool unlabelfortraining[20000];//ͬ�ϣ�ѡȡunlabel�����ݼ�


int ncategory;
int ntest, n;
double probfortest[20000][60];
double probforunlabeltraining[20000][60];
int tempclass[20000];

void gettestdatawebkb()       //��txt�ļ��ж�ȡѵ����
{
	ifstream testfile("webkb-test-stemmed.txt");
	string  words;
	while (getline(testfile, words))
	{
		ntest++;
		stringstream it(words);
		string test;
		it >> test;
		testlabel[ntest] = test;
		while (it)
		{
			it >> test;
			testwebpages[ntest][test]++;
		}
	}
}
void gettrainingdatawebkb()       //��txt�ļ��ж�ȡѵ����
{
	ifstream trainingfile("webkb-train-stemmed.txt");
	string  words;
	while (getline(trainingfile, words))
	{
		n++;
		stringstream it(words);
		string test;
		it >> test;
		label[n] = test;
		while (it)
		{
			it >> test;
			webpages[n][test]++;
		}
	}
}

void gettestdata20news()
{
	ifstream testfile("20ng-test-stemmed.txt");
	string  words;
	while (getline(testfile, words))
	{
		ntest++;
		stringstream it(words);
		string test;
		it >> test;
		testlabel[ntest] = test;
		while (it)
		{
			it >> test;
			testwebpages[ntest][test]++;
		}
	}
}
void gettrainingdata20news()
{
	ifstream trainingfile("20ng-train-stemmed.txt");
	string  words;
	while (getline(trainingfile, words))
	{
		n++;
		stringstream it(words);
		string test;
		it >> test;
		label[n] = test;
		while (it)
		{
			it >> test;
			webpages[n][test]++;
		}
	}
}

void gettrainingdatar8()
{
	ifstream trainingfile("r8-train-stemmed.txt");
	string  words;
	while (getline(trainingfile, words))
	{
		n++;
		stringstream it(words);
		string test;
		it >> test;
		label[n] = test;
		while (it)
		{
			it >> test;
			webpages[n][test]++;
		}
	}
}
void gettestdatar8()
{
	ifstream testfile("r8-test-stemmed.txt");
	string  words;
	while (getline(testfile, words))
	{
		ntest++;
		stringstream it(words);
		string test;
		it >> test;
		testlabel[ntest] = test;
		while (it)
		{
			it >> test;
			testwebpages[ntest][test]++;
		}
	}
}

void gettrainingdatar52()
{
	ifstream trainingfile("r52-train-stemmed.txt");
	string  words;
	while (getline(trainingfile, words))
	{
		n++;
		stringstream it(words);
		string test;
		it >> test;
		label[n] = test;
		while (it)
		{
			it >> test;
			webpages[n][test]++;
		}
	}
}
void gettestdatar52()
{
	ifstream testfile("r52-test-stemmed.txt");
	string  words;
	while (getline(testfile, words))
	{
		ntest++;
		stringstream it(words);
		string test;
		it >> test;
		testlabel[ntest] = test;
		while (it)
		{
			it >> test;
			testwebpages[ntest][test]++;
		}
	}
}


void choosetrainingset(int nsample)  //�����д���ǩ��ѵ������ѡȡnsample������ǩ�ı��Ľ��д˴β���
{
	DWORD seed = GetTickCount();
	srand(int(seed));
	int testcase = 0;
	int even[100];
	memset(even, 0, sizeof(even));
	if (ncategory != 52)
	{
		while (testcase < nsample)
		{
			int t = rand() % n + 1;
			if (fortraining[t])
				continue;
			else
			{

				if (even[classindex[label[t]]] == nsample / ncategory)
				{
					continue;
				}
				even[classindex[label[t]]]++;
				fortraining[t] = 1;
				testcase++;
			}
		}
	}
	else
	{
		while (testcase < nsample)
		{
			int t = rand() % n + 1;
			if (fortraining[t])
				continue;
			else
			{
				fortraining[t] = 1;
				testcase++;
			}
		}
	}
}

void chooseunlabeltrainingset(int nsample) //ѡȡnsample��unlabel�Ľ��д˴�ѵ��
{
	DWORD seed = GetTickCount();
	srand(int(seed));
	int testcase = 0;
	while (testcase < nsample)
	{
		int t = rand() % n + 1;
		if (fortraining[t] || unlabelfortraining[t])
			continue;
		unlabelfortraining[t] = 1;
		testcase++;
	}
}


int main()
{
	int type;
	cout << "��ѡ����Լ� 20news(0),webkb(1),r8(2),r52(3)" << endl;
	cin >> type;   //����ѡ�����ݼ�ѡ��ͬ�ļ��������ݣ��������ı����������
	if (type == 0)
	{
		ncategory = 20;
		unlabeltrainingnumber = 6000;
		for (int i = 0; i <ncategory; i++)
		{
			classnames[i] = classnames20news[i];
		}
		for (int i = 0; i <ncategory; i++)
		{
			classindex[classnames[i]] = i;
		}
		gettestdata20news();
		gettrainingdata20news();
	}
	else if (type == 1)
	{
		ncategory = 4;
		unlabeltrainingnumber = 2500;
		for (int i = 0; i < ncategory; i++)
		{
			classnames[i] = classnameswebkb[i];
		}
		for (int i = 0; i <ncategory; i++)
		{
			classindex[classnames[i]] = i;
		}
		gettestdatawebkb();//��ȡ���Լ�
		gettrainingdatawebkb();//��ȡ���Լ�
	}
	else if (type == 2)
	{
		ncategory = 8;
		unlabeltrainingnumber = 5000;
		for (int i = 0; i < ncategory; i++)
		{
			classnames[i] = classnamesr8[i];
		}
		for (int i = 0; i <ncategory; i++)
		{
			classindex[classnames[i]] = i;
		}
		gettestdatar8();//��ȡ���Լ�
		gettrainingdatar8();//��ȡ���Լ�
	}
	else
	{
		ncategory = 52;
		unlabeltrainingnumber = 5000;
		for (int i = 0; i < ncategory; i++)
		{
			classnames[i] = classnamesr52[i];
		}
		for (int i = 0; i <ncategory; i++)
		{
			classindex[classnames[i]] = i;
		}
		gettestdatar52();//��ȡ���Լ�
		gettrainingdatar52();//��ȡ���Լ�
							 //	cout << ncategory << endl;
	}


	cout << "��ѡ�����ǩ���Լ�����" << endl;
	cout << "����ǩ���Լ�������20ng �Ƽ�20������webkb �Ƽ�4����, r8 �Ƽ�8�ı�����r52����" << endl;
	cin >> trainingnumber;
	cout << "ѡ��unlabel�Ĳ��Լ�Ȩ�أ�0-1��" << endl;
	cin >> weighted;

	int totalerror1 = 0;
	int totalerror2 = 0;
	DWORD start = GetTickCount();

	cout << n << " " << ntest << endl;

	for (int ii = 0; ii < 10; ii++)
	{

		memset(fortraining, 0, sizeof(fortraining));
		memset(unlabelfortraining, 0, sizeof(unlabelfortraining));
		for (int j = 0; j < ncategory; j++)
		{
			probforeachclass[j] = 1;
			wordforeachclass[j].clear();
			totalwordsforeachclass[j] = 0;
		}

		choosetrainingset(trainingnumber);  //ѡȡtrainingnumber������ǩ�ı���Ϊѵ������ÿ�������ͬ��
		chooseunlabeltrainingset(unlabeltrainingnumber);//ѡȡunlabeltraining���ı���ΪEM�㷨ʹ�õ�unlabelѵ����
		map<string, int>::iterator it;
		wholeset.clear();
		for (int i = 1; i <= n; i++)
		{
			if (unlabelfortraining[i] || fortraining[i])
			{
				for (it = webpages[i].begin(); it != webpages[i].end(); it++)
				{
					wholeset.insert(it->first);
				}
			}
		}

		for (int i = 1; i <= ntest; i++)
		{
			for (it = testwebpages[i].begin(); it != testwebpages[i].end(); it++)
			{
				wholeset.insert(it->first);
			}
		}

		int nwords = wholeset.size();    //���г��ֵĵ�������

		for (int i = 1; i <= n; i++)
		{
			if (fortraining[i])
			{
				probforeachclass[classindex[label[i]]] += 1;   //����ÿ������ı�����
			}
		}

		for (int i = 0; i < ncategory; i++)
		{
			probforeachclass[i] /= (trainingnumber + ncategory);   //����ÿ������������
		}


		for (int i = 1; i <= n; i++)      //��һ��naive_bayes
		{
			if (fortraining[i])           //������ı���ѡΪ����ǩ��ѵ����
			{
				int thislabel = classindex[label[i]]; //��ȡ���ı������
				for (it = webpages[i].begin(); it != webpages[i].end(); it++)  //�������ı������е��� 
				{
					totalwordsforeachclass[thislabel] += it->second;    //ͳ�Ƹ�����ı��еĵ�������
					wordforeachclass[thislabel][it->first] += it->second;//ͳ�Ƹ�����ı��иõ��ʣ�it->first�����ֵĴ�����ÿ���ۼ�it->second��
				}
			}
		}


		//building the initial model using naive bayes


		for (int i = 1; i <= ntest; i++)   //��ʹ��EM�㷨ǰ����һ��naive_bayes�Ĳ���
		{
			for (int j = 0; j < ncategory; j++)  //�ֱ������ı�����ÿ�����ĸ���
			{
				double probforthis = log(probforeachclass[j]); //�������
				for (it = testwebpages[i].begin(); it != testwebpages[i].end(); it++)
				{
					double t1 = wordforeachclass[j][it->first] + 1; //����ÿ�����ʣ�ͳ�Ƹõ����ڸ�����г��ֵ��ܴ�����+1����ƽ����
					double t2 = totalwordsforeachclass[j] + nwords;//ͳ�Ƹ�����еĵ���������+nwords����ƽ����
					for (int tt = 0; tt < it->second; tt++)
						probforthis += log(t1 / t2);       //t1/t2��Ϊ�õ��������j�µ���������P(w|cj)
				}

				probfortest[i][j] = probforthis;
			}
		}
		//cout << "calculating 20 probability" << endl;
		int error = 0;
		for (int i = 1; i <= ntest; i++)
		{
			double max = probfortest[i][0];
			int maxclass = 0;
			for (int j = 1; j < ncategory; j++)   //����ÿ������������˵��ѡȡ����������������Ϊ�˴β��ԵĽ��
			{
				if (probfortest[i][j] > max)
				{
					max = probfortest[i][j];
					maxclass = j;
				}
			}
			if (maxclass != classindex[testlabel[i]])    //�ж��Ƿ���ȷ
			{
				error++;
				totalerror1++;
			}
		}
		cout << "the accuracy (before EM) rate is " << 100 - (double)error / ntest * 100 << "%" << endl;
		//�����ȷ��


		int EM = 0; //��ʼEM�㷨
		while (EM < 8)  //�������ģ�����8��EM�㷨
		{
			//E step,ʹ�õ�ǰ�ķ�����������unlabel��ѵ�������·���
			for (int i = 1; i <= n; i++)
			{                             //�������ͬ��һ��naive bayes��ͬ������׸��
				if (unlabelfortraining[i])
				{
					for (int j = 0; j < ncategory; j++)
					{
						double probforthis = log(probforeachclass[j]);
						for (it = webpages[i].begin(); it != webpages[i].end(); it++)
						{
							double t1 = wordforeachclass[j][it->first] + 1;
							double t2 = totalwordsforeachclass[j] + nwords;
							for (int tt = 0; tt < it->second; tt++)
							{
								probforthis += log(t1 / t2);
							}
						}
						probforunlabeltraining[i][j] = probforthis;
					}
				}
			}

			for (int i = 1; i <= n; i++)
			{
				if (unlabelfortraining[i])
				{
					double sumrelative = 0;
					double relative = probforunlabeltraining[i][0];
					double tmax = probforunlabeltraining[i][0];
					int tj = 0;
					for (int j = 0; j < ncategory; j++)
					{
						if (probforunlabeltraining[i][j] > tmax)
						{
							tmax = probforunlabeltraining[i][j];
							tj = j;
						}
					}
					for (int j = 0; j < ncategory; j++)
					{
						if (j == tj)
							probforunlabeltraining[i][j] = 1;
						else
							probforunlabeltraining[i][j] = 0;
					}
				}
			}




			//M step����ʹ���ϴ�naive bayes����������Ӷ����label��unlabelѵ������Ϊ�µ�ѵ�������õ��µ�naive_bayes������
			for (int j = 0; j < ncategory; j++)
			{
				probforeachclass[j] = 1;
			}
			//recalculating the prior probability
			for (int i = 1; i <= n; i++)
			{
				if (fortraining[i])
					probforeachclass[classindex[label[i]]] += 1;
				else if (unlabelfortraining[i])
				{
					for (int j = 0; j < ncategory; j++)
					{
						probforeachclass[j] += weighted* probforunlabeltraining[i][j];
					}
				}
			}
			for (int j = 0; j < ncategory; j++)
			{
				probforeachclass[j] /= (trainingnumber + unlabeltrainingnumber + ncategory);
			}
			//recalculating the conditional probability 

			for (int j = 0; j < ncategory; j++)
			{
				wordforeachclass[j].clear();
				totalwordsforeachclass[j] = 0;
			}
			int totaluntraining = 0;
			for (int i = 1; i <= n; i++)
			{
				if (fortraining[i])
				{
					int thislabel = classindex[label[i]];
					for (it = webpages[i].begin(); it != webpages[i].end(); it++)
					{
						totalwordsforeachclass[thislabel] += it->second;
						wordforeachclass[thislabel][it->first] += it->second;
					}
				}
				else if (unlabelfortraining[i])
				{
					for (int j = 0; j < ncategory; j++)
					{
						for (it = webpages[i].begin(); it != webpages[i].end(); it++)
						{
							totalwordsforeachclass[j] += weighted*it->second*probforunlabeltraining[i][j];
							wordforeachclass[j][it->first] += weighted*it->second*probforunlabeltraining[i][j];
						}
					}
				}
			}
			EM++;
		}

		//testing   ����8�κ����²�����ȷ��
		for (int i = 1; i <= ntest; i++)
		{

			for (int j = 0; j < ncategory; j++)
			{
				double probforthis = log(probforeachclass[j]);
				for (it = testwebpages[i].begin(); it != testwebpages[i].end(); it++)
				{
					double t1 = wordforeachclass[j][it->first] + 1;
					double t2 = totalwordsforeachclass[j] + nwords;
					for (int tt = 0; tt < it->second; tt++)
						probforthis += log(t1 / t2);
				}

				probfortest[i][j] = probforthis;
			}
		}
		error = 0;
		for (int i = 1; i <= ntest; i++)
		{
			double max = probfortest[i][0];
			int maxclass = 0;
			for (int j = 1; j < ncategory; j++)
			{
				if (probfortest[i][j] > max)
				{
					max = probfortest[i][j];
					maxclass = j;
				}
			}
			if (maxclass != classindex[testlabel[i]])
			{
				error++;
				totalerror2++;
			}
		}

		cout << "the accuracy rate (after EM) is " << 100 - (double)error / ntest * 100 << "%" << endl;
		cout << "-----------------" << endl;
	}
	cout << "The total accuracy rate (before EM) is " << 100 - (double)totalerror1 / ntest * 10 << "%" << endl;
	cout << "The total accuracy rate (after EM) is " << 100 - (double)totalerror2 / ntest * 10 << "%" << endl;
	DWORD end = GetTickCount();
	cout << "time: " << (end - start) / 1000 << "s" << endl;
	system("pause");
}