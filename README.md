版本:Alpha

GCC:4.8.5

适用平台:Linux

现有功能： 

        1、基于EPOLL开发的网络层框架，网络层为双线程，EPOLL事件一个线程，LISTEN一个线程，线程中采用无锁循环队列进行数据交互，框架已包含一个简单的服务端与客户端模型。 
        2、由LUAmanager类封装了LUA调用功能，通过使用luaManager类可以实现与LUA之间的交互。 
        3、日志，内存池等常用模块

下一步开发计划： 

        1、封装MYSQL等数据库的交互 
        2、封装PYTHON交互
