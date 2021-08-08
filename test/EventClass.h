#pragma once

#include<thread>



class EventClass
{
public:
	EventClass();
	~EventClass();

	static struct event_base* base;
private:
	static int NewCount;
	static  std::shared_ptr<std::thread> work_thread;
};

