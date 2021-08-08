#include "SocketClient.h"
#include <event.h>
#include "EventClass.h"
//#include <process.h>

SocketClient::SocketClient()
{
	eventClass = new EventClass();
}

SocketClient::~SocketClient()
{
	delete eventClass;
}

int SocketClient::ClientConnect(const char* IPHost, const char* Port, Type ConnectType)
{
	//创建地址信息
	struct addrinfo hints;
	//获取连接地址
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	//地址类型未指定 可以是IPV6/ AF_INET IPv4/ AF_INET6 
	hints.ai_flags = AI_PASSIVE; //匹配所有 IP 地址
	hints.ai_protocol = IPPROTO_IP;  //匹配所有协议
	hints.ai_socktype = ConnectType; //流类型 //TCP  SOCK_STREAM  1// UDP  SOCK_DGRAM 2
	//获取 ipv4 ipv6 域名 连接信息	
	int res = getaddrinfo(IPHost, Port, &hints, &(info));

	CurrentSocket = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

	const char chOpt = 1;
	int   nErr = setsockopt(CurrentSocket, IPPROTO_TCP, TCP_NODELAY, &chOpt, sizeof(char));
	if (nErr == -1)
	{
		//TRACE(_T("setsockopt()   error\n"), WSAGetLastError());
	}
	connect(CurrentSocket, info->ai_addr, info->ai_addrlen);

	EventPtr = event_new(EventClass::base, CurrentSocket, EV_READ | EV_PERSIST, ClientCallback, this);
	//int old_option = fcntl(CurrentSocket, F_GETFL);
    return 0;
}

int SocketClient::SetCallBack(std::function<void(unsigned char* data, int length, int SockeCount)> callback)
{
	callBack = callback;
	return 0;
}

int SocketClient::SendData(const char* data, size_t length)
{
	send(CurrentSocket, data, length, 0);
	return 0;
}

int SocketClient::DissConnect()
{

	return 0;
}

void SocketClient::ClientCallback(evutil_socket_t socke, short events, void* classPtr)
{
	SocketClient* SC = (SocketClient*)classPtr;


	int len = recv(socke, SC->RecData, 4096, 0);

	if (len <= 0)
	{
		//SC->FreeEvent();
		//closesocket(socke);
		//if (SC->ClientData->callBack != NULL)
		//{
		//	SC->ClientData->callBack((unsigned char*)RecData, 0, socke);
		//}
		//SC->ClientData->CurrentSocket = -1;
		return;
	}
	else
	{
		if (SC->callBack != NULL)
		{
			SC->callBack((unsigned char*)(SC->RecData), len, socke);
		}
	}
}




