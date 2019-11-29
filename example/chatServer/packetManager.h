#pragma once

#include "roomManager.h"

#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>

class PacketManager 
{
public:
	PacketManager() = default;
	~PacketManager() = default;

	void Init(UserManager* pUserManager, RoomManager* pRoomManager);

	void ProcessRecvPacket(int32_t connectionIndex, char* pBuf, int16_t copySize);
	void ProcessLogin(int32_t connIndex, char* pBuf, int16_t copySize);
	void ProcessEnterRoom(int32_t connIndex, char* pBuf, int16_t copySize);
	void ProcessLeaveRoom(int32_t connIndex, char* pBuf, int16_t copySize);
	void ProcessRoomChatMessage(int32_t connIndex, char* pBuf, int16_t copySize);
		

	void ClearConnectionInfo(int32_t connIndex);
		

	void AddRequesstPacket();

	std::function<void(int32_t, void*, int16_t)> SendPacketFunc;


private:
	typedef void(PacketManager::* PROCESS_RECV_PACKET_FUNCTION)(int32_t, char*, int16_t);
	std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION> m_RecvFuntionDictionary;

	UserManager* m_pUserManager;
	RoomManager* m_pRoomManager;
		
	std::function<void(int, char*)> m_SendMQDataFunc;
};

