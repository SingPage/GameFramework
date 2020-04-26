#include"stdafx.h"
#include"Connection.h"
#include"ConnManager.h"
#include "NetworkManager.h"

CNetworkManager::CNetworkManager() {

	m_pPacketHandler = NULL;

}

CNetworkManager::~CNetworkManager(){


}

CNetworkManager* CNetworkManager::GetInstancePtr(){

	static CNetworkManager NetworkManager;

	return &NetworkManager;
}

BOOL CNetworkManager::StartService(UINT16 usPortNum, UINT16 usMaxConn, INetPacketHandler* pPacketHandler){

	if (!CConnectionManager::GetInstancePtr()->Start(usPortNum, usMaxConn, this)) {
		LOG_ERROR_OUTPUT("Network start fail");
		return FALSE;
	}

	m_pPacketHandler = pPacketHandler;

	return TRUE;
}

BOOL CNetworkManager::CloseService(){

	if(!CConnectionManager::GetInstancePtr()->Close()) {
		LOG_ERROR_OUTPUT("Network close fail");
		return FALSE;
	}

	return TRUE;
}

BOOL CNetworkManager::NetPacketHandler(NetPacket* pPacket){

	return m_RecvQueue.push(pPacket);
}

BOOL CNetworkManager::NewConnectionHandler(CConnection* pConn) {

	IDataBuffer* pSelf = CMemoryPoll::GetInstancePtr()->GetDataBuffer(sizeof(NetPacket));
	NetPacket* pPacket = (NetPacket*)pSelf->GetBufferPtr();

	
	pPacket->pBuffer = (IDataBuffer*)pConn;
	pPacket->pHeader.uMsgID = NEW_CONNECT;
	pPacket->uConnID = pConn->GetConnectionID();
	pPacket->pSelf = pSelf;

	return m_RecvQueue.push(pPacket);
}

BOOL CNetworkManager::CloseConnectionHandler(CConnection* pConn){

	IDataBuffer* pSelf = CMemoryPoll::GetInstancePtr()->GetDataBuffer(sizeof(NetPacket));
	NetPacket* pPacket = (NetPacket*)pSelf->GetBufferPtr();


	pPacket->pBuffer = (IDataBuffer*)pConn;
	pPacket->pHeader.uMsgID = CLOSE_CONNECT;
	pPacket->uConnID = pConn->GetConnectionID();
	pPacket->pSelf = pSelf;

	return m_RecvQueue.push(pPacket);
}

CConnection* CNetworkManager::AsyncConnectToIP(CHAR* IPAddr, UINT16 usPortNum){

	if ((NULL == IPAddr) || (0 == usPortNum)) {
		LOG_ERROR_OUTPUT("AsyncConnectToIP error");
		return FALSE;
	}
	//调用连接函数
	CConnection* pConn = CConnectionManager::GetInstancePtr()->AsyncConnect(IPAddr, usPortNum);
	if (NULL == pConn) {
		LOG_ERROR_OUTPUT("AsyncConnect error");
		return NULL;
	};

	return pConn;
}

BOOL CNetworkManager::SendDataBuffer(UINT32 uConnID, IDataBuffer* pDataBuff){

	if (!CConnectionManager::GetInstancePtr()->SendNetPacketByConnID(uConnID, pDataBuff)) {
		LOG_ERROR_OUTPUT("SendDataBuffer fail");
		return FALSE;
	}

	return TRUE;
}

BOOL CNetworkManager::SendRawData(UINT32 uConnID, UINT32 uMsgID, UINT64 ulTargetID, const char * pData, UINT32 uSize){

	IDataBuffer* pSendPacket = NULL;
	PacketHeader* pHeader = NULL;

	pSendPacket = CMemoryPoll::GetInstancePtr()->GetDataBuffer(uSize + sizeof(PacketHeader));
	if (NULL == pSendPacket) {
		LOG_ERROR_OUTPUT("CMemoryPoll::GetInstancePtr()->GetDataBuffer error, memory size:%u", uSize + sizeof(PacketHeader));
		return FALSE;
	}

	pHeader = (PacketHeader*)pSendPacket->GetBufferPtr();
	pHeader->ulTargetID = htonl(ulTargetID);
	pHeader->uCheckCode = htonl(0xff);
	pHeader->uMsgID = htonl(uMsgID);
	pHeader->uPacketNo = htonl(1);
	pHeader->uSize = htonl(uSize + sizeof(PacketHeader));

	memcpy((CHAR*)pHeader + sizeof(PacketHeader), pData, uSize);
	pSendPacket->SetDataLength(uSize + sizeof(PacketHeader));

	if (!CConnectionManager::GetInstancePtr()->SendNetPacketByConnID(uConnID, pSendPacket)) {
		LOG_ERROR_OUTPUT("SendRawData:SendNetPacketByConnID error");
		pSendPacket->Free();
		return FALSE;
	}

	return TRUE;
}

BOOL CNetworkManager::NetPacketHandler(){

	NetPacket* pPacket;

	if (m_RecvQueue.pop(pPacket)) {
		if (m_pPacketHandler->DispatchPacket(pPacket)) {

			if ((pPacket->pHeader.uMsgID != NEW_CONNECT) && (pPacket->pHeader.uMsgID != CLOSE_CONNECT)) {
				pPacket->pBuffer->Free();
			}
			pPacket->pSelf->Free();

			return TRUE;
		}
		LOG_ERROR_OUTPUT("m_pPacketHandler->DispatchPacket fail");
		return FALSE;
	}
	return FALSE;
}
