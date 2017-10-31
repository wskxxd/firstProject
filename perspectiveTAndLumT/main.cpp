#include<io.h>
#include <iostream>
#include<fstream>
#include<string>
#include<vector>
#include<ctype.h>
#include <stdlib.h>
#include <opencv2/opencv.hpp>
#include <highgui/highgui.hpp>
#include <features2d/features2d.hpp>
#include <nonfree/nonfree.hpp>
#include<opencv2/legacy/legacy.hpp>
#include"include.h"

using namespace cv;
using namespace std;

/*
*	����ʵ�ֽ�
	1��ʹ��Ŀ�������Ƭ�е�������Ϊ�м��������Ƚ�Ŀ����Ƭ�����и�����������ƥ�䣬Ȼ��ʹ�ø�����Ͷ�Ӧ�ο���Ƭƥ�䣬ͨ��������Ϊ�м�ֵ���ҵ�Ŀ��ͼ��Ͳο�ͼ���ƥ���
	2������ǰnum_HModelsForCoding��Hģ�ͣ�����ƥ�����м��α��Σ����ת��

*/

// �з��ַ���
vector<string> split(string& str, char pattern)
{
	vector<string> vec;
	if (str == "")
		return vec;
	const char* chs  = str.c_str();
	int n = str.size();
	string tmp;
	for (int i = 0; i < n; i++)
	{
		if (chs[i] == pattern)
		{
			vec.push_back(tmp);
			tmp = "";
		}
		else{
			tmp += chs[i];
		}
	}
	return vec;
}
// ��ȡ������ַ���
void readRef(string path,string& tar,vector<string>& objPath, vector<string>& refPicPath)
{
	vector<string> v = split(path, ' ');
	tar = v[0];
	bool flag = 1;
	for (int i = 2; i < v.size(); i++)
	{
		if (v[i] == "remainder")
		{
			flag = 0;
		 	continue;
		} 
		if (flag)
		{
			objPath.push_back(v[i]);
			i++;
			refPicPath.push_back(v[i]);
		}
		else
		{
			refPicPath.push_back(v[i]);
		}
	}

}
// ���Դ�ӡ����
void testPrint(vector<string>& vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		printf("%s \n", vec[i].c_str());
	}
}
// �����ת�����ͼ���ƶ���ָ���Ĳο���ƬĿ¼
void move(string path1,string path2)
{
	for (int i = 0; i < num_HModelsForCoding; i++)
	{
		char num_str[4]="";
		_itoa_s(i, num_str, 4, 10);
		string imagePath = path1 + "photometricTransfImage"+num_str + ".jpg";
		char Ref_num_str[4];
		_itoa_s(Ref_num, Ref_num_str, 4, 10);
		string outPath = path2 + "finalref" + Ref_num_str + ".jpg";
		Mat I = imread(imagePath);
		imwrite(outPath, I);
		Ref_num++;
	}
}

// ��ͼ�񲹳���REF_NUM����������һ���ļ����ƣ�
void add2REF_NUM(string path)
{
	string imagePath = path + "finalref0" + ".jpg";
	Mat I = imread(imagePath);
	while (Ref_num<REF_NUM)
	{
		char num_str[4] = "";
		_itoa_s(Ref_num, num_str, 4, 10);
		string outPath = path + "finalref" + num_str + ".jpg";
		imwrite(outPath, I);
		Ref_num++;
	}
}
// ��ȡ�����ļ�
vector<string> readConfig(string path)
{
	vector<string> vec;
	ifstream configFile(path);
	if (!configFile.is_open())
	{
		std::cout << "error open configFile!" << std::endl;
	}
	string line;
	while (getline(configFile, line))
	{
		int n = line.size();
		bool flag = false;
		int start = 0, end = 0;
		for (int i = 0; i < n; i++)
		{
			if (line[i] == ' '&& !flag)
			{
				start = i;
				flag = ~flag;
			}
			else if (line[i] == ' '&& flag)
			{
				end = i;
				break;
			}
		}
		string temp = line.substr(start + 1, end - start - 1);
		vec.push_back(temp);
	}
	return vec;
}

