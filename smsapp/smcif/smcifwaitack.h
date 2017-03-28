// SmcIFWaitAck.h: interface for the CSmcIFWaitAck class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SMCWAITACK_H_INCLUDED_)
#define _SMCWAITACK_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "..\IFMNG\waitresp.h"

//ÿ��λΪ����ʱ��
#define SMCIF_MAX_TIMER_WAIT    3
//������Ϣ����ȥ����Ϣ�ĵȴ�����
//��Ϊ���ڶ���Ϣ���Ĳ�֧�ֶ�����DELIVERY ��Ϣ�������������Ϣ����
//��ѯ�ʵ�һ��ҵ��Ļش�ֻ����SUBMIT����ʽ�ύ������������SUBMIT ACK
//����ص�ҵ��������ģ����ԶԻ���ɲ���Ӱ�졣

class CSmcIFWaitAck : public CWaitResp  
{
public:
	CSmcIFWaitAck();
    CSmcIFWaitAck(int nMaxWaitCount):CWaitResp(nMaxWaitCount)
    {
        //CWaitResp::CWaitResp(nMaxWaitCount);
    };
	virtual ~CSmcIFWaitAck();
private:
    UL m_ulSrcSeq;      //Դ�ύ��Ϣ�����
public:
    UL GetSourceSeq()
    {
        return m_ulSrcSeq;
    }
	virtual BOOL SaveMsg(PTRMSG pMsg)
    {
        m_ulSrcSeq = ((tagSmsHead *)pMsg)->nSequenceID;
        return TRUE;
    }
};

#endif // !defined(_SMCWAITACK_H_INCLUDED_)
