#ifndef MY_MATH_H
#define MY_MATH_H

#include <assert.h>
#include <stdlib.h>

// 点
typedef struct MyPoint
{
    int y;
    int x;
} MyPoint;
MyPoint myPoint(int y, int x);

// 尺寸
typedef struct MySize
{
    int height;
    int width;
} MySize;

MySize mySize(int height, int width);

// 矩形
typedef struct MyRect
{
    int x;
    int y;
    int height;
    int width;
} MyRect;

MyRect myRect(const MyPoint& lt, const MyPoint& rb);
MyRect myRect(const MyPoint& point, const MySize& size);
bool myRectIsValid(const MyRect& rect);
bool myRectIsPoint(const MyRect& rect);
bool myRectIsLine(const MyRect& rect);
bool myRectIsRect(const MyRect& rect);

// 数组
template <class T>
struct MyArr
{
    int length;
    int size;
    T* data;
};
MyArr<int>* myCreateArrNull();
MyArr<int>* myCreateArr(int length);

template <class T>
MyArr<T>* myCloneArr(const MyArr<T>* arr)
{
    assert(arr != NULL);
    MyArr<T>* rtn = new MyArr<T>;
    rtn->length = arr->length;
    rtn->size = arr->length;
    if (rtn->size == 0) {
        rtn->data = NULL;
    } else {
        rtn->data = (T*)calloc(rtn->size, sizeof(T));
    }
    for (int i = 0; i < rtn->length; i++) {
        rtn->data[i] = arr->data[i];
    }
    return rtn;
}

template <class T>
void myReleaseArrData(MyArr<T>* arr)
{
    assert(arr != NULL);
    if (arr->size == 0) {
        assert(arr->data == NULL);
    } else {
        free(arr->data);
        arr->data = NULL;
    }
    arr->size = 0;
    arr->length = 0;
}

template <class T>
void myReleaseArr(MyArr<T>** arr)
{
    assert((*arr) != NULL);
    myReleaseArrData(*arr);
    delete (*arr);
    (*arr) = NULL;
}

// 矩阵
typedef struct MyMat
{
    //union {
        int rows;
    //    int height;
    //};
    //union {
        int cols;
    //    int width;
    //};
    int size;
    int* data;
} MyMat;

MyMat* myCreateMatNull();
MyMat* myCreateMat(int rows, int cols);
void myReleaseMatData(MyMat* mat);
void myReleaseMat(MyMat** mat);

const int RESULT_CEIL = 0;
const int RESULT_FLOOR = 0;
int myDiv(int dividend, int divisor, int round);
int myFloor(double val);
int myCeil(double val);
int myRound(double val);
template <class T>
T myMax(const MyArr<T>* arr)
{
    assert(arr != NULL);
    assert(arr->length >= 1);
    T max = arr->data[0];
    for (int i = 0; i < arr->length; i++) {
        if (max < arr->data[i]) {
            max = arr->data[i];
        }
    }
    return max;
}

template <class T>
T myMin(const MyArr<T>* arr)
{
    assert(arr != NULL);
    assert(arr->length >= 1);
    T min = arr->data[0];
    for (int i = 0; i < arr->length; i++) {
        if (min > arr->data[i]) {
            min = arr->data[i];
        }
    }
    return min;
}
template <class T>
void mySort(T* arr, int len)
{
    assert(arr != NULL && len >= 1);
    int i, j;
    T temp;
    for (i = 1; i < len; i++) {
        temp = arr[i];
        for (j = i - 1; j >= 0 && arr[j] > temp; j--) {
            arr[j + 1] = arr[j];
        }
        arr[j + 1] = temp;
    }
}

int arrMean(const MyArr<int>* arr);
int arrMode(const MyArr<int>* arr);
double arrMedian(const MyArr<int>* arr);
void arrDiv(MyArr<int>* arr, int divisor, int round);
void arrMod(MyArr<int>* arr, int divisor);
void arrPart(const MyArr<int>* src, MyArr<int>* dst, int first, int last);
void arrSort(const MyArr<int>* src, MyArr<int>* dst);
int arrSum(const MyArr<int>* arr);
template <class T>
void arrPushback(MyArr<T>* arr, T val)
{
    assert(arr != NULL);
    if (arr->length == arr->size) {
        if (arr->size == 0) {
            arr->size = 1;
            arr->data = (T*)calloc(arr->size, sizeof(T));
        }
        else {
            arr->size *= 2;
            T* tmp = (T*)calloc(arr->size, sizeof(T));
            for (int i = 0; i < arr->length; ++i) {
                tmp[i] = arr->data[i];
            }
            free(arr->data);
            arr->data = tmp;
        }
    }
    arr->data[length++] = T;
}

const int FIND_EQ = 0;
const int FIND_GT = 1;
template <class T>
MyArr<int>* myFind(T* arr, int len, int cond, T val)
{
    // 返回值：确保不为空，length、size 等值绝对有效，但 data 可能为空
    int* result = (int*)malloc(len * sizeof(int));
    int count = 0;
    for (int i = 0; i < len; i++) {
        if ((cond == FIND_EQ && arr[i] == val) || (cond == FIND_GT && arr[i] > val)) {
            result[count] = i;
            count++;
        }
    }
    MyArr<int>* ret = new MyArr<int>;
    ret->length = count;
    ret->size = count;
    if (count == 0) {
        ret->data = NULL;
    } else {
        ret->data = (int*)malloc(count * sizeof(int));
        for (int k = 0; k < count; k++) {
            ret->data[k] = result[k];
        }
    }
    free(result);
    return ret;
}


#endif // MY_MATH_H