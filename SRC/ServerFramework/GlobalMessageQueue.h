#pragma once

#define MessageQueueNum 10

#define MessageRoundNum 20

class CGlobalMessageQueue{

	CGlobalMessageQueue() : m_UniqueLock(m_SleepMutex){

		m_uPushIndex = 0;

		m_uPopIndex = 0;

		m_uPushNum = 0;

		m_uPopNum = 0;
	}

	virtual ~CGlobalMessageQueue() {

	}

public:

	static CGlobalMessageQueue*	GetInstancePtr();
	
	BOOL							Push(IDataBuffer* pBuff);

	BOOL							Pop(IDataBuffer* pBuff);

	//size_t							QueueSize();

private:

	CSpinLock						m_QueueLock[MessageQueueNum];

	std::queue<IDataBuffer*>		m_MessageQueue[MessageQueueNum];

	UINT32							m_uPushIndex;

	UINT32							m_uPopIndex;

	UINT32							m_uPushNum;
	
	UINT32							m_uPopNum;

	CSpinLock						m_SleepLock;

	std::condition_variable			m_SleepCondition;

	std::mutex						m_SleepMutex;

	std::unique_lock<std::mutex>	m_UniqueLock;

};