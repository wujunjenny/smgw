#ifndef _CHANGEHEAD_H_INCLUDED_
#define _CHANGEHEAD_H_INCLUDED_

#define MAX_CHARGE_SERVICE_ID   1024L

//����������������Ϊ��¼
#define SERVICE_SET             1
//����������������Ϊ����¼
#define SERVICE_SET_NO          0

//�����ļ���ȱʡ����
#define DEFAULT_CHARGE_FILE_LEN (1024 * 256)

//����Ϣ���Ļ���
#define CHARGE_TYPE_SMC     0

//��Ϣ����ϵͳ����
#define CHARGE_TYPE_INFO    1

#define MAKE_CHARGE_NO      0
#define MAKE_CHARGE_YES     1

#define CHARGE_FILE_HEAD_INFO   "Info"

#define CHARGE_FILE_HEAD_SMC    "Smc"

//����Ϣ���Ļ���
#define CHARGE_MAP_FILE_SMC     _T("SMSCTOBILL")

//ҵ�����Ļ���
#define CHARGE_MAP_FILE_INFO    _T("INFOTOBILL")

#endif   //_CHANGEHEAD_H_INCLUDED_