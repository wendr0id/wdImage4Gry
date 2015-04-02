#ifndef MY_GUI_H
#define MY_GUI_H


#include "MyImage.h"

int myCreateWindow(int wid, const char* name);
void myShowImage(int wid, const MyImage* image);
int myWaitKey(int delay);
void myDestroyWindow(int wid);

#endif //MY_GUI_H