#include "MyImage.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


typedef unsigned long dword_t;
typedef unsigned short word_t;
// ָ���� 2 �ֽڶ���
#pragma pack(2)
typedef struct BitmapFileHeader
{
    word_t bfType;
    dword_t bfSize;
    word_t bfReserved1;
    word_t bfReserved2;
    dword_t bfOffBits;
} BitmapFileHeader;
// ȡ��ָ�����룬�ָ�ȱʡ����
#pragma pack()

typedef struct BitmapInfoHeader
{
    dword_t biSize;
    long biWidth;
    long biHeight;
    word_t biPlanes;
    word_t biBitCount;
    dword_t biCompression;
    dword_t biSizeImage;
    long biXPelsPerMeter;
    long biYPelsPerMeter;
    dword_t biClrUsed;
    dword_t biClrImportant;
} BitmapInfoHeader;

typedef struct BitmapCoreHeader
{
    dword_t bcSize;
    word_t bcWidth;
    word_t bcHeight;
    word_t bcPlanes;
    word_t bcBitCount;
} BitmapCoreHeader;


MyImage* myLoadImage(const char* filename, int flags)
{
    // ���룺4GB ���µ� 24λ DIB ͼ��·���ļ�����.bmp��
    // ������Ҷ�ͼ���󣬿���Ϊ NULL
    assert(filename != NULL);
    assert(flags == MY_LOAD_IMAGE_GRAYSCALE);
    // ���ļ�
    FILE* strmFile;
    if ((strmFile = fopen(filename, "rb")) == NULL) {
        perror("fopen");
        return NULL;
    }

    // ��ȡ�ļ���С��ֻ֧�� 4GB ���µ��ļ�
    fseek(strmFile, 0L, SEEK_END);
    long sizeFile = ftell(strmFile);
    if (sizeFile == -1L) {
        perror("sizeFile");
        fclose(strmFile);
        return NULL;
    }

    // ���仺�����ڴ�
    byte_t* pFile;
    if ((pFile = (byte_t*)malloc(sizeFile)) == NULL) {
        perror("malloc");
        fclose(strmFile);
        return NULL;
    }

    // ��ȡ�ļ�
    size_t nBytesRead;
    rewind(strmFile);
    nBytesRead = fread(pFile, sizeof(byte_t), sizeFile, strmFile);
    if (nBytesRead != sizeFile) {
        perror("fread");
        fclose(strmFile);
        free(pFile);
        return NULL;
    }
    fclose(strmFile);

    // �ж��ļ�ǩ��
    BitmapFileHeader* pFileHeader = (BitmapFileHeader*)pFile;
    if (pFileHeader->bfType != *(word_t*)"BM") {
        perror("pFileHeader->bfType");
        free(pFile);
        return NULL;
    }

    
    BitmapInfoHeader* pInfoHeader = (BitmapInfoHeader*)(pFileHeader + 1);
    BitmapCoreHeader* pCoreHeader;
    int bmWidth;
    int bmHeight;
    int sizeLine;
    // OS/2 DIB
    if (pInfoHeader->biSize == sizeof(BitmapCoreHeader)) {
        pCoreHeader = (BitmapCoreHeader*)pInfoHeader;
        if (pCoreHeader->bcBitCount != 24) {
            perror("pCoreHeader->bcBitCount");
            free(pFile);
            return NULL;
        }
        bmWidth = pCoreHeader->bcWidth;
        bmHeight = pCoreHeader->bcHeight;
        sizeLine = ((bmWidth * pCoreHeader->bcBitCount + 31) & ~31) >> 3;
    }
    // Windows ��չ DIB
    else {
        // �ж�ͼ���Ƿ�Ϊ 24 λͼ
        if (pInfoHeader->biBitCount != 24) {
            perror("pInfoHeader->biBitCount");
            free(pFile);
            return NULL;
        }
        // ���ͼ����
        bmWidth = pInfoHeader->biWidth;
        bmHeight = pInfoHeader->biHeight;
        // һ���������ֽ�������Ϊ 4 �ı���
        sizeLine = ((bmWidth * pInfoHeader->biBitCount + 31) & ~31) >> 3;
    }


    // ���ͼ����������������ָ��
    byte_t* pBits = (byte_t*)pFileHeader + pFileHeader->bfOffBits;

    MyImage* ret = (MyImage*)calloc(1, sizeof(MyImage));
    ret->type = IMAGE_GRY;
    ret->rows = bmHeight;
    ret->cols = bmWidth;
    ret->size = ret->rows * ret->cols;
    ret->data = (byte_t*)calloc(ret->size, sizeof(byte_t));

    // �ҶȻ���д��
    for (int y = 0; y < bmHeight; ++y) {
        for (int x = 0; x < bmWidth; ++x) {
            byte_t B = *(pBits + sizeLine * y + x * 3);
            byte_t G = *(pBits + sizeLine * y + x * 3 + 1);
            byte_t R = *(pBits + sizeLine * y + x * 3 + 2);
            MY_MAT_ELEM(ret, bmHeight - y - 1, x) = (byte_t)(R * 0.2989 + G * 0.5870 + B * 0.1140);
        }
    }

    free(pFile);
    return ret;
}

