#include "MyGui.h"

#include <assert.h>
#include <Windows.h>
#include <tchar.h>

const int MAX_NAME = 512;
bool my_wndclassIsRegistered = false;
WNDCLASS my_wndclass;
const TCHAR* wndclassName = TEXT("MyWndClass");
HINSTANCE my_hInstance = NULL;
// 窗体边长
const int sidelengthWindow = 640;


// 一个图像输出窗体链表
typedef struct My_Wnd
{
    int id;
    HWND hwnd;
    BYTE* pBits;
    BITMAPINFO* pInfo;
    struct My_Wnd* next;
} My_Wnd;
My_Wnd* my_wndsHead = NULL;
My_Wnd* my_wndsTail = NULL;
int my_countWnd = 0;

LRESULT CALLBACK my_WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    PAINTSTRUCT ps;
    RECT rect;
    My_Wnd* iter;

    switch (uMsg) {
    case WM_CREATE:
        my_countWnd++;
        break;
    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
        GetClientRect(hWnd, &rect);
        for (iter = my_wndsHead; iter != NULL; iter = iter->next) {
            // 判断是否有图像需要绘制
            if (iter->hwnd == hWnd && iter->pBits != NULL && iter->pInfo != NULL) {
                SetDIBitsToDevice(hdc, 0, 0, iter->pInfo->bmiHeader.biWidth, iter->pInfo->bmiHeader.biHeight,
                                    0, 0, 0, iter->pInfo->bmiHeader.biHeight,
                                    iter->pBits, iter->pInfo, DIB_RGB_COLORS);
                //// 宽大于长
                //if (iter->pInfo->bmiHeader.biWidth >= iter->pInfo->bmiHeader.biHeight) {
                //    int destHeight = sidelengthWindow * iter->pInfo->bmiHeader.biHeight / iter->pInfo->bmiHeader.biWidth;
                //    StretchDIBits(hdc, 0, 0, sidelengthWindow, destHeight,
                //                  0, 0, iter->pInfo->bmiHeader.biWidth, iter->pInfo->bmiHeader.biHeight,
                //                  iter->pBits, iter->pInfo, DIB_RGB_COLORS, SRCCOPY);
                //}
                //// 长大于宽
                //else {
                //    int destWidth = sidelengthWindow * iter->pInfo->bmiHeader.biWidth / iter->pInfo->bmiHeader.biHeight;
                //    StretchDIBits(hdc, 0, 0, destWidth, sidelengthWindow,
                //                  0, 0, iter->pInfo->bmiHeader.biWidth, iter->pInfo->bmiHeader.biHeight,
                //                  iter->pBits, iter->pInfo, DIB_RGB_COLORS, SRCCOPY);
                //}
                break;
            }
        }
        EndPaint(hWnd, &ps);
        break;
    case WM_DESTROY:
        my_countWnd--;
        // 当所有窗体关闭时，结束消息循环
        if (my_countWnd == 0) {
            PostQuitMessage(0);
        }
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return 0;
}

