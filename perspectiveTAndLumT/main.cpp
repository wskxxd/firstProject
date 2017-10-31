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
*	程序实现将
	1、使用目标编码照片中的物体作为中间量，首先将目标照片和它切割出的物体进行匹配，然后使用该物体和对应参考照片匹配，通过物体作为中间值，找到目标图像和参考图像的匹配点
	2、计算前num_HModelsForCoding个H模型，并对匹配点进行几何变形，光度转换

*/

// 切分字符串
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
// 读取输入的字符串
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
// 测试打印函数
void testPrint(vector<string>& vec)
{
	for (int i = 0; i < vec.size(); i++)
	{
		printf("%s \n", vec[i].c_str());
	}
}
// 将光度转换后的图像移动至指定的参考照片目录
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

// 将图像补充至REF_NUM个（即将第一个文件复制）
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
// 读取配置文件
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

// 读取参考关系
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
	cout << "输入的行数或者参考文件非法！" << endl;
	exit(1);
}

int main(int argc, char *argv[])
{
	int line = atoi(argv[1]);						// 需要进行几何变形和光度转换的编码在ref.TXT中的行数
	string configTxtPath = argv[2];					// 配置文件目录
	vector<string> vec = readConfig(configTxtPath);


	string originalImagePath = vec[0];				// 图像集目录
	string rootPath = vec[1];						// 存储结果和中间数据的根目录
	string str_exe_path = vec[2];					// exe文件目录
	string str_txt_path = vec[3];					// TXT中间文件目录
	string final_txt_path = vec[4];					// 最终输出数据目录
	string ref_txt_path = vec[5];					// 参考关系的文件目录
	string s = readRef(ref_txt_path, line);			// 本次编码的参考图像和目标图像的字符串

	vector<string> objPath;							// 目标图像的物体路径
	vector<string> refPicPath;						// 图像参考照片的路径
	string tar;
	readRef(s, tar, objPath, refPicPath);
	testPrint(objPath);
	testPrint(refPicPath);
	int targetImageNum = atoi((tar.substr(0,tar.find(".jpg"))).c_str());
	string str_tarPic = originalImagePath + tar;	// 目标图像的完整路径

	Mat tarImage = imread(str_tarPic);
	SiftFeatureDetector feature;
	vector<KeyPoint> remainder_tar_keypoints;
	feature.detect(tarImage, remainder_tar_keypoints);

	if (objPath.size()>0)
	{
		// 对目标图像切出的物体进行几何变形和光度转换
		for (int i = 0; i < objPath.size(); i++)
		{
			// 对目标照片剩余特征点继续进行匹配，提取sift，几何变形、光度转换，每进行一次剔除一次匹配点，用剩余的匹配点和下次进行匹配
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
	// 对剩余的特征点进行匹配然后进行几何变形和光度转换，每进行一次剔除一次匹配点，用剩余的匹配点进行下一次
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

