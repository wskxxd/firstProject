#include<iostream>
#include<opencv2/opencv.hpp>
#include<fstream>
#include <sstream>
#include<ctype.h>
#include <stdlib.h>
#include<string>
#include<vector>
#include"include.h"


typedef double			DataType;		//数据类型													//&&&***要修改 一般不用改
typedef unsigned int  uint;				//无符号整型，用于vector
typedef std::vector<double> V1arr;	//用vector定义的double型一维数组
typedef std::vector<V1arr> V2arr;		//用vector定义的double型二维数组


//***********************************************************************光度转换的代码//***********************************************************************
//****************计算预测图像特征点********
/*
将参考图像坐标通过H模型转化为接近于目标图像的坐标，还要对越界坐标进行修正，保存到txt文件里
参数：参考图像坐标，变换后坐标保存的路径，H模型，图像宽度，图像高度
*/


void readFromTxt(std::string file, V1arr &gv)
{
	std::string line;
	std::ifstream f(file);
	if (!f.is_open())
	{
		std::cout << "error openfile" << std::endl;
	}
	while (!f.eof())
	{
		int num = 0;							//把num清零，以备下一次读取
		getline(f, line);					//读取txt中的一行数据
		if (line.empty())					//判断是否为空行
			continue;						//是空行的话，继续读取
		std::istringstream input(line);		//设置读取命令
		while (!input.eof())
		{
			input >> num;						//把读到的值赋给num
			gv.push_back(num);			//把num存入item
		}
	}
	f.close();
}

void readFromTxt(std::string file, V2arr &nft)
{
	std::string line;
	std::ifstream f(file);
	if (!f.is_open())
	{
		std::cout << "error openfile" << std::endl;
	}
	V1arr item;
	while (!f.eof())
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
			item.push_back(num);				//把num存入item
		}
		nft.push_back(item);
	}
	f.close();
}

void change(V2arr referenceImageCoordinates, const char* changedCoordinatesPath, const V2arr &singleHModel)
{

	//readFromTxt(coordinatesText,coordinatesInImage);

	std::ofstream fout(changedCoordinatesPath);

	int j = 0;
	/*
	//H模型，需要修改
	double a[3][3]= {
	{350998,  -15892.8,  1.2746e+008},
	{-60394.6,  563102,  3.59031e+007},
	{-160.08,  -14.4046,  632350}
	};
	*/


	for (uint i = 0; i < referenceImageCoordinates.size(); i++)              //根据H模型计算几何变形后图像SIFT特征点
	{
		V1arr item = referenceImageCoordinates[i];

		double c = singleHModel[2][0] * item[0] + singleHModel[2][1] * item[1] + singleHModel[2][2];
		item[0] = (singleHModel[0][0] * item[0] + singleHModel[0][1] * item[1] + singleHModel[0][2]) / c;
		item[1] = (singleHModel[1][0] * item[0] + singleHModel[1][1] * item[1] + singleHModel[1][2]) / c;



		if (item[0] < 0){ item[0] = 0; }                               //坐标有时会发生溢出，具体数值根据图片大小修改
		if (item[1] < 0){ item[1] = 0; }
		if (item[0] > imageWidth - 1){ item[0] = imageWidth - 2; }
		if (item[1] > imageHeight - 1){ item[1] = imageHeight - 2; }


		//std::cout<<item[0]<<"   "<<item[1];	
		fout << item[0] << "   " << item[1];

		j++;

		if (j == 1)
		{
			//std::cout<<std::endl;
			fout << std::endl;
			j = 0;
		}

	}

	//referenceImageCoordinates.clear();
	//h.clear();
	//std::cout<<std::endl;

}

