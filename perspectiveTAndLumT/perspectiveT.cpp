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


// 读取文件
void readHTxt(std::string file, vector<vector<double>>& nft, int n)
{
	std::string line;
	std::ifstream f(file);
	if (!f.is_open())
	{
		std::cout << "error openfile" << std::endl;
	}
	vector<double> item;
	int i = 0;
	while (!f.eof() && i <= n * 3)
	{
		double num = 0;						//把num清零，以备下一次读取					//改过
		getline(f, line);					//读取txt中的一行数据
		if (line.empty())					//判断是否为空行
			continue;						//是空行的话，继续读取
		std::istringstream input(line);		//设置读取命令
		item.clear();
		while (!input.eof())
		{
			input >> num;						//把读到的值赋给num
			item.push_back(num);			//把num存入item
		}
		nft.push_back(item);
		i++;
	}
	f.close();
}

vector<KeyPoint> removePoints(vector<DMatch>& matches, vector<KeyPoint>& tar_keypoints)
{

	vector<KeyPoint> remKeypoints;
	vector<bool> removeFlag;
	for (int i = 0; i < tar_keypoints.size(); i++)
	{
		removeFlag.push_back(0);
	}
	for (int i = 0; i < matches.size(); i++)
	{
		removeFlag[matches[i].queryIdx] = 1;
	}
	for (int i = 0; i < tar_keypoints.size(); i++)
	{
		if (removeFlag[i])
			continue;
		remKeypoints.push_back(tar_keypoints[i]);
	}
	return remKeypoints;
}

vector<KeyPoint> perspectiveT1(string str_tarPic, string str_objPic, string str_refPic, string str_txt_path,
	string str_exe_path, vector<KeyPoint>& tar_keypoints)
{
	// 提取特征点
	Mat tar = imread(str_tarPic);
	Mat obj = imread(str_objPic);
	Mat ref = imread(str_refPic);
	vector<KeyPoint> obj_keypoints;
	vector<KeyPoint> ref_keypoints;
	SiftFeatureDetector feature;
	feature.detect(obj, obj_keypoints);
	feature.detect(ref, ref_keypoints);
	Mat tar_description;
	Mat obj_description;
	Mat ref_description;
	SiftDescriptorExtractor descript;
	descript.compute(tar, tar_keypoints, tar_description);
	descript.compute(obj, obj_keypoints, obj_description);
	descript.compute(ref, ref_keypoints, ref_description);
	// 进行匹配
	void sift_match(Mat& description1, Mat& description2, vector<DMatch>& good_matches);
	vector<DMatch> matches_tar_obj;
	vector<DMatch> matches_obj_ref;
	sift_match(tar_description, obj_description, matches_tar_obj);
	sift_match(obj_description, ref_description, matches_obj_ref);


	vector<DMatch> perspectiveT(Mat& tar, Mat& ref, vector<KeyPoint>& tar_keypoints, vector<KeyPoint>& ref_keypoints, vector<DMatch>& matches_tar_obj, vector<DMatch>& matches_obj_ref, string str_txt_path,
		string str_exe_path);
	vector<DMatch> matches = perspectiveT(tar, ref, tar_keypoints, ref_keypoints, matches_tar_obj, matches_obj_ref, str_txt_path, str_exe_path);
	
	
	// 剔除当前匹配的点
	if (matches.size() != 0)
	{
		vector<KeyPoint> remKeypoints = removePoints(matches, tar_keypoints);
		return remKeypoints;
	}
	else
		return tar_keypoints;
}

