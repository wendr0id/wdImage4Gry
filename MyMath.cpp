#include "MyMath.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>

MyPoint myPoint(int y, int x)
{
    assert(y >= 0 && x >= 0);
    MyPoint point;
    point.y = y;
    point.x = x;
    return point;
}

MySize mySize(int height, int width)
{
    assert(height >= 1 && width >= 1);
    MySize size;
    size.height = height;
    size.width = width;
    return size;
}

MyRect myRect(const MyPoint& point, const MySize& size)
{
    MyRect rect;
    rect.y = point.y;
    rect.x = point.x;
    rect.height = size.height;
    rect.width = size.width;
    return rect;
}

MyRect myRect(const MyPoint& lt, const MyPoint& rb)
{
    assert((rb.y - lt.y) >= 0 && (rb.x - lt.x) >= 0);
    MyRect rect;
    rect.y = lt.y;
    rect.x = lt.x;
    rect.height = rb.y - lt.y + 1;
    rect.width = rb.x - lt.x + 1;
    return rect;
}

bool myRectIsValid(const MyRect& rect)
{
    if (rect.y >= 0 && rect.x >= 0 && rect.height >= 1 && rect.width >= 1) {
        return true;
    }
    else {
        return false;
    }
}

bool myRectIsPoint(const MyRect& rect)
{
    if  (rect.y >= 0 && rect.x >= 0 && rect.height == 1 && rect.width == 1) {
        return true;
    }
    else {
        return false;
    }
}

bool myRectIsLine(const MyRect& rect)
{
    if (rect.y >= 0 && rect.x >= 0 && ((rect.height == 1 && rect.width > 1) || (rect.width == 1 && rect.height > 1))) {
        return true;
    }
    else {
        return false;
    }
}

bool myRectIsRect(const MyRect& rect)
{
    if (rect.y >= 0 && rect.x >= 0 && rect.height > 1 && rect.width > 1) {
        return true;
    }
    else {
        return false;
    }
}

MyArr<int>* myCreateArrNull()
{
    MyArr<int>* ret = new MyArr<int>;
    ret->length = 0;
    ret->size = 0;
    ret->data = NULL;
    return ret;
}

MyArr<int>* myCreateArr(int length)
{
    assert(length >= 1);
    MyArr<int>* ret = new MyArr<int>;
    ret->length = length;
    ret->size = length;
    ret->data = (int*)calloc(length, sizeof(int));
    return ret;
}

MyMat* myCreateMatNull()
{
    MyMat* ret = (MyMat*)calloc(1, sizeof(MyMat));
    ret->rows = 0;
    ret->cols = 0;
    ret->size = 0;
    ret->data = NULL;
    return ret;
}

MyMat* myCreateMat(int rows, int cols)
{
    assert(rows >= 1 && cols >= 1);
    MyMat* ret = (MyMat*)calloc(1, sizeof(MyMat));
    ret->rows = rows;
    ret->cols = cols;
    ret->size = rows * cols;
    ret->data = (int*)calloc(ret->size, sizeof(int));
    return ret;
}

void myReleaseMatData(MyMat* mat)
{
    assert(mat != NULL);
    if (mat->size == 0) {
        assert(mat->data == NULL);
    } else {
        free(mat->data);
        mat->data = NULL;
    }
    mat->size = 0;
    mat->cols = 0;
    mat->rows = 0;
}

void myReleaseMat(MyMat** mat)
{
    assert((*mat) != NULL);
    myReleaseMatData(*mat);
    free(*mat);
    (*mat) = NULL;
}

int myDiv(int dividend, int divisor, int round)
{
    if (dividend % divisor == 0) {
        return dividend / divisor;
    }
    else {
        if (round == RESULT_FLOOR) {
            return myFloor((double)dividend / divisor);
        }
        else if (round == RESULT_CEIL) {
            return myCeil((double)dividend / divisor);
        }
        else {
            assert(1);
            return 0;
        }
    }
}

int myFloor(double val)
{
    return (val >= 0 ? (int)val : (int)(val - 1));
}

int myCeil(double val)
{
    return (val >= 0 ? (int)(val + 1) : (int)val);
}

int myRound(double val)
{
    return (val >= 0 ? (int)(val + 0.5) : (int)(val - 0.5));
}

int arrMean(const MyArr<int>* arr)
{
    assert(arr != NULL);
    assert(arr->length >= 1);
    float sum = 0;
    for (int i = 0; i < arr->length; i++) {
        sum += arr->data[i];
    }
    return (int)(sum / arr->length);
}