MyImage* myCreateImageNull()
{
    // ���أ�һ����ͼ��
    MyImage* ret = (MyImage*)calloc(1, sizeof(MyImage));
    ret->rows = 0;
    ret->cols = 0;
    ret->type = 0;
    ret->size = 0;
    ret->data = NULL;
    return ret;
}

MyImage* myCreateImage(const MySize& size, int type)
{
    // ���أ�һ��ָ����С�����͵�ȫ��ͼ��
    assert(size.height >= 1 && size.width >= 1);
    assert(type == IMAGE_BIN || type == IMAGE_GRY);
    MyImage* ret = (MyImage*)calloc(1, sizeof(MyImage));
    ret->rows = size.height;
    ret->cols = size.width;
    ret->type = type;
    ret->size = ret->rows * ret->cols;
    ret->data = (byte_t*)calloc(ret->size, sizeof(byte_t));
    return ret;
}

MyImage* myCloneImage(const MyImage* img)
{
    // ���أ���һ��ͼ��ĸ���
    assert(img != NULL);
    MyImage* ret = (MyImage*)calloc(1, sizeof(MyImage));
    ret->rows = img->rows;
    ret->cols = img->cols;
    ret->type = img->type;
    ret->size = ret->rows * ret->cols;
    if (ret->size == 0) {
        ret->data = NULL;
    } else {
        ret->data = (byte_t*)calloc(ret->size, sizeof(byte_t));
    }
    for (int i = 0; i < ret->size; i++) {
        ret->data[i] = img->data[i];
    }
    return ret;
}

void myReleaseImageData(MyImage* img)
{
    // ���ã�ʹͼ���Ϊ��ͼ��
    assert(img != NULL);
    if (img->size == 0) {
        assert(img->data == NULL);
    } else {
        free((img->data));
        img->data = NULL;
    }
    img->size = 0;
    img->type = 0;
    img->cols = 0;
    img->rows = 0;
}

void myReleaseImage(MyImage** img)
{
    // ���ã�ɾ��һ��ͼ��
    assert((*img) != NULL);
    myReleaseImageData(*img);
    free(*img);
    (*img) = NULL;
}

typedef struct
{
    int row;
    int st;
    int en;
    int label;
}Run;