// ��ȡ�ο���ϵ
string readRef(string path, int line)
{
	int count = 0;
	string line_str;
	ifstream refFile(path);
	while (getline(refFile, line_str))
	{
		if (line != count)
		{
			count++;
			continue;
		}
		return line_str;
	}
	cout << "������������߲ο��ļ��Ƿ���" << endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	int line = atoi(argv[1]);						// ��Ҫ���м��α��κ͹��ת���ı�����ref.TXT�е�����
	string configTxtPath = argv[2];					// �����ļ�Ŀ¼
	vector<string> vec = readConfig(configTxtPath);


	string originalImagePath = vec[0];				// ͼ��Ŀ¼
	string rootPath = vec[1];						// �洢������м����ݵĸ�Ŀ¼
	string str_exe_path = vec[2];					// exe�ļ�Ŀ¼
	string str_txt_path = vec[3];					// TXT�м��ļ�Ŀ¼
	string final_txt_path = vec[4];					// �����������Ŀ¼
	string ref_txt_path = vec[5];					// �ο���ϵ���ļ�Ŀ¼
	string s = readRef(ref_txt_path, line);			// ���α���Ĳο�ͼ���Ŀ��ͼ����ַ���

	vector<string> objPath;							// Ŀ��ͼ�������·��
	vector<string> refPicPath;						// ͼ��ο���Ƭ��·��
	string tar;
	readRef(s, tar, objPath, refPicPath);
	testPrint(objPath);
	testPrint(refPicPath);
	int targetImageNum = atoi((tar.substr(0,tar.find(".jpg"))).c_str());
	string str_tarPic = originalImagePath + tar;	// Ŀ��ͼ�������·��

	Mat tarImage = imread(str_tarPic);
	SiftFeatureDetector feature;
	vector<KeyPoint> remainder_tar_keypoints;
	feature.detect(tarImage, remainder_tar_keypoints);

	if (objPath.size()>0)
	{
		// ��Ŀ��ͼ���г���������м��α��κ͹��ת��
		for (int i = 0; i < objPath.size(); i++)
		{
			// ��Ŀ����Ƭʣ���������������ƥ�䣬��ȡsift�����α��Ρ����ת����ÿ����һ���޳�һ��ƥ��㣬��ʣ���ƥ�����´ν���ƥ��
			string str_objPic = objPath[i];
			string str_refPic = refPicPath[i];

			vector<KeyPoint> perspectiveT1(string str_tarPic, string str_objPic, string str_refPic, string str_txt_path,
				string str_exe_path, vector<KeyPoint>& tar_keypoints);
			vector<KeyPoint> remainder = perspectiveT1(str_tarPic, str_objPic, str_refPic, str_txt_path, str_exe_path, remainder_tar_keypoints);
			if (remainder.size() == remainder_tar_keypoints.size())
				break;
			remainder_tar_keypoints = remainder;
			void photometricTransformation(std::string originalImagePath, std::string rootPath, int targetImageNum);
			photometricTransformation(originalImagePath, rootPath, targetImageNum);
			move(rootPath + "PWW\\", final_txt_path);
			if (remainder_tar_keypoints.size() == 0)
				return 0;
		}
	}
	// ��ʣ������������ƥ��Ȼ����м��α��κ͹��ת����ÿ����һ���޳�һ��ƥ��㣬��ʣ���ƥ��������һ��
	for (int i = objPath.size(); i < refPicPath.size(); i++)
	{
		string str_refPic = refPicPath[i];
		vector<KeyPoint> perspectiveT2(string str_tarPic, string str_refPic, string str_txt_path,
			string str_exe_path, vector<KeyPoint>& tar_keypoints);
		vector<KeyPoint> remainder = perspectiveT2(str_tarPic, str_refPic, str_txt_path, str_exe_path, remainder_tar_keypoints);
		if (remainder.size() == remainder_tar_keypoints.size())
			break;
		remainder_tar_keypoints = remainder;
		void photometricTransformation(std::string originalImagePath, std::string rootPath, int targetImageNum);
		photometricTransformation(originalImagePath, rootPath, targetImageNum);
		move(rootPath + "PWW\\", final_txt_path);
		if (remainder_tar_keypoints.size() == 0)
			return 0;
	}


	if (Ref_num < REF_NUM)
	{
		add2REF_NUM(final_txt_path);
	}

//	system("pause");
	return 0;
}

