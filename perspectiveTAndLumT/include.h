#ifndef _Hinclude_H_
#define _Hinclude_H_


#define imageWidth 1024
#define imageHeight 1024								// 使用的图像大小
#define extensionOfImage ".jpg"
static const int num_HModelsForCoding = 2;				// 使用h模型的个数
static int Ref_num = 0;									// 几何变形光度转换后的参考图像的起始编号

static int perspectiveNum = 0;

const int Smoothing_threshold = 20;						// 平滑过滤点的阈值
const int Dis_h_threshold = 5;							// H模型匹配误差值阈值

#define REF_NUM 12										// 控制参考最终产生的参考图像个数，不足12 的使用第0 张照片补足




#endif