int myBwlabel(const MyImage* img, int connection, MyMat* labels)
{
    assert(img != NULL && labels != NULL);
    assert(connection == 4 || connection == 8);
    assert(img->type == IMAGE_BIN && img->rows >= 1 && img->cols >= 1);
    myReleaseMatData(labels);
    labels->rows = img->rows;
    labels->cols = img->cols;
    labels->size = labels->rows * labels->cols;
    labels->data = (int*)calloc(labels->size, sizeof(int));

    const int MAX_RUN = 10000;
    Run run[MAX_RUN] = {0};
    int numOfRuns = 0;
    int height = img->rows;
    int width = img->cols;
    for (int y = 0; y < height; y++) {
        const byte_t* rowData = img->data + y * img->cols;
        if (rowData[0] == 255) {
            run[numOfRuns].st = 0;
            run[numOfRuns].row = y;
        }
        for (int x = 1; x < width; x++) {
            if (rowData[x - 1] == 0 && rowData[x] == 255) {
                run[numOfRuns].st = x;
                run[numOfRuns].row = y;
            }
            else if (rowData[x - 1] == 255 && rowData[x] == 0) {
                run[numOfRuns].en = x - 1;
                numOfRuns++;
            }
        }
        if (rowData[width - 1] == 255) {
            run[numOfRuns].en = width - 1;
            numOfRuns++;
        }
    }

    // �ӵ�һ����������ɨ��
    int idxCurRow = 0;
    int idxLabel = 1;
    int idxFirstRunOnPreRow = 0;
    int idxLastRunOnPreRow = -1;
    int idxFirstRunOnCurRow = 0;
    int offset = (connection == 8 ? 1 : 0);
    // ����������
    for (int i = 0; i < numOfRuns; i++) {
        // �����ǰ����ɨ���е���һ��
        if (run[i].row == idxCurRow + 1) {
            // ɨ�����Ƶ���һ��
            idxCurRow++;
            idxFirstRunOnPreRow = idxFirstRunOnCurRow;
            idxLastRunOnPreRow = i - 1;
            idxFirstRunOnCurRow = i;
        }
        // ���ߵ�ǰ���ڵ�ǰɨ���е��ºü���
        else if (run[i].row >= idxCurRow + 2) {
            idxCurRow = run[i].row;
            idxFirstRunOnPreRow = 0;
            idxLastRunOnPreRow = -1;
            idxFirstRunOnCurRow = i;
        }
        for (int j = idxFirstRunOnPreRow; j <= idxLastRunOnPreRow; j++) {
            if ((run[i].st <= (run[j].en + offset)) && (run[i].en >= (run[j].st - offset))) {
                // û�б���Ź�
                if (run[i].label == 0) {
                    run[i].label = run[j].label;
                }
                // �Ѿ������
                else if (run[i].label != run[j].label) {
                    for (int k = 0; k <= j; k++) {
                        if (run[k].label == run[j].label) {
                            run[k].label = run[i].label;
                        }
                    }
                    // ����ȼ۶�
                    //equivalences.push_back(make_pair(runLabels[i], runLabels[j]));
                }
            }
        }
        // û����ǰһ�е��κ�run�غ�
        if (run[i].label == 0) {
            run[i].label = idxLabel++;
        }
    }

    int idxCorrectLabel = 0;
    int correctLabel[MAX_RUN] = {0};
    for (int i = 0; i < numOfRuns; ++i) {
        if (correctLabel[run[i].label] == 0) {
            ++idxCorrectLabel;
            correctLabel[run[i].label] = idxCorrectLabel;
        }
        for (int j = run[i].st; j <= run[i].en; ++j) {
            MY_MAT_ELEM(labels, run[i].row, j) = correctLabel[run[i].label];
        }
    }
    return idxCorrectLabel;
}

void myCopy(const MyImage* src, MyImage* dst)
{
    assert(src != NULL && dst != NULL && src != dst);
    myReleaseImageData(dst);
    dst->type = src->type;
    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->size = dst->rows * dst->cols;
    if (dst->size == 0) {
        dst->data = NULL;
    }
    else {
        dst->data = (byte_t*)calloc(dst->size, sizeof(byte_t));
        for (int y = 0; y < src->rows; y++) {
            for (int x = 0; x < src->cols; x++) {
                MY_MAT_ELEM(dst, y, x) = MY_MAT_ELEM(src, y, x);
            }
        }
    }
}

