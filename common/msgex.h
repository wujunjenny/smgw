////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

#ifndef _MSGEX_H_INCLUDED_
#define _MSGEX_H_INCLUDED_


#include "msginner.h"
#include "msgpure.h"

// 每个用户数据的大小为110字节，30个用户数据大约小于4k
// 不建议一个数据包太大。和DB进程协商
//
#define MAX_LOADUSERNUM             30

// 定义加载协议是否继续
//
#define LOADSTATE_CONTINUE          0
#define LOADSTATE_LASTDATAERR       1
#define LOADSTATE_ABORT             2

// 定义最大的进程间消息的处理时间
#define MAX_IPCMSGTIME              5000

// 定义进程间消息超时定时器参数
#define IPCMSGTIMER_PARA            0xffffffff

#define SMEI_PROCESS_PROTOCOL_BASE              600


// 以下定义了调度中心和备份进程之间的协议命令字
//
#define SMEI_RESTORE_BEGIN                      (SMEI_PROCESS_PROTOCOL_BASE+1)
#define SMEI_RESTORE_END                        (SMEI_PROCESS_PROTOCOL_BASE+2)
#define SMEI_BACKUP_SM                          (SMEI_PROCESS_PROTOCOL_BASE+3)
#define SMEI_RESTORE_SM                         (SMEI_PROCESS_PROTOCOL_BASE+4)
#define SMEI_RESTORE_SM_ACK                     (SMEI_RESTORE_SM|0X8000)
#define SMEI_BACKUP_MODIFY_SM                   (SMEI_PROCESS_PROTOCOL_BASE+5)
#define SMEI_BACKUP_DELETE_SM                   (SMEI_PROCESS_PROTOCOL_BASE+6)

//以下定义了调度中心和DB进程之间的协议命令字
//
#define SMEI_LOADUSER_BEGIN                     (SMEI_PROCESS_PROTOCOL_BASE+31)
#define SMEI_LOADUSER_END                       (SMEI_PROCESS_PROTOCOL_BASE+32)
#define SMEI_LOADUSER                           (SMEI_PROCESS_PROTOCOL_BASE+33)
#define SMEI_LOADUSER_CONTINUE                  (SMEI_PROCESS_PROTOCOL_BASE+34)
#define SMEI_ADDUSER_RESULT                     (SMEI_PROCESS_PROTOCOL_BASE+35)
#define SMEI_MODUSER_RESULT                     (SMEI_PROCESS_PROTOCOL_BASE+36)
#define SMEI_DELUSER_RESULT                     (SMEI_PROCESS_PROTOCOL_BASE+37)
#define SMEI_LOADUSER_WAITLOAD                  (SMEI_PROCESS_PROTOCOL_BASE+38)

#define SMEI_LOADPREDEFINESM_BEGIN              (SMEI_PROCESS_PROTOCOL_BASE+41)
#define SMEI_LOADPREDEFINESM_END                (SMEI_PROCESS_PROTOCOL_BASE+42)
#define SMEI_LOADPREDEFINESM                    (SMEI_PROCESS_PROTOCOL_BASE+43)
#define SMEI_LOADPREDEFINESM_CONTINUE           (SMEI_PROCESS_PROTOCOL_BASE+44)
#define SMEI_ADD_PREDEFINEDSM_RESULT			(SMEI_PROCESS_PROTOCOL_BASE+45)
#define SMEI_MOD_PREDEFINEDSM_RESULT			(SMEI_PROCESS_PROTOCOL_BASE+46)
#define SMEI_DEL_PREDEFINEDSM_RESULT			(SMEI_PROCESS_PROTOCOL_BASE+47)



//以下定义了调度中心和话单进程之间的协议命令字
//
#define SMEI_DUMP_BILL                          (SMEI_PROCESS_PROTOCOL_BASE+61)


//以下定义补充的协议详细格式
//

#define PUREMSG_RESTORE_BEGIN	                        \
            DATE    BeginTime;

#define PUREMSG_BACKUP_SM                               \
            PUREMSG_SUBMIT_SM
#define PUREMSG_RESTORE_SM                              \
            PUREMSG_SUBMIT_SM
//END
///////////////////////////////////////////

// 在MI_ONLYHEADER的sExtData字段中保存一个DATE类型的时间戳
// 表示备份的时间
//
#define PUREMSG_RESTORE_SM_ACK                          \
            DWORD   dwStatus;                           \
            DWORD   dwSMID;

