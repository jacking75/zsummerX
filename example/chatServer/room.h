#pragma once

#include "UserManager.h"
#include "Packet.h"

#include <functional>


class Room 
{
	const int MAX_USER_ID_LEN = 20;
public:
	Room() = default;
	~Room() = default;

	void Init(const int32_t roomNum, const int32_t maxUserCount)
	{
		m_RoomNum = roomNum;
		m_MaxUserCount = maxUserCount;
	}

	int32_t GetMaxUserCount() { return m_MaxUserCount; }
		
	int32_t GetCurrentUserCount() { return m_CurrentUserCount; }
		
	int32_t GetRoomNumber() { return m_RoomNum; }

	uint16_t EnterUser(User* pUser)
	{
		if (m_CurrentUserCount >= m_MaxUserCount)
		{
			return (uint16_t)ERROR_CODE::ENTER_ROOM_FULL_USER;
		}

		m_UserList.push_back(pUser);
		++m_CurrentUserCount;

		pUser->EnterRoom(m_RoomNum);
		return (uint16_t)ERROR_CODE::NONE;
	}
		
	void LeaveUser(User* pLeaveUser)
	{
		m_UserList.remove_if([leaveUserId = pLeaveUser->GetUserId()](User* pUser) {
			return leaveUserId == pUser->GetUserId();
		});
	}
						
	void NotifyChat(int32_t connIndex, const char* UserID, const char* Msg)
	{
		ROOM_CHAT_NOTIFY_PACKET roomChatNtfyPkt;
		roomChatNtfyPkt.PacketId = (uint16_t)PACKET_ID::ROOM_CHAT_NOTIFY;
		roomChatNtfyPkt.PacketLength = sizeof(roomChatNtfyPkt);

		memcpy(roomChatNtfyPkt.Msg, Msg, sizeof(roomChatNtfyPkt.Msg));
		memcpy(roomChatNtfyPkt.UserID, UserID, sizeof(roomChatNtfyPkt.UserID));
		SendToAllUser(sizeof(roomChatNtfyPkt), &roomChatNtfyPkt, connIndex, false);
	}
		
		
	std::function<void(int32_t, void*, uint16_t)> SendPacketFunc;

private:
	void SendToAllUser(const uint16_t dataSize, void* pData, const int32_t passUserindex, bool exceptMe)
	{
		for (auto pUser : m_UserList)
		{
			if (pUser == nullptr) {
				continue;
			}

			if (exceptMe && pUser->GetNetConnIdx() == passUserindex) {
				continue;
			}

			SendPacketFunc(pUser->GetNetConnIdx(), pData, dataSize);
		}
	}


	int32_t m_RoomNum = -1;

	std::list<User*> m_UserList;
		
	int32_t m_MaxUserCount = 0;

	uint16_t m_CurrentUserCount = 0;
};
