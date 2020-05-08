#include"stdafx.h"
#include"MemoryPoll.h"

CMemoryPoll::CMemoryPoll() {

	FUNCTION_RUNNING


	FUNCTION_FINISHING
}

CMemoryPoll::~CMemoryPoll(){

	FUNCTION_RUNNING


	FUNCTION_FINISHING

}

CMemoryPoll* CMemoryPoll::GetInstancePtr(){

	FUNCTION_RUNNING

	static CMemoryPoll memoryPoll;

	return &memoryPoll;

	FUNCTION_FINISHING
}

IDataBuffer* CMemoryPoll::GetDataBuffer(UINT32 uBuffSize){

	FUNCTION_RUNNING

	if (uBuffSize <= 64) {
		return m_BufferManager64B.AllocateBuffer();
	}
	else if (uBuffSize <= 128) {
		return m_BufferManager128B.AllocateBuffer();
	}
	else if (uBuffSize <= 256) {
		return m_BufferManager256B.AllocateBuffer();
	}
	else if (uBuffSize <= 512) {
		return m_BufferManager512B.AllocateBuffer();
	}
	else if (uBuffSize <= 1024) {
		return m_BufferManager1K.AllocateBuffer();
	}
	else if (uBuffSize <= 2048) {
		return m_BufferManager2K.AllocateBuffer();
	}
	else if (uBuffSize <= 4096) {
		return m_BufferManager4K.AllocateBuffer();
	}
	else if (uBuffSize <= 8192) {
		return m_BufferManager8K.AllocateBuffer();
	}
	else if (uBuffSize <= 16384) {
		return m_BufferManager16K.AllocateBuffer();
	}
	else if (uBuffSize <= 32768) {
		return m_BufferManager32K.AllocateBuffer();
	}
	else if (uBuffSize <= 65536) {
		return m_BufferManager64K.AllocateBuffer();
	}

	return NULL;

	FUNCTION_FINISHING
}
