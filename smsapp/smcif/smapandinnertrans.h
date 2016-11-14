// SmapAndInnerTrans.h: interface for the CSmapAndInnerTrans class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SMAPANDINNERTRANS_H_INCLUDED_
#define _SMAPANDINNERTRANS_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


/*-----------------------------------------------------

int CSmapAndInnerTrans.SmapToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int  nDestSize)

 ��ɴ�Smap����INNER���ı仯
 ���룺LPVOID pOrgBuff :����Smap�ṹ����
	   LPVOID pDestBuff:���������
	   nDestSize:�������������
 ����ֵ��TRANS_SUCCEED (ֵΪ0) ����ȷ�ֻ���pDestBuff�з���INNER�ṹ��,nDestSize:INNER
		�ṹ���ĳ���
 ������ �޶�ӦINNER�������������

int CSmapAndInnerTrans.InnerToSmap(LPVOID pOrgBuff int &nOrgSize,LPVOID pDestBuff,int  nDestSize)

 ��ɴ�INNER����Smap���ı仯
 ���룺LPVOID pOrgBuff :����INNER�ṹ����
	   LPVOID pDestBuff:���������
	   nOrgSize:���뻺�����ĳ���
	   nDestSize:�������������
 ����ֵ��TRANS_SUCCEED (ֵΪ0) ����ȷ�ֻ���pDestBuff�з���Smap�ṹ����nDestSize:Smap
		�ṹ���ĳ��ȣ�nOrgSize:��ʹ�ù��Ļ���������
		 ������ �޶�ӦSmap�������������

-------------------------------------------------------*/

#include "Smapdef.h"    //Smapͷ�ļ�,�ṹ�������ȶ���
#include "msgpure.h"   //inner ����Ϣ��ʽ����
#include "msginner.h"   //inner ��Ϣ�ṹ����
#include "codeanddecode.h"


#define SERVICE_TYPE 1
#define COUNTRY_CODE "86"
#define SYSTEM_VERSION 1

//��HEADER�ж�ȡEXTDATA����
#define EXTDATA_SIZE	4
//���Smap������Ŀ
#define MAX_SMAP_CMD	36
//Smap������û����Ӧ��INNER����
#define NO_INNER_CMD	-1
//��ʾSmap��INNER�ṹ�໥ת���ɹ�,��ֵ����CODE ��DECODE�ɹ�ֵ���
#define TRANS_SUCCEED	0
//Э���д�
#define TRANS_ERROR		DECODE_PROTOCOL_ERROR
#define TRANS_ADDR_ERROR DECODE_PROTOCOL_ERROR
//ʱ��ת���д�
#define TRANS_TIME_ERROR DECODE_PROTOCOL_ERROR
//��Э��ֻ����Ŀ���ִ����ȵ��ַ�
#define CODECOPY(errorcode,dest,org)	{ \
	DWORD retVALUE=CodeCopy((char*)dest,(char*)org,sizeof((dest)));\
	if(retVALUE!=TRANS_SUCCEED) return errorcode; \
	}
//INNER��Smapʱ��ת����
#define INNER_TO_SMAP_TIME_TRANS(csSmap,csInner) { \
	DWORD retVALUE=ChangeInnerTimeToSmapTime(csSmap,csInner);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}
//INNER��Smap֮��ʱ��ת����
#define SMAP_TO_INNER_TIME_TRANS(csInner,csSmap) { \
	DWORD retVALUE=ChangeSmapTimeToInnerTime(csInner,csSmap);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}

class CSmapAndInnerTrans:CCodeAndDecode
{
public:
	//Smap�ṹ����INNER�ṹ��֮���ת��
	int SmapToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize);
	//INNER�ṹ����Smap��֮���ת��
	int InnerToSmap(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize);
	//��ʼ��COMMAND IDת����
	CSmapAndInnerTrans();