void myEdge(const MyImage* srcImage, MyImage* dstImage)
{
    if (srcImage != dstImage) {
        myReleaseImageData(dstImage);
        dstImage->type = IMAGE_BIN;
        dstImage->rows = srcImage->rows;
        dstImage->cols = srcImage->cols;
        dstImage->size = dstImage->rows * dstImage->cols;
        dstImage->data = (byte_t*)calloc(dstImage->size, sizeof(byte_t));
    }
    else {
        dstImage->type = IMAGE_BIN;
    }

    MyMat* mattmp = myCreateMat(dstImage->rows, dstImage->cols);
    int d00, d01, d02; // d00 d01 d02   +0.125  0.000 -0.125
    int d10, d11, d12; // d10 d11 d12   +0.250  0.000 -0.250
    int d20, d21, d22; // d20 d21 d22   +0.125  0.000 -0.125
    const int ROWS = srcImage->rows;
    const int COLS = srcImage->cols;
    //float* mattmp = (float*)calloc(imgbin->height * imgbin->width, sizeof(float));
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            d00 = MY_MAT_ELEM(srcImage,        (y >= 1 ? y - 1 : 0),               (x >= 1 ? x - 1 : 0));
            d10 = MY_MAT_ELEM(srcImage,                  y,                        (x >= 1 ? x - 1 : 0));
            d20 = MY_MAT_ELEM(srcImage, (y <= ROWS - 2 ? y + 1 : ROWS - 1),        (x >= 1 ? x - 1 : 0));
            d02 = MY_MAT_ELEM(srcImage,        (y >= 1 ? y - 1 : 0),        (x <= COLS - 2 ? x + 1 : COLS - 1));
            d12 = MY_MAT_ELEM(srcImage,                  y,                 (x <= COLS - 2 ? x + 1 : COLS - 1));
            d22 = MY_MAT_ELEM(srcImage, (y <= ROWS - 2 ? y + 1 : ROWS - 1), (x <= COLS - 2 ? x + 1 : COLS - 1));
            //int d11 = d00 / 8 + d10 / 4 + d20 / 8 - d02 / 8 - d12 / 4 - d22 / 8;
            int d11 = d00 + d10 * 2 + d20 - d02 - d12 * 2 - d22;
            MY_MAT_ELEM(mattmp, y, x) = d11 * d11;
        }
    }

    int length = mattmp->rows * mattmp->cols;
    double sum = 0;
    for (int i = 0; i < length; ++i) {
        sum += mattmp->data[i];
    }
    int cutoff = int(sum / length * 4);

    //for (int i = 0; i < length; ++i) {
    //    dstImage->data[i] = (mattmp->data[i] > cutoff ? COLOR_WHITE : COLOR_BLACK);
    //}
    for (int r = 0; r < mattmp->rows; ++r) {
        for (int c = 0; c < mattmp->cols; ++c) {
            bool b1, b2, b3, b4;
            if ((c - 1) < 0) {
                b1 = true;
            }
            else {
                b1 = (MY_MAT_ELEM(mattmp, r, c - 1) <= MY_MAT_ELEM(mattmp, r, c));
            }
            if ((c + 1) >= mattmp->cols) {
                b2 = true;
            }
            else {
                b2 = (MY_MAT_ELEM(mattmp, r, c) > MY_MAT_ELEM(mattmp, r, c + 1));
            }
            if ((r - 1) < 0) {
                b3 = true;
            }
            else {
                b3 = (MY_MAT_ELEM(mattmp, r - 1, c) <= MY_MAT_ELEM(mattmp, r, c));
            }
            if ((r + 1) >= mattmp->rows) {
                b4 = true;
            }
            else {
                b4 = (MY_MAT_ELEM(mattmp, r, c) > MY_MAT_ELEM(mattmp, r + 1, c));
            }
            MY_MAT_ELEM(dstImage, r, c) = ((MY_MAT_ELEM(mattmp, r, c) > cutoff) && ((b1 && b2) || (b3 && b4)) ? COLOR_WHITE : COLOR_BLACK);
        }
    }
    myReleaseMat(&mattmp);
}

