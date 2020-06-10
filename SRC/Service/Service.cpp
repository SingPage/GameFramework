#include"../ServerFramework/stdafx.h"
#include"Service.h"

Service::Service() {

}

Service::~Service(){
	CNetworkManager::GetInstancePtr()->CloseService();
}

Service* Service::GetInstancePtr(){

	static Service Service;

	return &Service;
}

BOOL Service::StartNetwork(){

	if (!CNetworkManager::GetInstancePtr()->StartService(m_nPort, m_nMaxConnection, this, TRUE)) {
		LOG_ERROR_OUTPUT("CNetworkManager StartService fail");
		return FALSE;
	};

	LOG_DEBUG_OUTPUT("CNetworkManager StartService success");
	return TRUE;
}

BOOL Service::CloseNetwork(){

	CNetworkManager::GetInstancePtr()->CloseService();

	return TRUE;
}

BOOL Service::MessageDispatch(NetPacket* pPacket){

	if (NEW_CONNECT == pPacket->pHeader.uMsgID) {
		CConnection* pConn = (CConnection*)pPacket->pBuffer;
		m_pClient = pConn;
		printf("New connect:ConnId:%u\n", (pConn->GetConnectionID()));

		CNetworkManager::GetInstancePtr()->SendRawData(m_pClient->GetConnectionID(), 0x2, 0x6, "Connect ACK", 12);
	}
	else if (CLOSE_CONNECT == pPacket->pHeader.uMsgID) {
		CConnection* pConn = (CConnection*)pPacket->pBuffer;
		m_pClient = NULL;
		printf("Close connect:ConnId:%u\n", (pConn->GetConnectionID()));
	}
	else {

		printf("MsgID:%u ConnID:%u Data:%*s\n",
			pPacket->pHeader.uMsgID, pPacket->uConnID, pPacket->pHeader.uSize - sizeof(PacketHeader),
			pPacket->pBuffer->GetBufferPtr() + sizeof(PacketHeader));

		CNetworkManager::GetInstancePtr()->SendRawData(m_pClient->GetConnectionID(), 0x3, 0x6, "Service", 8);
	}

	return TRUE;
}

BOOL Service::InitClientConfig(char * szConfigFileName){

	INT32 nRet;

	nRet = CConfigFile::GetInstancePtr()->LoadConfigFile(szConfigFileName);
	if (FALSE == nRet) {
		LOG_ERROR_OUTPUT("LoadConfigFile Error");
		return FALSE;
	}

	nRet = CConfigFile::GetInstancePtr()->GetIntValue("Self_Port", m_nPort);
	if (FALSE == nRet) {
		LOG_ERROR_OUTPUT("Get Self_Port error");
		return FALSE;
	}

	nRet = CConfigFile::GetInstancePtr()->GetIntValue("Max_Connection", m_nMaxConnection);
	if (FALSE == nRet) {
		LOG_ERROR_OUTPUT("Get Max_Connection error");
		return FALSE;
	}

	return TRUE;
}
