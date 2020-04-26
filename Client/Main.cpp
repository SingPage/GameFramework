#include"../ServerFramework/stdafx.h"
#include"Client.h"

int main() {
	CLog::GetInstancePtr()->StartLog("Client", "LOG_Client");
	Client::GetInstancePtr()->StartNetwork();
	Client::GetInstancePtr()->ConnectToService();

	while (TRUE) {
		CNetworkManager::GetInstancePtr()->NetPacketHandler();
		sleep(1);
	}

	Client::GetInstancePtr()->CloseNetwork();
	CLog::GetInstancePtr()->CloseLog();
}