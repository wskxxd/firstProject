#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <fstream>
#include <string>
#include"include.h"
using namespace std;
using namespace cv;

/*
	sift����˺�����
			ƽ�����ˣ�filter����
			hģ�͹��ˣ�h_filter����

	����siftƽ�����뺯��
			double ave_dis_sift

*/





// ���㷽��
double variance(uchar a1, uchar a2, uchar a3, uchar a4, uchar a5, uchar a6, uchar a7, uchar a8, uchar a9){
	double ave = (a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8 + a9) / 9;
	return ((a1 - ave)*(a1 - ave) + (a2 - ave)*(a2 - ave) + (a3 - ave)*(a3 - ave) + (a4 - ave)*(a4 - ave) + (a5 - ave)*(a5 - ave) + (a6 - ave)*(a6 - ave) + (a7 - ave)*(a7 - ave) + (a8 - ave)*(a8 - ave) + (a9 - ave)*(a9 - ave)) / 9;
}
// ����ƽ����
vector<DMatch> filter(vector< cv::DMatch >& good_matches, Mat& input1, Mat& input2, vector<cv::KeyPoint>& keypoints1, vector<cv::KeyPoint>& keypoints2)
{

	cout << "ƽ������ǰƥ���ԣ�" << good_matches.size() << endl;

	// תΪ�Ҷ�ͼ
	Mat gray1, gray2;
	vector<DMatch> good_matches2;	// ƽ�����˺��ƥ���
	cvtColor(input1, gray1, CV_BGR2GRAY);
	cvtColor(input2, gray2, CV_BGR2GRAY);
	// ������������Χ���ص㷽��<30�ĵ㶪��
	for (int i = 0; i < good_matches.size(); i++)
	{
		// image1�ĵ㼯��Ϊ��ѯ�㼯
		double x = keypoints1[good_matches[i].queryIdx].pt.x;
		double y = keypoints1[good_matches[i].queryIdx].pt.y;
		// image2�ĵ㼯��Ϊѵ���㼯
		double X = keypoints2[good_matches[i].trainIdx].pt.x;
		double Y = keypoints2[good_matches[i].trainIdx].pt.y;

		double varG1 = 0.0;
		double varG2 = 0.0;

		if ((int)x>1 && (int)x < gray1.cols - 1 && (int)y>1 && (int)y < gray1.rows - 1)
		{
			Scalar intensity = gray1.at<uchar>(y - 1, x - 1);
			uchar g1 = intensity.val[0];

			intensity = gray1.at<uchar>(y - 1, x);
			uchar g2 = intensity.val[0];

			intensity = gray1.at<uchar>(y - 1, x + 1);
			uchar g3 = intensity.val[0];

			intensity = gray1.at<uchar>(y, x - 1);
			uchar g4 = intensity.val[0];

			intensity = gray1.at<uchar>(y, x);
			uchar g5 = intensity.val[0];

			intensity = gray1.at<uchar>(y, x + 1);
			uchar g6 = intensity.val[0];

			intensity = gray1.at<uchar>(y + 1, x - 1);
			uchar g7 = intensity.val[0];

			intensity = gray1.at<uchar>(y + 1, x);
			uchar g8 = intensity.val[0];

			intensity = gray1.at<uchar>(y + 1, x + 1);
			uchar g9 = intensity.val[0];

			varG1 = variance(g1, g2, g3, g4, g5, g6, g7, g8, g9);
		}

		if ((int)X > 1 && (int)X < gray2.cols - 1 && (int)Y>1 && (int)Y < gray2.rows - 1)
		{
			Scalar intensity = gray2.at<uchar>(Y - 1, X - 1);
			uchar g1 = intensity.val[0];

			intensity = gray2.at<uchar>(Y - 1, X);
			uchar g2 = intensity.val[0];

			intensity = gray2.at<uchar>(Y - 1, X + 1);
			uchar g3 = intensity.val[0];

			intensity = gray2.at<uchar>(Y, X - 1);
			uchar g4 = intensity.val[0];

			intensity = gray2.at<uchar>(Y, X);
			uchar g5 = intensity.val[0];

			intensity = gray2.at<uchar>(Y, X + 1);
			uchar g6 = intensity.val[0];

			intensity = gray2.at<uchar>(Y + 1, X - 1);
			uchar g7 = intensity.val[0];

			intensity = gray2.at<uchar>(Y + 1, X);
			uchar g8 = intensity.val[0];

			intensity = gray2.at<uchar>(Y + 1, X + 1);
			uchar g9 = intensity.val[0];

			varG2 = variance(g1, g2, g3, g4, g5, g6, g7, g8, g9);
		}

		if (varG1 > Smoothing_threshold && varG2 > Smoothing_threshold)
		{
			good_matches2.push_back(good_matches[i]);
		}
	}
	cout << "ƽ�����˺�ƥ���ԣ�" << good_matches2.size() << endl;
	return good_matches2;
}

