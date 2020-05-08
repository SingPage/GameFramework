//包含经常使用且不常修改的头文件
#pragma once

#include<mutex>
#include<thread>
#include<cstring>
#include<time.h>
#include<stdio.h>
#include<error.h>
#include<dirent.h>
#include<stdarg.h>
#include<unistd.h>
#include<fcntl.h>
#include<string.h>
#include<vector>
#include<list>
#include<string>
#include<map>
#include<signal.h>
#include<netinet/tcp.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/epoll.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>


#include"Macro.h"
#include"FuncLib.h"
#include"IDataBuffer.h"
#include"DataBuffer.h"
#include"MemoryPoll.h"
#include"LockFreeQueue.h"
#include"Log.h"
#include"Connection.h"
#include"NetworkManager.h"
#include"Assert.h"