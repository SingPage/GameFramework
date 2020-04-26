#pragma once

class CMemoryPoll {

	CMemoryPoll();
	virtual ~CMemoryPoll();

public:
	static CMemoryPoll* GetInstancePtr();
	IDataBuffer* GetDataBuffer(UINT32 uBuffSize);

private:
	CBufferManager<64>		m_BufferManager64B;
	CBufferManager<128>		m_BufferManager128B;
	CBufferManager<256>		m_BufferManager256B;
	CBufferManager<512>		m_BufferManager512B;
	CBufferManager<1024>	m_BufferManager1K;
	CBufferManager<2048>	m_BufferManager2K;
	CBufferManager<4096>	m_BufferManager4K;
	CBufferManager<8192>	m_BufferManager8K;
	CBufferManager<16384>	m_BufferManager16K;
	CBufferManager<32768>	m_BufferManager32K;
	CBufferManager<65536>	m_BufferManager64K;
};