#include "packet.h"
#include "packetManager.h"

#include <utility>
#include <cstring>


void PacketManager::Init(UserManager* pUserManager, RoomManager* pRoomManager) 
{
	m_RecvFuntionDictionary = std::unordered_map<int, PROCESS_RECV_PACKET_FUNCTION>();

	m_RecvFuntionDictionary[(int)PACKET_ID::LOGIN_REQUEST] = &PacketManager::ProcessLogin;
	m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_ENTER_REQUEST] = &PacketManager::ProcessEnterRoom;
	m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_LEAVE_REQUEST] = &PacketManager::ProcessLeaveRoom;
	m_RecvFuntionDictionary[(int)PACKET_ID::ROOM_CHAT_REQUEST] = &PacketManager::ProcessRoomChatMessage;
				
	m_pUserManager = pUserManager;
	m_pRoomManager = pRoomManager;
}

void PacketManager::AddRequesstPacket()
{

}

void PacketManager::ProcessRecvPacket(int32_t connectionIndex, char* pBuf, int16_t copySize)
{
	PACKET_HEADER* pHeader = reinterpret_cast<PACKET_HEADER*>(pBuf);

	auto iter = m_RecvFuntionDictionary.find(pHeader->PacketId);
	if (iter != m_RecvFuntionDictionary.end())
	{
		(this->*(iter->second))(connectionIndex, pBuf, copySize);
	}

}
		
void PacketManager::ProcessLogin(const int32_t connIndex, char* pBuf, int16_t copySize)
{ 
	if (LOGIN_REQUEST_PACKET_SZIE != copySize)
	{
		return;
	}

	auto pLoginReqPacket = reinterpret_cast<LOGIN_REQUEST_PACKET*>(pBuf);

	auto pUserID = pLoginReqPacket->UserID;
	printf("requested user id = %s\n", pUserID);

	LOGIN_RESPONSE_PACKET loginResPacket;
	loginResPacket.PacketId = (uint16_t)PACKET_ID::LOGIN_RESPONSE;
	loginResPacket.PacketLength = sizeof(LOGIN_RESPONSE_PACKET);

	if (m_pUserManager->GetCurrentUserCnt() >= m_pUserManager->GetMaxUserCnt()) 
	{ 
		//�����ڼ��� �ִ���� �����ؼ� ���ӺҰ�
		loginResPacket.Result = (uint16_t)ERROR_CODE::LOGIN_USER_USED_ALL_OBJ;
		SendPacketFunc(connIndex, &loginResPacket, sizeof(LOGIN_RESPONSE_PACKET));
		return;
	}

	//���⿡�� �̹� ���ӵ� �������� Ȯ���ϰ�, ���ӵ� ������� �����Ѵ�.
	if (m_pUserManager->FindUserIndexByID(pUserID) == -1) 
	{ 
		//�������� �ƴ� �������
		//������ �����ϴ� ��ü�� �ִ´�.
		m_pUserManager->AddUser(pUserID, connIndex);
		loginResPacket.Result = (uint16_t)ERROR_CODE::NONE;
	}
	else 
	{
		//�������� �������� ���и� ��ȯ�Ѵ�.
		loginResPacket.Result = (uint16_t)ERROR_CODE::LOGIN_USER_ALREADY;
		SendPacketFunc(connIndex, &loginResPacket, sizeof(LOGIN_RESPONSE_PACKET));
		return;
	}

	SendPacketFunc(connIndex, &loginResPacket, sizeof(LOGIN_RESPONSE_PACKET));				
}



