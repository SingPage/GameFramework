#pragma once

class Service: public INetPacketHandler {
	Service();
	virtual ~Service();
public:
	static Service*	GetInstancePtr();

	BOOL			StartNetwork();

	BOOL			CloseNetwork();

	virtual BOOL	MessageDispatch(NetPacket* pPacket) override;

	BOOL			InitClientConfig(char* szConfigFileName);
private:
	CConnection*	m_pClient;

	INT32			m_nPort;

	INT32			m_nMaxConnection;
};