//****************插值计算原始与目标图像灰度值*************************************************
//*******************************************************
/*
提取图像的灰度值
参数：图像路径，图像的二维vector坐标，图像灰度值保存的路径
*/
void extractGrayValues(const char* image, V2arr imageCoordinates, const char* grayValuesPath)
{
	std::ofstream fout(grayValuesPath);		//将各种结果存放于"grayValues.txt"中						
	IplImage* img = cvLoadImage(image, 0);	//将图像转换成灰度图像
	//readFromTxt(coordinatesText,coordinatesInImage);  //读入图片SIFT坐标
	CvScalar c, d, e, f;
	int j = 0;
	int g, h, k, l, z;


	for (uint i = 0; i < imageCoordinates.size(); i++)
	{
		V1arr item = imageCoordinates[i];   //提取坐标处的灰度值

		int a = floor(item[1]);               //根据公式进行灰度插值
		int b = floor(item[0]);

		j++;

		double  u = item[1] - a;
		double 	v = item[0] - b;

		c = cvGet2D(img, a, b);
		d = cvGet2D(img, a, b + 1);
		e = cvGet2D(img, a + 1, b);
		f = cvGet2D(img, a + 1, b + 1);

		g = c.val[0];
		h = d.val[0];
		k = e.val[0];
		l = f.val[0];

		z = (1 - u) * (1 - v) * g + (1 - u) * v * h + u * (1 - v) *  k + u * v * l;


		//std::cout<<z<<"	";
		fout << z << "	";

		if (j == 1)
		{
			//std::cout<<std::endl;
			fout << std::endl;
			j = 0;
		}
	}
	//coordinatesInImage.clear();
	//std::cout<<std::endl;

}

