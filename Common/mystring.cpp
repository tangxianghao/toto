#include "mystring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

//�Ƚ������ַ����Ƿ����, len=�Ƚϵĳ���
bool strneq(char* str_1, char* str_2, unsigned int len) { 
	for (unsigned int i = 0; i < len; i++) {
		if (str_1[i] != str_2[i]) {
			return false;
		}
	}
	return true;
}

// ʮ������ת������
int hextoi32(char* data)
{
	if (data == nullptr)
		return 0;

	if (data[0] == '0') {
		if (data[1] == 'x' || data[1] == 'X')
			data += 2;
	}
	//printf("%s\n", data);
	int value = 0;
	while (*data) {
		char ch = *data;
		//printf("%c ", ch);
		if (ch >= '0' && ch <= '9') {
			ch = ch - '0';
		}
		else if (ch >= 'A' && ch <= 'F') {
			ch = ch - 'A' + 0x0A;
		}
		else if (ch >= 'a' && ch <= 'f') {
			ch = ch - 'a' + 0x0a;
		}
		else {
			ch = 0;
		}
	   // printf("%d\n", ch);
		value = value * 0x10 + ch;
		data++;
	}
	return value;
}

char* trim(char* string, char ch)
{
	ltrim(string, ch);
	rtrim(string, ch);
	return string;
}

char* ltrim(char* string, char ch)
{
	int left = 0;
	char* tmp = string;
	while (*tmp) {
		bool find = false;
		if (ch > 0) {
			find = ch == *tmp;
		}
		else {
			find = ' ' == *tmp || '\r' == *tmp || '\n' == *tmp || '\t' == *tmp;
		}
		if (!find) {
			break;
		}

		left++;
		tmp++;
	}
	if (left > 0) {
		int i;
		tmp = string + left;
		for (i = 0; *tmp; i++, tmp++) {
			string[i] = *tmp;
		}
		string[i] = 0;
	}
	return string;
}

char* rtrim(char* string, char ch)
{
	int length = strlen(string);
	for (int i = length - 1; i >= 0; i--) {
		bool find = false;
		if (ch > 0) {
			find = ch == string[i];
		}
		else {
			find = ' ' == string[i] || '\r' == string[i] || '\n' == string[i] || '\t' == string[i];
		}
		if (!find) {
			break;
		}
		string[i] = 0;
	}
	return string;
}

// �滻�ַ�
char* replace(char* string, char find, char rep)
{
	if (rep) {
		for (char* p = string; *p; p++) {
			bool eq = false;
			if (find > 0) {
				eq = find == *p;
			}
			else {
				eq = '\r' == *p || '\n' == *p || '\t' == *p;
			}
			if (eq) {
				*p = rep;
			}
		}
	}
	else {
		for (char* p = string; *p; p++) {

			bool eq = false;
			if (find > 0) {
				eq = find == *p;
			}
			else {
				eq = '\r' == *p || '\n' == *p || '\t' == *p;
			}
			if (eq) {
				int i = 0;
				char* p2 = p + 1;
				for (; p2[i]; i++) {
					p[i] = p2[i];
				}
				p[i] = p2[i];

				p--;
			}
		}
	}
	return string;
}

char* copy_str(char* start, char* end) {
	int len = end - start;
	if (len <= 0) {
		return NULL;
	}
	char* str = (char*)malloc(len + 1);
	char* s = start;
	char* tmp = str;
	for (; s < end; s++, tmp++) {
		if (*s == '\\') {
			if (s + 1 < end) {
				*tmp = *++s;
			}
		}
		else {
			*tmp = *s;
		}
	}
	*tmp = 0;

	return str;
}


int strpos(char* text, char* need, unsigned int start, unsigned int end) { //�����ַ���need��text���ֵ�λ��[0��ʼ], start=��ʼ����λ��, end=��������λ��
	if (end > 0) {
		end += (unsigned int)text + 1;
	}
	int need_len = strlen(need);
	bool find = false;
	char* ptr = text + start;
	while (*ptr) {
		if (end > 0 && ((unsigned int)ptr + need_len) > end) {
			break;
		}
		if (strneq(ptr, need, need_len)) {
			find = true;
			break;
		}
		ptr++;
	}
	return find ? ptr - text : -1;
}

char* substr(char* text, int start, int end, int length)
{
	length = length < 0 ? strlen(text) : length;
	int i = 0;
	if (start < 0) // ��ʼλ��Ϊ����
		start += length;
	if (end < 0)   // ����λ��Ϊ����
		end += length;
	if (end == 0)
		end = length; // ����λ��Ϊ0 Ĭ��Ϊ�ַ�������
	if (start < 0)
		start = 0;
	if (end < 0)
		end = 0;
	if (end > length)
		end = length;

	for (int i = start,j = 0; i < end; i++,j++) {
		text[j] = text[i];
	}
	int zero_pos = end - start;
	text[zero_pos < 0 ? 0 : zero_pos] = 0;
end:
	return text;
}

