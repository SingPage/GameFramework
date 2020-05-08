#pragma once

#define SEND_ERROR			(0)
#define SEND_SUCCESS		(1)
#define SEND_UNFINISHED		(2)

class CConnectionManager;

struct NetBuffer {
	BOOL Init() {
		if (NULL == pBuffer) {
			pCurPos = NULL;
			uBufferSize = 0;
		}
		else {
			pCurPos = pBuffer->GetBufferPtr();
			uBufferSize = pBuffer->GetBufferLength();
		}

		pCachBuffer = NULL;
		uDataSize = 0;
	}

	UINT32			uBufferSize;

	UINT32			uDataSize;

	CHAR*			pCurPos;

	IDataBuffer*	pBuffer;

	IDataBuffer*	pCachBuffer;
};

class CConnection {
	friend class CConnectionManager;
public:
	CConnection(CConnectionManager* pManager);

	virtual ~CConnection();

	BOOL	Close();

	VOID	SetConnectedStatus(BOOL bStatus);

	VOID	SetConnectionID(UINT32 uConnID);

	UINT32	GetConnectionID();

	BOOL	GetConnectedStatus();

	UINT32	GetIpAddr();

	VOID	SetIpAddr(UINT32 IpAddr);

	BOOL	ReceiveData();

	VOID	SetSocket(SOCKET socket);

	SOCKET	GetSocket();

	BOOL	PushSendQueue(IDataBuffer* pBuff);

	BOOL	SendData();

	BOOL	ParseBuffer();

	BOOL	CheckPacket(IDataBuffer* pPacket);

private:
	SOCKET								m_nSocket;

	UINT32								m_uIpAddr;

	BOOL								m_bConnectedStatus;

	UINT32								m_uConnID;

	NetBuffer							m_pRecvPacket;

	NetBuffer							m_pSendPacket;

	ArrayLockFreeQueue<IDataBuffer*>	m_SendBuffQueue;

	CConnection*						m_pPreConnection;

	CConnection*						m_pNextConnection;

	CConnectionManager*					m_pManager;
};