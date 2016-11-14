#ifndef _SMAPCODE_H_INCLUDED_
#define _SMAPCODE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "codeanddecode.h"
#include "SmapAndInnerTrans.h"
#include "Smapdef.h" 
#include "SmapCompAndUncomp.h"


class CSmapCode:public CCodeAndDecode
{
public:
	virtual int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	virtual int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
private:
	CSmapAndInnerTrans SmapAndInnerTrans; //Smap和INNER结构之间的转换类
	CSmapCompAndUnComp SmapCompAndUncomp; //Smap的压缩和解压类 

	//Smap信息解压
	int OnlySmapDecode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
	{
		return SmapCompAndUncomp.Decode(pOrgBuff,orgsize,pDestBuff,destsize);
	};
	//Smap信息压缩
	int OnlySmapCode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
	{
		return SmapCompAndUncomp.Code(pOrgBuff,orgsize,pDestBuff,destsize);
	};
	//Smap信息翻译到INNER格式
	int SmapToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize)
	{
		return SmapAndInnerTrans.SmapToInner(pOrgBuff,pDestBuff,nDestSize);
	};
	//INNER信息翻译到Smap格式
	int InnerToSmap(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
	{
		return SmapAndInnerTrans.InnerToSmap(pOrgBuff,nOrgSize,pDestBuff,nDestSize);
	};

};

class CSmapSmeiCode:public CSmapCode
{
public:
	virtual int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	virtual int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
};

#endif	//_SMAPCODE_H_INCLUDED_

