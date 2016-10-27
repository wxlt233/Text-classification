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

map<string, int> webpages[20000];   //每个文本使用一个map<string,int>表示，map中每个节点的第一关键字为单词，第二关键字为单词出现次数
map<string, int> testwebpages[20000];//同上

map<int, string> label;  //每个文本对应的标签，第一关键字为该文本的编号，第二关键字为该文本的标签（字符串形式）
map<int, string> testlabel;//同上

set<string> wholeset; //用于储存所有出现过的单词，计算总的单词数量,用于随后进行拉普拉斯平滑

map<string, double> wordforeachclass[60]; //统计每个类别中单词出现的数量，每个类别用一个map<string,int>表示，第一关键字为单词，第二关键字为该单词在该类别中出现的次数
double  totalwordsforeachclass[60];   //统计每个类别出现的单词总数

double probforeachclass[60];  //计算每个类别的先验概率

bool fortraining[20000];  //bool数组用于选取测试集进行验证
bool unlabelfortraining[20000];//同上，选取unlabel的数据集


int ncategory;
int ntest, n;
double probfortest[20000][60];
double probforunlabeltraining[20000][60];
int tempclass[20000];

void gettestdatawebkb()       //从txt文件中读取训练集
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
void gettrainingdatawebkb()       //从txt文件中读取训练集
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


void choosetrainingset(int nsample)  //在所有带标签的训练集中选取nsample个带标签文本的进行此次测试
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

void chooseunlabeltrainingset(int nsample) //选取nsample个unlabel的进行此次训练
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
	cout << "请选择测试集 20news(0),webkb(1),r8(2),r52(3)" << endl;
	cin >> type;   //根据选择数据集选择不同文件读入数据，并设置文本类别数量等
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
		gettestdatawebkb();//获取测试集
		gettrainingdatawebkb();//获取测试集
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
		gettestdatar8();//获取测试集
		gettrainingdatar8();//获取测试集
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
		gettestdatar52();//获取测试集
		gettrainingdatar52();//获取测试集
							 //	cout << ncategory << endl;
	}


	cout << "请选择带标签测试集数量" << endl;
	cout << "带标签测试集数量：20ng 推荐20倍数，webkb 推荐4倍数, r8 推荐8的倍数，r52随意" << endl;
	cin >> trainingnumber;
	cout << "选择unlabel的测试集权重（0-1）" << endl;
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

		choosetrainingset(trainingnumber);  //选取trainingnumber个带标签文本作为训练集（每类个数相同）
		chooseunlabeltrainingset(unlabeltrainingnumber);//选取unlabeltraining个文本作为EM算法使用的unlabel训练集
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

		int nwords = wholeset.size();    //所有出现的单词数量

		for (int i = 1; i <= n; i++)
		{
			if (fortraining[i])
			{
				probforeachclass[classindex[label[i]]] += 1;   //计算每个类的文本个数
			}
		}

		for (int i = 0; i < ncategory; i++)
		{
			probforeachclass[i] /= (trainingnumber + ncategory);   //计算每个类的先验概率
		}


		for (int i = 1; i <= n; i++)      //第一次naive_bayes
		{
			if (fortraining[i])           //如果该文本被选为带标签的训练集
			{
				int thislabel = classindex[label[i]]; //获取该文本的类别
				for (it = webpages[i].begin(); it != webpages[i].end(); it++)  //遍历该文本的所有单词 
				{
					totalwordsforeachclass[thislabel] += it->second;    //统计该类别文本中的单词总数
					wordforeachclass[thislabel][it->first] += it->second;//统计该类别文本中该单词（it->first）出现的次数（每次累加it->second）
				}
			}
		}


		//building the initial model using naive bayes


		for (int i = 1; i <= ntest; i++)   //在使用EM算法前进行一次naive_bayes的测试
		{
			for (int j = 0; j < ncategory; j++)  //分别计算该文本属于每个类别的概率
			{
				double probforthis = log(probforeachclass[j]); //先验概率
				for (it = testwebpages[i].begin(); it != testwebpages[i].end(); it++)
				{
					double t1 = wordforeachclass[j][it->first] + 1; //对于每个单词，统计该单词在该类别中出现的总次数（+1用于平滑）
					double t2 = totalwordsforeachclass[j] + nwords;//统计该类别中的单词总数（+nwords用于平滑）
					for (int tt = 0; tt < it->second; tt++)
						probforthis += log(t1 / t2);       //t1/t2即为该单词在类别j下的条件概率P(w|cj)
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
			for (int j = 1; j < ncategory; j++)   //对于每个测试样本来说，选取后验概率最大的类别，作为此次测试的结果
			{
				if (probfortest[i][j] > max)
				{
					max = probfortest[i][j];
					maxclass = j;
				}
			}
			if (maxclass != classindex[testlabel[i]])    //判断是否正确
			{
				error++;
				totalerror1++;
			}
		}
		cout << "the accuracy (before EM) rate is " << 100 - (double)error / ntest * 100 << "%" << endl;
		//输出正确率


		int EM = 0; //开始EM算法
		while (EM < 8)  //根据论文，迭代8次EM算法
		{
			//E step,使用当前的分类器，对于unlabel的训练集重新分类
			for (int i = 1; i <= n; i++)
			{                             //分类过程同第一次naive bayes相同，不再赘述
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




			//M step，将使用上次naive bayes分类器分类从而获得label的unlabel训练集作为新的训练集，得到新的naive_bayes分类器
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

		//testing   迭代8次后重新测试正确率
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