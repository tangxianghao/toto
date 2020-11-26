#include "Preg.h"
#include "mystring.h"
#include <string.h>
#include <stdio.h>


/* dstStr=×Ö·û´®Ö¸Õë */
Preg::Preg(const char * dstStr)
{
	//AllocMValue(MAX_ARRAY_NUM, MAX_VALUE_SIZE, false);
	SetString(dstStr);
}

void Preg::SetString(const char* dstStr)
{
	m_sDstStr = dstStr;
	m_iMatchCount = 0;
}

char* Preg::Match(const char* beginStr, const char* endStr, bool include)
{
	const char* begin = strstr(m_sDstStr, beginStr);
	if (!begin)
		return nullptr;

	const char* end = strstr(begin + strlen(beginStr), endStr);
	if (!end)
		return nullptr;

	m_iMatchCount = 1;
	if (include) {
		return copy_str((char*)begin, (char*)end + strlen(endStr));
	}
	else {
		return copy_str((char*)begin + strlen(beginStr), (char*)end);
	}
}

char** Preg::MatchAll(const char* beginStr, const char* endStr, int& length, bool include)
{
	length = GetMacthAllCount(beginStr, endStr);
	if (length == 0)
		return nullptr;

	char** result = new char*[length];
	int num = 0;
	int beginStrLen = strlen(beginStr);
	int endStrLen = strlen(endStr);
	const char* str = m_sDstStr;
	while (num <= length) {
		const char* begin = strstr(str, beginStr);
		if (!begin) {
			length = num;
			m_iMatchCount = num;
			break;
		}

		const char* end = strstr(begin + beginStrLen, endStr);
		if (!end) {
			length = num;
			m_iMatchCount = num;
			break;
		}

		if (include) {
			result[num] = copy_str((char*)begin, (char*)end + endStrLen);
		}
		else {
			result[num] = copy_str((char*)begin + beginStrLen, (char*)end);
		}

		str = end + endStrLen;
		num++;
	}
	return result;
}

char** Preg::AllocMValue(int arrayNum, int size, bool clear)
{
	if (clear)
		Clear();

	m_iArrayNum = arrayNum;
	m_iValueSize = size;
	m_sValue = new char*[m_iArrayNum];
	for (int i = 0; i < m_iArrayNum; i++) {
		m_sValue[i] = new char[m_iValueSize];
	}
	return m_sValue;
}

/* Çå³ým_iValue */
void Preg::Clear()
{
	for (int i = 0; i < m_iArrayNum; i++) {
		delete[] m_sValue[i];
	}
	delete[] m_sValue;
}

int Preg::GetMacthAllCount(const char * beginStr, const char * endStr)
{
	m_iMatchCount = 0;
	int endStrLen = strlen(endStr);
	const char* str = m_sDstStr;
	while (true) {
		const char* begin = strstr(str, beginStr);
		const char* end = strstr(str, endStr);
		if (!begin || !end) {
			break;
		}

		m_iMatchCount++;
		str = end + endStrLen;
	}
	return m_iMatchCount;
}

char* Preg::Match(const char* dstStr, const char * beginStr, const char* endStr, bool include)
{
	const char* begin = strstr(dstStr, beginStr);
	const char* end = strstr(dstStr, endStr);
	if (!begin || !end) {
		return nullptr;
	}

	if (include) {
		return copy_str((char*)begin, (char*)end + strlen(endStr));
	}
	else {
		return copy_str((char*)begin + strlen(begin), (char*)end);
	}
}