int myWaitKey(int delay)
{
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

int myCreateWindow(int wid, const char* name)
{
    assert(name != NULL);
    // 获得程序实例
    if (my_hInstance == NULL) {
        my_hInstance = GetModuleHandle(NULL);
    }
    // 注册窗体类
    if (!my_wndclassIsRegistered) {
        my_wndclass.style         = CS_HREDRAW | CS_VREDRAW;
        my_wndclass.lpfnWndProc   = my_WndProc;
        my_wndclass.cbClsExtra    = 0;
        my_wndclass.cbWndExtra    = 0;
        my_wndclass.hInstance     = my_hInstance;
        my_wndclass.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
        my_wndclass.hCursor       = LoadCursor(NULL, IDC_ARROW);
        my_wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        my_wndclass.lpszMenuName  = wndclassName;
        my_wndclass.lpszClassName = wndclassName;
        if (!RegisterClass(&my_wndclass)) {
            MessageBox(NULL, TEXT("This program requires Windows NT!"), wndclassName, MB_ICONERROR);
            return 0;
        }
        my_wndclassIsRegistered = true;
    }
    // 链表尾部插入
    if (my_wndsHead == NULL) {
        my_wndsHead = (My_Wnd*)calloc(1, sizeof(My_Wnd));
        my_wndsTail = my_wndsHead;
    }
    else {
        my_wndsTail->next = (My_Wnd*)calloc(1, sizeof(My_Wnd));
        my_wndsTail = my_wndsTail->next;
    }
    my_wndsTail->next = NULL;
    my_wndsTail->id = wid;
    // 将 char* 转为 TCHAR*
    TCHAR* tname = new TCHAR[MAX_NAME];
#ifdef UNICODE
    MultiByteToWideChar(CP_ACP, 0, name, -1, tname, MAX_NAME);
#else
    strcpy(tname, name);
#endif
    // 窗体句柄
    my_wndsTail->hwnd = CreateWindow(wndclassName, tname,
                         WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         sidelengthWindow, sidelengthWindow,
                         NULL, NULL, my_hInstance, NULL);
    if (!IsWindow (my_wndsTail->hwnd)) {
        return 0;
    }
    ShowWindow(my_wndsTail->hwnd, SW_SHOWDEFAULT);
    UpdateWindow(my_wndsTail->hwnd);
    // 图像像素阵列
    my_wndsTail->pBits = NULL;
    // 图像信息头
    my_wndsTail->pInfo = NULL;
    return 1;
}

void myShowImage(int wid, const MyImage* image)
{
    assert(image != NULL);
    assert(image->rows * image->cols >= 1);

    // 查找链表是否有 wid 该项
    My_Wnd* iter;
    bool wndsHasThisId = false;
    for (iter = my_wndsHead; iter != NULL; iter = iter->next) {
        if (iter->id == wid) {
            wndsHasThisId = true;
            // hwnd 不可能为 NULL
            assert(iter->hwnd != NULL);
            // 清除之前的图像像素阵列
            if (iter->pBits != NULL) {
                free(iter->pBits);
            }
            // 清除之前的图像信息头
            if (iter->pInfo != NULL) {
                free(iter->pInfo);
            }
            break;
        }
    }
    if (!wndsHasThisId) {
        return;
    }

    // 灰度图每个像素8位
    int bitCount = 8;
    // 像素每行字节数必须为 4 的倍数
    int rowLength = ((image->cols * bitCount + 31) & ~31) >> 3;
    // 图像像素阵列字节数
    int sizeImage = rowLength * image->rows;

    // 写图像信息头
    PBITMAPINFOHEADER pInfo = (PBITMAPINFOHEADER)calloc(1, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 256);
    pInfo->biSize = sizeof(BITMAPINFOHEADER);
    pInfo->biWidth = image->cols;
    pInfo->biHeight = image->rows;
    pInfo->biPlanes = 1;
    pInfo->biBitCount = bitCount;
    pInfo->biCompression = BI_RGB;
    pInfo->biSizeImage = sizeImage;
    pInfo->biXPelsPerMeter = 0;
    pInfo->biYPelsPerMeter = 0;
    pInfo->biClrUsed = 0;
    pInfo->biClrImportant = 0;
    // 写图像颜色表
    RGBQUAD* pColors = (RGBQUAD*)(pInfo + 1);
    for (int i = 0; i < 256; ++i) {
        pColors[i].rgbRed = pColors[i].rgbGreen = pColors[i].rgbBlue = i;
        pColors[i].rgbReserved = 0;
    }
    // 写图像像素阵列
    unsigned char* pBits = (unsigned char*)calloc(sizeImage, sizeof(unsigned char));
    for (int y = 0; y < image->rows; ++y) {
        for (int x = 0; x < image->cols; ++x) {
            *(pBits + rowLength * y + x) = MY_MAT_ELEM(image, image->rows - y - 1, x);
        }
    }

    // 绑定
    iter->pBits = pBits;
    iter->pInfo = (PBITMAPINFO)pInfo;
    // 强制重画
    RedrawWindow(iter->hwnd, NULL, NULL, RDW_INVALIDATE);
}


void myDestroyWindow(int wid)
{
    My_Wnd* iter;
    My_Wnd* prevBackup = NULL;
    for (iter = my_wndsHead; iter != NULL; iter = iter->next) {
        if (iter->id == wid) {
            if (prevBackup != NULL) {
                prevBackup->next = iter->next;
            }
            // 窗体句柄，在 WM_CLOSE 时已删除
            iter->hwnd = NULL;
            // 图像像素阵列
            if (iter->pBits != NULL) {
                free(iter->pBits);
            }
            // 图像信息头
            if (iter->pInfo != NULL) {
                free(iter->pInfo);
            }
            iter->next = NULL;
            free(iter);
            if (iter == my_wndsHead) {
                my_wndsHead = NULL;
            }
            if (iter == my_wndsTail) {
                my_wndsTail = NULL;
            }
            break;
        }
        
    }
}