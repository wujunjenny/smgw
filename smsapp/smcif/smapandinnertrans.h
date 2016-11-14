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

 完成从Smap流到INNER流的变化
 输入：LPVOID pOrgBuff :输入Smap结构流，
	   LPVOID pDestBuff:输出缓冲区
	   nDestSize:输出缓冲区长度
 返回值：TRANS_SUCCEED (值为0) ：正确轮换，pDestBuff中返回INNER结构流,nDestSize:INNER
		结构流的长度
 其它： 无对应INNER命令或其它错误

int CSmapAndInnerTrans.InnerToSmap(LPVOID pOrgBuff int &nOrgSize,LPVOID pDestBuff,int  nDestSize)

 完成从INNER流到Smap流的变化
 输入：LPVOID pOrgBuff :输入INNER结构流，
	   LPVOID pDestBuff:输出缓冲区
	   nOrgSize:输入缓冲区的长度
	   nDestSize:输出缓冲区长度
 返回值：TRANS_SUCCEED (值为0) ：正确轮换，pDestBuff中返回Smap结构流，nDestSize:Smap
		结构流的长度，nOrgSize:已使用过的缓冲区长度
		 其它： 无对应Smap命令或其它错误

-------------------------------------------------------*/

#include "Smapdef.h"    //Smap头文件,结构，常量等定义
#include "msgpure.h"   //inner 纯消息格式定义
#include "msginner.h"   //inner 消息结构定义
#include "codeanddecode.h"


#define SERVICE_TYPE 1
#define COUNTRY_CODE "86"
#define SYSTEM_VERSION 1

//从HEADER中读取EXTDATA长度
#define EXTDATA_SIZE	4
//最大Smap命令数目
#define MAX_SMAP_CMD	36
//Smap中命令没有相应的INNER命令
#define NO_INNER_CMD	-1
//表示Smap和INNER结构相互转换成功,此值须与CODE 与DECODE成功值相等
#define TRANS_SUCCEED	0
//协议有错
#define TRANS_ERROR		DECODE_PROTOCOL_ERROR
#define TRANS_ADDR_ERROR DECODE_PROTOCOL_ERROR
//时间转换有错
#define TRANS_TIME_ERROR DECODE_PROTOCOL_ERROR
//按协议只拷贝目标字串长度的字符
#define CODECOPY(errorcode,dest,org)	{ \
	DWORD retVALUE=CodeCopy((char*)dest,(char*)org,sizeof((dest)));\
	if(retVALUE!=TRANS_SUCCEED) return errorcode; \
	}
//INNER向Smap时间转换宏
#define INNER_TO_SMAP_TIME_TRANS(csSmap,csInner) { \
	DWORD retVALUE=ChangeInnerTimeToSmapTime(csSmap,csInner);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}
//INNER与Smap之间时间转换宏
#define SMAP_TO_INNER_TIME_TRANS(csInner,csSmap) { \
	DWORD retVALUE=ChangeSmapTimeToInnerTime(csInner,csSmap);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}

class CSmapAndInnerTrans:CCodeAndDecode
{
public:
	//Smap结构流到INNER结构流之间的转换
	int SmapToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize);
	//INNER结构流到Smap流之间的转换
	int InnerToSmap(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize);
	//初始化COMMAND ID转换表
	CSmapAndInnerTrans();

private:
	//command id 从Smap定义到INNER定义变换表
	DWORD m_CmdIdTrans[MAX_SMAP_CMD][2];
	//在HEX字符串中，最大需要转换的字符界限
	int m_Max_Hex_Trans;

	WORD  m_seqhead;
	MESSAGE_ID_TYPE m_referNum;	//smapdebug,查询时需要，暂时不用
	//oc 输入结构，dc输出结构，cmdStaus：消息状态（位于HEADER中)
	//如果1参数类型为char * oc,则此Smap结构无消息体
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

	//INNER TO Smap,oc 输出结构，dc输入结构，cmdStaus：消息状态
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