void PacketManager::ProcessEnterRoom(int32_t connIndex, char* pBuf, int16_t copySize)
{
	//UNREFERENCED_PARAMETER(copySize);

	auto pRoomEnterReqPacket = reinterpret_cast<ROOM_ENTER_REQUEST_PACKET*>(pBuf);
	auto pReqUser = m_pUserManager->GetUserByConnIdx(connIndex);

	if (!pReqUser || pReqUser == nullptr) 
	{
		return;
	}
				
	ROOM_ENTER_RESPONSE_PACKET roomEnterResPacket;
	roomEnterResPacket.PacketId = (uint16_t)PACKET_ID::ROOM_ENTER_RESPONSE;
	roomEnterResPacket.PacketLength = sizeof(ROOM_ENTER_RESPONSE_PACKET);
				
	roomEnterResPacket.Result = m_pRoomManager->EnterUser(pRoomEnterReqPacket->RoomNumber, pReqUser);

	SendPacketFunc(connIndex, &roomEnterResPacket, sizeof(ROOM_ENTER_RESPONSE_PACKET));
	printf("Response Packet Sended");
}


void PacketManager::ProcessLeaveRoom(int32_t connIndex, char* pBuf, int16_t copySize)
{
	//UNREFERENCED_PARAMETER(pBuf);
	//UNREFERENCED_PARAMETER(copySize);

	ROOM_LEAVE_RESPONSE_PACKET roomLeaveResPacket;
	roomLeaveResPacket.PacketId = (uint16_t)PACKET_ID::ROOM_LEAVE_RESPONSE;
	roomLeaveResPacket.PacketLength = sizeof(ROOM_LEAVE_RESPONSE_PACKET);

	auto reqUser = m_pUserManager->GetUserByConnIdx(connIndex);
	auto roomNum = reqUser->GetCurrentRoom();
				
	//TODO Room���� UserList��ü�� �� Ȯ���ϱ�
	roomLeaveResPacket.Result = m_pRoomManager->LeaveUser(roomNum, reqUser);
	SendPacketFunc(connIndex, &roomLeaveResPacket, sizeof(ROOM_LEAVE_RESPONSE_PACKET));
}


void PacketManager::ProcessRoomChatMessage(int32_t connIndex, char* pBuf, int16_t copySize)
{
	//UNREFERENCED_PARAMETER(copySize);

	auto pRoomChatReqPacketet = reinterpret_cast<ROOM_CHAT_REQUEST_PACKET*>(pBuf);
		
	ROOM_CHAT_RESPONSE_PACKET roomChatResPacket;
	roomChatResPacket.PacketId = (uint16_t)PACKET_ID::ROOM_CHAT_RESPONSE;
	roomChatResPacket.PacketLength = sizeof(ROOM_CHAT_RESPONSE_PACKET);
	roomChatResPacket.Result = (int16_t)ERROR_CODE::NONE;

	auto reqUser = m_pUserManager->GetUserByConnIdx(connIndex);
	auto roomNum = reqUser->GetCurrentRoom();
				
	auto pRoom = m_pRoomManager->GetRoomByNumber(roomNum);
	if (pRoom == nullptr)
	{
		roomChatResPacket.Result = (int16_t)ERROR_CODE::CHAT_ROOM_INVALID_ROOM_NUMBER;
		SendPacketFunc(connIndex, &roomChatResPacket, sizeof(ROOM_CHAT_RESPONSE_PACKET));
		return;
	}
		
	SendPacketFunc(connIndex, &roomChatResPacket, sizeof(ROOM_CHAT_RESPONSE_PACKET));

	pRoom->NotifyChat(connIndex, reqUser->GetUserId().c_str(), pRoomChatReqPacketet->Message);		
}		   

void PacketManager::ClearConnectionInfo(int32_t connIndex)
{
	auto pReqUser = m_pUserManager->GetUserByConnIdx(connIndex);

	if (pReqUser->GetDomainState() == User::DOMAIN_STATE::ROOM) 
	{
		auto roomNum = pReqUser->GetCurrentRoom();
		m_pRoomManager->LeaveUser(roomNum, pReqUser);
	}

	if (pReqUser->GetDomainState() != User::DOMAIN_STATE::NONE) 
	{
		m_pUserManager->DeleteUserInfo(pReqUser);
	}
}


