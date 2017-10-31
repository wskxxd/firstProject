# firstProject
## input

行数
配置文件目录

例如：

```3 G:\single\data\config.txt```

(注意这里行数从0开始的，因为第0行没有提供参考，所以不能为第0行)

config.txt中配置（中间为空格键）：

```
  originalImagePath: G:\single\pic\6\1024\    图像集目录
  rootPath: G:\single\data\                   存储结果和中间数据的根目录
  exePath: G:\single\exe\                     调用exe的目录
  txtPath: G:\single\data\LMH\                中间数据的目录
  outRefPath: G:\single\data\finalref\        最终的输出数据目录
  refPath: G:\single\pic\6\1024\ref.txt       参考关系的文件目录
```
ref.txt中文件为：
```
  0.jpg ref: -1 
  4.jpg ref: G:\single\pic\6\1024\4\cut1_76.jpg G:\single\pic\6\1024\0.jpg remainder G:\single\pic\6\1024\0.jpg G:\single\pic\6\1024\0.jpg 
  7.jpg ref: G:\single\pic\6\1024\7\cut4_75.jpg G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\7\cut2_63.jpg G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\7\cut1_73.jpg G:\single\pic\6\1024\4.jpg remainder G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\0.jpg 
  1.jpg ref: G:\single\pic\6\1024\1\cut4_38.jpg G:\single\pic\6\1024\7.jpg remainder G:\single\pic\6\1024\7.jpg G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\0.jpg 
  2.jpg ref: G:\single\pic\6\1024\2\cut3_39.jpg G:\single\pic\6\1024\1.jpg G:\single\pic\6\1024\2\cut1_57.jpg G:\single\pic\6\1024\1.jpg remainder G:\single\pic\6\1024\1.jpg G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\7.jpg 
  6.jpg ref: G:\single\pic\6\1024\6\cut2_78.jpg G:\single\pic\6\1024\7.jpg G:\single\pic\6\1024\6\cut1_38.jpg G:\single\pic\6\1024\7.jpg remainder G:\single\pic\6\1024\7.jpg G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\1.jpg 
  3.jpg ref: G:\single\pic\6\1024\3\cut1_50.jpg G:\single\pic\6\1024\1.jpg remainder G:\single\pic\6\1024\1.jpg G:\single\pic\6\1024\2.jpg G:\single\pic\6\1024\6.jpg 
  5.jpg ref: G:\single\pic\6\1024\5\cut1_60.jpg G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\5\cut2_63.jpg G:\single\pic\6\1024\3.jpg remainder G:\single\pic\6\1024\7.jpg G:\single\pic\6\1024\3.jpg G:\single\pic\6\1024\0.jpg 
  8.jpg ref: G:\single\pic\6\1024\8\cut1_85.jpg G:\single\pic\6\1024\6.jpg remainder G:\single\pic\6\1024\5.jpg G:\single\pic\6\1024\4.jpg G:\single\pic\6\1024\3.jpg 
```

## output

为每一行指定参考的几何变形和光度转换后的图像
