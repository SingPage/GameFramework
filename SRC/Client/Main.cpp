#include"../ServerFramework/stdafx.h"
#include"Client.h"

int main() {
	CNetworkManager* Network = CNetworkManager::GetInstancePtr();

	CLog::GetInstancePtr()->StartLog("Client", "LOG_Client");
	Client::GetInstancePtr()->InitClientConfig("Client_Config.ini");
	Client::GetInstancePtr()->StartNetwork();
	Client::GetInstancePtr()->ConnectToService();


	while (TRUE) {

		Network->NetPacketHandler();
	}

	Client::GetInstancePtr()->CloseNetwork();
	CLog::GetInstancePtr()->CloseLog();

	return 0;
}