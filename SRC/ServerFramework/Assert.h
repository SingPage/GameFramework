#pragma once

#define FUNCTION_RUNNING {try{
#define FUNCTION_FINISHING }catch(...){CLog::GetInstancePtr()->LogOutput(CLog::LogLevel::LogError,\
 "FILE:%s, LINE:%u, FUNCTION:%s", __FILE__, __LINE__, __FUNCTION__);}}