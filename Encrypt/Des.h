#define _CRT_SECURE_NO_WARNINGS

#include <memory.h>
#include <stdio.h>
#include <conio.h>

// DES����
int  DesEncrypt(char* out, char* key, char* text, int text_length);
int  DesDecrypt(char* out, char* key, char* text, int text_length, bool hex = false);
static void Des_Run(char Out[16], char In[8], int InLength, bool decode=false);//���ļ����㷨
static void SetKey(const char Key[8]);// ������Կ
static void F_func(bool In[32], const bool Ki[48]);// f ����
static void S_func(bool Out[32], const bool In[48]);// S �д���
static void Transform(bool *Out, bool *In, const char *Table, int len);// �任
static void Xor(bool *InA, const bool *InB, int len);// ���
static void RotateL(bool *In, int len, int loop);// ѭ������
static void ByteToBit(bool *Out, const char *In, int bits);// �ֽ���ת����λ��
static void ShowInHex(const bool In[64]);     //��16�������
static char Hex2Char(char ch, char ch2);
static char Char2Num(char ch);