void get_remainder_tar_keypoints(string& str_tarPic, vector<string>& str_objPic, vector<string>& str_refPic, vector<KeyPoint>& tar_keypoints)
{
	cout << "剔除物体匹配点前，目标图像特征点个数：" << tar_keypoints.size() << endl;
	void sift_match(Mat& description1, Mat& description2, vector<DMatch>& good_matches);
	// 提取特征点
	Mat tar = imread(str_tarPic);
	Mat tar_description;
	vector<vector<DMatch>> vec_matches_tar_obj;
	vector<vector<DMatch>> vec_matches_obj_ref;
	SiftFeatureDetector feature;
	SiftDescriptorExtractor descript;
	descript.compute(tar, tar_keypoints, tar_description);
	for (int i = 0; i < str_objPic.size(); i++)
	{
		Mat ref = imread(str_refPic[i]);
		Mat obj = imread(str_objPic[i]);
		vector<KeyPoint> obj_keypoints, ref_keypoints;
		feature.detect(obj, obj_keypoints);
		feature.detect(ref, ref_keypoints);
		Mat obj_description;
		Mat ref_description;
		descript.compute(obj, obj_keypoints, obj_description);
		descript.compute(ref, ref_keypoints, ref_description);
		vector<DMatch> matches_tar_obj;
		vector<DMatch> matches_obj_ref;
		sift_match(tar_description, obj_description, matches_tar_obj);
		sift_match(obj_description, ref_description, matches_obj_ref);
		vec_matches_tar_obj.push_back(matches_tar_obj);
		vec_matches_obj_ref.push_back(matches_obj_ref);
	}
	// 进行剔除
	vector<bool> removeFlag;
	for (int i = 0; i < tar_keypoints.size(); i++)
	{
		removeFlag.push_back(0);
	}
	for (int i = 0; i < vec_matches_tar_obj.size();i++)
	{
		vector<DMatch> matches_tar_obj = vec_matches_tar_obj[i];
		vector<DMatch> matches_obj_ref = vec_matches_obj_ref[i];
		for (int p = 0; p < matches_tar_obj.size(); p++)
		{
			for (int q = 0; q < matches_obj_ref.size(); q++)
			{
				if (matches_tar_obj[p].trainIdx == matches_obj_ref[q].queryIdx)
					removeFlag[matches_tar_obj[p].queryIdx] = 1;
			}
		}
	}
	vector<KeyPoint> re_tar_keypoints;
	for (int i = 0; i < tar_keypoints.size(); i++)
	{
		if (!removeFlag[i])
			re_tar_keypoints.push_back(tar_keypoints[i]);
	}
	tar_keypoints.swap(re_tar_keypoints);
	cout << "剔除物体匹配点后，目标图像特征点个数：" << tar_keypoints.size() << endl;
}

vector<KeyPoint> perspectiveT2(string str_tarPic, string str_refPic, string str_txt_path,
	string str_exe_path, vector<KeyPoint>& tar_keypoints)
{
	// 提取特征点
	Mat tar = imread(str_tarPic);
	Mat ref = imread(str_refPic);
	vector<KeyPoint> ref_keypoints;
	SiftFeatureDetector feature;
	feature.detect(ref, ref_keypoints);
	Mat tar_description;
	Mat ref_description;
	SiftDescriptorExtractor descript;
	descript.compute(tar, tar_keypoints, tar_description);
	descript.compute(ref, ref_keypoints, ref_description);
	// 进行匹配
	void sift_match(Mat& description1, Mat& description2, vector<DMatch>& good_matches);
	vector<DMatch> matches_tar_ref;
	sift_match(tar_description, ref_description, matches_tar_ref);

	// 剔除当前匹配的点

	vector<DMatch> perspectiveTT(Mat& tar, Mat& ref, vector<KeyPoint>& tar_keypoints, vector<KeyPoint>& ref_keypoints, vector<DMatch>& matches_tar_ref, string str_txt_path,
		string str_exe_path);
	vector<DMatch> matches = perspectiveTT(tar, ref, tar_keypoints, ref_keypoints, matches_tar_ref, str_txt_path, str_exe_path);
	if (matches.size() != 0)
	{
		vector<KeyPoint> remKeypoints = removePoints(matches, tar_keypoints);
		return remKeypoints;
	}
	else
		return tar_keypoints;
}

