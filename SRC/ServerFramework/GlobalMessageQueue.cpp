#include"stdafx.h"
#include"GlobalMessageQueue.h"

CGlobalMessageQueue* CGlobalMessageQueue::GetInstancePtr() {

	static CGlobalMessageQueue Queue;

	return &Queue;
}

BOOL CGlobalMessageQueue::Push(IDataBuffer* pBuff){

	UINT32 SourceIndex;

	m_QueueLock[m_uPushIndex].Lock();

	SourceIndex = m_uPushIndex;

	m_MessageQueue[m_uPushIndex].push(pBuff);

	if (++m_uPushNum >= MessageRoundNum) {

		m_uPushNum = 0;
		if (++m_uPushIndex >= MessageQueueNum) {

			m_uPushIndex = 0;
		}

		m_QueueLock[SourceIndex].UnLock();

		if (m_SleepLock.TryLock()) {

			m_SleepLock.UnLock();
		}
		else {
			m_SleepCondition.notify_all();
		}
	}
	else {

		m_QueueLock[SourceIndex].UnLock();
	}



	return TRUE;
}

BOOL CGlobalMessageQueue::Pop(IDataBuffer* pBuff){

	UINT32 SourceIndex;

	m_QueueLock[m_uPopIndex].Lock();

	SourceIndex = m_uPopIndex;

	if (m_MessageQueue[m_uPopIndex].empty()) {
		m_QueueLock[SourceIndex].UnLock();

		m_SleepLock.Lock();
		m_SleepCondition.wait(m_UniqueLock);
		m_SleepLock.UnLock();

		m_QueueLock[m_uPopIndex].Lock();
		SourceIndex = m_uPopIndex;
	}

	pBuff = m_MessageQueue[m_uPopIndex].front();
	m_MessageQueue[m_uPopIndex].pop();

	if (++m_uPopNum >= MessageRoundNum) {

		m_uPopNum = 0;
		if (++m_uPopIndex >= MessageQueueNum) {

			m_uPopIndex = 0;
		}
	}

	m_QueueLock[SourceIndex].UnLock();


	return TRUE;
}