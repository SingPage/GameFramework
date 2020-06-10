#include"../ServerFramework/stdafx.h"
#include"../ServerFramework/Connection.h"
#include"Service.h"

int main() {

	CNetworkManager* Network = CNetworkManager::GetInstancePtr();

	CLog::GetInstancePtr()->StartLog("Service", "LOG_Service");
	Service::GetInstancePtr()->InitClientConfig("Service_Config.ini");
	Service::GetInstancePtr()->StartNetwork();



	for (INT32 i = 0; TRUE;) {
		
		Network->NetPacketHandler();

	}

	Service::GetInstancePtr()->CloseNetwork();

	return 0;
}





//int main() {
//	clock_t start, end;
//	IDataBuffer* mem[100000];
//	CMemoryPoll* Memory = CMemoryPoll::GetInstancePtr();
//
//	start = clock();
//	for (int i = 0; i < 100000; i++) {
//		mem[i] = Memory->GetDataBuffer(128);
//	}
//	
//	end = clock();
//
//	double seconds = (double)(end - start) / CLOCKS_PER_SEC;
//
//	printf("CMemoryPoll::GetInstancePtr()->GetDataBuffer(128):%lf s\n", seconds);
//
//	return TRUE;
//}