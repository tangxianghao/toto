#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>

class Game;

typedef void(*onconnect_error_func)(Game*, const char* host, USHORT port);
typedef void(*onconnect_func)(Game*, const char* host, USHORT port, SOCKET socket);
typedef void(*onread_func)(Game*, const char* host, USHORT port, const char* data, int length, const char* wist);
typedef void(*onclose_func)(Game*, const char* host, USHORT port);
typedef void(*onerror_func)(Game*, const char* host, USHORT port);
typedef void(*ontime_out_func)(Game*, const char* host, USHORT port, int sec);

class Client
{
public:
	bool m_Close = false;
public:
	/* ³¬Ê±Ê±¼ä */
	Game* m_pGame;
	int m_iTimeOut = 0;

	onconnect_error_func onconnect_error;
	onconnect_func onconect;
	onread_func onread;
	onclose_func onclose;
	onerror_func onerror;
	ontime_out_func ontimeout;
public:
	Client(Game* game):
		onconnect_error(nullptr),
		onconect(nullptr),
		onread(nullptr),
		onclose(nullptr),
		onerror(nullptr),
		ontimeout(nullptr)
	{
		m_pGame = game;
	}
	void Connect(const char* host, USHORT port);
	void Close(bool var);
	int  Intel2Length(const char* data);
};