private:
	//command id ��Smap���嵽INNER����任��
	DWORD m_CmdIdTrans[MAX_SMAP_CMD][2];
	//��HEX�ַ����У������Ҫת�����ַ�����
	int m_Max_Hex_Trans;

	WORD  m_seqhead;
	MESSAGE_ID_TYPE m_referNum;	//smapdebug,��ѯʱ��Ҫ����ʱ����
	//oc ����ṹ��dc����ṹ��cmdStaus����Ϣ״̬��λ��HEADER��)
	//���1��������Ϊchar * oc,���Smap�ṹ����Ϣ��
	int CmdTransToInner(char * oc,MI_UNBIND_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(char * oc,MI_UNBIND *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Bind_Req  *oc,MI_BIND *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Bind_Resp *oc,MI_BIND_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Submit_Sm  *oc,MI_SUBMIT_SM *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Submit_Sm_Resp *oc,MI_SUBMIT_SM_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Submit_Sm  *oc,MI_DELIVER_SM *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Submit_Sm_Resp  *oc,MI_DELIVER_SM_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Cancel_Sm  *oc,MI_CANCEL_SM *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Cancel_Sm_Resp  *oc,MI_CANCEL_SM_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Replace_Sm  *oc,MI_REPLACE_SM *dc,int cmdStatus=0);
	int CmdTransToInner(Smap_Replace_Sm_Resp* oc,MI_REPLACE_SM_ACK *dc,int cmdStatus);
	int CmdTransToInner(Smap_Query_Sm_Details * oc,MI_QUERY_SM_DETAILS *dc,int cmdStatus);
	int CmdTransToInner(Smap_Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_ACK *dc,int cmdStatus);

	//INNER TO Smap,oc ����ṹ��dc����ṹ��cmdStaus����Ϣ״̬
	int CmdTransToSmap(Smap_Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_ACK *dc,int& cmdStatus);
	int CmdTransToSmap (Smap_Query_Sm_Details * oc,MI_QUERY_SM_DETAILS *dc,int& cmdStatus);
	int CmdTransToSmap(char * oc,MI_UNBIND *dc,int& cmdStatus);
	int CmdTransToSmap(char * oc,MI_UNBIND_ACK *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Bind_Req  *oc,MI_BIND *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Bind_Resp *oc,MI_BIND_ACK *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Submit_Sm  *oc,MI_SUBMIT_SM *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Submit_Sm_Resp *oc,MI_SUBMIT_SM_ACK *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Submit_Sm  *oc,MI_DELIVER_SM *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Submit_Sm_Resp  *oc,MI_DELIVER_SM_ACK *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Cancel_Sm  *oc,MI_CANCEL_SM *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Cancel_Sm_Resp  *oc,MI_CANCEL_SM_ACK *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Replace_Sm  *oc,MI_REPLACE_SM *dc,int& cmdStatus);
	int CmdTransToSmap(Smap_Replace_Sm_Resp  *oc,MI_REPLACE_SM_ACK *dc,int& cmdStatus);
	unsigned int HexStrToInt(char *str);
	unsigned int SMcopy(BYTE unicodeflag,char *deststr,char *orgstr);
	unsigned int SMcopy(char *deststr,char *orgstr,int UdLen);
	unsigned int IntToHexStr(char *str,int NUM);
	DWORD findSmapCMD(DWORD innerID);
	DWORD findINNERCMD(DWORD SmapID);
	void makeSmapHEAD(Smap_Message_Header *SmapHEADER,MI_ONLYHEADER *innerHEADER,int SmapMSGLEN,int cmdSTATUS);
	void makeINNERHEAD(Smap_Message_Header *SmapHEADER,MI_ONLYHEADER *innerHEADER,int innerMSGLEN);
	DWORD CodeCopy(char *dest,char *org,int len);
	DWORD InnerToSmapErrMap(DWORD innerStatus,DWORD& SmapStatus);
	DWORD SmapToInnerErrMap(DWORD SmapStatus,DWORD& innerStatus);

	DWORD ChangeInnerTimeToSmapTime( BYTE* buf,const char *cs);
	DWORD ChangeSmapTimeToInnerTime( const BYTE* cs,char* csinnertime );

	BOOL  ConvertInnerAddrToSmapAddr(BYTE& oclen,BYTE* ocaddr,
										BYTE dcTON,BYTE dcNPI,char* dcAddr);
	BOOL  ConvertSmapAddrToInnerAddr(BYTE oclen,BYTE* ocaddr,
										BYTE& dcTON,BYTE& dcNPI,char* dcAddr);

	UC ConvertCharToBCD(char nCh);
};
#endif	//_SMAPANDINNERTRANS_H_INCLUDED_

