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


// ����Hģ��
int computeH(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& matches, string exe_path, string txt_path)
{

	// ���ƥ���������
	if (matches.size() < MIN_SIFT_NUM)
	{
		cout << "ƥ�������"<<MIN_SIFT_NUM<<"�����˳�" << endl;
		return -1;
	}

	// ��ƥ������д��TXT
	void writeTXT(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& good_matches, string txt_path);
	writeTXT(keypoints1, keypoints2, matches, txt_path);


	// 2��ȥ���ظ�
	string step2_remove = exe_path + "removeRepeated" + " " + txt_path;
	const char* or2 = step2_remove.c_str();
	system(or2);

	// 3������MATLAB����
	string step3_delaunay = exe_path + "delaunayTriangulationAndHModelsProduction2_1" + " " + txt_path;
	const char* or3 = step3_delaunay.c_str();
	system(or3);


	// 4������������С������H
	string step4_Energy = exe_path + "EnergyOpti_11" + " " + txt_path;
	const char* or4 = step4_Energy.c_str();
	system(or4);

	return 0;
}

// sift����ƥ�亯��
void sift_match(Mat& description1, Mat& description2 , vector<DMatch>& good_matches)
{
	
	// ��¼ƥ����
	vector<vector<DMatch>> ratio_matches;

	int remove = 0; // ��¼�Ƴ������ݸ���

	double ratio = 0.6;

	BruteForceMatcher< L2<float> > matcher;
	// ʹ�õڶ�����Ƭƥ���һ����Ƭ2->1
	matcher.knnMatch(description1, description2, ratio_matches, 2);


	for (int i = 0; i < ratio_matches.size(); i++)
	{
		if (ratio_matches[i].size()>1)
		{
			// �ڵڶ���ͼ���ҵ�����ƥ���
			if (ratio_matches[i][0].distance / ratio_matches[i][1].distance > ratio)
			{
				ratio_matches[i].clear();   // ɾ��ƥ������
				remove++;
			}
			else
			{
				good_matches.push_back(ratio_matches[i][0]);
			}
		}
		else
		{
			// �ڶ���ͼ��û���ҵ�����ƥ���
			ratio_matches[i].clear();
			remove++;
		}
	}

	cout << "����ȴν�ƥ�����������Ϊ:" << good_matches.size() << endl;

}
void writeTXT(vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2, vector<DMatch>& good_matches, string txt_path)
{
	string referenceImageCoordinatesPath = txt_path + "referenceImageCoordinates.txt";
	string targetImageCoordinatesPath = txt_path + "targetImageCoordinates.txt";
	std::ofstream foutReference(referenceImageCoordinatesPath);
	std::ofstream foutTarget(targetImageCoordinatesPath);
	for (int i = 0; i <good_matches.size(); i++)
	{
		foutTarget << keypoints1[good_matches[i].queryIdx].pt.x << "   " << keypoints1[good_matches[i].queryIdx].pt.y << "  1" << std::endl;//ѵ��ͼ������� 
		foutReference << keypoints2[good_matches[i].trainIdx].pt.x << "   " << keypoints2[good_matches[i].trainIdx].pt.y << "  1" << std::endl;//�ο�ͼ�������

	}

}