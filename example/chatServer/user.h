#pragma once

#include "Packet.h"

#include <cstdint>
#include <string>


class User
{
public:
	enum class DOMAIN_STATE 
	{
		NONE = 0,
		LOGIN = 1,
		ROOM = 2
	};


	User() = default;
	~User() = default;

	void Init(const int32_t index)
	{
		m_Index = index;
	}

	void Clear()
	{
		m_RoomIndex = -1;
		m_UserId = "";
		m_IsConfirm = false;
		m_CurDomainState = DOMAIN_STATE::NONE;
	}

	int SetLogin(char* login_id)
	{
		m_CurDomainState = DOMAIN_STATE::LOGIN;
		m_UserId = login_id;

		return 0;
	}
		
	void EnterRoom(int32_t roomIndex)
	{
		m_RoomIndex = roomIndex;
		m_CurDomainState = DOMAIN_STATE::ROOM;
	}
		
	void SetDomainState(DOMAIN_STATE val) { m_CurDomainState = val; }

	int32_t GetCurrentRoom()
	{
		return m_RoomIndex;
	}

	int32_t GetNetConnIdx()
	{
		return m_Index;
	}

	std::string GetUserId() const
	{
		return  m_UserId;
	}

	DOMAIN_STATE GetDomainState() 
	{
		return m_CurDomainState;
	}
				

private:
	//TODO type�� window���� ���ǵȰ����� (INT16 INT32 INT64) �� ������ ������ header�� window.h
	//�̰��� cross �÷����� �ƴ϶� windows������ ����ϱ⿡ ���� type. ���� ũ�ν��÷����� ���ٸ� c++���� ���ǵ� Ÿ���� ��

	INT32 m_RoomIndex = -1;

	std::string m_UserId;
	bool m_IsConfirm = false;
	std::string AuthToken;

	INT32 m_Index = -1;

	DOMAIN_STATE m_CurDomainState = DOMAIN_STATE::NONE;		
};

