#include<iostream>
#include<opencv2/opencv.hpp>
#include<fstream>
#include <sstream>
#include<ctype.h>
#include <stdlib.h>
#include<string>
#include<vector>
#include"include.h"


typedef double			DataType;		//��������													//&&&***Ҫ�޸� һ�㲻�ø�
typedef unsigned int  uint;				//�޷������ͣ�����vector
typedef std::vector<double> V1arr;	//��vector�����double��һά����
typedef std::vector<V1arr> V2arr;		//��vector�����double�Ͷ�ά����


//***********************************************************************���ת���Ĵ���//***********************************************************************
//****************����Ԥ��ͼ��������********
/*
���ο�ͼ������ͨ��Hģ��ת��Ϊ�ӽ���Ŀ��ͼ������꣬��Ҫ��Խ������������������浽txt�ļ���
�������ο�ͼ�����꣬�任�����걣���·����Hģ�ͣ�ͼ���ȣ�ͼ��߶�
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
		int num = 0;							//��num���㣬�Ա���һ�ζ�ȡ
		getline(f, line);					//��ȡtxt�е�һ������
		if (line.empty())					//�ж��Ƿ�Ϊ����
			continue;						//�ǿ��еĻ���������ȡ
		std::istringstream input(line);		//���ö�ȡ����
		while (!input.eof())
		{
			input >> num;						//�Ѷ�����ֵ����num
			gv.push_back(num);			//��num����item
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
		double num = 0;						//��num���㣬�Ա���һ�ζ�ȡ					//�Ĺ�
		getline(f, line);					//��ȡtxt�е�һ������
		if (line.empty())					//�ж��Ƿ�Ϊ����
			continue;						//�ǿ��еĻ���������ȡ
		std::istringstream input(line);		//���ö�ȡ����
		item.clear();
		while (!input.eof())
		{
			input >> num;						//�Ѷ�����ֵ����num
			item.push_back(num);				//��num����item
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
	//Hģ�ͣ���Ҫ�޸�
	double a[3][3]= {
	{350998,  -15892.8,  1.2746e+008},
	{-60394.6,  563102,  3.59031e+007},
	{-160.08,  -14.4046,  632350}
	};
	*/


	for (uint i = 0; i < referenceImageCoordinates.size(); i++)              //����Hģ�ͼ��㼸�α��κ�ͼ��SIFT������
	{
		V1arr item = referenceImageCoordinates[i];

		double c = singleHModel[2][0] * item[0] + singleHModel[2][1] * item[1] + singleHModel[2][2];
		item[0] = (singleHModel[0][0] * item[0] + singleHModel[0][1] * item[1] + singleHModel[0][2]) / c;
		item[1] = (singleHModel[1][0] * item[0] + singleHModel[1][1] * item[1] + singleHModel[1][2]) / c;



		if (item[0] < 0){ item[0] = 0; }                               //������ʱ�ᷢ�������������ֵ����ͼƬ��С�޸�
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

//****************��ֵ����ԭʼ��Ŀ��ͼ��Ҷ�ֵ*************************************************
//*******************************************************
/*
��ȡͼ��ĻҶ�ֵ
������ͼ��·����ͼ��Ķ�άvector���꣬ͼ��Ҷ�ֵ�����·��
*/
void extractGrayValues(const char* image, V2arr imageCoordinates, const char* grayValuesPath)
{
	std::ofstream fout(grayValuesPath);		//�����ֽ�������"grayValues.txt"��						
	IplImage* img = cvLoadImage(image, 0);	//��ͼ��ת���ɻҶ�ͼ��
	//readFromTxt(coordinatesText,coordinatesInImage);  //����ͼƬSIFT����
	CvScalar c, d, e, f;
	int j = 0;
	int g, h, k, l, z;


	for (uint i = 0; i < imageCoordinates.size(); i++)
	{
		V1arr item = imageCoordinates[i];   //��ȡ���괦�ĻҶ�ֵ

		int a = floor(item[1]);               //���ݹ�ʽ���лҶȲ�ֵ
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
//*************��ʽ����********************************************
//���㹫ʽ(15)��A�ĺ���	gv1:�ο�ͼ��Ҷ�ֵ	gv2��Ŀ��ͼ��Ҷ�ֵ
double solveA(V1arr & gv1, V1arr & gv2)
{
	double sum = 0.0;
	for (uint i = 0; i < gv1.size(); i++)
	{
		sum += gv1[i] * gv2[i];
	}
	return sum;
}
//���㹫ʽ(15)��B����C�ĺ���	gv���ο�ͼ�������Ŀ��ͼ��Ҷ�ֵ
double solveBOrC(V1arr & gv)
{
	double sum = 0.0;
	for (uint i = 0; i < gv.size(); i++)
	{
		sum += gv[i];
	}
	return sum;
}
//���㹫ʽ(15)��D�ĺ���		gv���ο�ͼ��Ҷ�ֵ
double solveD(V1arr & gv)
{
	double sum = 0.0;
	for (uint i = 0; i < gv.size(); i++)
	{
		sum += gv[i] * gv[i];
	}
	return sum;
}
//���㹫ʽ(15)�Ц�l�ĺ���
//matchedPixel_num:�ο�ͼ����Ŀ��ͼ��SIFTƥ�����
double solveAlpha(double A, double B, double C, double D, uint matchedPixel_num)
{
	return ((matchedPixel_num * A) - (B * C)) / ((matchedPixel_num * D) - (B * B));
}
//���㹫ʽ(15)�Ц�l�ĺ���
//matchedPixel_num:�ο�ͼ����Ŀ��ͼ��SIFTƥ�����
double solveBeta(double A, double B, double C, double D, uint matchedPixel_num)
{
	return ((C * D) - (A * B)) / ((matchedPixel_num * D) - (B * B));
}
//**********************************�����l����l��������ת��ͼ��*******************
//����ͼ��Ĺ��ת���Ĵ���
//������Ŀ��ͼ��Ҷ�ֵ�����α���͸�ӱ任���ͼ��Ҷ�ֵ�����α���͸�ӱ任���ͼ�񣬲ο�ͼ����Ŀ��ͼ��SIFTƥ�����,���ת����֮���ͼ�񱣴�·��
void singlePhotometricTransformation(V1arr &grayValuesOfTargetImage, V1arr &grayValuesOfReferenceImageChange, const char* image, uint matchedPixel_num, const char* photometricTransfImage){
	//readFromTxt(grayValues,grayValues2);    //��ȡĿ��ͼ��Ҷ�ֵ
	//readFromTxt(grayValuesText,grayValues1);//��ȡ���α��κ�ͼ��Ҷ�ֵ
	//����A��B��C��D
	double A = solveA(grayValuesOfReferenceImageChange, grayValuesOfTargetImage);
	double B = solveBOrC(grayValuesOfReferenceImageChange);
	double C = solveBOrC(grayValuesOfTargetImage);
	double D = solveD(grayValuesOfReferenceImageChange);
	//��ʾA��B��C��D
	//std::cout<<"A: "<<solveA(grayValuesOfReferenceImageChange, grayValuesOfTargetImage)<<std::endl;
	//std::cout<<"B: "<<solveBOrC(grayValuesOfReferenceImageChange)<<std::endl;
	//std::cout<<"C: "<<solveBOrC(grayValuesOfTargetImage)<<std::endl;
	//std::cout<<"D: "<<solveD(grayValuesOfReferenceImageChange)<<std::endl;
	std::cout << "A: " << A << std::endl;
	std::cout << "B: " << B << std::endl;
	std::cout << "C: " << C << std::endl;
	std::cout << "D: " << D << std::endl;

	//�����l
	double alphal = solveAlpha(A, B, C, D, matchedPixel_num);
	//�����l
	double betal = solveBeta(A, B, C, D, matchedPixel_num);

	std::cout << "��l: " << alphal << std::endl;
	std::cout << "��l: " << betal << std::endl;

	IplImage* img = cvLoadImage(image, 1);           //ͼƬ����1ͨ�����
	CvScalar p;

	for (int i = 0; i<img->width; i++)                    //�ı�ͼ��Ҷ�ֵ
	{
		for (int j = 0; j<img->height; j++)
		{
			p = cvGet2D(img, j, i);//(j,i)
			p.val[0] = p.val[0] * alphal + betal;
			p.val[1] = p.val[1] * alphal + betal;
			p.val[2] = p.val[2] * alphal + betal;
			cvSet2D(img, j, i, p);                     //���ã�i,j��������ֵ         
		}

	}

	//cvShowImage("Image",img);                    //������ת�����ͼ��
	cvSaveImage(photometricTransfImage, img);       //���漸�α��κ��ͼ��
	//cvWaitKey(0);                                //�ȴ�����

	//cvDestroyWindow( "Image" );                  //���ٴ���
	cvReleaseImage(&img);                        //�ͷ�ͼ��  


}
//���ת���Ĵ���
void photometricTransformation(std::string originalImagePath, std::string rootPath, int targetImageNum)
{
	//�ο�ͼ�� �����ꡱ ��·��
	std::string referenceImageCoordinatesPath = rootPath + "LMH\\referenceImageCoordinates.txt";
	//Ŀ��ͼ�� �����ꡱ ��·��
	std::string targetImageCoordinatesPath = rootPath + "LMH\\targetImageCoordinates.txt";

	//����char������������תΪ�ַ�ת��
	char numToStr[4] = "";
	//������ת��Ϊ�ַ���
	_itoa_s(targetImageNum, numToStr, 4, 10);
	//Ŀ��ͼ���·��
	const std::string targetImagePathStr = originalImagePath + numToStr + extensionOfImage;
	//Ŀ��ͼ��Ҷ�ֵ��·��
	std::string grayValuesOfTargetImagePath = rootPath + "LMH\\resultOfPhotometricTransformation\\grayValuesOfTargetImage.txt";
	//����Hģ�͵�·��
	std::string finalHModelsPath = rootPath + "LMH\\finalHModels.txt";

	//���ȶ�ȡ�ο�ͼ�����꣬����ȷ������ͼ��ƥ�����صĸ���
	V2arr referenceImageCoordinates;
	readFromTxt(referenceImageCoordinatesPath, referenceImageCoordinates);

	int matchedPixel_num = referenceImageCoordinates.size();
	std::cout << "ƥ�����ظ����� " << matchedPixel_num << std::endl;

	//��ȡĿ��ͼ������
	V2arr targetImageCoordinates;
	readFromTxt(targetImageCoordinatesPath, targetImageCoordinates);

	//����allFinalHModels�����ڴ���������յ�Hģ��
	V2arr allFinalHModels;
	//��ȡ����Hģ��
	readFromTxt(finalHModelsPath, allFinalHModels);
	uint num_allFinalHModels = (allFinalHModels.size() / 3);
	std::cout << "Hģ�͸�����	" << num_allFinalHModels << std::endl;

	//���Hģ������4�����򽫵�һ��Hģ�ͽ��п�����ֱ��4��Ϊֹ
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

	//����finalHModels�����ڴ��4�����յ�Hģ��
	V2arr finalHModels;
	//��allFinalHModels��ǰ�ĸ�����finalHModels
	for (uint i = 0; i < num_HModelsForCoding; i++)
	{
		for (uint j = 0; j < 3; j++)
		{
			finalHModels.push_back(allFinalHModels[i * 3 + j]);
		}
	}

	//���allFinalHModels
	allFinalHModels.clear();

	std::cout << "finalHģ�͸�����	" << (finalHModels.size() / 3) << std::endl;
	//��Ŀ��ͼ��Ҷ�ֵ
	extractGrayValues(targetImagePathStr.c_str(), targetImageCoordinates, grayValuesOfTargetImagePath.c_str());
	//��Ŀ��ͼ��Ҷ�ֵ������
	V1arr grayValuesOfTargetImage;
	readFromTxt(grayValuesOfTargetImagePath, grayValuesOfTargetImage);
	//************************************************************************************

	//����char������������תΪ�ַ�ת
	//char tmp[2];
	for (uint i = 0; i < num_HModelsForCoding; i++)
	{
		//�ο�ͼ�񾭹�Hģ�ͱ仯֮��� �����ꡱ �Ļ���·��
		std::string  referenceImageChangedCoordinatesPath_str = rootPath + "LMH\\resultOfPhotometricTransformation\\referenceImageChangedCoordinates";

		//͸�ӱ任��֮���ͼ�� �Ļ���·��
		std::string perspectiveImagePath_str = rootPath + "LMH\\perspectiveImage";

		//����͸�ӱ���֮��ͼ��ĻҶ�ֵ �Ļ���·��
		std::string grayValuesOfPerspectiveImageChangePath_str = rootPath + "LMH\\resultOfPhotometricTransformation\\grayValuesOfPerspectiveTransfImageChange";

		//���ת����ͼ�񱣴� �Ļ���·��
		std::string photometricTransfImagePath_str = rootPath + "PWW\\photometricTransfImage";

		//����singleHModel�����������Hģ�͵ĵ���Hģ�Ͳ���
		V2arr singleHModel;
		for (uint j = 0; j < 3; j++)
		{
			singleHModel.push_back(finalHModels[i * 3 + j]);
		}
		//������ת��Ϊ�ַ���
		_itoa_s(i, numToStr, 4, 10);
		//itoa(i,tmp,10);

		//�õ��ο�ͼ�񾭹�Hģ�ͱ仯������֮���·��		
		referenceImageChangedCoordinatesPath_str = referenceImageChangedCoordinatesPath_str + numToStr + ".txt";
		std::cout << "�����������·����	" << referenceImageChangedCoordinatesPath_str << std::endl;
		const char* referenceImageChangedCoordinatesPath = referenceImageChangedCoordinatesPath_str.c_str();

		change(referenceImageCoordinates, referenceImageChangedCoordinatesPath, singleHModel);

		//���ı������������
		V2arr referenceImageChangedCoordinates;
		readFromTxt(referenceImageChangedCoordinatesPath, referenceImageChangedCoordinates);

		//�õ�֮ǰ͸�ӱ任֮���ͼ���·��

		perspectiveImagePath_str = perspectiveImagePath_str + numToStr + extensionOfImage;
		std::cout << "͸�ӱ任���·����	" << perspectiveImagePath_str << std::endl;
		const std::string perspectiveImagePath = perspectiveImagePath_str.c_str();

		//�õ�����ͼ��Ҷ�ֵ��·��
		grayValuesOfPerspectiveImageChangePath_str = grayValuesOfPerspectiveImageChangePath_str + numToStr + ".txt";
		std::cout << "͸�ӱ任ͼ��ĻҶ�ֵ·����	" << grayValuesOfPerspectiveImageChangePath_str << std::endl;
		const std::string grayValuesOfPerspectiveImageChangePath = grayValuesOfPerspectiveImageChangePath_str.c_str();

		//��ο�ͼ�񾭹����α���,͸�ӱ任��ĻҶ�ֵ
		extractGrayValues(perspectiveImagePath.c_str(), referenceImageChangedCoordinates, grayValuesOfPerspectiveImageChangePath.c_str());

		//���ο�ͼ�񾭹����α��Σ�͸�ӱ任���ͼ��Ҷ�ֵ������
		V1arr grayValuesOfReferenceImageChange;
		readFromTxt(grayValuesOfPerspectiveImageChangePath_str, grayValuesOfReferenceImageChange);

		//�õ�������ת��֮���ͼ���·��
		photometricTransfImagePath_str = photometricTransfImagePath_str + numToStr + extensionOfImage;
		std::cout << "���ת��֮���ͼ��ı���·����	" << photometricTransfImagePath_str << std::endl;
		const std::string photometricTransfImagePath = photometricTransfImagePath_str.c_str();

		singlePhotometricTransformation(grayValuesOfTargetImage, grayValuesOfReferenceImageChange, perspectiveImagePath.c_str(), matchedPixel_num, photometricTransfImagePath.c_str());  //���й��ת���������ͼ����Ҫ�޸Ĳ���
	}
	std::cout << "���ת�����" << std::endl;
	//	system("pause");
}