//*****************************************************************
//*************公式计算********************************************
//计算公式(15)中A的函数	gv1:参考图像灰度值	gv2：目的图像灰度值
double solveA(V1arr & gv1, V1arr & gv2)
{
	double sum = 0.0;
	for (uint i = 0; i < gv1.size(); i++)
	{
		sum += gv1[i] * gv2[i];
	}
	return sum;
}
//计算公式(15)中B或者C的函数	gv：参考图像或者是目的图像灰度值
double solveBOrC(V1arr & gv)
{
	double sum = 0.0;
	for (uint i = 0; i < gv.size(); i++)
	{
		sum += gv[i];
	}
	return sum;
}
//计算公式(15)中D的函数		gv：参考图像灰度值
double solveD(V1arr & gv)
{
	double sum = 0.0;
	for (uint i = 0; i < gv.size(); i++)
	{
		sum += gv[i] * gv[i];
	}
	return sum;
}
//计算公式(15)中αl的函数
//matchedPixel_num:参考图像与目标图像SIFT匹配个数
double solveAlpha(double A, double B, double C, double D, uint matchedPixel_num)
{
	return ((matchedPixel_num * A) - (B * C)) / ((matchedPixel_num * D) - (B * B));
}
//计算公式(15)中βl的函数
//matchedPixel_num:参考图像与目标图像SIFT匹配个数
double solveBeta(double A, double B, double C, double D, uint matchedPixel_num)
{
	return ((C * D) - (A * B)) / ((matchedPixel_num * D) - (B * B));
}
//**********************************计算αl、βl并输出光度转换图像*******************
//单张图像的光度转换的代码
//参数：目标图像灰度值、几何变形透视变换后的图像灰度值、几何变形透视变换后的图像，参考图像与目标图像SIFT匹配个数,光度转换完之后的图像保存路径
void singlePhotometricTransformation(V1arr &grayValuesOfTargetImage, V1arr &grayValuesOfReferenceImageChange, const char* image, uint matchedPixel_num, const char* photometricTransfImage){
	//readFromTxt(grayValues,grayValues2);    //读取目的图像灰度值
	//readFromTxt(grayValuesText,grayValues1);//读取几何变形后图像灰度值
	//计算A、B、C、D
	double A = solveA(grayValuesOfReferenceImageChange, grayValuesOfTargetImage);
	double B = solveBOrC(grayValuesOfReferenceImageChange);
	double C = solveBOrC(grayValuesOfTargetImage);
	double D = solveD(grayValuesOfReferenceImageChange);
	//显示A、B、C、D
	//std::cout<<"A: "<<solveA(grayValuesOfReferenceImageChange, grayValuesOfTargetImage)<<std::endl;
	//std::cout<<"B: "<<solveBOrC(grayValuesOfReferenceImageChange)<<std::endl;
	//std::cout<<"C: "<<solveBOrC(grayValuesOfTargetImage)<<std::endl;
	//std::cout<<"D: "<<solveD(grayValuesOfReferenceImageChange)<<std::endl;
	std::cout << "A: " << A << std::endl;
	std::cout << "B: " << B << std::endl;
	std::cout << "C: " << C << std::endl;
	std::cout << "D: " << D << std::endl;

	//计算αl
	double alphal = solveAlpha(A, B, C, D, matchedPixel_num);
	//计算βl
	double betal = solveBeta(A, B, C, D, matchedPixel_num);

	std::cout << "αl: " << alphal << std::endl;
	std::cout << "βl: " << betal << std::endl;

	IplImage* img = cvLoadImage(image, 1);           //图片按照1通道输出
	CvScalar p;

	for (int i = 0; i<img->width; i++)                    //改变图像灰度值
	{
		for (int j = 0; j<img->height; j++)
		{
			p = cvGet2D(img, j, i);//(j,i)
			p.val[0] = p.val[0] * alphal + betal;
			p.val[1] = p.val[1] * alphal + betal;
			p.val[2] = p.val[2] * alphal + betal;
			cvSet2D(img, j, i, p);                     //设置（i,j）点像素值         
		}

	}

	//cvShowImage("Image",img);                    //输出光度转换后的图像
	cvSaveImage(photometricTransfImage, img);       //储存几何变形后的图像
	//cvWaitKey(0);                                //等待按键

	//cvDestroyWindow( "Image" );                  //销毁窗口
	cvReleaseImage(&img);                        //释放图像  


}
//光度转换的代码
void photometricTransformation(std::string originalImagePath, std::string rootPath, int targetImageNum)
{
	//参考图像 “坐标” 的路径
	std::string referenceImageCoordinatesPath = rootPath + "LMH\\referenceImageCoordinates.txt";
	//目标图像 “坐标” 的路径
	std::string targetImageCoordinatesPath = rootPath + "LMH\\targetImageCoordinates.txt";

	//定义char数组来将数字转为字符转换
	char numToStr[4] = "";
	//将数字转化为字符串
	_itoa_s(targetImageNum, numToStr, 4, 10);
	//目标图像的路径
	const std::string targetImagePathStr = originalImagePath + numToStr + extensionOfImage;
	//目标图像灰度值的路径
	std::string grayValuesOfTargetImagePath = rootPath + "LMH\\resultOfPhotometricTransformation\\grayValuesOfTargetImage.txt";
	//最终H模型的路径
	std::string finalHModelsPath = rootPath + "LMH\\finalHModels.txt";

	//首先读取参考图像坐标，并且确定两幅图像匹配像素的个数
	V2arr referenceImageCoordinates;
	readFromTxt(referenceImageCoordinatesPath, referenceImageCoordinates);

	int matchedPixel_num = referenceImageCoordinates.size();
	std::cout << "匹配像素个数： " << matchedPixel_num << std::endl;

	//读取目标图像坐标
	V2arr targetImageCoordinates;
	readFromTxt(targetImageCoordinatesPath, targetImageCoordinates);

	//定义allFinalHModels，用于存放所有最终的H模型
	V2arr allFinalHModels;
	//读取最终H模型
	readFromTxt(finalHModelsPath, allFinalHModels);
	uint num_allFinalHModels = (allFinalHModels.size() / 3);
	std::cout << "H模型个数：	" << num_allFinalHModels << std::endl;

	//如果H模型少于4个，则将第一个H模型进行拷贝，直至4个为止
	if (num_allFinalHModels < num_HModelsForCoding)
	{
		for (uint i = 0; i < (num_HModelsForCoding - num_allFinalHModels); ++i)
		{
			for (uint j = 0; j < 3; j++)
			{
				allFinalHModels.push_back(allFinalHModels[j]);
			}
		}
	}

	//定义finalHModels，用于存放4个最终的H模型
	V2arr finalHModels;
	//将allFinalHModels的前四个赋给finalHModels
	for (uint i = 0; i < num_HModelsForCoding; i++)
	{
		for (uint j = 0; j < 3; j++)
		{
			finalHModels.push_back(allFinalHModels[i * 3 + j]);
		}
	}

	//清空allFinalHModels
	allFinalHModels.clear();

	std::cout << "finalH模型个数：	" << (finalHModels.size() / 3) << std::endl;
	//求目标图像灰度值
	extractGrayValues(targetImagePathStr.c_str(), targetImageCoordinates, grayValuesOfTargetImagePath.c_str());
	//将目的图像灰度值读进来
	V1arr grayValuesOfTargetImage;
	readFromTxt(grayValuesOfTargetImagePath, grayValuesOfTargetImage);
	//************************************************************************************

	//定义char数组来将数字转为字符转
	//char tmp[2];
	for (uint i = 0; i < num_HModelsForCoding; i++)
	{
		//参考图像经过H模型变化之后的 “坐标” 的基本路径
		std::string  referenceImageChangedCoordinatesPath_str = rootPath + "LMH\\resultOfPhotometricTransformation\\referenceImageChangedCoordinates";

		//透视变换完之后的图像 的基本路径
		std::string perspectiveImagePath_str = rootPath + "LMH\\perspectiveImage";

		//经过透视变形之后图像的灰度值 的基本路径
		std::string grayValuesOfPerspectiveImageChangePath_str = rootPath + "LMH\\resultOfPhotometricTransformation\\grayValuesOfPerspectiveTransfImageChange";

		//光度转换后图像保存 的基本路径
		std::string photometricTransfImagePath_str = rootPath + "PWW\\photometricTransfImage";

		//定义singleHModel，来存放最终H模型的单个H模型参数
		V2arr singleHModel;
		for (uint j = 0; j < 3; j++)
		{
			singleHModel.push_back(finalHModels[i * 3 + j]);
		}
		//将数字转化为字符串
		_itoa_s(i, numToStr, 4, 10);
		//itoa(i,tmp,10);

		//得到参考图像经过H模型变化和修正之后的路径		
		referenceImageChangedCoordinatesPath_str = referenceImageChangedCoordinatesPath_str + numToStr + ".txt";
		std::cout << "坐标修正后的路径：	" << referenceImageChangedCoordinatesPath_str << std::endl;
		const char* referenceImageChangedCoordinatesPath = referenceImageChangedCoordinatesPath_str.c_str();

		change(referenceImageCoordinates, referenceImageChangedCoordinatesPath, singleHModel);

		//将改变后的坐标读进来
		V2arr referenceImageChangedCoordinates;
		readFromTxt(referenceImageChangedCoordinatesPath, referenceImageChangedCoordinates);

		//得到之前透视变换之后的图像的路径

		perspectiveImagePath_str = perspectiveImagePath_str + numToStr + extensionOfImage;
		std::cout << "透视变换后的路径：	" << perspectiveImagePath_str << std::endl;
		const std::string perspectiveImagePath = perspectiveImagePath_str.c_str();

		//得到保存图像灰度值的路径
		grayValuesOfPerspectiveImageChangePath_str = grayValuesOfPerspectiveImageChangePath_str + numToStr + ".txt";
		std::cout << "透视变换图像的灰度值路径：	" << grayValuesOfPerspectiveImageChangePath_str << std::endl;
		const std::string grayValuesOfPerspectiveImageChangePath = grayValuesOfPerspectiveImageChangePath_str.c_str();

		//求参考图像经过几何变形,透视变换后的灰度值
		extractGrayValues(perspectiveImagePath.c_str(), referenceImageChangedCoordinates, grayValuesOfPerspectiveImageChangePath.c_str());

		//将参考图像经过几何变形，透视变换后的图像灰度值读进来
		V1arr grayValuesOfReferenceImageChange;
		readFromTxt(grayValuesOfPerspectiveImageChangePath_str, grayValuesOfReferenceImageChange);

		//得到保存光度转换之后的图像的路径
		photometricTransfImagePath_str = photometricTransfImagePath_str + numToStr + extensionOfImage;
		std::cout << "光度转换之后的图像的保存路径：	" << photometricTransfImagePath_str << std::endl;
		const std::string photometricTransfImagePath = photometricTransfImagePath_str.c_str();

		singlePhotometricTransformation(grayValuesOfTargetImage, grayValuesOfReferenceImageChange, perspectiveImagePath.c_str(), matchedPixel_num, photometricTransfImagePath.c_str());  //进行光度转换，并输出图像，需要修改参数
	}
	std::cout << "光度转换完毕" << std::endl;
	//	system("pause");
}
