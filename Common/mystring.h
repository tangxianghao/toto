#pragma once

// �Ƚ������ַ����Ƿ����, len=�Ƚϵĳ���
bool strneq(char* str_1, char* str_2, unsigned int len);
// ȥ�������ַ�
char* trim(char* string, char ch = 0);
// ȥ������ַ�
char* ltrim(char* string, char ch = 0);
// ȥ���ұ��ַ�
char* rtrim(char* string, char ch=0);
// �����ַ���
char* copy_str(char* start, char* end);
// �����ַ���need��text���ֵ�λ��[0��ʼ], start=��ʼ����λ��, end=��������λ��
int strpos(char* text, char* need, unsigned int start = 0, unsigned int end = 0);
/*
  ��ȡ�ַ���
  text=Ҫ��ȡ���ַ���
  start=��ʼλ��(������ĩβ��ʼ��ȡ)
  end=��ȡ����(0Ϊ�ַ�������, ������ĩβ��ʼ����)
  length=�ַ�������(Ĭ���Զ�����)
*/
char* substr(char* text, int start, int end, int length=-1);
/*
* ����������ת��utf8�ַ���
* dest �ַ��������ַ
* num ����[����]
* len ���鳤��
*/
char* int2utf8(char* dest, int* num, int len);