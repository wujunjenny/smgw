#ifndef _CHANGEHEAD_H_INCLUDED_
#define _CHANGEHEAD_H_INCLUDED_

#define MAX_CHARGE_SERVICE_ID   1024L

//话单服务类型设置为记录
#define SERVICE_SET             1
//话单服务类型设置为不记录
#define SERVICE_SET_NO          0

//话单文件的缺省长度
#define DEFAULT_CHARGE_FILE_LEN (1024 * 256)

//短消息中心话单
#define CHARGE_TYPE_SMC     0

//信息服务系统话单
#define CHARGE_TYPE_INFO    1

#define MAKE_CHARGE_NO      0
#define MAKE_CHARGE_YES     1

#define CHARGE_FILE_HEAD_INFO   "Info"

#define CHARGE_FILE_HEAD_SMC    "Smc"

//短消息中心话单
#define CHARGE_MAP_FILE_SMC     _T("SMSCTOBILL")

//业务中心话单
#define CHARGE_MAP_FILE_INFO    _T("INFOTOBILL")

#endif   //_CHANGEHEAD_H_INCLUDED_