void myInverse(const MyImage* src, MyImage* dst)
{
    assert(src != NULL && dst != NULL);
    assert(src->type == IMAGE_BIN && src->rows >= 1 && src->cols >= 1);
    if (src != dst) {
        myReleaseImageData(dst);
        dst->type = src->type;
        dst->rows = src->rows;
        dst->cols = src->cols;
        dst->size = dst->rows * dst->cols;
        dst->data = (byte_t*)calloc(dst->size, sizeof(byte_t));
    }
    for (int y = 0; y < src->rows; ++y) {
        for (int x = 0; x < src->cols; ++x) {
            if (MY_MAT_ELEM(src, y, x) == COLOR_WHITE) {
                MY_MAT_ELEM(dst, y, x) = COLOR_BLACK;
            }
            else if (MY_MAT_ELEM(src, y, x) == COLOR_BLACK) {
                MY_MAT_ELEM(dst, y, x) = COLOR_WHITE;
            }
            else {
                assert(1);
            }
        }
    }
}

void myMedfilter(const MyImage* src, MyImage* dst)
{
    assert(src != NULL && dst != NULL && src != dst);
    assert(src->type == IMAGE_GRY);
    myReleaseImageData(dst);
    dst->type = IMAGE_GRY;
    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->size = dst->rows * dst->cols;
    dst->data = (byte_t*)calloc(dst->size, sizeof(byte_t));
    byte_t data[8]; //���������е�����
    for (int y = 0; y < src->rows; ++y) {
        for (int x = 0; x < src->cols; ++x) {
            int num = 0;
            for (int m = -1; m <= 1; ++m) {
                for (int n = -1; n <= 1; ++n) {
                    if (m != 0 || n != 0) {
                        if (0 <= y + m && y + m < src->rows && 0 <= x + n && x + n < src->cols) {
                            data[num++] = MY_MAT_ELEM(src, y + m, x + n);
                        }
                        else {
                            data[num++] = 0;
                        }
                    }
                }
            }
            assert(num == 8);
            mySort(data, num);
            byte_t medium;
            if (num % 2 == 0) {
                medium = (byte_t)((data[(num - 1) / 2] + data[(num + 1) / 2]) / 2);
            }
            else {
                medium = data[num / 2];
            }
            MY_MAT_ELEM(dst, y, x) = medium;
        }
    }
}

void myResize(const MyImage* src, MyImage* dst, int interpolation)
{
    assert(src != NULL && dst != NULL && src != dst);
    assert(interpolation == MY_INTER_LINEAR);
    // ˫���Բ�ֵ
    // (1)����Դͼ����Ŀ��ͼ�����ߵı���
    int h0 = src->rows;
    int w0 = src->cols;
    int h1 = dst->rows;
    int w1 = dst->cols;
    double fw = double(w0) / w1;
    double fh = double(h0) / h1;
    //for (int y = 0; y < h1; y++) {
    //    for (int x = 0; x < w1; x++) {
    //        // (2)���Ŀ��ͼ���һ����(x, y),������Դͼ���еĶ�Ӧ���꣬���Ϊ������
    //        float y0 = y * fh;
    //        float x0 = x * fw;
    //        int y1 = int(y0);
    //        int y2 = y1 + 1;
    //        int x1 = int(x0);
    //        int x2 = x1 + 1;
    //        //�����Դͼ���е��ĸ�������Ϊ(x1, y1) (x1, y2) (x2, y1) (x2,y2)
    //        // (3)����Χ�ĸ�����ռ��Ȩ�ر�ֵ
    //        float fy1 = y0 - y1;
    //        float fy2 = 1.0f - fy1;
    //        float fx1 = x0 - x1;
    //        float fx2 = 1.0f - fx1;
    //        float s1 = fx1 * fy1;
    //        float s2 = fx2 * fy1;
    //        float s3 = fx2 * fy2;
    //        float s4 = fx1 * fy2;
    //        // (4)���ֵ����䵽Ŀ��ͼ���ϾͿ�����
    //        MY_MAT_ELEM(dst, y, x) = MY_MAT_ELEM(src, y2, x2) * s1 + MY_MAT_ELEM(src, y2, x1) * s2 + MY_MAT_ELEM(src, y1, x1) * s3 + MY_MAT_ELEM(src, y1, x2) * s4;
    //    }
    //}
    // ��������������Ż�
    int* arr_y1 = new int[h1];
    int* arr_y2 = new int[h1];
    double* arr_fy1 = new double[h1];
    double* arr_fy2 = new double[h1];
    for (int y = 0; y < h1; y++) {
        double y0 = y * fh;
        arr_y1[y] = int(y0);
        arr_y2[y] = arr_y1[y] + 1;
        arr_fy1[y] = y0 - arr_y1[y];
        arr_fy2[y] = 1.0f - arr_fy1[y];
    }

    int* arr_x1 = new int[w1];
    int* arr_x2 = new int[w1];
    double* arr_fx1 = new double[w1];
    double* arr_fx2 = new double[w1];
    for (int x = 0; x < w1; x++) {
        double x0 = x * fw;
        arr_x1[x] = int(x0);
        arr_x2[x] = arr_x1[x] + 1;
        arr_fx1[x] = x0 - arr_x1[x];
        arr_fx2[x] = 1.0f - arr_fx1[x];
    }
    for (int y = 0; y < h1; y++) {
        for (int x = 0; x < w1; x++) {
            double s1 = arr_fx1[x] * arr_fy1[y];
            double s2 = arr_fx2[x] * arr_fy1[y];
            double s3 = arr_fx2[x] * arr_fy2[y];
            double s4 = arr_fx1[x] * arr_fy2[y];
            MY_MAT_ELEM(dst, y, x) = (int)(MY_MAT_ELEM(src, arr_y2[y], arr_x2[x]) * s1 + MY_MAT_ELEM(src, arr_y2[y], arr_x1[x]) * s2 + MY_MAT_ELEM(src, arr_y1[y], arr_x1[x]) * s3 + MY_MAT_ELEM(src, arr_y1[y], arr_x2[x]) * s4);
        }
    }
    delete []arr_y1;
    delete []arr_y2;
    delete []arr_fy1;
    delete []arr_fy2;
    delete []arr_x1;
    delete []arr_x2;
    delete []arr_fx1;
    delete []arr_fx2;
}

