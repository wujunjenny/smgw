#ifndef _ERRORLEVEL_H_INCLUDED_
#define _ERRORLEVEL_H_INCLUDED_
/*
enum WARN_LEVEL
{
    LOG_WARN_PRMT = 0,    //��ʾ��Ϣ
    LOG_WARN_ERROR = 1,    //����
    LOG_WARN_GRAVITY_ERROR = 2,  //���ش���
    LOG_WARN_OTHER         //������Ϣ
};
*/
enum WARN_LEVEL
{
    LOG_WARN_GRAVITY_ERROR = 0, //���ش���
	LOG_WARN_IMPORTANCE = 1,	//��Ҫ����
    LOG_WARN_ERROR = 2,			//һ�����
    LOG_WARN_PRMT = 3,			//��ʾ��Ϣ
    LOG_WARN_OTHER = 4,         //������Ϣ
};
#endif	//_ERRORLEVEL_H_INCLUDED_