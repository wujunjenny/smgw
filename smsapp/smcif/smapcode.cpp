// SmapCode.cpp: implementation of the CSmapCode class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "stdafx.h" 
//ʹ�÷������� 
#include "codeanddecode.h"
#include "SmapCode.h"
#include "SMEIMsg.h"
#include <assert.h>



int CSmapCode::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	int CodeReturn;

	int isSize=nDestSize;   //���InnerToSmapת��������Smap�ṹ���ȵ�ֵ
	char *isBuff=new char[isSize];  //���InnerToSmapת��������Smap�ṹ��
	
	memset(pDestBuff,0,nDestSize);
	CodeReturn=InnerToSmap(pOrgBuff,nOrgSize,isBuff,isSize);
	
	if(CodeReturn!=CODE_SUCCEED)
	{
		delete isBuff;
		nDestSize=0;
		return CodeReturn;
	}

	CodeReturn=OnlySmapCode(isBuff,isSize,((BYTE*)pDestBuff)+2,nDestSize);
	if(CodeReturn!=CODE_SUCCEED)
	{
		//Just for test
		//MessageBox("OnlySmapCode error");

		delete isBuff;
		nDestSize=0;
		return CodeReturn;
	}
	delete isBuff;
	((BYTE*)pDestBuff)[0]=1;	//��Ϣ��
	((BYTE*)pDestBuff)[1]=nDestSize;	//��Ϣ����

	Smap_Message_Header *smapHEADER=(Smap_Message_Header*)(((BYTE*)pDestBuff)+2);
	smapHEADER->Command_Length=nDestSize-1;

	nDestSize=MAX_SMAP_PACKET_LENGTH;
	return CODE_SUCCEED;
}
int CSmapCode::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	if(nOrgSize<MAX_SMAP_PACKET_LENGTH)
	{
		nDestSize=0;
		return SMAP_DECODE_NO_ENOUGHT_ORG_BUF;
	}
	int CodeReturn;
	int siSize=nDestSize;   //���InnerToSmapת��������Smap�ṹ���ȵ�ֵ
	char *siBuff=new char[siSize];  //���InnerToSmapת��������Smap�ṹ��

	int inputbuffsize=nOrgSize;
	int nInputOrgSize = nOrgSize;

	memset(pDestBuff,0,nDestSize);
	//Smap�д������ṹ�ı仯

	char psOrgBuff[MAX_SMAP_PACKET_LENGTH];
	memset(psOrgBuff,0,MAX_SMAP_PACKET_LENGTH);
	if(nOrgSize<=MAX_SMAP_PACKET_LENGTH)
	{
		memcpy(psOrgBuff,((BYTE*)pOrgBuff)+2,nOrgSize-2); //ƫ����Ϣ���ͳ����ֶε��ֽ���
	}
	else
	{
		ASSERT(FALSE);
	}
	int orgsize=int(((BYTE*)pOrgBuff)[1]);
	CodeReturn=OnlySmapDecode(psOrgBuff,orgsize,siBuff,siSize);

	if(CodeReturn!=DECODE_SUCCEED)
	{
		//MessageBox("OnlySmapDeCode error");
		delete siBuff;
		nDestSize=0;
		return CodeReturn;
	}
	
	// add it for return orgsize error
	Smap_Message_Header *Smaphead=(Smap_Message_Header*)siBuff;
	int Smaplen=Smaphead->Command_Length+sizeof(Smaphead->Command_Length);
	if(Smaplen>inputbuffsize)   //no org buff
	{
		//add by gxj 20031015
		delete siBuff;
		//end gxj
		return DECODE_NO_ENOUGH_ORG_BUF;
	}
	ASSERT(((BYTE*)pOrgBuff)[1]==Smaplen);
	//add for adiscom error return
	nOrgSize = ((BYTE*)pOrgBuff)[1];
	////////////////////////

	CodeReturn=SmapToInner(siBuff,pDestBuff,nDestSize);
	if(CodeReturn!=DECODE_SUCCEED)
	{
		//Just for test
		//MessageBox("InnerToSmap error");
		delete siBuff;
		nDestSize=0;
		return CodeReturn;
	}
	
	delete siBuff;
	return CodeReturn;
}