void myRotate(const MyImage* src, MyImage* dst, double radian)
{
    assert(src != NULL && dst != NULL && src != dst);
    assert(src->rows >= 1 && src->cols >= 1);
    //int new_size = (int)sqrt((double)src->rows * src->rows + (double)src->cols * src->cols);
    int rowsNew = int(abs(src->rows * cos(radian)) + abs(src->cols * sin(radian)));
    int colsNew = int(abs(src->rows * sin(radian)) + abs(src->cols * cos(radian)));
    myReleaseImageData(dst);
    dst->type = src->type;
    dst->rows = rowsNew;
    dst->cols = colsNew;
    dst->size = dst->rows * dst->cols;
    dst->data = (byte_t*)calloc(dst->size, sizeof(byte_t));

    int offsetYSrc = src->rows / 2;
    int offsetXSrc = src->cols / 2;
    int offsetYDst = dst->rows / 2;
    int offsetXDst = dst->cols / 2;
    // ����
    //double r = angle * 3.141592 / 180.0;
    float c = (float)cos(radian);
    float s = (float)sin(radian);
    // Ŀ��Ҷ�
    int d;
    for (int yDst = -offsetYDst; yDst < offsetYDst; ++yDst) {
        for (int xDst = -offsetXDst; xDst < offsetXDst; ++xDst) {
            // dst(yDst, xDst) Ӧ�ö�Ӧ src(ySrc, xSrc)��Ȼ�� ySrc��xSrc �������ͨ��Ϊ������������� (ySrc, xSrc) ������4��������
            float ySrc = xDst * s + yDst * c;
            float xSrc = xDst * c - yDst * s;
            // (yInt, xInt) Ϊ���� (ySrc, xSrc) ��һ��������
            int yInt = (ySrc > 0 ? (int)ySrc : (int)(ySrc - 1));
            int xInt = (xSrc > 0 ? (int)xSrc : (int)(xSrc - 1));
            // q, p Ϊ (yInt, xInt) �� (ySrc, xSrc) �ľ���
            float q = ySrc - yInt;
            float p = xSrc - xInt;
            // (yInt, xInt) ������ src ��ȡֵ��Χ��
            //if ((m >= -ys) && (m < ys) && (n >= -xs) && (n < xs)) {
            //if (0 <= m + ys && m + ys + 1 < src->rows && 0 <= n + xs && n + 1 + xs < src->cols) {
            if (0 <= yInt + offsetYSrc && yInt + offsetYSrc + 1 < src->rows && 0 <= xInt + offsetXSrc && xInt + offsetXSrc + 1 < src->cols) {
                // 4 ����������ݸ����� (ySrc, xSrc) �ľ���������Բ�ֵ
                d = (int)((1.0 - q) * ((1.0 - p) * MY_MAT_ELEM(src, yInt + offsetYSrc, xInt + offsetXSrc) + p * MY_MAT_ELEM(src, yInt + offsetYSrc, xInt + 1 + offsetXSrc))
                    + q * ((1.0 - p) * MY_MAT_ELEM(src, yInt + 1 + offsetYSrc, xInt + offsetXSrc) + p * MY_MAT_ELEM(src, yInt + 1 + offsetYSrc, xInt + 1 + offsetXSrc)));
                // ��ֵ������ 0 - 255 ��Χ��
                if (d < 0) {
                    d = COLOR_BLACK;
                }
                if (d > 255) {
                    d = COLOR_WHITE;
                }
                MY_MAT_ELEM(dst, yDst + offsetYDst, xDst + offsetXDst) = (byte_t)d;
            }
            // (ySrc, xSrc) ���� src ȡֵ��Χ������ dst �õ�Ϊ��
        }
    }
}


