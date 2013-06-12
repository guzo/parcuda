#ifndef _CALLBACKS_HPP_
#define _CALLBACKS_HPP_

#include "scene.hpp"
extern Scene scene;

void reshapeFunc(int w, int h);
void displayFunc();
void keyboardFunc  (unsigned char key, int /*x*/, int /*y*/);
void keyboardUpFunc(unsigned char key, int /*x*/, int /*y*/);
void specialFunc   (         int  key, int /*x*/, int /*y*/);
void specialUpFunc (         int  key, int /*x*/, int /*y*/);
void idleFunc();

#endif//_CALLBACKS_HPP_
