#ifndef _SMPPCODE_H_INCLUDED_
#define _SMPPCODE_H_INCLUDED_

#include "codeanddecode.h"
#include "SmppAndInnerTrans.h"
#include "smppdef.h" 
#include "smppcodeanddecode.h"


class CSmppCode:public CCodeAndDecode
{
public:
	// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
	virtual int MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen );
	// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //
	virtual int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	virtual int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
private:
	CSmppAndInnerTrans SmppAndInnerTrans; //SMPP和INNER结构之间的转换类
	CSmppCodeAndDecode SmppCompAndUncomp; //SMPP的压缩和解压类 

	//SMPP信息解压
	int OnlySmppDecode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
	{
		return SmppCompAndUncomp.Decode(pOrgBuff,orgsize,pDestBuff,destsize);
	};
	//SMPP信息压缩
	int OnlySmppCode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
	{
		return SmppCompAndUncomp.Code(pOrgBuff,orgsize,pDestBuff,destsize);
	};
	//SMPP信息翻译到INNER格式
	int SmppToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize)
	{
		return SmppAndInnerTrans.SmppToInner(pOrgBuff,pDestBuff,nDestSize);
	};
	int InnerToSmpp(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
	{
		return SmppAndInnerTrans.InnerToSmpp(pOrgBuff,nOrgSize,pDestBuff,nDestSize);
	};
};

class CSmppSmeiCode:public CSmppCode
{
public:
	virtual int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	virtual int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
};




class CSmppNewCode:public CCodeAndDecode
{
private:
	CSmppCodeAndDecode SmppCompAndUncomp; //SMPP的压缩和解压类 
public:
//	CSmppNewCode();
	virtual int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	virtual int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
	virtual int MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen );
	// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //



};


#endif	//_SMPPCODE_H_INCLUDED_