void myThreshold(const MyImage* src, MyImage* dst, byte_t threshold)
{
    assert(src != NULL && dst != NULL && src != dst);
    assert(src->rows >= 1 && src->cols >= 1);
    myReleaseImageData(dst);
    dst->type = IMAGE_BIN;
    dst->rows = src->rows;
    dst->cols = src->cols;
    dst->size = dst->rows * dst->cols;
    dst->data = (byte_t*)calloc(dst->size, sizeof(byte_t));
    for (int y = 0; y < src->rows; ++y) {
        for (int x = 0; x < src->cols; ++x) {
            if (MY_MAT_ELEM(src, y, x) >= threshold) {
                MY_MAT_ELEM(dst, y, x) = COLOR_WHITE;
            }
            else {
                MY_MAT_ELEM(dst, y, x) = COLOR_BLACK;
            }
        }
    }
}

void myPart(const MyImage* src, const MyRect& rect, MyImage* dst)
{
    // ���룺һ���ǿ�ͼ�񣬺�һ���������򣨲������ǵ���ߣ�
    // ���������ͼ���һ���֣��������������ָ����ͼ����ͬ
    assert(src != NULL && dst != NULL);
    assert(myRectIsRect(rect));
    assert(src->rows >= (rect.y + rect.height) && src->cols >= (rect.x + rect.width));
    // ���������ָ��ͬһ��ͼ��
    if (src == dst) {
        MyImage* tmp = myCloneImage(src);
        myReleaseImageData(dst);
        dst->type = tmp->type;
        dst->rows = rect.height;
        dst->cols = rect.width;
        dst->size = rect.height * rect.width;
        dst->data = (byte_t*)calloc(dst->size, sizeof(byte_t));
        for (int y = 0; y < dst->rows; y++) {
            for (int x = 0; x < dst->cols; x++) {
                MY_MAT_ELEM(dst, y, x) = MY_MAT_ELEM(tmp, y + rect.y, x + rect.x);
            }
        }
        myReleaseImage(&tmp);
    }
    else {
        myReleaseImageData(dst);
        dst->type = src->type;
        dst->rows = rect.height;
        dst->cols = rect.width;
        dst->size = rect.height * rect.width;
        dst->data = (byte_t*)calloc(dst->size, sizeof(byte_t));
        for (int y = 0; y < dst->rows; y++) {
            for (int x = 0; x < dst->cols; x++) {
                MY_MAT_ELEM(dst, y, x) = MY_MAT_ELEM(src, y + rect.y, x + rect.x);
            }
        }
    }
}