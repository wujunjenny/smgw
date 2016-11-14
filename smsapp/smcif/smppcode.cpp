#include "stdafx.h" 
//使用返回码用 
#include "codeanddecode.h"
#include "SmppCode.h"
#include "SMEIMsg.h"
#include <assert.h>

// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
int CSmppCode::MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
{
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);

	UC sSeq[7], sHour[3], sMin[3], sSec[3];
	__int64 iSeq, iHour, iMin, iSec;
	__int64 itmpid=0;
	memset(sSeq, 0, sizeof(sSeq));
	memset(sHour, 0, sizeof(sHour));
	memset(sMin, 0, sizeof(sMin));
	memset(sSec, 0, sizeof(sSec));
	memcpy(sSeq, (UC*)pOrgMsgID, 6);
	memcpy(sHour, (UC*)pOrgMsgID+10, 2);
	memcpy(sMin, (UC*)pOrgMsgID+12, 2);
	memcpy(sSec, (UC*)pOrgMsgID+14, 2);

	iSeq = atoi((char*)sSeq);
	iHour = atoi((char*)sHour);
	iMin = atoi((char*)sMin);
	iSec = atoi((char*)sSec);

	itmpid = iSeq*86400 + iHour*3600 + iMin*60 + iSec;
	sprintf(tmpid,"%I64x",itmpid);

	destlen = MESSAGE_ID_LEN+1;
	memcpy( pDestMsgID, tmpid, destlen );
	
	return 0;
};

int CSmppNewCode::MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
{
	char tmpid[MESSAGE_ID_LEN+1];
	memset(tmpid,0,MESSAGE_ID_LEN);

	UC sSeq[7], sHour[3], sMin[3], sSec[3];
	__int64 iSeq, iHour, iMin, iSec;
	__int64 itmpid=0;
	memset(sSeq, 0, sizeof(sSeq));
	memset(sHour, 0, sizeof(sHour));
	memset(sMin, 0, sizeof(sMin));
	memset(sSec, 0, sizeof(sSec));
	memcpy(sSeq, (UC*)pOrgMsgID, 6);
	memcpy(sHour, (UC*)pOrgMsgID+10, 2);
	memcpy(sMin, (UC*)pOrgMsgID+12, 2);
	memcpy(sSec, (UC*)pOrgMsgID+14, 2);

	iSeq = atoi((char*)sSeq);
	iHour = atoi((char*)sHour);
	iMin = atoi((char*)sMin);
	iSec = atoi((char*)sSec);

	itmpid = iSeq*86400 + iHour*3600 + iMin*60 + iSec;
	sprintf(tmpid,"%I64x",itmpid);

	destlen = MESSAGE_ID_LEN+1;
	memcpy( pDestMsgID, tmpid, destlen );
	
	return 0;
};
// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //
int CSmppCode::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	int CodeReturn;
	int isSize=nDestSize;   //存放InnerToSmpp转换而生的SMPP结构长度的值
	char *isBuff=new char[isSize];  //存放InnerToSmpp转换而生的SMPP结构流
	if(!isBuff)
		return DECODE_NO_ENOUGH_ORG_BUF;
	
	memset(pDestBuff,0,nDestSize);
	CodeReturn=InnerToSmpp(pOrgBuff,nOrgSize,isBuff,isSize);
	
	if(CodeReturn!=CODE_SUCCEED)
	{
		delete isBuff;
		nDestSize=0;
		return CodeReturn;
	}

	CodeReturn=OnlySmppCode(isBuff,isSize,pDestBuff,nDestSize);
	if(CodeReturn!=CODE_SUCCEED)
	{
		//Just for test
		//MessageBox("OnlySmppCode error");

		delete isBuff;
		nDestSize=0;
		return CodeReturn;
	}
	delete isBuff;
	return CODE_SUCCEED;
}
int CSmppCode::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	int CodeReturn;
	int siSize=nDestSize;   //存放InnerToSmpp转换而生的SMPP结构长度的值
	char *siBuff=new char[siSize];  //存放InnerToSmpp转换而生的SMPP结构流
	if(!siBuff)
		return DECODE_NO_ENOUGH_ORG_BUF;

	memset(siBuff, 0, siSize);
	int inputbuffsize=nOrgSize;
	int nInputOrgSize = nOrgSize;

	memset(pDestBuff,0,nDestSize);
	char psOrgBuff[MAX_SMPP_PACKET_LENGTH];
	memset(psOrgBuff,0,MAX_SMPP_PACKET_LENGTH);
	if(nOrgSize<MAX_SMPP_PACKET_LENGTH)
	{
		memcpy(psOrgBuff,pOrgBuff,nOrgSize);
	}
	else
	{	
		memcpy(psOrgBuff,pOrgBuff,MAX_SMPP_PACKET_LENGTH);
	}
	CodeReturn=OnlySmppDecode(psOrgBuff,nOrgSize,siBuff,siSize);

	if(CodeReturn!=DECODE_SUCCEED)
	{
		delete siBuff;
		nDestSize=0;
		return CodeReturn;
	}
	
	Message_header *smpphead=(Message_header*)siBuff;
	int smpplen=smpphead->Command_Length;
	if(smpplen>inputbuffsize)   //no org buff
	{
        delete siBuff;
		nDestSize=0;
		return DECODE_NO_ENOUGH_ORG_BUF;
	}
