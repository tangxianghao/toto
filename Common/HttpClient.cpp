#include "HttpClient.h"

HTTP_STATUS HttpClient::Request(WCHAR* host, WCHAR* path, CString& result, int type)
{
	result.Empty();

	HTTP_STATUS status = 0;
	CInternetSession session(_T("mm160"));
	session.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, 15000);
	try {
		//MessageBox(NULL, m_SendParam, L"���͵Ĳ���", MB_OK);
		CHttpConnection* pServer = session.GetHttpConnection(host, (INTERNET_PORT)HOST_PORT);
		CHttpFile* pFile = pServer->OpenRequest(type, path, NULL, 1, NULL, NULL, INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_COOKIES);
		pFile->AddRequestHeaders(L"Content-Type: application/x-www-form-urlencoded");
		if (!m_Cookie.IsEmpty()) {
			CString cookie = L"Cookie: JSESSIONID=";
			cookie += m_Cookie;
			pFile->AddRequestHeaders(cookie);
		}
		
		USES_CONVERSION;
		char* param = T2A(m_SendParam.GetBuffer(0));
		// ��ʼ��������
		pFile->SendRequest(NULL, 0, param, strlen(param));
		pFile->QueryInfoStatusCode(status);
		printf("HTTP CODE:%d\n", status);
		if (status == HTTP_STATUS_OK) {
			CString newline;
			while (pFile->ReadString(newline)) { // ѭ����ȡÿ������ 
				result += newline + "\r\n";
			}
			printf("����:%s\n", result.GetBuffer()); // ��ʾ�������� 
		}

		if (pFile)
			delete pFile;
		if (pServer)
			delete pServer;
	}
	catch (CInternetException* pEx) {
		printf("����:������δ��Ӧ������\n"); // ��ʾ�쳣��Ϣ 
		status = 0;
	}
	session.Close();
	return status;
}

void HttpClient::SetCookie(char * cookie)
{
	m_Cookie = cookie;
}

void HttpClient::SetCookie(wchar_t * cookie)
{
	m_Cookie = cookie;
}

void HttpClient::AddParam(WCHAR* key, CHAR* value)
{
	m_SendParam += key;
	m_SendParam += L"=";
	m_SendParam += value;
	m_SendParam += L"&";
}

void HttpClient::AddParam(WCHAR* key, WCHAR* value)
{
	m_SendParam += key;
	m_SendParam += L"=";
	m_SendParam += value;
	m_SendParam += L"&";
}

void HttpClient::AddParam(WCHAR* key, CString value)
{
	m_SendParam += key;
	m_SendParam += L"=";
	m_SendParam += value;
	m_SendParam += L"&";
}

void HttpClient::ResetParam()
{
	m_SendParam.Empty();
}
