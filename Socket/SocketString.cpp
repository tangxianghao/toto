#include "SocketString.h"
#include <string.h>
#include <stdio.h>

#define MAX_LENGTH 1024

SocketString::SocketString()
{
	m_SendString.value = new char[MAX_LENGTH];
	ClearSendString();
}

SocketString::~SocketString()
{
	delete[] m_SendString.value;
	m_SendString.value = nullptr;
}

SocketString& SocketString::SetKeyValue(const char* key, int value)
{
	char string[16];
	sprintf_s(string, "%d", value);
	return SetKeyValue(key, string);
}

SocketString& SocketString::SetKeyValue(const char * key, float value)
{
	char string[16];
	sprintf_s(string, "%.2f", value);
	return SetKeyValue(key, string);
}

SocketString& SocketString::SetKeyValue(const char* key, const char* value)
{
	strcat(m_SendString.value, key);
	strcat(m_SendString.value, ":");
	strcat(m_SendString.value, value);
	strcat(m_SendString.value, " | ");
	m_SendString.length = strlen(m_SendString.value);

	return *this;
}

char* SocketString::MakeSendString()
{
	m_SendString.length += 5;
	for (int i = m_SendString.length; i >= 0; i--) {
		m_SendString.value[i] = m_SendString.value[i - 5];
	}
	
	char string[5];
	sprintf_s(string, "%04d", m_SendString.length);
	memcpy(m_SendString.value, string, 4);
	m_SendString.value[4] = ' ';
	return m_SendString.value;
}

void SocketString::SetRecvString(const char* string)
{
	strcpy(m_RecvString, string);
}

int SocketString::GetRecvValueInt(const char * key, int default_value)
{
	const char* ptr = strstr(m_RecvString, key);
	if (!ptr) {
		return default_value;
	}

	int value = 0;
	ptr += strlen(key);
	for (int i = 0; true; i++) {
		if (ptr[i] < '0' || ptr[i] > '9') {
			break;
		}
		value = value * 10 + (ptr[i] - '0');
	}

	return value;
}

char* SocketString::GetRecvValue(const char* key)
{
	const char* ptr = strstr(m_RecvString, key);
	if (!ptr) {
		return nullptr;
	}
	ptr += strlen(key);
	//printf("%s\n", ptr);
	int i = 0;
	for (; true; i++) {
		if (ptr[i] == 0 || i > 100)
			break;

		if (ptr[i] == ' ' && ptr[i + 1] == '|' && ptr[i + 2] == ' ') {
			break;
		}
		//printf("i:%d\n", i);
		m_Value[i] = ptr[i];
	}
	m_Value[i] = 0;

	return m_Value;
}


