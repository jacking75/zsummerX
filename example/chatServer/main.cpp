// chatServer.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "server.h"

#include <iostream>
#include <thread>
#include <string>

int main()
{
	NetConfig netConfig;
	ChatServer server;

	std::thread logicThread([&]() {
		server.Start(netConfig); }
	);


	std::cout << "Server quits when you press (quit) !!!" << std::endl;

	while (true)
	{
		std::string inputCmd;
		std::getline(std::cin, inputCmd);

		if (inputCmd == "quit")
		{
			break;
		}
	}

	server.Stop();
	logicThread.join();
}

