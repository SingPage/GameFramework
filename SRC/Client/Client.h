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

	BOOL			InitClientConfig(char* szConfigFileName);

	// Í¨¹ý INetPacketHandler ¼Ì³Ð
	virtual BOOL	MessageDispatch(NetPacket * pPacket) override;

private:
	CConnection*	m_pService;

	std::string		m_strServiceIp;

	INT32			m_strServicePort;

	INT32			m_nMaxConnection;
};