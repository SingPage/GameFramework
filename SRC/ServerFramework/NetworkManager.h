#pragma once

#define NEW_CONNECT		(0)
#define CLOSE_CONNECT	(1)

class CNetworkManager : public IDataHandler{
	CNetworkManager();
	virtual ~CNetworkManager();
public:
	static CNetworkManager* GetInstancePtr();

	BOOL			StartService(UINT16 usPortNum, UINT16 usMaxConn, INetPacketHandler* pPacketHandler);

	BOOL			CloseService();

	BOOL			NetPacketHandler(NetPacket* pPacket);

	BOOL			NewConnectionHandler(CConnection* pConn);

	BOOL			CloseConnectionHandler(CConnection* pConn);

	CConnection*	AsyncConnectToIP(CHAR* IPAddr, UINT16 usPortNum);

	BOOL			SendDataBuffer(UINT32 uConnID, IDataBuffer* pDataBuff);

	BOOL			SendRawData(UINT32 uConnID, UINT32 uMsgID, UINT64 ulTargetID, const char* pData,
		UINT32 uSize);

	BOOL			NetPacketHandler();

private:
	ArrayLockFreeQueue<NetPacket*>	m_RecvQueue;

	INetPacketHandler*				m_pPacketHandler;
};