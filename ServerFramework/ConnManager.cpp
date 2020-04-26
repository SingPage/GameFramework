#include"stdafx.h"
#include"Connection.h"
#include"ConnManager.h"

CConnectionManager::CConnectionManager(){

	m_nEpollSocket = 0;

	m_usPortNum = 0;

	m_usMaxConn = 0;

	m_bIsRunning = 0;

	m_nListenSocket = 0;

	m_pListenThread = NULL;

	m_pEpollEventThread = NULL;

	m_pFreeConnectionHeader = NULL;

	m_pHigherHandler = NULL;
}

CConnectionManager::~CConnectionManager(){

}

CConnectionManager* CConnectionManager::GetInstancePtr(){

	static CConnectionManager connManager;

	return &connManager;
}

BOOL CConnectionManager::Start(UINT16 usPortNum, UINT16 usMaxConn, IDataHandler* pDataHandler){

	if (FALSE == m_bIsRunning) {

		m_usPortNum = usPortNum;
		m_pHigherHandler = pDataHandler;
		m_bIsRunning = TRUE;

		InitConnectionVector(usMaxConn);

		if (!StartEpollEvents(usMaxConn)) {
			LOG_ERROR_OUTPUT("StartEpollEvents fail");
			return FALSE;
		}

		if (!StartListen(usPortNum)) {
			LOG_ERROR_OUTPUT("StartListen fail");
			return FALSE;
		};
		
		return TRUE;
	}

	return FALSE;
}

BOOL CConnectionManager::Close(){

	if (m_bIsRunning) {

		m_bIsRunning = FALSE;

		CloseListenThread();

		CLoseEpollEventsThread();

		CloseAllConnection();

		DestroyAllConnection();
	}

	return TRUE;
}

BOOL CConnectionManager::SendNetPacketByConnID(UINT32 uConnID, IDataBuffer* pBuff){

	CConnection* pConn = NULL;

	pConn = GetConnectionByConnID(uConnID);
	if (NULL == pConn) {
		LOG_ERROR_OUTPUT("GetConnectionByConnID error, uConnID: %u", uConnID);
		return FALSE;
	}

	if (!pConn->GetConnectedStatus()) {
		LOG_ERROR_OUTPUT("ConnectionID:%u, connection closed", uConnID);
		return FALSE;
	}

	if (!pConn->PushSendQueue(pBuff)) {
		LOG_ERROR_OUTPUT("pConn->PushSendQueue error");
		pBuff->Free();
		return FALSE;
	}

	EpollEventOperation(pConn, EPOLL_CTL_MOD, EPOLLOUT);

	return TRUE;
}

BOOL CConnectionManager::RecvConnectionData(UINT32 pConnID, IDataBuffer * pBuff){

	if ((0 == pConnID) || (NULL == pBuff)) {
		return FALSE;
	}

	IDataBuffer* pSelf = CMemoryPoll::GetInstancePtr()->GetDataBuffer(sizeof(NetPacket));
	NetPacket* pPacket = (NetPacket*)pSelf->GetBufferPtr();
	PacketHeader* pHeader = (PacketHeader*)pBuff->GetBufferPtr();

	//Parse Packet
	pPacket->pHeader.uMsgID = ntohl(pHeader->uMsgID);
	pPacket->pHeader.uCheckCode = ntohl(pHeader->uCheckCode);
	pPacket->pHeader.ulTargetID = ntohl(pHeader->ulTargetID);
	pPacket->pHeader.uPacketNo = ntohl(pHeader->uPacketNo);
	pPacket->pHeader.uSize = ntohl(pHeader->uSize);

	pPacket->pBuffer = pBuff;
	pPacket->uConnID = pConnID;
	pPacket->pSelf = pSelf;

	m_pHigherHandler->NetPacketHandler(pPacket);

	return TRUE;
}

