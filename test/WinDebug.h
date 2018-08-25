#ifndef __DEBUG_H
#define __DEBUG_H

#include <windows.h>

void initDebug(HANDLE);

void int2hex(unsigned int, char*, int);
void int2str(int, char*);
void dump0(const void*, char*);

void print(const char*);
void printx(int);
void printi(int);
void printd(double);
void printDump(void*, int);
void nl();
void space();

#endif //__DEBUG_H