#pragma once
#include <stdint.h> 
#include<functional>

#ifdef _WIN32
#define evutil_socket_t intptr_t
#else
#define evutil_socket_t int
#endif

class SocketServer
{
public:
	SocketServer();
	~SocketServer();
	enum Type
	{
		TCP = 1,
		UDP = 2
	};
	int StartServer(const char* port, int ConnectType = 1);//2 udp
	int StopServer();
	int SetCallBack(std::function<void(unsigned char* data, int length, int SockeCount)> callback);
	//Ĭ��-1 Ϊ��ȫ���ͻ��˹㲥
	int SendData(const char* data, int length, unsigned __int64 socket = -1);
;

	bool IsConnect();

private:
	char RecData[65535];
	struct addrinfo* info;
	class EventClass* eventClass;
	struct event* EventPtr;
	unsigned __int64 CurrentSocket = -1;//��ǰ����
	std::function<void(unsigned char* data, int length, int NumSock)> callBack = 0;  //�ص�����

protected:
	static void AcceptCall(evutil_socket_t socke, short events, void* classPtr);
	static void Event_Callback(evutil_socket_t socke, short events, void* classPtr);
};