BOOL CConnectionManager::StartListen(UINT16 usPortNum){

	sockaddr_in ServerAddr;
	INT32 nReuse = 1;

	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(usPortNum);
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	m_nListenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == m_nListenSocket) {
		LOG_ERROR_OUTPUT("Create m_nListenSocket fail:%s", strerror(errno));
		return FALSE;
	}

	setsockopt(m_nListenSocket, SOL_SOCKET, SO_REUSEADDR, &nReuse, sizeof(nReuse));

	if (0 != bind(m_nListenSocket, (sockaddr*)&ServerAddr, sizeof(ServerAddr))){
		LOG_ERROR_OUTPUT("Bind m_nListenSocket fail:%s", strerror(errno));
		return FALSE;
	};

	if (0 != listen(m_nListenSocket, 30)) {
		LOG_ERROR_OUTPUT("Listen m_nListenSocket fail:%s", strerror(errno));
		return FALSE;
	};

	//start listen thread
	m_pListenThread = new std::thread(&CConnectionManager::Thread_ListenSocket, this);
	if (NULL == m_pListenThread) {
		LOG_ERROR_OUTPUT("Thread_ListenSocket create fail:%s", strerror(errno));
		return FALSE;
	}

	return TRUE;
}

BOOL CConnectionManager::Thread_HandleEpollEvents(){

	struct epoll_event EpollEvent[20];
	CConnection* pConnection = NULL;
	INT32 nRet = 0;
	INT32 nSocketError = 0;
	socklen_t uLen;

	while (m_bIsRunning) {
		nRet = epoll_wait(m_nEpollSocket, EpollEvent, 20, 1000);
		if (-1 == nRet) {
			continue;
		}

		for (int i = 0; i < nRet; i++) {

			pConnection = (CConnection*)EpollEvent[i].data.ptr;
			if (NULL == pConnection) {
				LOG_ERROR_OUTPUT("EpollEvent[%d].data.ptr is \"NULL\"", i);
				continue;
			}

			if ((EpollEvent[i].events & EPOLLERR) || (EpollEvent[i].events & EPOLLHUP)) {
				EpollEventOperation(pConnection, EPOLL_CTL_DEL, 0);
				pConnection->Close();
				m_pHigherHandler->CloseConnectionHandler(pConnection);
				continue;
			}

			if (getsockopt(pConnection->GetSocket(), SOL_SOCKET, SO_ERROR, &nSocketError, &uLen) < 0) {
				LOG_ERROR_OUTPUT("getsockopt fail:%s", strerror(errno));
				continue;
			}

			if (0 != nSocketError) {
				if (EpollEvent[i].events & EPOLLIN) {
					LOG_ERROR_OUTPUT("EPOLLIN fail");
				}

				if (EpollEvent[i].events & EPOLLOUT) {
					LOG_ERROR_OUTPUT("EPOLLOUT fail");
				}

				continue;
			}

			if (EpollEvent[i].events & EPOLLIN) {
				if (!pConnection->GetConnectedStatus()) {

					pConnection->SetConnectedStatus(TRUE);
					m_pHigherHandler->NewConnectionHandler(pConnection);
				}
				if (!pConnection->ReceiveData()) {
					EpollEventOperation(pConnection, EPOLL_CTL_DEL, 0);
					pConnection->Close();
					m_pHigherHandler->CloseConnectionHandler(pConnection);
					continue;
				}

				EpollEventOperation(pConnection, EPOLL_CTL_MOD, EPOLLOUT);
			}

			if (EpollEvent[i].events & EPOLLOUT) {
				if (!pConnection->GetConnectedStatus()) {

					pConnection->SetConnectedStatus(TRUE);
					m_pHigherHandler->NewConnectionHandler(pConnection);
				}
				INT32 nRet = pConnection->SendData();
				if (SEND_SUCCESS == nRet) {
					EpollEventOperation(pConnection, EPOLL_CTL_MOD, EPOLLIN);
				}
				else if (SEND_ERROR == nRet) {

					EpollEventOperation(pConnection, EPOLL_CTL_DEL, 0);
					pConnection->Close();
					continue;
				}
				else if (SEND_UNFINISHED == nRet) {

					EpollEventOperation(pConnection, EPOLL_CTL_MOD, EPOLLOUT);
				}
			}
		}
	}

	return TRUE;
}

BOOL CConnectionManager::CLoseEpollEventsThread(){

	shutdown(m_nEpollSocket, SHUT_RDWR);

	close(m_nEpollSocket);

	m_pEpollEventThread->join();

	return TRUE;
}