//��SMEI��ʽ����Ϣ����Smap��ʽ����Ϣ
int CSmapSmeiCode::Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	//��SMEI��ʽ����Ϣ����INNER��ʽ����Ϣ
	char* smeiptr=(char*)pOrgBuff;
	int smeiheadsize=sizeof(EI_HEAD);
	int innerheadsize=sizeof(MI_ONLYHEADER);
	//��SMEI�ĳ��ȱ�ΪINNER�ĳ���
	int nOrgINNERsize=nOrgSize+innerheadsize-smeiheadsize;
	char* innerptr=new char[nOrgINNERsize];
	if(!innerptr)
		return SMAP_CODE_NO_ENOUGHT_DEST_BUF;

	memset(innerptr,0,nOrgINNERsize);
	

	EI_HEAD* pEIhead=(EI_HEAD*)smeiptr;
	MI_ONLYHEADER *pINNERhead=(MI_ONLYHEADER*)innerptr;
	
	//////////////////////////////////////////////
	//��SMEI��ʽ����Ϣ����INNER��ʽ����Ϣ:ͷ��ת��
	pINNERhead->nCommand=pEIhead->command_id;
	pINNERhead->nInterfaceID=0;
	
	//ת��SEQUENCE
	char* ptr=(char *)&(pEIhead->sequence);
	for(int i=0;i<4;i++)
	{
		pINNERhead->sExtData[i]=ptr[i];
	}
	//INNER NLEN ֻ��ʾ��Ϣ�峤�ȣ�SMEI������Ϣͷ����
	pINNERhead->nLen=pEIhead->len-smeiheadsize;
	////////////////////////////////////////////
	
	//��Ϣ�帴��
	memcpy((innerptr+innerheadsize),(smeiptr+smeiheadsize),(nOrgSize-smeiheadsize));
	//����
	int retVALUE=CSmapCode::Code(innerptr,nOrgINNERsize,pDestBuff,nDestSize);
	delete innerptr;
	return retVALUE;
}

//��Smap��ʽ����Ϣ��ΪSMEI��ʽ����Ϣ
int CSmapSmeiCode::Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	//��SMEI��ʽ����Ϣ����INNER��ʽ����Ϣ

	
	
	int smeiheadsize=sizeof(EI_HEAD);
	int innerheadsize=sizeof(MI_ONLYHEADER);
	
	//Ŀ�껺������SMEI�ĳ��ȱ�ΪINNER�ĳ���	
	int nDestINNERsize=nDestSize+innerheadsize-smeiheadsize;
	char* innerptr=new char[nDestINNERsize];	
	if(!innerptr)
		return SMAP_CODE_NO_ENOUGHT_DEST_BUF;

	memset(innerptr,0,nDestINNERsize);
	
	//����
	int retVALUE=CSmapCode::Decode(pOrgBuff,nOrgSize,innerptr,nDestINNERsize);
	//set the right dest buffer size
	nDestSize=nDestINNERsize+smeiheadsize-innerheadsize; 

	if(retVALUE!=DECODE_SUCCEED)
	{
		delete innerptr;
		return retVALUE;
	}
	
	EI_HEAD* pEIhead=(EI_HEAD*)pDestBuff;
	MI_ONLYHEADER *pINNERhead=(MI_ONLYHEADER*)innerptr;
	//////////////////////////////////////////////
	//��INNER��ʽ����Ϣ����SMEI��ʽ����Ϣ:ͷ��ת��
	pEIhead->command_id=pINNERhead->nCommand;
	
	//ת��SEQUENCE
	char* ptr=(char *)&(pEIhead->sequence);
	for(int i=0;i<4;i++)
	{
		ptr[i]=pINNERhead->sExtData[i];
	}
	//INNER NLEN ֻ��ʾ��Ϣ�峤�ȣ�SMEI������Ϣͷ����
	pEIhead->len=pINNERhead->nLen+smeiheadsize;
	////////////////////////////////////////////
	//��Ϣ�帴��
	char* smeiptr=(char*)pDestBuff;
	memcpy((smeiptr+smeiheadsize),(innerptr+innerheadsize),(nDestINNERsize-innerheadsize));
	
	delete innerptr;
	nOrgSize=MAX_SMAP_PACKET_LENGTH;
	return retVALUE;
}



