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

	if (!CNetworkManager::GetInstancePtr()->StartService(10010, 2000, this)) {
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

BOOL Service::DispatchPacket(NetPacket* pPacket){

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
