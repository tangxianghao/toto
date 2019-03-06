#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

typedef void(*onconnect_error_func)(const char* host, USHORT port);
typedef void(*onconnect_func)(const char* host, USHORT port, SOCKET socket);
typedef void(*onread_func)(const char* host, USHORT port, const char* data, int length, const char* wist);
typedef void(*onclose_func)(const char* host, USHORT port);
typedef void(*onerror_func)(const char* host, USHORT port);
typedef void(*ontime_out_func)(const char* host, USHORT port, int sec);

class Client
{
protected:
	bool m_Close = false;
public:
	/* ��ʱʱ�� */
	int m_iTimeOut = 0;

	onconnect_error_func onconnect_error;
	onconnect_func onconect;
	onread_func onread;
	onclose_func onclose;
	onerror_func onerror;
	ontime_out_func ontimeout;
public:
	Client():
		onconnect_error(nullptr),
		onconect(nullptr),
		onread(nullptr),
		onclose(nullptr),
		onerror(nullptr),
		ontimeout(nullptr)
	{}
	void Connect(const char* host, USHORT port);
	void Close(bool var);
	int  Intel2Length(const char* data);
};