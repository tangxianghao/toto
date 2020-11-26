#pragma once

#define MAX_ARRAY_NUM 20
#define MAX_VALUE_SIZE 1024

class Preg
{
public:
	/* dstStr=�ַ���ָ�� */
	Preg(const char* dstStr);
	/* dstStr=�ַ���ָ�� */
	void SetString(const char* dstStr);
	/* 
	ƥ��[Ĭ�ϵ�һ��]
	beginStr=ƥ��Ŀ�ʼ�ַ�
	endStr=ƥ��Ľ����ַ�
	include=�Ƿ����beginStr��endStr
	���ؽ��ָ��
	*/
	char*  Match(const char* beginStr, const char* endStr, bool include=true);
	/*
	ƥ������
	beginStr=ƥ��Ŀ�ʼ�ַ�
	endStr=ƥ��Ľ����ַ�
	length=���鳤��
	���ؽ������ָ��
	*/
	char** MatchAll(const char* beginStr, const char* endStr, int& length, bool include=true);
	/* ��ȡ��ƥ������� */
	int GetMatchCount() { return m_iMatchCount;  }
	/* ����m_iValue */
	char** AllocMValue(int arrayNum, int size, bool clear=true);
	/* ���m_iValue */
	void Clear();
protected:
	/* �洢�����ָ�� */
	char** m_sValue;
	/* ���ٸ����� */
	int    m_iArrayNum;
	/* ÿ��ֵ��� */
	int    m_iValueSize;
	/* Դ�ַ���ָ�� */
	const char* m_sDstStr;
	/* ƥ������ */
	int m_iMatchCount;
	/* ��ȡƥ������ */
	int GetMacthAllCount(const char* beginStr, const char* endStr);
	/* ƥ�� */
	char*  Match(const char* dstStr, const char* beginStr, const char* endStr, bool include=true);
};