BOOL CConnectionManager::StartEpollEvents(UINT32 usMaxConn){

	m_nEpollSocket = epoll_create(usMaxConn);
	if (-1 == m_nEpollSocket) {
		LOG_ERROR_OUTPUT("epoll_create fail, m_nEpollSocket is -1:%s", strerror(errno));
		return FALSE;
	}

	m_pEpollEventThread = new std::thread(&CConnectionManager::Thread_HandleEpollEvents, this);

	return TRUE;
}

BOOL CConnectionManager::EpollEventOperation(CConnection* pConn, UINT32 nEPollOpt, UINT32 uEpollEvents) {

	epoll_event EpollEvent;
	EpollEvent.data.ptr = pConn;
	EpollEvent.events = uEpollEvents | EPOLLET;

	INT32 nRet = epoll_ctl(m_nEpollSocket, nEPollOpt, pConn->GetSocket(), &EpollEvent);
	if (-1 == nRet) {
		LOG_ERROR_OUTPUT("epoll_ctl error:%s", strerror(errno));
		return FALSE;
	}

	return TRUE;
}

BOOL CConnectionManager::Thread_ListenSocket(){

	sockaddr_in ClientAddr;
	socklen_t nLen = sizeof(ClientAddr);
	SOCKET nClientSocket = 0;
	CConnection* pConnection = NULL;

	while (m_bIsRunning) {
		memset(&ClientAddr, 0, nLen);
		nClientSocket = accept(m_nListenSocket, (sockaddr*)&ClientAddr, &nLen);
		if (-1 == nClientSocket) {

			LOG_ERROR_OUTPUT("accept m_nListenSocket fail:%s", strerror(errno));
			return FALSE;
		}

		fcntl(nClientSocket, F_SETFL, fcntl(nClientSocket, F_GETFL, 0) | O_NONBLOCK);

		pConnection = BindConnection(nClientSocket, ClientAddr.sin_addr.s_addr, TRUE);

		if (NULL != pConnection) {
			//New Connect
			m_pHigherHandler->NewConnectionHandler(pConnection);
		}
		else {

			LOG_ERROR_OUTPUT("BindConnection error, return NULL");
		}

	}

	return TRUE;
}

BOOL CConnectionManager::CloseListenThread(){

	shutdown(m_nListenSocket, SHUT_RDWR);
	close(m_nListenSocket);

	m_pListenThread->join();

	return TRUE;
}

CConnection* CConnectionManager::BindConnection(SOCKET nSocket, UINT32 IPAddr, BOOL ConnetcionStatus){

	CConnection* pConnection;
	UINT32 EpollEvents;

	if ((-1 == nSocket) || (0 == nSocket)) {
		return NULL;
	}
	pConnection = RequestFreeConnection();
	if (NULL == pConnection) {
		LOG_ERROR_OUTPUT("RequestFreeConnection fail, pConnection is NULL");
		return FALSE;
	}

	pConnection->m_nSocket = nSocket;
	pConnection->m_uIpAddr = IPAddr;
	pConnection->m_bConnectedStatus = ConnetcionStatus;

	if (ConnetcionStatus) {
		EpollEvents = EPOLLIN | EPOLLOUT;
	}
	else {
		EpollEvents = EPOLLIN;
		
	}

	if (!EpollEventOperation(pConnection, EPOLL_CTL_ADD, EpollEvents)) {
		pConnection->Close();
		return NULL;
	}

	return pConnection;
}

BOOL CConnectionManager::InitConnectionVector(UINT16 usMaxConn){

	CConnection* pConnection = NULL;

	m_mutex.lock();
	if (0 == m_vtConnectionVector.size()) {

		m_vtConnectionVector.assign(usMaxConn, NULL);

		for (INT32 i = 0; i < usMaxConn; i++) {
			pConnection = new CConnection(this);
			m_vtConnectionVector.at(i) = pConnection;

			if (NULL != m_pFreeConnectionHeader) {
				m_pFreeConnectionHeader->m_pPreConnection = pConnection;
			}
			pConnection->m_pNextConnection = m_pFreeConnectionHeader;
			m_pFreeConnectionHeader = pConnection;

			pConnection->SetConnectionID(i + 1);
		}
	}

	m_mutex.unlock();

	return TRUE;
}

