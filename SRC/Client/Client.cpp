#include"../ServerFramework/stdafx.h"
#include"../ServerFramework/Connection.h"
#include"Client.h"

Client::Client() {
	
	m_pService = NULL;
}

Client::~Client(){

}

BOOL Client::MessageDispatch(NetPacket* pPacket){

	if (NEW_CONNECT == pPacket->pHeader.uMsgID) {
		CConnection* pConn = (CConnection*)pPacket->pBuffer;
		m_pService = pConn;

		printf("New connect:ConnId:%u\n", (pConn->GetConnectionID()));

		CNetworkManager::GetInstancePtr()->SendRawData(m_pService->GetConnectionID(), 0x5, 0x6, "Connect ACK", 12);
	}
	else if (CLOSE_CONNECT == pPacket->pHeader.uMsgID) {
		CConnection* pConn = (CConnection*)pPacket->pBuffer;
		m_pService = pConn;

		printf("Close connect:ConnId:%u\n", (pConn->GetConnectionID()));
	}
	else {
		printf("MsgID:%u ConnID:%u Data:%*s\n",
			pPacket->pHeader.uMsgID, pPacket->uConnID, pPacket->pHeader.uSize - sizeof(PacketHeader),
			pPacket->pBuffer->GetBufferPtr() + sizeof(PacketHeader));

		CNetworkManager::GetInstancePtr()->SendRawData(m_pService->GetConnectionID(), 0x6, 0x6, "Client", 7);
	}

	return TRUE;
}

BOOL Client::StartNetwork() {

	if (!CNetworkManager::GetInstancePtr()->StartService(NULL, m_nMaxConnection, this, FALSE)) {
		LOG_ERROR_OUTPUT("CNetworkManager StartService fail");
		return FALSE;
	};

	LOG_DEBUG_OUTPUT("CNetworkManager StartService success");
	return TRUE;
}

BOOL Client::CloseNetwork() {

	CNetworkManager::GetInstancePtr()->CloseService();

	return TRUE;
}

CConnection* Client::ConnectToService(){

	return CNetworkManager::GetInstancePtr()->AsyncConnectToIP(m_strServiceIp.c_str(), m_strServicePort);
}

BOOL Client::InitClientConfig(char * szConfigFileName){

	INT32 nRet;

	nRet = CConfigFile::GetInstancePtr()->LoadConfigFile(szConfigFileName);
	if (FALSE == nRet) {
		LOG_ERROR_OUTPUT("LoadConfigFile Error");
		return FALSE;
	}

	nRet = CConfigFile::GetInstancePtr()->GetStringValue("Service_Ip", m_strServiceIp);
	if (FALSE == nRet) {
		LOG_ERROR_OUTPUT("Get m_strServiceIp error");
		return FALSE;
	}

	nRet = CConfigFile::GetInstancePtr()->GetIntValue("Service_Port", m_strServicePort);
	if (FALSE == nRet) {
		LOG_ERROR_OUTPUT("Get m_strServiceIp error");
		return FALSE;
	}

	nRet = CConfigFile::GetInstancePtr()->GetIntValue("Max_Connection", m_nMaxConnection);
	if (FALSE == nRet) {
		LOG_ERROR_OUTPUT("Get Max_Connection error");
		return FALSE;
	}

	return TRUE;
}
