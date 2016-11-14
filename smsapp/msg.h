#ifndef _MSG_H_INCLUDED_
#define _MSG_H_INCLUDED_
#include <TIME.H>
#pragma pack( push, 1 )

/*
CommandId*	命令ID，十六进制数字   4字节
UniKey	唯一序列号，
SrcNumber	源号码
UserNumber	手机号
ServiceType	业务代码，由SP定义，字符
ChargeNumber	付费号码
State	状态，成功or失败
SPNumber	SP的接入号码，字符
FeeType*	计费类型，字符
FeeValue	取值范围0-99999，该条短消息的收费值，单位为分，由SP定义，字符
对于包月制收费的用户，该值为月租费的值
该值为实际扣取值
MessageContent	短消息的内容
*/
typedef struct FeeSubmitRepMsg{
    ULONG CommandID;
    CHAR  UniKey[65];
    CHAR  SrcNumber[21];
    CHAR  UserNumber[21];
    CHAR  ServiceType[10];
    CHAR  ChargeNumber[21];
    ULONG State;
    ULONG ErrCode;
    time_t ltime;
    CHAR  SPNumber[21];
    CHAR  FeeType;
    INT   FeeValue;
    CHAR  MessageContent[160];
}FEESUBMITREPMSG;
/*
收到Report后转发出的消息格式
参数名称	说明
CommandId*	命令ID，十六进制数字   4字节
UniKey	唯一序列号，十六进制数字    4字节
State	返回状态（成功或者失败）
ErrCode	错误码
*/
typedef struct FeeReportMsg{
    ULONG CommandID;
    CHAR UniKey[65];
    ULONG State;
    ULONG ErrCode;
}FEEREPORTMSG;


typedef enum FeeCommand{
    FEE_SUBMITRESP = 1,
    FEE_REPORT
}FEECOMMAND;

typedef struct FeeStatistics{
    ULONG AckCount;
    ULONG AckSuccCount;
    ULONG AckFailCount;
    ULONG ReportCount;
    ULONG ReportSuccCount;
    ULONG ReportFailCount;
    ULONG ReportTimeoutCount;
}FEESTATISTICS;

#pragma pack( pop)

#define SM_STATE_SUCC  0
#define SM_STATE_FAIL  1
#define SM_STATE_TIMEOUT  2

#define MAX_RESULTS_COUNT  65535
#define MAX_SM_HASH_SIZE   65535

#define MAX_WAIT_REPORT_TIME     300 //secs

#endif	//_MSG_H_INCLUDED_