// Hģ�͹���
void h_filter(string finalHModelPath, vector<DMatch>& good_matches2, vector<cv::KeyPoint>& keypoints1, vector<cv::KeyPoint>& keypoints2,
	vector<DMatch>& good_matches3)
{

	ifstream fileH1(finalHModelPath);

	if (!fileH1)
	{
		cout << "open finalHModelPath.txt error!" << endl;
	}

	// ͳ������
	char ch;
	int line = 0;
	int Hnum = 0;
	while (fileH1.get(ch))
	{
		if (ch == '\n')
			line++;
	}
	fileH1.close();

	ifstream fileH(finalHModelPath);
	if (!fileH)
	{
		cout << "open finalHModelPath.txt error!" << endl;
	}

	double H[12][3];
	if (line >= num_HModelsForCoding * 3)
	{
		Hnum = num_HModelsForCoding;
		for (int i = 0; i < num_HModelsForCoding * 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				fileH >> H[i][j];
			}
		}
		// ��ӡ�ĸ�H����
		/*
		cout << "�ĸ�H����:" << endl;
		for (int i = 0; i < 12; i++)
		{
			if (i % 3 == 0)
			{
				cout << "��" << i / 3 + 1 << "��H����" << endl;
			}
			for (int j = 0; j < 3; j++)
			{
				cout << H[i][j] << " ";
			}
			cout << endl;

		}
		*/
	}
	else
	{
		Hnum = line / 3;
		for (int i = 0; i < line; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				fileH >> H[i][j];
			}
		}
		// ��ӡH����
		/*
		cout << "H����:" << endl;
		for (int i = 0; i < line; i++)
		{
			if (i % 3 == 0)
			{
				cout << "��" << i / 3 + 1 << "��H����" << endl;
			}
			for (int j = 0; j < 3; j++)
			{
				cout << H[i][j] << " ";
			}
			cout << endl;

		}
		*/
	}

	// ɸѡ���DС��5��
	double D[4][10000];
	cout << "ÿ��H�����Ӧ����С�����ƫ��ֵ" << endl;
	for (int j = 0; j < Hnum; j++)
	{
		double min = 0.0;
		double max = 0.0;
		for (int i = 0; i < good_matches2.size(); i++)
		{
			// xi��Ϊ�ο���xj��ΪĿ�꣬Ij - H*Ii
			double xj = keypoints1[good_matches2[i].queryIdx].pt.x;
			double yj = keypoints1[good_matches2[i].queryIdx].pt.y;
			double xi = keypoints2[good_matches2[i].trainIdx].pt.x;
			double yi = keypoints2[good_matches2[i].trainIdx].pt.y;

			// H * Ii = Ij
			D[j][i] = sqrt(pow((xj - (xi*H[j * 3][0] + yi*H[j * 3][1] + H[j * 3][2]) / (xi*H[j * 3 + 2][0] + yi*H[j * 3 + 2][1] + H[j * 3 + 2][2])), 2) +
				pow((yj - (xi*H[j * 3 + 1][0] + yi*H[j * 3 + 1][1] + H[j * 3 + 1][2]) / (xi*H[j * 3 + 2][0] + yi*H[j * 3 + 2][1] + H[j * 3 + 2][2])), 2));



			if (i == 0)
			{
				max = D[j][i];
				min = D[j][i];
			}
			else
			{
				if (min > D[j][i])
				{
					min = D[j][i];
				}
				if (max < D[j][i])
				{
					max = D[j][i];
				}
			}
		}
		cout << "min:" << min << " max:" << max << endl;
	}

	int recordI[10000];
	for (int i = 0; i < 10000; i++)
	{
		recordI[i] = -1;
	}
	for (int j = 0; j < Hnum; j++)
	{
		for (int i = 0; i < good_matches2.size(); i++)
		{
			if ((D[j][i] < Dis_h_threshold) && (recordI[i] != i))
			{
				good_matches3.push_back(good_matches2[i]);
				recordI[i] = i;
			}
		}
	}

	cout << "����" << num_HModelsForCoding << "��H����ɸѡ����ƫ��С��" << Dis_h_threshold << "��ʣ��ƥ���ĸ�����" << good_matches3.size() << endl;
}


// ����siftƽ������
double ave_dis_sift(string txt_path, vector<DMatch>& good_matches3, vector<KeyPoint>& keypoints1, vector<KeyPoint>& keypoints2,
	Mat& description1, Mat& description2)
{
	double d_fi_fj[10000];
	for (int i = 0; i < 10000; i++)
	{
		d_fi_fj[i] = -1;
	}

	string target_Path = txt_path + "targetImageDescript.txt";
	string ref_Path = txt_path + "referenceImageDescript.txt";
	ofstream target(target_Path);
	ofstream ref(ref_Path);

	for (int i = 0; i < good_matches3.size(); i++)
	{
		float Vn1 = 0;
		float Vn2 = 0;
		int ic1 = good_matches3[i].trainIdx;	// �ο���Ƭ
		int ic2 = good_matches3[i].queryIdx;	// Ŀ����Ƭ
		for (int j = 0; j < 128; j++)
		{

			ref << description1.at<float>(ic1, j) << " ";
			target << description2.at<float>(ic2, j) << " ";

			Vn1 += description1.at<float>(ic1, j);
			Vn2 += description2.at<float>(ic2, j);
		}

		ref << std::endl;
		target << std::endl;

		double sum = 0;
		for (int j = 0; j < 128; j++)
		{
			sum += pow(description1.at<float>(ic1, j) / Vn1 - description2.at<float>(ic2, j) / Vn2, 2);
		}
		d_fi_fj[i] = (sqrt(sum)) / 128;
	}

	double D_sum = 0;
	for (int i = 0; i < good_matches3.size(); i++)
	{
		D_sum += d_fi_fj[i];
	}
	double D_ave = 0;
	D_ave = D_sum / good_matches3.size();
	cout << "�����һ���ľ�ֵ:" << D_ave << endl;

	return D_ave;
}