#define PUREMSG_RESTORE_END

#define PUREMSG_BACKUP_MODIFY_SM                        \
            PUREMSG_REPLACE_SM_EX

#define PUREMSG_BACKUP_DELETE_SM                        \
            DWORD   dwSMID;

#define PUREMSG_LOADUSER_BEGIN                          

#define PUREMSG_LOADUSER_END                          

#define PUREMSG_LOADUSER                                \
            DWORD   dwUserNum;                          \
            UC      SubMSISDN[MAX_ADDR_LEN];            \
            UC      SubName[MAX_ADDR_LEN];              \
            UC      SubAddress[MAX_SUB_ADDRESS];        \
            UC      ucMSType;                           \
            UC      ucTON;                              \
            UC	    ucNPI;	                            \
            DWORD   dwService;                          \
            UC	    ucSM;                               \
            DWORD   dwOCOS;	                            \
            DWORD	dwTCOS;	                            \
            US      usSMExpTime;                        \
            UC      ucPassword[MAX_PASSWORD];

#define PUREMSG_LOADUSER_CONTINUE                       \
            DWORD   dwContinue;

#define PUREMSG_ADDUSER_RESULT                          \
            PUREMSG_ADD_SUB                             \
            DWORD   dwCmdStatus;                        

#define PUREMSG_MODUSER_RESULT                          \
            PUREMSG_MOD_SUB                             \
            DWORD   dwCmdStatus;
            

#define PUREMSG_DELUSER_RESULT                          \
            PUREMSG_DEL_SUB                             \
            DWORD   dwCmdStatus;
            

#define PUREMSG_LOADPREDEFINESM_BEGIN

#define PUREMSG_LOADPREDEFINESM_END

#define PUREMSG_LOADPREDEFINESM							\
            DWORD   dwID;                               \
            UC      ucPRI;                              \
            UC      ucDCS;                              \
            UC      SMMsg1[MAX_UD_LEN];                 \
            UC      DefaultMsg[MAX_SM_LEN];

#define PUREMSG_LOADPREDEFINESM_CONTINUE                \
            DWORD   dwContinue;


#define PUREMSG_ADD_PREDEFINEDSM_RESULT                 \
            PUREMSG_ADD_PREDEFINEDSM                    \
		    DWORD dwCmdStatus;
            

		
#define PUREMSG_DEL_PREDEFINEDSM_RESULT                 \
            PUREMSG_DEL_PREDEFINEDSM                    \
		    DWORD dwCmdStatus;
		    


#define PUREMSG_MOD_PREDEFINEDSM_RESULT                 \
            PUREMSG_MOD_PREDEFINEDSM                    \
		    DWORD dwCmdStatus;
            

//PPS VERSION ADD char    sSMSCAddr[MAX_ADDR_LEN];
#define PUREMSG_DUMP_BILL                               \
            DWORD   dwSMID;                             \
            UC      ucOrgTON;                           \
            UC      ucOrgNPI;                           \
            UC      OrgAddr[MAX_ADDR_LEN];              \
            UC      ucDestTON;                          \
            UC      ucDestNPI;                          \
            UC      DestAddr[MAX_ADDR_LEN];             \
            UC      SubmitTime[MAX_TIMESTAMP_LEN];      \
            UC      FinalTime[MAX_TIMESTAMP_LEN];       \
            UC      ucPRI;                              \
            UC      ucSRR;                              \
            UC      ucPID;                              \
            UC      ucDCS;                              \
            DWORD   dwUDL;                              \
            UC      ucSMStatus;                         \
            UC      ucError;                            \
            DWORD   dwSMType;                           \
            UC      Reserved[16];                       \
            char    sSMSCAddr[MAX_ADDR_LEN];


//以下定义了补充的协议格式
//
typedef struct tag_MI_RESTORE_BEGIN
{
    MSG_HEADER_INNER
    PUREMSG_RESTORE_BEGIN
}MI_RESTORE_BEGIN;

typedef struct tag_MI_RESTORE_END
{
    MSG_HEADER_INNER
    PUREMSG_RESTORE_END
}MI_RESTORE_END;

typedef struct tag_MI_BACKUP_SM
{
    MSG_HEADER_INNER
    PUREMSG_BACKUP_SM
}MI_BACKUP_SM;

