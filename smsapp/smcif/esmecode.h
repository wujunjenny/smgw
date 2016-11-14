// ESMECode.h: interface for the CESMECode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ESMECODE_H_INCLUDED_)
#define _ESMECODE_H_INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "codeanddecode.h"

class CESMECode : public CCodeAndDecode  
{
public:
	CESMECode();
	virtual ~CESMECode();

	virtual int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	virtual int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);


};

#endif // !defined(_ESMECODE_H_INCLUDED_)
