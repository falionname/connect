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
	//************************************************��ʼ�����ӵ�ַ********************************************************
	struct addrinfo hints;//������ַ��Ϣ
	ZeroMemory(&hints, sizeof(hints));//��ʼ������
	hints.ai_family = AF_UNSPEC;	//��ַ����δָ�� ������IPV6/ AF_INET IPv4/ AF_INET6 
	hints.ai_flags = AI_PASSIVE; //ƥ������ IP ��ַ
	hints.ai_protocol = IPPROTO_IP;  //ƥ������Э��
	hints.ai_socktype = ConnectType; //������ //TCP  SOCK_STREAM  1// UDP  SOCK_DGRAM 2
	//��ȡ ipv4 ipv6 ���� ������Ϣ
	if (int res = getaddrinfo("0.0.0.0", port, &hints, &(info)) != 0)
	{
		printf_s("��ַ�������");
		return -1;
	}


	//socket ����> bind ����> listen ����> accept
	//*************************************************����socket********************************************************************
	CurrentSocket = socket(info->ai_family, info->ai_socktype,info->ai_protocol);
	//socket ���� ���������ȷ���رռ����׽��ֺ���ʹ�õĵ�ַ������������һ���׽���ʹ�ã�
	evutil_make_listen_socket_reuseable(CurrentSocket);
	//��
	if (bind(CurrentSocket, info->ai_addr, info->ai_addrlen) == SOCKET_ERROR)
	{
		evutil_closesocket(CurrentSocket);
		CurrentSocket = -1;
		return -1;
	}
	//����
	if (::listen(CurrentSocket, 1000) < 0)
	{
		evutil_closesocket(CurrentSocket);
		CurrentSocket = -1;
		return -1;
	}
	//��ƽ̨ͳһ�ӿڣ����׽�������Ϊ������״̬  
	evutil_make_socket_nonblocking(CurrentSocket);


	//***************************************************����¼�******************************************************************
	EventPtr = event_new(EventClass::base, CurrentSocket, EV_READ | EV_PERSIST, AcceptCall, this);
	//���Ѿ���ʼ����eventע�ᵽlibevent���¼�������
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
	evutil_make_socket_nonblocking(sockfd);//���÷�����

	SocketServer* SC = (SocketServer*)classPtr;

	//SocketInfo Info;
	//GetPeerNameInfo(sockfd, &Info);
	//SC->serverData->SocketList.push_back(Info);//����socket���б�
	//�������ݽ����¼�
	struct event* ev = event_new(NULL, -1, 0, NULL, NULL);
	//����̬�����Ľṹ����Ϊevent�Ļص�����  
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
		//ɾ������socket
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
