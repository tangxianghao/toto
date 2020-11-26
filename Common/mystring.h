#pragma once

// �Ƚ������ַ����Ƿ����, len=�Ƚϵĳ���
bool strneq(char* str_1, char* str_2, unsigned int len);
// ʮ������ת������
int hextoi32(char* data);
// ȥ�������ַ�
char* trim(char* string, char ch = 0);
// ȥ������ַ�
char* ltrim(char* string, char ch = 0);
// ȥ���ұ��ַ�
char* rtrim(char* string, char ch=0);
// �滻�ַ�
char* replace(char* string, char find=0, char rep=0);
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

/*
�����ֶ�ת������
*/
int ctoi32(const char* data);

/*
�����ֶ�ת������
*/
__int64 ctoi64(const char* data);

/*
 ����int32���ڴ�
*/
char* copy_i32(char* dst, int v);

/*
 ����int32��ת�������ֽڣ����ڴ�
*/
char* copy_i32_net(char* dst, int v);

/*
 ����int64���ڴ�
*/
char* copy_i64(char* dst, __int64 v);

/*
 ����int64��ת�������ֽڣ����ڴ�
*/
char* copy_i64_net(char* dst, __int64 v);

/*
utf8ת��gb2312
*/
char* Utf82GB2312(const char* utf8);

/*
��ȡʱ��
*/
int gettimeofday(struct timeval* tv);

/*
��ȡ����
 */
__int64 getmillisecond();

/* ��ȡ���� */
int getday(int* hour);

unsigned int getMorningTime();