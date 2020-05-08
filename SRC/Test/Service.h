#pragma once

class Service: public INetPacketHandler {
	Service();
	virtual ~Service();
public:
	static Service*	GetInstancePtr();

	BOOL			StartNetwork();

	BOOL			CloseNetwork();

	virtual BOOL	DispatchPacket(NetPacket* pPacket) override;
private:
	CConnection*	m_pClient;
};