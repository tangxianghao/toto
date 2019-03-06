#include "Client.h"
#include <stdio.h>
#include <time.h>

void Client::Connect(const char* host, USHORT port)
{
	WSADATA     wsaData;
	SOCKET      socket_client;
	SOCKADDR_IN server_in;
	int         iaddrSize = sizeof(SOCKADDR_IN);
	DWORD       dwThreadId;

	WSAStartup(0x0202, &wsaData);

	socket_client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	server_in.sin_addr.S_un.S_addr = inet_addr(host);
	server_in.sin_family = AF_INET;
	server_in.sin_port = htons(port);
	if (connect(socket_client, (struct sockaddr *)&server_in, sizeof(server_in)) == SOCKET_ERROR) {
		this->onconnect_error(host, port);
		return;
	}

	this->onconect(host, port, socket_client);

	time_t last_time = time(NULL);
	char  wist[4];
	char* recv_data = new char[1024 * 4];
	while (!m_Close) {
		if (m_iTimeOut > 0) {
			time_t now_time = time(NULL);
			if ((now_time - last_time) >= m_iTimeOut) {
				if (ontimeout) {
					ontimeout(host, port, now_time - last_time);
				}
				break;
			}
		}
		int ret = recv(socket_client, recv_data, 12, 0);
		if (ret > 0) {
			if (recv_data[0] == 'W' && recv_data[1] == 'I' &&recv_data[2] == 'S' &&recv_data[3] == 'T') {
				memcpy(wist, &recv_data[4], 4);
				int length = Intel2Length(&recv_data[8]);
				printf("%c%c%c%c %08x\n", recv_data[0], recv_data[1], recv_data[2], recv_data[3], length);
				if (length >= 0) {
					int ret = recv(socket_client, recv_data, length, 0);
					if (ret > 0) {
						this->onread(host, port, recv_data, length, wist);
					}
					else {
						goto error;
					}
				}
			}
			else {
				recv(socket_client, recv_data, 1024, 0);
			}
		}
	error:
		if (ret == 0) {
			//��ret == 0 ˵�����������ߡ�
			break;
		}
		else if (ret < 0) {
			//��ret < 0 ˵���������쳣 ��������״̬��������߶�ȡ����ʱ����ָ�����ȡ�
			//������������Ҫ�����Ͽ��Ϳͻ��˵����ӡ�
			this->onerror(host, port);
			closesocket(socket_client);
			break;
		}
	}
	delete recv_data;
	closesocket(socket_client);
	WSACleanup();

	this->onclose(host, port);
}

void Client::Close(bool var)
{
	m_Close = var;
}

int Client::Intel2Length(const char* data)
{
	int length = 0;
	length |= (data[0] << 24) & 0xff000000;
	length |= (data[1] << 16) & 0x00ff0000;
	length |= (data[2] << 8)  & 0x0000ff00;
	length |= (data[3] << 0)  & 0x000000ff;
	return length;
}
