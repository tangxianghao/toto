#include "mystring.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//�Ƚ������ַ����Ƿ����, len=�Ƚϵĳ���
bool strneq(char* str_1, char* str_2, unsigned int len) { 
	for (unsigned int i = 0; i < len; i++) {
		if (str_1[i] != str_2[i]) {
			return false;
		}
	}
	return true;
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