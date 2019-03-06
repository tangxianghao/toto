#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include<stdio.h>  
#include<string.h>  

static void CharToBit(const char input[], int output[], int bits);
static void BitToChar(const int intput[], char output[], int bits);
static void Xor(int *INA, int *INB, int len);
static  void IP(const int input[64], int output[64], int table[64]);
static  void E(const int input[32], int output[48], int table[48]);
static  void P(const int input[32], int output[32], int table[32]);
static  void IP_In(const int input[64], int output[64], int table[64]);
static  void PC_1(const int input[64], int output[56], int table[56]);
static  void PC_2(const int input[56], int output[48], int table[48]);
static  void S(const int input[48], int output[32], int table[8][4][16]);
static void F_func(int input[32], int output[32], int subkey[48]);
static void RotateL(const int input[28], int output[28], int leftCount);
static void  subKey_fun(const int input[64], int Subkey[16][48]);
void  DES_Efun(char* input, int in_length, char* key_in, char* output);
void  DES_Dfun(int input[64], char key_in[8], char output[8]);