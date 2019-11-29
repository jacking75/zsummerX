#pragma once

#include <cstdint>

struct PacketData
{

};
// 100 ~ 1000 ������ ��ȣ�� ��� ����!
enum class  PACKET_ID : uint16_t
{

	/// ChatClient�� ��Ŷ ID

	LOGIN_REQUEST = 201,
	LOGIN_RESPONSE = 202,

	ROOM_ENTER_REQUEST = 206,
	ROOM_ENTER_RESPONSE = 207,

	ROOM_LEAVE_REQUEST = 209,
	ROOM_LEAVE_RESPONSE = 210,

	ROOM_CHAT_REQUEST = 214,
	ROOM_CHAT_RESPONSE = 215,
	ROOM_CHAT_NOTIFY = 216,
};

#pragma pack(push,1)
struct PACKET_HEADER
{
	uint16_t PacketLength;
	uint16_t PacketId;
	uint8_t Type; //���࿩�� ��ȣȭ���� �� �Ӽ��� �˾Ƴ��� ��
};

//- �α��� ��û
const int MAX_USER_ID_LEN = 20;
const int MAX_USER_PW_LEN = 20;

struct LOGIN_REQUEST_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN + 1];
	char UserPW[MAX_USER_PW_LEN + 1];
};
const size_t LOGIN_REQUEST_PACKET_SZIE = sizeof(LOGIN_REQUEST_PACKET);


struct LOGIN_RESPONSE_PACKET : public PACKET_HEADER
{
	uint16_t Result;
};



//- �뿡 ���� ��û
const int MAX_ROOM_TITLE_SIZE = 16;
struct ROOM_ENTER_REQUEST_PACKET : public PACKET_HEADER
{
	int RoomNumber;
};

struct ROOM_ENTER_RESPONSE_PACKET : public PACKET_HEADER
{
	uint16_t Result;
	char RivalUserID[MAX_USER_ID_LEN + 1] = { 0, };
};


//- �� ������ ��û
struct ROOM_LEAVE_REQUEST_PACKET : public PACKET_HEADER
{

};

struct ROOM_LEAVE_RESPONSE_PACKET : public PACKET_HEADER
{
	uint16_t Result;
};



// �� ä��
const int MAX_CHAT_MSG_SIZE = 256;
struct ROOM_CHAT_REQUEST_PACKET : public PACKET_HEADER
{
	char Message[MAX_CHAT_MSG_SIZE + 1] = { 0, };
};

struct ROOM_CHAT_RESPONSE_PACKET : public PACKET_HEADER
{
	uint16_t Result;
};

struct ROOM_CHAT_NOTIFY_PACKET : public PACKET_HEADER
{
	char UserID[MAX_USER_ID_LEN + 1] = { 0, };
	char Msg[MAX_CHAT_MSG_SIZE + 1] = { 0, };
};
#pragma pack(pop) //���� ������ ��ŷ������ �����