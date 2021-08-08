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
	//������ַ��Ϣ
	struct addrinfo hints;
	//��ȡ���ӵ�ַ
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;	//��ַ����δָ�� ������IPV6/ AF_INET IPv4/ AF_INET6 
	hints.ai_flags = AI_PASSIVE; //ƥ������ IP ��ַ
	hints.ai_protocol = IPPROTO_IP;  //ƥ������Э��
	hints.ai_socktype = ConnectType; //������ //TCP  SOCK_STREAM  1// UDP  SOCK_DGRAM 2
	//��ȡ ipv4 ipv6 ���� ������Ϣ	
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




