#pragma once

#include "errorCode.h"
#include "user.h"

#include <unordered_map>


class UserManager
{

public:
	UserManager() = default;
	~UserManager() = default;

	void Init(const int32_t maxUserCount)
	{
		m_MaxUserCnt = maxUserCount;
		UserObjPool = std::vector<User*>(m_MaxUserCnt);

		for (auto i = 0; i < m_MaxUserCnt; i++)
		{
			UserObjPool[i] = new User();
			UserObjPool[i]->Init(i);
		}
	}

	int32_t GetCurrentUserCnt() { return m_CurrentUserCnt; }

	int32_t GetMaxUserCnt() { return m_MaxUserCnt; }
		
	void IncreaseUserCnt() { m_CurrentUserCnt++; }
		
	void DecreaseUserCnt() 
	{
		if (m_CurrentUserCnt > 0) 
		{
			m_CurrentUserCnt--;
		}
	}

	ERROR_CODE AddUser(char* userID, int conn_idx)
	{
		//TODO 유저 중복 조사하기

		auto user_idx = conn_idx;

		UserObjPool[user_idx]->SetLogin(userID);
		UserDictionary.insert(std::pair< char*, int>(userID, conn_idx));

		return ERROR_CODE::NONE;
	}
		
	int32_t FindUserIndexByID(char* userID)
	{
		if (auto res = UserDictionary.find(userID); res != UserDictionary.end())
		{
			return (*res).second;
		}
			
		return -1;
	}
		
	void DeleteUserInfo(User* deleteUser)
	{
		UserDictionary.erase(deleteUser->GetUserId());
		deleteUser->Clear();
	}

	User* GetUserByConnIdx(int32_t conn_idx)
	{
		return UserObjPool[conn_idx];
	}


private:
	int32_t m_MaxUserCnt = 0;
	int32_t m_CurrentUserCnt = 0;

	std::vector<User*> UserObjPool; //vector로
	std::unordered_map<std::string, int> UserDictionary;
};
