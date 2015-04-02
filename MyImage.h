#ifndef MY_IMAGE_H
#define MY_IMAGE_H

#include "MyMath.h"

// 图像
typedef unsigned char byte_t;

const int IMAGE_BIN = 1;
const int IMAGE_GRY = 2;
const byte_t COLOR_BLACK = 0;
const byte_t COLOR_WHITE = 255;
typedef struct MyImage
{
    int type; // 灰度图或二值图
    int rows;
    int cols;
    int size; // 内存分配
    byte_t* data;
} MyImage;

const int MY_LOAD_IMAGE_COLOR = 0;
const int MY_LOAD_IMAGE_GRAYSCALE = 1;
MyImage* myLoadImage(const char* filename, int flags = MY_LOAD_IMAGE_COLOR);
MyImage* myCreateImageNull();
MyImage* myCreateImage(const MySize& size, int type);
MyImage* myCloneImage(const MyImage* image);
#define MY_MAT_ELEM(mat, y, x) ((mat)->data[(y) * (mat)->cols + (x)])

//void mySet(MyImage* img, const MyRect& rect, byte_t val);

void myReleaseImageData(MyImage* image);
void myReleaseImage(MyImage** image);


int myBwlabel(const MyImage* srcImage, int connection, MyMat* dstLabel);
void myCopy(const MyImage* src, MyImage* dst);
void myEdge(const MyImage* src, MyImage* dst);
void myInverse(const MyImage* src, MyImage* dst);
void myMedfilter(const MyImage* src, MyImage* dst);
const int MY_INTER_LINEAR = 0;
void myResize(const MyImage* src, MyImage* dst, int interpolation = MY_INTER_LINEAR);
void myRotate(const MyImage* src, MyImage* dst, double radian);
void myThreshold(const MyImage* src, MyImage* dst, byte_t threshold);
void myPart(const MyImage* src, const MyRect& rect, MyImage* dst);


#endif //MY_IMAGE_H