/*
* ����������ת��utf8�ַ���
* dest �ַ��������ַ
* num ����[����]
* len ���鳤��
*/
char* int2utf8(char* dest, int* num, int len) {
	int index = 0;
	for (int i = 0; i < len; i++) {
		if (num[i] < 128) {
			dest[index] = num[i] & 0xff;
			index++;
		}
		else {
			dest[index] = (num[i] >> 16) & 0xff;
			dest[index + 1] = (num[i] >> 8) & 0xff;
			dest[index + 2] = (num[i]) & 0xff;
			index += 3;
		}
	}
	dest[index] = 0;
	return dest;
}

/*
�����ֶ�ת������
*/
int ctoi32(const char * data)
{
	int v = 0;
	v |= (data[0] << 24) & 0xff000000;
	v |= (data[1] << 16) & 0x00ff0000;
	v |= (data[2] << 8) & 0x0000ff00;
	v |= (data[3] << 0) & 0x000000ff;
	return v;
}

/*
�����ֶ�ת������--��ѡ
data ����ָ��
start_sub ���鿪ʼ�±�
str_len ȡ���ٸ��ֽ�
*/
int ctoinum(const char* data, int start_sub, char str_len)
{
	int res_str = 0;
	int yidong = 0;
	for (int i = 0; i < str_len; i++) {
		yidong = (str_len - 1 - i) * 8;
		res_str = res_str | (data[start_sub + i] << yidong);
	}
	return res_str;
}

/*
�����ֶ�ת������
*/
__int64 ctoi64(const char* data)
{
	__int64 v = 0;
	for (int i = 0; i < 8; i++) {
		//printf("%02x ", data[i] & 0xff);
		v |= (data[i] & 0xff) << ((7 - i) * 8);
	}
	//printf("\n");
	//printf(" --> %lld��%d\n", v, 123);
	return v;
}

/*
 ����int32���ڴ�
*/
char* copy_i32(char * dst, int v)
{
	for (int i = 0; i < 4; i++) { 
		dst[i] = (v >> (i * 8)) & 0xff;
	}
	return dst;
}

/*
 ����int32��ת�������ֽڣ����ڴ�
*/
char* copy_i32_net(char * dst, int v)
{
	for (int i = 0; i < 4; i++) { 
		dst[i] = (v >> ((3 - i) * 8)) & 0xff;
	}
	return dst;
}

/*
 ����int64���ڴ�
*/
char* copy_i64(char * dst, __int64 v)
{
	for (int i = 0; i < 8; i++) {
		dst[i] = (v >> (i * 8)) & 0xff;
	}
	return dst;
}

/*
 ����int64��ת�������ֽڣ����ڴ�
*/
 char* copy_i64_net(char * dst, __int64 v)
 {
	 for (int i = 0; i < 8; i++) {
		 dst[i] = (v >> ((7 - i) * 8)) & 0xff;
	 }
	 return dst;
 }

 /*
utf8ת��gb2312
*/
 char* Utf82GB2312(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if (wstr) delete[] wstr;
	return str;
}

 /*
��ȡʱ��
*/
int gettimeofday(struct timeval* tv)
{
	FILETIME ft;
	unsigned __int64 tmpres = 0;
	static int tzflag;

	if (NULL != tv)
	{
		GetSystemTimeAsFileTime(&ft);

		tmpres |= ft.dwHighDateTime;
		tmpres <<= 32;
		tmpres |= ft.dwLowDateTime;
	#define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
		/*converting file time to unix epoch*/
		tmpres /= 10;  /*convert into microseconds*/
		tmpres -= DELTA_EPOCH_IN_MICROSECS;
		tv->tv_sec = (long)(tmpres / 1000000UL);
		tv->tv_usec = (long)(tmpres % 1000000UL);
	}

	return 0;
}

/*
��ȡ����
*/
__int64 getmillisecond()
{
	timeval t;
	gettimeofday(&t);
	return ((__int64)(t.tv_sec) * 1000) + (t.tv_usec / 1000);
}

/* ��ȡ���� */
int getday(int* hour)
{
	time_t now_time = time(nullptr);
	tm t;
	localtime_s(&t, &now_time);

	if (hour) {
		*hour = t.tm_hour;
	}

	return t.tm_mday;
}


unsigned int getMorningTime()
{
	time_t t = time(NULL);
	struct tm* tm = localtime(&t);
	tm->tm_hour = 0;
	tm->tm_min = 0;
	tm->tm_sec = 0;
	return  mktime(tm);
}


