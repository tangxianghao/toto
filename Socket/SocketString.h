#pragma once

#define _CRT_SECURE_NO_WARNINGS

class SocketString
{
private:
	struct {
		char* value;
		int   length;
	} m_SendString;
	char m_RecvString[1024];
	char m_Value[128];
public:
	SocketString();
	~SocketString();
	SocketString& SetKeyValue(const char* key, int value);
	SocketString& SetKeyValue(const char* key, float value);
	SocketString& SetKeyValue(const char* key, const char* value);
	char* MakeSendString();
	char* GetSendString() { return m_SendString.value; }
	int   GetSendLength() { return m_SendString.length; }
	void  SetRecvString(const char* string);
	int   GetRecvValueInt(const char* key, int default_value=0);
	char* GetRecvValue(const char* key);
	void ClearSendString() {
		m_SendString.value[0] = 0;
		m_SendString.length = 0;
	}
};