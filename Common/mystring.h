#pragma once

// 比较两个字符串是否相等, len=比较的长度
bool strneq(char* str_1, char* str_2, unsigned int len);
// 去除两边字符
char* trim(char* string, char ch = 0);
// 去除左边字符
char* ltrim(char* string, char ch = 0);
// 去除右边字符
char* rtrim(char* string, char ch=0);
// 复制字符串
char* copy_str(char* start, char* end);
// 查找字符串need在text出现的位置[0起始], start=开始搜索位置, end=结束搜索位置
int strpos(char* text, char* need, unsigned int start = 0, unsigned int end = 0);
/*
  截取字符串
  text=要截取的字符串
  start=开始位置(负数从末尾开始截取)
  end=截取长度(0为字符串长度, 负数从末尾开始计算)
  length=字符串长度(默认自动计算)
*/
char* substr(char* text, int start, int end, int length=-1);
/*
* 把整型数字转成utf8字符串
* dest 字符串保存地址
* num 数字[数组]
* len 数组长度
*/
char* int2utf8(char* dest, int* num, int len);