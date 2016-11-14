//---------------------------------------------------------------
// AlarmMsgDef.h
//----------------------------------------------------------------------

#ifndef _ALARMMSGDEF_H
#define _ALARMMSGDEF_H

#define MAX_MODULENAME_LEN  50
#define MAX_MODULE_COUNT  20
#define MAX_ALARMINFO_LEN 256

#define MODULE_RUNCOUNT_ERR 1;
#define MODULE_OUTTIME_ERR 2;

#define SHANKE	0
#define ERR_GW_CONNECT	1
#define ERR_DB_CONNECT	2


typedef struct tag_ALARMMSG
{
	// 模块名称
	char  ModuleName[MAX_MODULENAME_LEN];  
	//  模块是否是告警终端, true = 是告警终端, false ＝ 不是告警终端
	bool IsClientAlarm;
	// 告警消息ID
	DWORD MsgID;
	// 告警的级别，0＝普通消息；1＝一般告警；2＝严重告警；3＝灾难性告警
	int  AlarmLevel;
	// 告警消息类别，0＝ 告警消息；1 = 取消告警消息; 2＝握手消息；3=登录消息
	int  MsgType;    
	// 告警类型，  H＝硬盘；M＝内存；U＝CPU；S＝SOCKET；D＝DATABASE；F＝文件处理；P＝程序；C＝安全；N = 网络；O＝其他
	char AlarmType;   
	// 告警的时间 格式为YYYY-MM-DD hh:mm:ss
	char  szTime[20];         
	// 告警的具体内容
	char  szAlarmInfo[MAX_ALARMINFO_LEN];   
	// 发送该告警的IP地址
	char SrcIP[16];
	// 发送该告警的端口号
	int SrcPort;
	// 对端IP地址，即发送时为接收端IP地址，接收时为发送端IP地址
	char DestIP[16];
	// 对端端口号，即发送时为接收端端口号，接收时为发送端端口号
	int DestPort;
} ALARMMSG;

#endif