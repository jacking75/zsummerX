#pragma once
#include "room.h"

class RoomManager
{
public:
	RoomManager() = default;
	~RoomManager() = default;

	void Init(const int32_t beginRoomNumber, const int32_t maxRoomCount, const int32_t maxRoomUserCount)
	{
		m_BeginRoomNumber = beginRoomNumber;
		m_MaxRoomCount = maxRoomCount;
		m_EndRoomNumber = beginRoomNumber + maxRoomCount;

		m_RoomList = std::vector<Room*>(maxRoomCount);

		for (auto i = 0; i < maxRoomCount; i++)
		{
			m_RoomList[i] = new Room();
			m_RoomList[i]->SendPacketFunc = SendPacketFunc;
			m_RoomList[i]->Init((i+ beginRoomNumber), maxRoomUserCount);
		}
	}

	uint32_t GetMaxRoomCount() { return m_MaxRoomCount; }
		
	uint16_t EnterUser(int32_t roomNumber, User* pUser)
	{
		auto pRoom = GetRoomByNumber(roomNumber);
		if (pRoom == nullptr)
		{
			return (uint16_t)ERROR_CODE::ROOM_INVALID_INDEX;
		}


		return pRoom->EnterUser(pUser);
	}
		
	int16_t LeaveUser(int32_t roomNumber, User* pUser)
	{
		auto pRoom = GetRoomByNumber(roomNumber);
		if (pRoom == nullptr)
		{
			return (int16_t)ERROR_CODE::ROOM_INVALID_INDEX;
		}
			
		pUser->SetDomainState(User::DOMAIN_STATE::LOGIN);
		pRoom->LeaveUser(pUser);
		return (int16_t)ERROR_CODE::NONE;
	}

	Room* GetRoomByNumber(int32_t number)
	{ 
		if (number < m_BeginRoomNumber || number >= m_EndRoomNumber)
		{
			return nullptr;
		}

		auto index = (number - m_BeginRoomNumber);
		return m_RoomList[index]; 
	} 

		
	std::function<void(int32_t, void*, int16_t)> SendPacketFunc;
		

private:
	std::vector<Room*> m_RoomList;
	int32_t m_BeginRoomNumber = 0;
	int32_t m_EndRoomNumber = 0;
	int32_t m_MaxRoomCount = 0;
};
