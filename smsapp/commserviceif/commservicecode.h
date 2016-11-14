// CommServiceCode.h: interface for the CCommServiceCode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_COMMSERVICECODE_H_INCLUDED_)
#define _COMMSERVICECODE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "codeanddecode.h"

class CCommServiceCode : public CCodeAndDecode  
{
public:
	CCommServiceCode();
	virtual ~CCommServiceCode();

public:
    //��׼ҵ��ӿڴ��ⲿ��Ϣ���ڲ���Ϣ��ת��
    int Decode(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    //��׼ҵ��ӿڴ��ڲ���Ϣ���ⲿ��Ϣ��ת��
	int Code(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
};

#endif // !defined(_COMMSERVICECODE_H_INCLUDED_)
