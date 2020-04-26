#pragma once

class CConnectionManager {
private:
	CConnectionManager();
	virtual ~CConnectionManager();
public:
	static CConnectionManager* GetInstancePtr();

	BOOL			Start(UINT16 usPortNum, UINT16 usMaxConn, IDataHandler* pDataHandler);
	
	BOOL			Close();

	BOOL			SendNetPacketByConnID(UINT32 uConnID, IDataBuffer* pBuff);

	BOOL			RecvConnectionData(UINT32 pConnID, IDataBuffer* pBuff);

	CConnection*	AsyncConnect(CHAR* IPAddr, UINT16 usPortNum);

private:
	BOOL			Thread_ListenSocket();

	BOOL			CloseListenThread();

	BOOL			Thread_HandleEpollEvents();

	BOOL			CLoseEpollEventsThread();

	BOOL			StartListen(UINT16 usPortNum);

	BOOL			StartEpollEvents(UINT32 usMaxConn);

	BOOL			EpollEventOperation(CConnection* pConn, UINT32 nEPollOpt, UINT32 uEpollEvents);

	CConnection*	BindConnection(SOCKET nSocket, UINT32 IPAddr,BOOL ConnetcionStatus);

	BOOL			InitConnectionVector(UINT16 usMaxConn);

	CConnection*	RequestFreeConnection();

	CConnection*	GetConnectionByConnID(UINT32 uConnID);

	BOOL			CloseConnection(CConnection* pConn);

	BOOL			CloseAllConnection();

	BOOL			DestroyAllConnection();

private:
	EPOLL						m_nEpollSocket;

	UINT16						m_usPortNum;

	UINT16						m_usMaxConn;

	BOOL						m_bIsRunning;

	SOCKET						m_nListenSocket;

	std::thread*				m_pListenThread;

	std::thread*				m_pEpollEventThread;

	std::vector<CConnection*>	m_vtConnectionVector;

	CConnection*				m_pFreeConnectionHeader;

	IDataHandler*				m_pHigherHandler;

	std::mutex					m_mutex;
};