int arrMode(const MyArr<int>* arr)
{
    assert(arr != NULL);
    assert(arr->length >= 1);
    int max = arr->data[0];
    int min = arr->data[0];
    for (int i = 1; i < arr->length; ++i) {
        if (max < arr->data[i]) {
            max = arr->data[i];
        }
        if (min > arr->data[i]) {
            min = arr->data[i];
        }
    }
    int num = max - min + 1;
    int* table = (int*)calloc(num, sizeof(int));
    for (int i = 0; i < arr->length; ++i) {
        table[arr->data[i] - min]++;
    }
    int freq = 0;
    int mode = 0;
    for (int i = 0; i < num; ++i) {
        if (freq < table[i]) {
            freq = table[i];
            mode = i + min;
        }
    }

    //for (int i = 0; i < num; ++i) {
    //    if (table[i] != 0) {
    //        std::cout << i + min << "[" << table[i] << "]" << std::endl;
    //    }
    //}
    //std::cout << "$$$$$$$" << mode << "$$$$$$$" << std::endl;
    free(table);
    return mode;
}

double arrMedian(const MyArr<int>* arr)
{
    assert(arr != NULL);
    assert(arr->length >= 1);
    MyArr<int>* tmp = myCreateArrNull();
    arrSort(arr, tmp);
    double ret;
    if (arr->length % 2 == 0) {
        ret = (tmp->data[tmp->length / 2 - 1] + tmp->data[tmp->length / 2]) / 2.0;
    }
    else {
        ret = (double)(tmp->data[tmp->length / 2]);
    }
    myReleaseArr(&tmp);
    return ret;
}

void arrDiv(MyArr<int>* arr, int divisor, int round)
{
    assert(arr != NULL);
    assert(arr->length >= 1);
    for (int i = 0; i < arr->length; i++) {
        arr->data[i] = myFloor(arr->data[i] / (float)divisor);
    }
}

void arrMod(MyArr<int>* arr, int divisor)
{
    assert(arr != NULL);
    assert(arr->length >= 1);
    for (int i = 0; i < arr->length; i++) {
        arr->data[i] %= divisor;
    }
}

template <class T>
int partition(T* arr, int left, int right)
{
    int index = left;
    T pivot = arr[index];
    {T swap = arr[index]; arr[index] = arr[right]; arr[right] = swap;}
    for (int i = left; i < right; i++) {
        if (pivot > arr[i]) {   // ÉýÐò
            {T swap = arr[index]; arr[index] = arr[i]; arr[i] = swap;}
            ++index;
        }
    }
    {T swap = arr[right]; arr[right] = arr[index]; arr[index] = swap;}
    return index;
}

template <class T>
void myQsort(T* arr, int left, int right)
{
    if (left >= right) {
        return;
    }
    int index = partition(arr, left, right);
    myQsort(arr, left, index - 1);
    myQsort(arr, index + 1, right);
}

void arrSort(const MyArr<int>* src, MyArr<int>* dst)
{
    assert(src != NULL && dst != NULL && src != dst);
    assert(src->length >= 1);
    myReleaseArr(&dst);
    dst = myCloneArr(src);
    myQsort(dst->data, 0, dst->length - 1);
}

void arrPart(const MyArr<int>* src, MyArr<int>* dst, int first, int last)
{
    assert(src != NULL && src->length >= 1 && dst != NULL);
    assert(first >= 0 && last > first && src->length > last);
    if (src != dst) {
        myReleaseArrData(dst);
        dst->length = last - first + 1;
        dst->size = dst->length;
        dst->data = (int*)calloc(dst->size, sizeof(int));
        for (int i = 0; i < dst->length; ++i) {
            dst->data[i] = src->data[first + i];
        }
    }
    else {
        MyArr<int>* tmp = myCloneArr(src);
        myReleaseArrData(dst);
        dst->length = last - first + 1;
        dst->size = dst->length;
        dst->data = (int*)calloc(dst->size, sizeof(int));
        for (int i = 0; i < dst->length; ++i) {
            dst->data[i] = tmp->data[first + i];
        }
    }
}

int arrSum(const MyArr<int>* arr)
{
    assert(arr != NULL && arr->length >= 1);
    int sum = 0;
    for (int i = 0; i < arr->length; ++i) {
        sum += arr->data[i];
    }
    return sum;
}
