#include <event.h>
#include "event2/thread.h"
#include <process.h>

#include <iostream>

#include "SocketClient.h"
#include "SocketServer.h"

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wsock32.lib")










void Heartbeat(unsigned char* data, int length, int SockeCount)
{
	if (length>5)
	{
		std::cout << data << std::endl;
	}
	
}

void LoopDelay(int ticks)
{
	if (ticks) 
	{
		LARGE_INTEGER litmp;
		LONGLONG QPart1, QPart2;
		double dfMinus, dfFreq, dfTim, dfSleep;
		dfSleep = (double)ticks / 1000;
		QueryPerformanceFrequency(&litmp);
		dfFreq = (double)litmp.QuadPart;// 獲得計數器的時鐘頻率
		QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart;// 獲得初始值
		do
		{
			QueryPerformanceCounter(&litmp);
			QPart2 = litmp.QuadPart;//獲得中止值
			dfMinus = (double)(QPart2 - QPart1);
			dfTim = dfMinus / dfFreq;// 獲得對應的時間值，單位為秒k;
		} while (dfTim < dfSleep);
	}
}


void mainkk()
{
	//SocketServer sv;

	//sv.SetCallBack(Heartbeat);

	//sv.StartServer("3000");


	//system("pause");

	SocketClient sc;
	sc.ClientConnect("127.0.0.1","3001", SocketClient::UDP);


	

	//timeval two_sec = { 0, 500000 };
	//struct event* EventHert = event_new(base, -1, EV_PERSIST | EV_TIMEOUT, Heartbeat, 0); //EV_PERSIST  多次重复
	//event_add(EventHert, &two_sec);



	system("pause");

	while (true)
	{
		sc.SendData("test",4);
		//usleep(1);
		//LoopDelay(1);
		
	}

	system("pause");
}



#include "log_generator.h"

enum Error
{
	NO_ERRORS,
	ERROR_CODE_1,
	ERROR_CODE_2,
	ERROR_CODE_3,
	ERROR_CODE_4
};

int main()
{
	Error status = NO_ERRORS;

	/* ... something bad happened */
	status = ERROR_CODE_2;

	if (status != NO_ERRORS)
	{
		LG_Init();

		// starts an error message with error style class
		LG_WriteMessageStart(LG_STYLE_CLASS_ERROR);

		LG_Write("An ERROR occured!\n");
		LG_Write("Error code is %d\n", status);

		LG_WriteMessageEnd();

		LG_Close();
	}

	return 0;
}








