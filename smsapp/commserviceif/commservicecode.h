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
    //标准业务接口从外部消息到内部消息的转换
    int Decode(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    //标准业务接口从内部消息到外部消息的转换
	int Code(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
};

#endif // !defined(_COMMSERVICECODE_H_INCLUDED_)
