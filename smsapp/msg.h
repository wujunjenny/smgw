#ifndef _MSG_H_INCLUDED_
#define _MSG_H_INCLUDED_
#include <TIME.H>
#pragma pack( push, 1 )

/*
CommandId*	����ID��ʮ����������   4�ֽ�
UniKey	Ψһ���кţ�
SrcNumber	Դ����
UserNumber	�ֻ���
ServiceType	ҵ����룬��SP���壬�ַ�
ChargeNumber	���Ѻ���
State	״̬���ɹ�orʧ��
SPNumber	SP�Ľ�����룬�ַ�
FeeType*	�Ʒ����ͣ��ַ�
FeeValue	ȡֵ��Χ0-99999����������Ϣ���շ�ֵ����λΪ�֣���SP���壬�ַ�
���ڰ������շѵ��û�����ֵΪ����ѵ�ֵ
��ֵΪʵ�ʿ�ȡֵ
MessageContent	����Ϣ������
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
�յ�Report��ת��������Ϣ��ʽ
��������	˵��
CommandId*	����ID��ʮ����������   4�ֽ�
UniKey	Ψһ���кţ�ʮ����������    4�ֽ�
State	����״̬���ɹ�����ʧ�ܣ�
ErrCode	������
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