// 几何变形
vector<DMatch> perspectiveTT(Mat& tar, Mat& ref, vector<KeyPoint>& tar_keypoints, vector<KeyPoint>& ref_keypoints, vector<DMatch>& matches_tar_ref, string str_txt_path,
	string str_exe_path)
{
	// 计算H模型
	int computeH(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& matches, string exe_path, string txt_path);
	int t = computeH(tar_keypoints, ref_keypoints, matches_tar_ref, str_exe_path, str_txt_path);


	// 利用H模型和平滑过滤，过滤sift匹配点
	vector<DMatch> filter(vector< cv::DMatch >& good_matches, Mat& input1, Mat& input2, vector<cv::KeyPoint>& keypoints1, vector<cv::KeyPoint>& keypoints2);
	matches_tar_ref = filter(matches_tar_ref, tar, ref, tar_keypoints, ref_keypoints);
	// H模型过滤
	void h_filter(string finalHModelPath, vector<DMatch>& good_matches2, vector<cv::KeyPoint>& keypoints1, vector<cv::KeyPoint>& keypoints2,
		vector<DMatch>& good_matches3);
	vector<DMatch> matches;
	if (t == -1){
		cout << "没有H模型，不能计算" << endl;
		return matches;
	}
	h_filter(str_txt_path + "finalHModels.txt", matches_tar_ref, tar_keypoints, ref_keypoints, matches);
	// 将剩余匹配点写入到对应TXT中
	void writeTXT(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& good_matches, string txt_path);
	writeTXT(tar_keypoints, ref_keypoints, matches, str_txt_path);


	vector<vector<double>>  Hmodle;					// 读入的H模型
	Hmodle.clear();					// 清空原来内容
	readHTxt(str_txt_path + "finalHModels.txt", Hmodle, num_HModelsForCoding);
	cout << "H模型个数:" << Hmodle.size() << "   " << Hmodle.size() / 3 << endl;
	// H模型数小于指定H模型,将第一个H模型复制到后边
	if (Hmodle.size() / 3 < num_HModelsForCoding)
	{
		while (Hmodle.size() / 3 != num_HModelsForCoding)
		{
			for (int j = 0; j < 3; j++)
			{
				vector<double> vec1;
				vec1.push_back(Hmodle[j][0]);
				vec1.push_back(Hmodle[j][1]);
				vec1.push_back(Hmodle[j][2]);
				Hmodle.push_back(vec1);
			}
		}
	}

	// 透视变换每个H模型
	for (int j = 0; j < num_HModelsForCoding; j++)
	{
		// Mat 转为IplImage
		IplImage image = ref;
		IplImage *src = cvCloneImage(&image);
		IplImage *dst = cvCloneImage(src);//直接把src这个图像复制给dst，不用给dst内存空间了，即不用写dst = cvCreateImage(cvGetSize(src),8,3)
		dst->origin = src->origin;// int origin;        图像原点位置： 0表示顶-左结构,1表示底-左结构 
		cvZero(dst);//是让矩阵的值都为0，有初始化的作用，或者说清零
		// 生成变换矩阵H
		CvMat *warp_mat = cvCreateMat(3, 3, CV_64FC1);//CvMat是一个结构体，CV_32FC1像素值类型为浮点型的单通道图像
		for (int p = 0; p<3; p++)
		{
			for (int k = 0; k<3; k++)
			{
				cvmSet(warp_mat, p, k, Hmodle[j * 3 + p][k]);
			}
		}
		// 进行透视变换
		cvWarpPerspective(src, dst, warp_mat);
		//			cvNamedWindow("1");
		//			cvShowImage("1", dst);
		//			waitKey(0);
		// 将透视变换结果保存
		string str = str_txt_path + "perspectiveImage" + to_string(j) + ".jpg";
		const char * saveImagePath = str.c_str();
		cvSaveImage(saveImagePath, dst);
	}

	return matches;
}

