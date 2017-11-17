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


// 计算H模型
int computeH(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& matches, string exe_path, string txt_path)
{

	// 如果匹配点数过少
	if (matches.size() < MIN_SIFT_NUM)
	{
		cout << "匹配点少于"<<MIN_SIFT_NUM<<"个，退出" << endl;
		return -1;
	}

	// 将匹配坐标写入TXT
	void writeTXT(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& good_matches, string txt_path);
	writeTXT(keypoints1, keypoints2, matches, txt_path);


	// 2、去除重复
	string step2_remove = exe_path + "removeRepeated" + " " + txt_path;
	const char* or2 = step2_remove.c_str();
	system(or2);

	// 3、运行MATLAB代码
	string step3_delaunay = exe_path + "delaunayTriangulationAndHModelsProduction2_1" + " " + txt_path;
	const char* or3 = step3_delaunay.c_str();
	system(or3);


	// 4、运行能量最小化计算H
	string step4_Energy = exe_path + "EnergyOpti_11" + " " + txt_path;
	const char* or4 = step4_Energy.c_str();
	system(or4);

	return 0;
}

// sift进行匹配函数
void sift_match(Mat& description1, Mat& description2 , vector<DMatch>& good_matches)
{
	
	// 记录匹配结果
	vector<vector<DMatch>> ratio_matches;

	int remove = 0; // 记录移除的数据个数

	double ratio = 0.6;

	BruteForceMatcher< L2<float> > matcher;
	// 使用第二张照片匹配第一张照片2->1
	matcher.knnMatch(description1, description2, ratio_matches, 2);


	for (int i = 0; i < ratio_matches.size(); i++)
	{
		if (ratio_matches[i].size()>1)
		{
			// 在第二幅图上找到两个匹配点
			if (ratio_matches[i][0].distance / ratio_matches[i][1].distance > ratio)
			{
				ratio_matches[i].clear();   // 删除匹配数据
				remove++;
			}
			else
			{
				good_matches.push_back(ratio_matches[i][0]);
			}
		}
		else
		{
			// 第二幅图上没有找到两个匹配点
			ratio_matches[i].clear();
			remove++;
		}
	}

	cout << "最近比次近匹配的特征点数为:" << good_matches.size() << endl;

}
void writeTXT(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& good_matches, string txt_path)
{
	string referenceImageCoordinatesPath = txt_path + "referenceImageCoordinates.txt";
	string targetImageCoordinatesPath = txt_path + "targetImageCoordinates.txt";
	std::ofstream foutReference(referenceImageCoordinatesPath);
	std::ofstream foutTarget(targetImageCoordinatesPath);
	for (int i = 0; i <good_matches.size(); i++)
	{
		foutTarget << keypoints1[good_matches[i].queryIdx].pt.x << "   " << keypoints1[good_matches[i].queryIdx].pt.y << "  1" << std::endl;//训练图像的坐标 
		foutReference << keypoints2[good_matches[i].trainIdx].pt.x << "   " << keypoints2[good_matches[i].trainIdx].pt.y << "  1" << std::endl;//参考图像的坐标

	}

}