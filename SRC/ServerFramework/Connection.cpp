#include"stdafx.h"
#include"Connection.h"
#include"ConnManager.h"

CConnection::CConnection(CConnectionManager* pManager){

	FUNCTION_RUNNING

	m_nSocket = 0;

	m_uIpAddr = 0;

	m_bConnectedStatus = FALSE;

	m_uConnID = 0;

	m_pRecvPacket.pBuffer = CMemoryPoll::GetInstancePtr()->GetDataBuffer(8192);

	m_pRecvPacket.Init();
	
	m_pSendPacket.pBuffer = NULL;

	m_pSendPacket.Init();

	m_pPreConnection = NULL;

	m_pNextConnection = NULL;

	m_pManager = pManager;

	FUNCTION_FINISHING
}

CConnection::~CConnection(){

	FUNCTION_RUNNING

	m_pRecvPacket.pBuffer->Free();

	FUNCTION_FINISHING
}

BOOL CConnection::Close(){

	FUNCTION_RUNNING

	IDataBuffer* pBuff;

	shutdown(m_nSocket, SHUT_RDWR);
	close(m_nSocket);

	m_nSocket = -1;

	m_bConnectedStatus = FALSE;

	m_pSendPacket.Init();

	m_pRecvPacket.Init();

	while (m_SendBuffQueue.pop(pBuff)) {
		continue;
	}

	return TRUE;

	FUNCTION_FINISHING
}

VOID CConnection::SetConnectedStatus(BOOL bStatus){

	FUNCTION_RUNNING

	m_bConnectedStatus = bStatus;

	FUNCTION_FINISHING
}

VOID CConnection::SetConnectionID(UINT32 uConnID){

	FUNCTION_RUNNING

	m_uConnID = uConnID;

	FUNCTION_FINISHING
}

UINT32 CConnection::GetConnectionID(){

	FUNCTION_RUNNING

	return m_uConnID;

	FUNCTION_FINISHING
}

BOOL CConnection::GetConnectedStatus(){

	FUNCTION_RUNNING

	return m_bConnectedStatus;

	FUNCTION_FINISHING
}

UINT32 CConnection::GetIpAddr(){

	FUNCTION_RUNNING

	return m_uIpAddr;

	FUNCTION_FINISHING
}

VOID CConnection::SetIpAddr(UINT32 IpAddr){

	FUNCTION_RUNNING

	m_uIpAddr = IpAddr;

	FUNCTION_FINISHING
}

BOOL CConnection::ReceiveData(){

	FUNCTION_RUNNING

	INT32 nBytes = 0;

	while (TRUE) {
		nBytes = recv(m_nSocket, m_pRecvPacket.pCurPos, m_pRecvPacket.uBufferSize, 0);
		

		if (nBytes < 0) {
			if (EAGAIN == errno) {
				return TRUE;
			}
			else {
				LOG_ERROR_OUTPUT("ReceiveData, errno: %d, error message:%s", errno, strerror(errno));
				return FALSE;
			}
		}
		else if (0 == nBytes) {
			return FALSE;
		}
		else {
			m_pRecvPacket.uDataSize += nBytes;

			if (nBytes < m_pRecvPacket.uDataSize) {
				return TRUE;
			}

			if (!ParseBuffer()) {
				return FALSE;
			}
		}
	}
	
	return TRUE;

	FUNCTION_FINISHING
}

VOID CConnection::SetSocket(SOCKET socket){

	FUNCTION_RUNNING

	m_nSocket = socket;

	FUNCTION_FINISHING
}

SOCKET CConnection::GetSocket(){

	FUNCTION_RUNNING

	return m_nSocket;

	FUNCTION_FINISHING
}