CConnection* CConnectionManager::RequestFreeConnection(){

	CConnection* pConnection = NULL;

	m_mutex.lock();
	if (NULL == m_pFreeConnectionHeader) {
		return NULL;
	}

	pConnection = m_pFreeConnectionHeader;
	m_pFreeConnectionHeader = pConnection->m_pNextConnection;
	if (NULL != m_pFreeConnectionHeader) {
		m_pFreeConnectionHeader->m_pPreConnection = NULL;
	}
	pConnection->m_pNextConnection = NULL;
	m_mutex.unlock();

	return pConnection;
}

CConnection* CConnectionManager::GetConnectionByConnID(UINT32 uConnID){

	CConnection* pConnection = NULL;

	if ((0 == uConnID) || (uConnID > m_vtConnectionVector.size())) {
		return NULL;
	}
	pConnection = m_vtConnectionVector.at(uConnID - 1);

	if (pConnection->m_uConnID != uConnID) {
		return NULL;
	}

	return pConnection;
}

BOOL CConnectionManager::CloseConnection(CConnection* pConn){

	m_mutex.lock();
	if (NULL == pConn) {

		m_mutex.unlock();
		return FALSE;
	}
	
	pConn->Close();
	pConn->m_pNextConnection = m_pFreeConnectionHeader;
	if (NULL != m_pFreeConnectionHeader) {
		m_pFreeConnectionHeader->m_pPreConnection = pConn;
	}
	m_pFreeConnectionHeader = pConn;

	m_mutex.unlock();
	return TRUE;
}

BOOL CConnectionManager::CloseAllConnection(){

	auto nSize = m_vtConnectionVector.size();

	m_mutex.lock();

	if (nSize > 0) {

		m_vtConnectionVector.at(0)->m_pPreConnection = NULL;
		m_vtConnectionVector.at(nSize - 1)->m_pPreConnection = NULL;

		for (INT32 i = 0; i < m_vtConnectionVector.size(); i++) {
			m_vtConnectionVector.at(i)->Close();
			if ((0 != i) && (nSize - 1 != i)) {
				m_vtConnectionVector.at(i)->m_pNextConnection = m_vtConnectionVector.at(i + 1);
				m_vtConnectionVector.at(i + 1)->m_pPreConnection = m_vtConnectionVector.at(i);
			}
		}
	}

	m_pFreeConnectionHeader = m_vtConnectionVector.at(0);
	m_mutex.unlock();
	return TRUE;
}

BOOL CConnectionManager::DestroyAllConnection(){

	m_mutex.lock();

	if (m_vtConnectionVector.size() > 0){

		for (INT32 i = 0; i < m_vtConnectionVector.size(); i++) {
			m_vtConnectionVector.at(i)->Close();
			delete m_vtConnectionVector.at(i);
		}
	}

	m_vtConnectionVector.clear();
	m_pFreeConnectionHeader = NULL;

	m_mutex.unlock();

	return TRUE;
}

CConnection* CConnectionManager::AsyncConnect(CHAR* IPAddr, UINT16 usPortNum){

	INT32 nSocket = FunctionLib::CreateSocket(AF_INET, SOCK_STREAM);
	if (-1 == nSocket) {
		LOG_ERROR_OUTPUT("AsyncConnect:CreateSocket error");
		return NULL;
	}

	if (FALSE == FunctionLib::SetSocketAttr(nSocket, O_NONBLOCK)) {
		LOG_ERROR_OUTPUT("AsyncConnect:SetSocketAttr error")
		return NULL;
	};

	BOOL bOn = 1;
	if (0 != setsockopt(nSocket, IPPROTO_TCP, TCP_NODELAY, &bOn, sizeof(bOn))) {
		LOG_ERROR_OUTPUT("AsyncConnect:setsockopt error:%s", strerror(errno));
		return NULL;
	}
	
	
	if (FALSE == FunctionLib::ConnectSocket(nSocket, IPAddr, usPortNum)) {
		LOG_ERROR_OUTPUT("AsyncConnect:ConnectSocket error");
		return NULL;
	}

	UINT32 uIPAddr = 0;
	inet_pton(AF_INET, IPAddr, &uIPAddr);
	CConnection* pConn = BindConnection(nSocket, uIPAddr, FALSE);
	if (NULL == pConn) {
		LOG_ERROR_OUTPUT("AsyncConnect:BindConnection error");
		return NULL;
	}

	return pConn;
}
