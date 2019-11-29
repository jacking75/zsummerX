#pragma once

#include "ringBuffer.h"

#include <zsummerX/zsummerX.h>
#include <log4z/log4z.h>


class Session : public std::enable_shared_from_this<Session>
{
public:
	Session() = default;
	~Session() = default;

	void initialize(zsummer::network::TcpSocketPtr sockptr)
	{
		mSockptr = sockptr;

#ifndef WIN32
		_sockptr->setFloodSendOptimize(false);
#endif
		
		mRecvBuffer = std::make_unique<char[]>(mRecvBufferSize);
		mRecvPacketBuffer.Create(16000);
		mSendPacketBuffer.Create(16000);

		doRecv();

		mIsEstablished = true;
		
		//_process._nTotalOpen++;
	}



private:
	void Dispose()
	{
	}

	void onConnected(zsummer::network::NetErrorCode ec)
	{

	}

	void doRecv()
	{
		//TODO onRecv 함수를 매번 바인딩 하지 말자
		bool bSuccess = mSockptr->doRecv(mRecvBuffer.get() + mRecvBufferPos, mRecvBufferSize,
						std::bind(&Session::onRecv, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		if (!bSuccess)
		{
			onClose();
		}
	}

	unsigned int onRecv(zsummer::network::NetErrorCode ec, int recvedLen)
	{
		if (ec)
		{
			LOGW("remote socket closed");
			onClose();
			return 0;
		}
		
		//_process._nTotalRecvCount++;
		//_recving._offset += nRecvedLen;
		
		uint16_t totalByte = mRecvBufferPos + recvedLen;
		uint16_t remainByte = totalByte;
		while (true)
		{
			PacketForwardingLoop(remainByte, mRecvBuffer.get());
		}
		
		if (remainByte == 0)
		{
			mRecvBufferPos = 0;
			
		}
		else
		{
			auto pos = totalByte - remainByte;
			memmove(mRecvBuffer.get(), mRecvBuffer.get() + pos, remainByte);
			mRecvBufferPos = remainByte;
		}

		doRecv();
		return 0;
	}
	
	void PacketForwardingLoop(uint16_t& remainByte, char* pBuffer)
	{
		const int PACKET_HEADER_LENGTH = 5;
		const int PACKET_SIZE_LENGTH = 2;
		const int PACKET_TYPE_LENGTH = 2;

		int readPos = 0;

		while (true)
		{
			if (remainByte < PACKET_HEADER_LENGTH)
			{
				break;
			}

			uint16_t packetSize = 0;
			CopyMemory(&packetSize, &pBuffer[readPos], PACKET_SIZE_LENGTH);
			
			if (0 >= packetSize)
			{
				LOGE("Invalid Packet Size !");
				return;
			}

			if (remainByte >= packetSize)
			{
				mRecvPacketBuffer.SetData(packetSize, &pBuffer[readPos]);

				//TODO 애플리케이션에 알려줘야 한다

				remainByte -= packetSize;
				readPos += packetSize;
			}
			else
			{
				break;
			}
		}
	}

			
	void doSend(char* buf, unsigned int len)
	{
		auto pSendData = mSendPacketBuffer.SetData(len, buf); // 스레드 세이프 하지 않다!

		//TODO onSend 함수를 매번 바인딩 하지 말자
		mSockptr->doSend(pSendData, len, std::bind(&Session::onSend, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		
	}
	
	void onSend(zsummer::network::NetErrorCode ec, int nSentLen)
	{
		if (ec)
		{
			LOGD("remote socket closed");
			return;
		}						
	}
	
	void onClose()
	{
		LOGI("Session Closed!");
		
		mIsEstablished = false;
	}

	
	
	
	zsummer::network::TcpSocketPtr  mSockptr;
	bool mIsEstablished = false;

	uint16_t mRecvBufferSize = 4012;
	uint16_t mRecvBufferPos = 0;
	std::unique_ptr<char[]> mRecvBuffer;

	SimpleRingBuffer mRecvPacketBuffer;

	SimpleRingBuffer mSendPacketBuffer;
	//Packet _recving;
	//std::string _recvTextCache;

	//std::queue<Packet*> _sendque;

	//Packet _sending;
	unsigned int _needSendLen = 0;

	unsigned long long _lastDelayTime = 0; 
};