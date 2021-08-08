#include "EventClass.h"
#include <event.h>
#include "event2/thread.h"
#include <process.h>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wsock32.lib")

int EventClass::NewCount = 0;
event_base* EventClass::base = NULL;
std::shared_ptr<std::thread> EventClass::work_thread;
EventClass::EventClass()
{
	if (base== NULL)
	{
#ifdef _WIN32
		WSADATA wsa_data;
		WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif

		int done = evthread_use_windows_threads();//�������߳�
		base = event_base_new();//����ʵ��
		if (base==NULL)
		{
			throw("event_base create error");
			return;
		}

		work_thread = std::make_shared<std::thread>([]() {
			event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY); //��Ҫ�ֶ��ر��¼�����
			//event_base_dispatch(base); //û���¼�ʱ���Զ��ر�
			});	
	}
	NewCount++;
}

EventClass::~EventClass()
{
	if (NewCount==1)
	{
		NewCount--;
		struct timeval delay = { 2, 0 };
		event_base_loopbreak(base);
		event_base_loopexit(base, &delay);
		event_base_free(base);
		work_thread->join();
	}
	else
	{
		NewCount--;
	}
}