typedef struct tag_MI_RESTORE_SM
{
    MSG_HEADER_INNER
    PUREMSG_RESTORE_SM
}MI_RESTORE_SM;

typedef struct tag_MI_RESTORE_SM_ACK
{
    MSG_HEADER_INNER
    PUREMSG_RESTORE_SM_ACK
}MI_RESTORE_SM_ACK;

typedef struct tag_MI_BACKUP_MODIFY_SM
{
    MSG_HEADER_INNER
    PUREMSG_BACKUP_MODIFY_SM
}MI_BACKUP_MODIFY_SM;

typedef struct tag_MI_BACKUP_DELETE_SM
{
    MSG_HEADER_INNER
    PUREMSG_BACKUP_DELETE_SM
}MI_BACKUP_DELETE_SM;

typedef struct tag_MI_LOADUSER_BEGIN
{
    MSG_HEADER_INNER
    PUREMSG_LOADUSER_BEGIN
}MI_LOADUSER_BEGIN;

typedef struct tag_MI_LOADUSER_END
{
    MSG_HEADER_INNER
    PUREMSG_LOADUSER_END
}MI_LOADUSER_END;

typedef struct tag_MI_WAITFIRSTDATE
{
    MSG_HEADER_INNER
}MI_LOADUSER_WAITLOAD;

typedef struct tag_MI_LOADUSER
{
    MSG_HEADER_INNER
    PUREMSG_LOADUSER
}MI_LOADUSER;

typedef struct tag_MI_LOADUSER_CONTINUE
{
    MSG_HEADER_INNER
    PUREMSG_LOADUSER_CONTINUE
}MI_LOADUSER_CONTINUE;

typedef struct tag_MI_ADDUSER_RESULT
{
    MSG_HEADER_INNER
    PUREMSG_ADDUSER_RESULT
}MI_ADDUSER_RESULT;

typedef struct tag_MI_MODUSER_RESULT
{
    MSG_HEADER_INNER
    PUREMSG_MODUSER_RESULT
}MI_MODUSER_RESULT;

typedef struct tag_MI_DELUSER_RESULT
{
    MSG_HEADER_INNER
    PUREMSG_DELUSER_RESULT
}MI_DELUSER_RESULT;

typedef struct tag_MI_LOADPREDEFINESM_BEGIN
{
    MSG_HEADER_INNER
    PUREMSG_LOADPREDEFINESM_BEGIN
}MI_LOADPREDEFINESM_BEGIN;

typedef struct tag_MI_LOADPREDEFINESM_END
{
    MSG_HEADER_INNER
    PUREMSG_LOADPREDEFINESM_END
}MI_LOADPREDEFINESM_END;

typedef struct tag_MI_LOADPREDEFINESM
{
    MSG_HEADER_INNER
    PUREMSG_LOADPREDEFINESM
}MI_LOADPREDEFINESM;

typedef struct tag_MI_LOADPREDEFINESM_CONTINUE
{
    MSG_HEADER_INNER
    PUREMSG_LOADPREDEFINESM_CONTINUE
}MI_LOADPREDEFINESM_CONTINUE;

typedef struct tag_MI_ADD_PREDEFINEDSM_RESULT
{
    MSG_HEADER_INNER
    PUREMSG_ADD_PREDEFINEDSM_RESULT
}MI_ADD_PREDEFINEDSM_RESULT;

typedef struct tag_MI_DEL_PREDEFINEDSM_RESULT
{
    MSG_HEADER_INNER
    PUREMSG_DEL_PREDEFINEDSM_RESULT
}MI_DEL_PREDEFINEDSM_RESULT;

typedef struct tag_MI_MOD_PREDEFINEDSM_RESULT
{
    MSG_HEADER_INNER
    PUREMSG_MOD_PREDEFINEDSM_RESULT
}MI_MOD_PREDEFINEDSM_RESULT;

typedef struct tag_MI_DUMP_BILL
{
    MSG_HEADER_INNER
    PUREMSG_DUMP_BILL
}MI_DUMP_BILL;


#define CMDSTATUS_DB_SUCCEED		0	
#define CMDSTATUS_DB_ILLEGALPARA	101
#define CMDSTATUS_DB_NOLINK			102
#define CMDSTATUS_DB_DBFAIL			103
#define CMDSTATUS_DB_LINKFAIL		104


#endif	//_MSGEX_H_INCLUDED_