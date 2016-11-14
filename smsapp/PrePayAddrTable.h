// PrePayAddrTable.h: interface for the CPrePayAddrTable class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PREPAYADDRTABLE_H__3999D896_55B2_40A8_AADC_EAA01BE3BD60__INCLUDED_)
#define AFX_PREPAYADDRTABLE_H__3999D896_55B2_40A8_AADC_EAA01BE3BD60__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "Tree10.h"

class CPrePayAddrTable  
{
public:
	CPrePayAddrTable();
	virtual ~CPrePayAddrTable();

	//�жϺ����Ƿ���Ԥ�����û���TRUE=��Ԥ���ѣ�FALSE������Ԥ����
	BOOL IsPrePayAddr(LPCSTR addr);
	//���¼���Ԥ���ѺŶα�
	BOOL Reload();


protected:

	CTree10* m_pTree10;
	CRITICAL_SECTION m_Lock;
	int m_Flag;
};

#endif // !defined(AFX_PREPAYADDRTABLE_H__3999D896_55B2_40A8_AADC_EAA01BE3BD60__INCLUDED_)
