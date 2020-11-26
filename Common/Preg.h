#pragma once

#define MAX_ARRAY_NUM 20
#define MAX_VALUE_SIZE 1024

class Preg
{
public:
	/* dstStr=字符串指针 */
	Preg(const char* dstStr);
	/* dstStr=字符串指针 */
	void SetString(const char* dstStr);
	/* 
	匹配[默认第一个]
	beginStr=匹配的开始字符
	endStr=匹配的结束字符
	include=是否包含beginStr和endStr
	返回结果指针
	*/
	char*  Match(const char* beginStr, const char* endStr, bool include=true);
	/*
	匹配所有
	beginStr=匹配的开始字符
	endStr=匹配的结束字符
	length=数组长度
	返回结果数组指针
	*/
	char** MatchAll(const char* beginStr, const char* endStr, int& length, bool include=true);
	/* 获取已匹配的数量 */
	int GetMatchCount() { return m_iMatchCount;  }
	/* 分配m_iValue */
	char** AllocMValue(int arrayNum, int size, bool clear=true);
	/* 清除m_iValue */
	void Clear();
protected:
	/* 存储结果的指针 */
	char** m_sValue;
	/* 多少个数组 */
	int    m_iArrayNum;
	/* 每个值多大 */
	int    m_iValueSize;
	/* 源字符串指针 */
	const char* m_sDstStr;
	/* 匹配数量 */
	int m_iMatchCount;
	/* 获取匹配数量 */
	int GetMacthAllCount(const char* beginStr, const char* endStr);
	/* 匹配 */
	char*  Match(const char* dstStr, const char* beginStr, const char* endStr, bool include=true);
};