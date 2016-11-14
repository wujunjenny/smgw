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
	// ģ������
	char  ModuleName[MAX_MODULENAME_LEN];  
	//  ģ���Ƿ��Ǹ澯�ն�, true = �Ǹ澯�ն�, false �� ���Ǹ澯�ն�
	bool IsClientAlarm;
	// �澯��ϢID
	DWORD MsgID;
	// �澯�ļ���0����ͨ��Ϣ��1��һ��澯��2�����ظ澯��3�������Ը澯
	int  AlarmLevel;
	// �澯��Ϣ���0�� �澯��Ϣ��1 = ȡ���澯��Ϣ; 2��������Ϣ��3=��¼��Ϣ
	int  MsgType;    
	// �澯���ͣ�  H��Ӳ�̣�M���ڴ棻U��CPU��S��SOCKET��D��DATABASE��F���ļ�����P������C����ȫ��N = ���磻O������
	char AlarmType;   
	// �澯��ʱ�� ��ʽΪYYYY-MM-DD hh:mm:ss
	char  szTime[20];         
	// �澯�ľ�������
	char  szAlarmInfo[MAX_ALARMINFO_LEN];   
	// ���͸ø澯��IP��ַ
	char SrcIP[16];
	// ���͸ø澯�Ķ˿ں�
	int SrcPort;
	// �Զ�IP��ַ��������ʱΪ���ն�IP��ַ������ʱΪ���Ͷ�IP��ַ
	char DestIP[16];
	// �Զ˶˿ںţ�������ʱΪ���ն˶˿ںţ�����ʱΪ���Ͷ˶˿ں�
	int DestPort;
} ALARMMSG;

#endif