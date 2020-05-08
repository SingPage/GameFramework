#pragma once

template<UINT32 SIZE>
class CBufferManager;

template<UINT32 SIZE>
class CDataBuffer : public IDataBuffer {

	friend class CBufferManager<SIZE>;

public:
	CDataBuffer() {
		Iint();
	}

	virtual ~CDataBuffer() {

	}

	VOID Iint() {
		m_pPreBuffer = NULL;
		m_pNextBuffer = NULL;
		m_pBufferManager = NULL;
		m_uBufferLength = SIZE;
		m_uDataLength = 0;
		m_nRefCount = 0;
	}

	CHAR* GetBufferPtr() {

		return m_cBuffer;
	}

	virtual CHAR* GetDataTail() {
		return m_cBuffer + m_uDataLength;
	}

	UINT32	GetBufferLength() {
		return  m_uBufferLength;
	}

	UINT32 GetDataLength() {
		return m_uDataLength;
	}

	VOID SetDataLength(UINT32 uDataLength) {
		m_uDataLength = uDataLength;
	}

	BOOL AddRef() {
		m_mutex.lock();
		m_nRefCount++;
		m_mutex.unlock();

		return TRUE;
	}

	BOOL ReduceRef() {
		m_mutex.lock();
		m_nRefCount--;
		m_mutex.unlock();

		return TRUE;
	}

	INT32 GetRefCount() {
		return m_nRefCount;
	}

	BOOL CopyFromBuffer(IDataBuffer& dataBuffer, UINT32 size) {

		if (size <= 0 || size > dataBuffer.GetBufferLength()) {
			return FALSE;
		}

		m_mutex.lock();
		dataBuffer.LockMutex();
		memcpy(dataBuffer.GetBufferPtr(), m_cBuffer, size);
		m_uDataLength = size;
		dataBuffer.UnlockMutex();
		m_mutex.unlock();

		return TRUE;
	}

	VOID LockMutex() {
		m_mutex.lock();
	}

	VOID UnlockMutex() {
		m_mutex.unlock();
	}

	BOOL Free(){
		m_pBufferManager->RealseBuffer(this);
	}

private:
	CHAR					m_cBuffer[SIZE];

	UINT32					m_uDataLength;

	UINT32					m_uBufferLength;

	INT32					m_nRefCount;

	CDataBuffer<SIZE>*		m_pPreBuffer;

	CDataBuffer<SIZE>*		m_pNextBuffer;

	CBufferManager<SIZE>*	m_pBufferManager;

	std::mutex				m_mutex;
};

template<UINT32 SIZE>
class CBufferManager {
public:
	CBufferManager() {

		m_pFreeList = NULL;
		m_pUsedList = NULL;

		m_uBufferNum = 0;
	};

	virtual ~CBufferManager() {

		RealseALLBuffer();
	}

	IDataBuffer* AllocateBuffer() {

		m_mutex.lock();
		CDataBuffer<SIZE>* pTempPtr = NULL;

		if (NULL != m_pFreeList) {
			pTempPtr = m_pFreeList;
			//Head insert
			m_pFreeList = m_pFreeList->m_pNextBuffer;

			if (NULL != m_pFreeList) {
				m_pFreeList->m_pPreBuffer = NULL;
			}
			
		}
		else {
			pTempPtr = new CDataBuffer<SIZE>();
			pTempPtr->m_pBufferManager = this;
			pTempPtr->m_pPreBuffer = NULL;
		}

		//Head insert
		pTempPtr->m_pNextBuffer = m_pUsedList;

		if (NULL != m_pUsedList) {
			m_pUsedList->m_pPreBuffer = pTempPtr;
		}
		m_pUsedList = pTempPtr;

		m_uBufferNum++;
		m_mutex.unlock();
		return pTempPtr;
	}

	BOOL RealseBuffer(CDataBuffer<SIZE>* pBuffer) {
		m_mutex.lock();
		CDataBuffer<SIZE>* pTempPtr = NULL;

		if (pBuffer->m_nRefCount < 0) {
			m_mutex.unlock();
			return FALSE;
		}

		if (0 == pBuffer->m_nRefCount) {

			if (NULL != pBuffer->m_pPreBuffer) {

				pBuffer->m_pPreBuffer->m_pNextBuffer = pBuffer->m_pNextBuffer;
			}

			if (NULL != pBuffer->m_pNextBuffer) {

				pBuffer->m_pNextBuffer->m_pPreBuffer = pBuffer->m_pPreBuffer;
			}

			if (m_pUsedList == pBuffer) {

				m_pUsedList = pBuffer->m_pNextBuffer;
			}

			pBuffer->Iint();
			pBuffer->m_pBufferManager = this;
			pBuffer->m_pNextBuffer = m_pFreeList;
			if (NULL != m_pFreeList) {
				m_pFreeList->m_pPreBuffer = pBuffer;
			}
			m_pFreeList = pBuffer;
		}

		m_uBufferNum--;
		m_mutex.unlock();
		return TRUE;
	}

	BOOL RealseALLBuffer() {
		m_mutex.lock();

		CDataBuffer<SIZE>* pTempPtr = NULL;

		pTempPtr = m_pUsedList;
		while (pTempPtr) {
			m_pUsedList = pTempPtr->m_pNextBuffer;
			delete pTempPtr;
			pTempPtr = m_pUsedList;
		}

		pTempPtr = m_pFreeList;
		while (pTempPtr) {
			m_pFreeList = pTempPtr->m_pNextBuffer;
			delete pTempPtr;
			pTempPtr = m_pFreeList;
		}

		m_uBufferNum = 0;
		m_mutex.unlock();
		return TRUE;
	}

private:
	CDataBuffer<SIZE>* m_pFreeList;

	CDataBuffer<SIZE>* m_pUsedList;

	UINT32				m_uBufferNum;

	std::mutex			m_mutex;
}; 