ASSERT(nOrgSize==smpplen);
	nOrgSize = smpplen;

	CodeReturn=SmppToInner(siBuff,pDestBuff,nDestSize);
	if(CodeReturn!=DECODE_SUCCEED)
	{
		delete siBuff;
		nDestSize=0;
		return CodeReturn;
	}
	
	delete siBuff;
	return CODE_SUCCEED;
	return true;
}

int CSmppSmeiCode::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	char* smeiptr=(char*)pOrgBuff;
	int smeiheadsize=sizeof(EI_HEAD);
	int innerheadsize=sizeof(MI_ONLYHEADER);

	int nOrgINNERsize=nOrgSize+innerheadsize-smeiheadsize;
	char* innerptr=new char[nOrgINNERsize];
	if(!innerptr)
		return DECODE_NO_ENOUGH_ORG_BUF;

	memset(innerptr,0,nOrgINNERsize);
	

	EI_HEAD* pEIhead=(EI_HEAD*)smeiptr;
	MI_ONLYHEADER *pINNERhead=(MI_ONLYHEADER*)innerptr;
	
	//////////////////////////////////////////////
	//把SMEI格式的消息变以INNER格式的消息:头部转换
	pINNERhead->nCommand=pEIhead->command_id;
	pINNERhead->nInterfaceID=0;
	
	//转换SEQUENCE
	unsigned char* ptr=(unsigned char *)&(pEIhead->sequence);
	for(int i=0;i<4;i++)
	{
		pINNERhead->sExtData[i]=ptr[i];
	}
	//INNER NLEN 只表示消息体长度，SMEI包括消息头长度
	pINNERhead->nLen=pEIhead->len-smeiheadsize;
	////////////////////////////////////////////
	
	//消息体复制
	memcpy((innerptr+innerheadsize),(smeiptr+smeiheadsize),(nOrgSize-smeiheadsize));
	//编码
	int retVALUE=CSmppCode::Code(innerptr,nOrgINNERsize,pDestBuff,nDestSize);
    ASSERT(retVALUE == 0);
	delete innerptr;
	return retVALUE;
}

//把SMPP格式的消息变为SMEI格式的消息
int CSmppSmeiCode::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	//把SMEI格式的消息变以INNER格式的消息

	
	
	int smeiheadsize=sizeof(EI_HEAD);
	int innerheadsize=sizeof(MI_ONLYHEADER);
	
	//目标缓冲区从SMEI的长度变为INNER的长度	
	int nDestINNERsize=nDestSize+innerheadsize-smeiheadsize;
	char* innerptr=new char[nDestINNERsize];	
	if(!innerptr)
		return DECODE_NO_ENOUGH_ORG_BUF;

	memset(innerptr,0,nDestINNERsize);
	
	//解码
	int retVALUE=CSmppCode::Decode(pOrgBuff,nOrgSize,innerptr,nDestINNERsize);
	nDestSize=nDestINNERsize+smeiheadsize-innerheadsize; 
	if(retVALUE!=DECODE_SUCCEED)
	{
        ASSERT(0);
		delete innerptr;
		innerptr = NULL;
		return retVALUE;
	}
	
	EI_HEAD* pEIhead=(EI_HEAD*)pDestBuff;
	MI_ONLYHEADER *pINNERhead=(MI_ONLYHEADER*)innerptr;
	pEIhead->command_id=pINNERhead->nCommand;
	
	char* ptr=(char *)&(pEIhead->sequence);
	for(int i=0;i<4;i++)
	{
		ptr[i]=pINNERhead->sExtData[i];
	}
	pEIhead->len=pINNERhead->nLen+smeiheadsize;
	char* smeiptr=(char*)pDestBuff;
	memcpy((smeiptr+smeiheadsize),(innerptr+innerheadsize),(nDestINNERsize-innerheadsize));
	
	delete innerptr;
	return retVALUE;
}

int CSmppNewCode::Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
{
	return SmppCompAndUncomp.Decode(pOrgBuff,orgsize,pDestBuff,destsize);
}

int CSmppNewCode::Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
{
	return SmppCompAndUncomp.Code(pOrgBuff,orgsize,pDestBuff,destsize);
}