// 几何变形
vector<DMatch> perspectiveT(Mat& tar, Mat& ref, vector<KeyPoint>& tar_keypoints, vector<KeyPoint>& ref_keypoints, vector<DMatch>& matches_tar_obj, vector<DMatch>& matches_obj_ref, string str_txt_path,
	string str_exe_path)
{
	// 通过物体作为中间值对目标照片和参考照片进行匹配
	void indirect_matches(vector<DMatch>& matches1_2, vector<DMatch>& matches2_3, vector<DMatch>& matches1_3);
	vector<DMatch> matches_tar_ref;
	indirect_matches(matches_tar_obj, matches_obj_ref, matches_tar_ref);
	// 计算H模型
	int computeH(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& matches, string exe_path, string txt_path);
	int t = computeH(tar_keypoints, ref_keypoints, matches_tar_ref, str_exe_path, str_txt_path);


	// 利用H模型和平滑过滤，过滤sift匹配点
	vector<DMatch> filter(vector< cv::DMatch >& good_matches, Mat& input1, Mat& input2, vector<cv::KeyPoint>& keypoints1, vector<cv::KeyPoint>& keypoints2);
	matches_tar_ref = filter(matches_tar_ref, tar, ref, tar_keypoints, ref_keypoints);
	// H模型过滤
	void h_filter(string finalHModelPath, vector<DMatch>& good_matches2, vector<cv::KeyPoint>& keypoints1, vector<cv::KeyPoint>& keypoints2,
		vector<DMatch>& good_matches3);
	vector<DMatch> matches;
	if (t == -1){
		cout << "没有H模型，不能计算" << endl;
		return matches;
	}
	h_filter(str_txt_path + "finalHModels.txt", matches_tar_ref, tar_keypoints, ref_keypoints, matches);
	// 将剩余匹配点写入到对应TXT中
	void writeTXT(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& good_matches, string txt_path);
	writeTXT(tar_keypoints, ref_keypoints, matches, str_txt_path);


	vector<vector<double>>  Hmodle;					// 读入的H模型
	Hmodle.clear();					// 清空原来内容
	readHTxt(str_txt_path + "finalHModels.txt", Hmodle, num_HModelsForCoding);
	cout << "H模型个数:" << Hmodle.size() << "   " << Hmodle.size() / 3 << endl;
	// H模型数小于指定H模型,将第一个H模型复制到后边
	if (Hmodle.size() / 3 < num_HModelsForCoding)
	{
		while (Hmodle.size() / 3 != num_HModelsForCoding)
		{
			for (int j = 0; j < 3; j++)
			{
				vector<double> vec1;
				vec1.push_back(Hmodle[j][0]);
				vec1.push_back(Hmodle[j][1]);
				vec1.push_back(Hmodle[j][2]);
				Hmodle.push_back(vec1);
			}
		}
	}

	// 透视变换每个H模型
	for (int j = 0; j < num_HModelsForCoding; j++)
	{
		// Mat 转为IplImage
		IplImage image = ref;
		IplImage *src = cvCloneImage(&image);
		IplImage *dst = cvCloneImage(src);//直接把src这个图像复制给dst，不用给dst内存空间了，即不用写dst = cvCreateImage(cvGetSize(src),8,3)
		dst->origin = src->origin;// int origin;        图像原点位置： 0表示顶-左结构,1表示底-左结构 
		cvZero(dst);//是让矩阵的值都为0，有初始化的作用，或者说清零
		// 生成变换矩阵H
		CvMat *warp_mat = cvCreateMat(3, 3, CV_64FC1);//CvMat是一个结构体，CV_32FC1像素值类型为浮点型的单通道图像
		for (int p = 0; p<3; p++)
		{
			for (int k = 0; k<3; k++)
			{
				cvmSet(warp_mat, p, k, Hmodle[j * 3 + p][k]);
			}
		}
		// 进行透视变换
		cvWarpPerspective(src, dst, warp_mat);
		//			cvNamedWindow("1");
		//			cvShowImage("1", dst);
		//			waitKey(0);
		// 将透视变换结果保存
		string str = str_txt_path + "perspectiveImage" + to_string(j) + ".jpg";
		const char * saveImagePath = str.c_str();
		cvSaveImage(saveImagePath, dst);
	}

	return matches;
}

// 计算间接匹配点
void indirect_matches(vector<DMatch>& matches1_2, vector<DMatch>& matches2_3, vector<DMatch>& matches1_3)
{
	for (int i = 0; i < matches1_2.size(); i++)
	{
		for (int j = 0; j < matches2_3.size(); j++)
		{
			if (matches1_2[i].trainIdx == matches2_3[j].queryIdx)
			{
				//DMatch(int _queryIdx, int _trainIdx, float _distance )
				matches1_3.push_back(DMatch(matches1_2[i].queryIdx, matches2_3[j].trainIdx, matches2_3[j].distance));
				break;
			}
		}
	}
}