BOOL CConnection::PushSendQueue(IDataBuffer* pBuff){

	FUNCTION_RUNNING

	PacketHeader* pheader = (PacketHeader*)pBuff->GetBufferPtr();

	if (m_SendBuffQueue.push(pBuff)) {
		return TRUE;
	}

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CConnection::SendData(){

	FUNCTION_RUNNING

	INT32 nRet = 0;
	IDataBuffer* pBuff = NULL;
	//First send last time
	while (TRUE) {

		if (NULL != m_pSendPacket.pBuffer) {
			nRet = send(m_nSocket, m_pSendPacket.pCurPos, m_pSendPacket.uDataSize, 0);
			//printf("m_pSendPacket.pCurPos: %s\n", m_pSendPacket.pCurPos + sizeof(PacketHeader));

			if (nRet < m_pSendPacket.uDataSize) {
				if ((nRet < 0) && (nRet != EAGAIN)) {
					m_pSendPacket.pBuffer->Free();
					LOG_ERROR_OUTPUT("error: send(m_nSocket, m_pSendPacket.pCurPos, m_pSendPacket.uDataSize, 0)\
					return %u", nRet);
					return SEND_ERROR;
				}
				else {
					m_pSendPacket.pCurPos = m_pSendPacket.pCurPos + m_pSendPacket.uDataSize;
					m_pSendPacket.uDataSize -= nRet;

					return SEND_UNFINISHED;
				}
			}
			else {
				m_pSendPacket.pBuffer->Free();
				m_pSendPacket.pBuffer = NULL;
				m_pSendPacket.pCurPos = NULL;
				m_pSendPacket.uBufferSize = 0;
				m_pSendPacket.uDataSize = 0;
			}
		}

		if (!m_SendBuffQueue.pop(pBuff)) {

			return SEND_SUCCESS;
		}
		else {
			m_pSendPacket.pBuffer = pBuff;
			m_pSendPacket.pCurPos = pBuff->GetBufferPtr();
			m_pSendPacket.uBufferSize = pBuff->GetBufferLength();
			m_pSendPacket.uDataSize = pBuff->GetDataLength();
		}
	}

	return SEND_SUCCESS;

	FUNCTION_FINISHING
}

BOOL CConnection::ParseBuffer(){

	FUNCTION_RUNNING

	IDataBuffer* pPacketBuff = NULL;
	UINT32 uPacketSize = 0;
	UINT32 uRemainSize = 0;

	while (TRUE) {

		if (0 == m_pRecvPacket.uDataSize) {

			m_pRecvPacket.pCurPos = m_pRecvPacket.pBuffer->GetBufferPtr();

			return TRUE;
		}

		if (NULL == m_pRecvPacket.pBuffer || NULL == m_pRecvPacket.pCurPos) {
			LOG_ERROR_OUTPUT("error: m_recvPacket->pBuffer or m_recvPacket->pCurPos is \"NULL\"");
			return FALSE;
		}
		
		if (NULL == m_pRecvPacket.pCachBuffer) {
			uPacketSize = ntohl(((PacketHeader*)m_pRecvPacket.pCurPos)->uSize);
			pPacketBuff = CMemoryPoll::GetInstancePtr()->GetDataBuffer(uPacketSize);
			pPacketBuff->SetDataLength(0);
			m_pRecvPacket.pCachBuffer = pPacketBuff;
			//New packet verify packet validity
			//if (!CheckPacket(pPacketBuff)) {
			//	LOG_ERROR_OUTPUT("error: PacketHeader error");
			//	return FALSE;
			//}

		}
		else {
			pPacketBuff = m_pRecvPacket.pCachBuffer;
			uPacketSize = ntohl(((PacketHeader*)m_pRecvPacket.pCachBuffer->GetBufferPtr())->uSize);
		}

		uRemainSize = uPacketSize - pPacketBuff->GetDataLength();
		if (m_pRecvPacket.uDataSize >= uRemainSize) {

			memcpy(pPacketBuff->GetDataTail(), m_pRecvPacket.pCurPos, uRemainSize);
			//m_pRecvPacket.pCurPos = m_pRecvPacket.pBuffer->GetBufferPtr() + uRemainSize;
			m_pRecvPacket.pCurPos = m_pRecvPacket.pCurPos + uRemainSize;

			m_pRecvPacket.uDataSize -= uRemainSize;
			pPacketBuff->SetDataLength(uRemainSize + pPacketBuff->GetDataLength());

			m_pManager->RecvConnectionData(m_uConnID, pPacketBuff);
			m_pRecvPacket.pCachBuffer = NULL;
		}
		else {
			memcpy(pPacketBuff->GetDataTail(), m_pRecvPacket.pCurPos, m_pRecvPacket.uDataSize);
			m_pRecvPacket.pCurPos = m_pRecvPacket.pBuffer->GetBufferPtr();
			m_pRecvPacket.uDataSize = 0;
			pPacketBuff->SetDataLength(pPacketBuff->GetDataLength() + m_pRecvPacket.uDataSize);
		}
	}

	return TRUE;

	FUNCTION_FINISHING
}

BOOL CConnection::CheckPacket(IDataBuffer* pPacket){

	FUNCTION_RUNNING

	PacketHeader* pPacketHeader = (PacketHeader*)(pPacket->GetBufferPtr());
	if (ntohl(pPacketHeader->uCheckCode) != 0xff){

		LOG_ERROR_OUTPUT("error: pPacketHeader->uCheckCode != 0xff");
		return FALSE;
	}

	if (ntohl(pPacketHeader->uSize) > 65536){

		LOG_ERROR_OUTPUT("error: pPacketHeader->uSize > 65536");
		return FALSE;
	}

	if (ntohl(pPacketHeader->uMsgID) > 999999){

		LOG_ERROR_OUTPUT("error: pPacketHeader->uMsgID > 999999");
		return FALSE;
	}

	return TRUE;

	FUNCTION_FINISHING
}
