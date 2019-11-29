#pragma once

#include <string.h>

class SimpleRingBuffer
{
public:
	SimpleRingBuffer() = default;

	~SimpleRingBuffer()
	{ 
		delete[] m_pRingBuffer;
	}

	
	bool Create(const int ringBufferSize)
	{
		m_RingBufferSize = ringBufferSize;
		m_pRingBuffer = new char[m_RingBufferSize];

		Init();
		return true;
	}

	void Init()
	{
		mCurMark = 0;
	}

	char* SetData(const int size, char* pData)
	{
		char* pResult = nullptr;
		
		if ((mCurMark + size) >= m_RingBufferSize)
		{
			mCurMark = 0;
		}

		pResult = &m_pRingBuffer[mCurMark];

		memcpy(pResult, pData, size);

		return pResult;
	}
			
	int GetBufferSize() { return m_RingBufferSize; }
	
	
private:
	int m_RingBufferSize = 0;
	char* m_pRingBuffer = nullptr;
	int mCurMark = 0;
};
