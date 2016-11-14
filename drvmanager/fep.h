#if !defined(_FEP_H_INCLUDED_)
#define _FEP_H_INCLUDED_

 //所有调用返回 0 ：成功 ，其它 ： 失败

int __stdcall TcpInit(unsigned long MyID);
//该函数提供本机进程号为MyID的初始化工作。
void __stdcall TcpExit();
//该函数提供本机进程号的退出工作。
//一个进程可以多次调用TcpInit(unsigned long MyID)，表明自己有多个不同进程号。
//每调用TcpInit一次 ，退出时相应调用一次TcpExit();

int __stdcall TcpSend(unsigned long TargetIP, unsigned long TargetID, char * data, unsigned long datalen, unsigned long timeout);
//该函数提供向指定的主机TargetIP和进程号TargetID发送信息。以第一次调用TcpInit时的MyID为自己的进程号。

int __stdcall TcpRecv( unsigned long * SourceIP, unsigned long * SourceprocID, char * data, unsigned long * datalen);
//该函数接收信息  ，调用成功时 信息从*SourceIP ，*SourceprocID来，以第一次调用TcpInit时的MyID为自己的进程号。
//该函数阻塞掉用。

int __stdcall TcpSendExt( unsigned long MyID , unsigned long TargetIP, unsigned long TargetprocID, char * data, unsigned long datalen, unsigned long timeout);
//该函数提供向指定的主机TargetIP和进程号TargetprocID发送信息。MyID为自己的进程号。
int __stdcall TcpRecvExt( unsigned long * SourceIP, unsigned long * SourceprocID, unsigned long MyID , char * data, unsigned long * datalen);
//该函数接收信息  ，调用成功时 信息从*SourceIP ，*SourceprocID，MyID为自己的进程号。
//该函数阻塞掉用。

#endif // !defined(_FEP_H_INCLUDED_)
