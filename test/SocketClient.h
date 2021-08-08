#pragma once
#include <stdint.h> 
#include<functional>

#ifdef _WIN32
#define evutil_socket_t intptr_t
#else
#define evutil_socket_t int
#endif

class SocketClient
{
public:
	SocketClient();
	~SocketClient();
	enum Type
	{
		TCP=1,
		UDP=2
	};
	int ClientConnect(const char* IPHost, const char* Port, Type ConnectType = TCP);
	int SetCallBack(std::function<void(unsigned char* data, int length, int SockeCount)> callback);
	int SendData(const char* data, size_t length);
	int DissConnect();
private:

	char RecData[65535];
	class EventClass* eventClass;
	struct event* EventPtr = 0;
	struct addrinfo* info;
	unsigned __int64 CurrentSocket = -1;//当前连接
	std::function<void(unsigned char* data, int length, int NumSock)> callBack = 0;  //回调数据
	static void ClientCallback(evutil_socket_t socke, short events, void* classPtr);
};

