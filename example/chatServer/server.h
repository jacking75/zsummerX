#pragma once

#include "session.h"

#include <zsummerX/zsummerX.h>
#include <log4z/log4z.h>

#include <cstdint>
#include <atomic>

struct NetConfig
{
	std::string RemoteIP;
	uint16_t Port = 32452;
	int32_t IOThreadCount = 1;
};

class ChatServer
{
public:
	ChatServer() = default;
	~ChatServer() = default;

	bool Start(NetConfig netConfig)
	{
		mNetConfig = netConfig;

		/*auto sendPacketFunc = [&](INT32 connectionIndex, void* pSendPacket, INT16 packetSize)
		{
			m_pIOCPServer->SendPacket(connectionIndex, pSendPacket, packetSize);
		};*/

		if (!mAcceptEventLoop->initialize())
		{
			return false;
		}

		/*for (unsigned int i = 0; i < g_multiThread; i++)
		{
			CProcess* p = new CProcess();
			if (p->start())
			{
				_process.push_back(p);
			}
		}*/

		if (mAccepter->openAccept(mNetConfig.RemoteIP.c_str(), mNetConfig.Port) == false)
		{
			LOGE("open server port [" << mNetConfig.Port << "] fail");
			return false;
		}
		LOGI("open server port [" << mNetConfig.Port << "] success");

		zsummer::network::TcpSocketPtr s(new zsummer::network::TcpSocket()); //동적 할당을 하지 않도록 해보자

		mAccepter->SetAcceptCallback(std::bind(&ChatServer::OnAccept, this, std::placeholders::_1, std::placeholders::_2));
		return mAccepter->doAccept(s);
	}

	void Stop()
	{
		Stop_impl();
	}

	bool SendPacket()
	{
		return true;
	}


private:
	void run()
	{
		for (int i = 0; i < mNetConfig.IOThreadCount; ++i)
		{
			mIOWorkThreads.emplace_back([&, this]() { IoWorkerThread(i); });
		}
	}

	void OnAccept(zsummer::network::NetErrorCode ec, zsummer::network::TcpSocketPtr sockptr)
	{
		if (ec)
		{
			LOGE("OnAccept error. ec=" << ec);
			return;
		}


		LOGD("OnAccept one socket");
		
		++mAcceptedCount;
		auto curThreadindex = mAcceptedCount % mNetConfig.IOThreadCount;
		
		sockptr->initialize(mIOEventLoops[curThreadindex]);
		mIOEventLoops[curThreadindex]->post(std::bind(&ChatServer::OnNewConnected, this, sockptr));
		

		zsummer::network::TcpSocketPtr s(new zsummer::network::TcpSocket());
		mAccepter->doAccept(s);
	}

	void IoWorkerThread(int index)
	{
		while (mIsRunning)
		{
			mIOEventLoops[index]->runOnce();
		}
	}
	
	void Stop_impl()
	{
		mIsRunning = false;

		for (auto& thread : mIOWorkThreads)
		{
			if (thread.joinable())
			{
				thread.join();
			}
		}
	}

	void OnNewConnected(std::shared_ptr<zsummer::network::TcpSocket> sockptr)
	{
		LOGD("OnNewConnected");

		std::shared_ptr<Session> pSession(new Session);
		pSession->initialize(sockptr);
	}



	NetConfig mNetConfig;

	bool    mIsRunning = false;//_running = false;

	zsummer::network::EventLoopPtr mAcceptEventLoop;// _summer;
	std::thread     mAcceptThread;// _thread;	
	zsummer::network::TcpAcceptPtr mAccepter;// _accept;

	std::vector < zsummer::network::EventLoopPtr> mIOEventLoops;// _summer;
	std::vector<std::thread> mIOWorkThreads;

	std::atomic<uint64_t> mAcceptedCount;
};