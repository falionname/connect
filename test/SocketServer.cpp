#include "SocketServer.h"
#include <event.h>
#include "EventClass.h"

struct Ptr
{
	void* classptr;
	void* eventptr;
};

SocketServer::SocketServer()
{
	eventClass = new EventClass();
}

SocketServer::~SocketServer()
{
	delete eventClass;
}

int SocketServer::StartServer(const char* port, int ConnectType)
{
	//************************************************初始化连接地址********************************************************
	struct addrinfo hints;//创建地址信息
	ZeroMemory(&hints, sizeof(hints));//初始化清零
	hints.ai_family = AF_UNSPEC;	//地址类型未指定 可以是IPV6/ AF_INET IPv4/ AF_INET6 
	hints.ai_flags = AI_PASSIVE; //匹配所有 IP 地址
	hints.ai_protocol = IPPROTO_IP;  //匹配所有协议
	hints.ai_socktype = ConnectType; //流类型 //TCP  SOCK_STREAM  1// UDP  SOCK_DGRAM 2
	//获取 ipv4 ipv6 域名 连接信息
	if (int res = getaddrinfo("0.0.0.0", port, &hints, &(info)) != 0)
	{
		printf_s("地址输入错误");
		return -1;
	}


	//socket ——> bind ——> listen ——> accept
	//*************************************************创建socket********************************************************************
	CurrentSocket = socket(info->ai_family, info->ai_socktype,info->ai_protocol);
	//socket 回收 （这个函数确保关闭监听套接字后，它使用的地址可以立即被另一个套接字使用）
	evutil_make_listen_socket_reuseable(CurrentSocket);
	//绑定
	if (bind(CurrentSocket, info->ai_addr, info->ai_addrlen) == SOCKET_ERROR)
	{
		evutil_closesocket(CurrentSocket);
		CurrentSocket = -1;
		return -1;
	}
	//监听
	if (::listen(CurrentSocket, 1000) < 0)
	{
		evutil_closesocket(CurrentSocket);
		CurrentSocket = -1;
		return -1;
	}
	//跨平台统一接口，将套接字设置为非阻塞状态  
	evutil_make_socket_nonblocking(CurrentSocket);


	//***************************************************添加事件******************************************************************
	EventPtr = event_new(EventClass::base, CurrentSocket, EV_READ | EV_PERSIST, AcceptCall, this);
	//将已经初始化的event注册到libevent的事件链表上
	event_add(EventPtr, NULL);
	//serverData->IsRun = true;
	return 0;
	return 0;
}

int SocketServer::StopServer()
{
	return 0;
}

int SocketServer::SetCallBack(std::function<void(unsigned char* data, int length, int SockeCount)> callback)
{
	callBack = callback;
	return 0;
}

int SocketServer::SendData(const char* data, int length, unsigned __int64 socket)
{
	return 0;
}

bool SocketServer::IsConnect()
{
	return false;
}

void SocketServer::AcceptCall(evutil_socket_t socke, short events, void* classPtr)
{
	evutil_socket_t sockfd;
	struct sockaddr_in client;
	socklen_t len = sizeof(client);
	sockfd = ::accept(socke, (struct sockaddr*)&client, &len);
	evutil_make_socket_nonblocking(sockfd);//设置非阻塞

	SocketServer* SC = (SocketServer*)classPtr;

	//SocketInfo Info;
	//GetPeerNameInfo(sockfd, &Info);
	//SC->serverData->SocketList.push_back(Info);//保存socket到列表
	//创建数据接收事件
	struct event* ev = event_new(NULL, -1, 0, NULL, NULL);
	//将动态创建的结构体作为event的回调参数  
	Ptr* ptr = new Ptr();
	ptr->eventptr = ev;
	ptr->classptr = classPtr;
	event_assign(ev, EventClass::base, sockfd, EV_READ | EV_PERSIST, Event_Callback, ptr);
	event_add(ev, NULL);
}

void SocketServer::Event_Callback(evutil_socket_t socke, short events, void* classPtr)
{
	int d = event_base_get_num_events(EventClass::base, EVENT_BASE_COUNT_ADDED);

	SocketServer* SC = (SocketServer*)(((Ptr*)classPtr)->classptr);


	int len = recv(socke, SC->RecData, 4096, 0);

	if (len <= 0)
	{
		//删除掉线socket
		//for (std::vector<SocketInfo>::iterator it = SC->serverData->SocketList.begin(); it != SC->serverData->SocketList.end();)
		//{
		//	if (it->socket == socke)
		//		it = SC->serverData->SocketList.erase(it);
		//	else
		//		++it;
		//}
		//event_free((struct event*)(((Ptr*)classPtr)->eventptr));
		//closesocket(socke);
		//if (SC->serverData->callBack != NULL)
		//{
		//	SC->serverData->callBack((unsigned char*)RecData, 0, socke);
		//}
		//delete classPtr;
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
