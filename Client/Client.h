#pragma once

class Client : public INetPacketHandler {
private:
	Client();
	virtual ~Client();
public:
	static Client* GetInstancePtr() {
			static Client ClassObj; 
			return &ClassObj;
	}

	BOOL			StartNetwork();

	BOOL			CloseNetwork();

	CConnection*	ConnectToService();

	virtual BOOL	DispatchPacket(NetPacket* pPacket) override;

private:

	CConnection*	m_pService;

};