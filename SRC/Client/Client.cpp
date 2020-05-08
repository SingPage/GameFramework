#include"../ServerFramework/stdafx.h"
#include"../ServerFramework/Connection.h"
#include"Client.h"

Client::Client() {
	
	m_pService = NULL;
}

Client::~Client(){

}

BOOL Client::DispatchPacket(NetPacket* pPacket){

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

	if (!CNetworkManager::GetInstancePtr()->StartService(10020, 2000, this)) {
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

	return CNetworkManager::GetInstancePtr()->AsyncConnectToIP("127.0.0.1", 10010);;
}
