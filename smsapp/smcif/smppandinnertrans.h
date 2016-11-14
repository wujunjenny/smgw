#ifndef _SMPPANDINNERTRANS_H_INCLUDED_
#define _SMPPANDINNERTRANS_H_INCLUDED_


/*-----------------------------------------------------

int CSmppAndInnerTrans.SmppToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int  nDestSize)

 完成从SMPP流到INNER流的变化
 输入：LPVOID pOrgBuff :输入SMPP结构流，
	   LPVOID pDestBuff:输出缓冲区
	   nDestSize:输出缓冲区长度
 返回值：TRANS_SUCCEED (值为0) ：正确轮换，pDestBuff中返回INNER结构流,nDestSize:INNER
		结构流的长度
 其它： 无对应INNER命令或其它错误

int CSmppAndInnerTrans.InnerToSmpp(LPVOID pOrgBuff int &nOrgSize,LPVOID pDestBuff,int  nDestSize)

 完成从INNER流到SMPP流的变化
 输入：LPVOID pOrgBuff :输入INNER结构流，
	   LPVOID pDestBuff:输出缓冲区
	   nOrgSize:输入缓冲区的长度
	   nDestSize:输出缓冲区长度
 返回值：TRANS_SUCCEED (值为0) ：正确轮换，pDestBuff中返回SMPP结构流，nDestSize:SMPP
		结构流的长度，nOrgSize:已使用过的缓冲区长度
		 其它： 无对应SMPP命令或其它错误

-------------------------------------------------------*/

#include "smppdef.h"    //SMPP头文件,结构，常量等定义
#include "msgpure.h"   //inner 纯消息格式定义
#include "msginner.h"   //inner 消息结构定义
#include "codeanddecode.h"

//GSM,INNER的UDHI标志置位
#define INNER_UDHI_FLAG  1
//Aldiscon短消息中心的UDHI标志，它位于ESM_FLAG的第6BIT上。
//#define ALSDISCON_UDHI  0X40

// 以下定义ESM_CLASS的位定义
#define ESM_CLASS_REPLYPATH         0X80
#define ESM_CLASS_UDHI              0X40
#define ESM_CLASS_RCPT              0X20
#define ESM_CLASS_MESSAGEINGMODE    0X03

#define ESM_CLASS_STATUS_REPORT 0X04  //XX0001XX
#define ESM_CLASS_STATUS_REPORT_SEC 0X08
#define ESM_CLASS_STATUS_REPORT_MASK 0X6C //XX1111XX

//从HEADER中读取EXTDATA长度
#define EXTDATA_SIZE	4
//最大SMPP命令数目

#define MAX_SMPP_CMD	40

//SMPP中命令没有相应的INNER命令
#define NO_INNER_CMD	-1
//表示SMPP和INNER结构相互转换成功,此值须与CODE 与DECODE成功值相等
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
//INNER向SMPP时间转换宏
#define INNER_TO_SMPP_TIME_TRANS(csSmpp,csInner) { \
	DWORD retVALUE=ChangeInnerTimeToSmppTime(csSmpp,csInner);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}
//INNER与SMPP之间时间转换宏
#define SMPP_TO_INNER_TIME_TRANS(csInner,csSmpp) { \
	DWORD retVALUE=ChangeSmppTimeToInnerTime(csInner,csSmpp);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}

class CSmppAndInnerTrans:CCodeAndDecode
{
public:
	//smpp结构流到INNER结构流之间的转换
	int SmppToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize);
	//INNER结构流到SMPP流之间的转换
	int InnerToSmpp(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize);
	//初始化COMMAND ID转换表
	CSmppAndInnerTrans();

private:
	//command id 从SMPP定义到INNER定义变换表
	DWORD m_CmdIdTrans[MAX_SMPP_CMD][2];
	//在HEX字符串中，最大需要转换的字符界限
	int m_Max_Hex_Trans;


	//oc 输入结构，dc输出结构，cmdStaus：消息状态（位于HEADER中)
	//如果1参数类型为char * oc,则此SMPP结构无消息体
	int CmdTransToInner(Enquire_Sub_Resp * oc,MI_ENQUIRE_SUB_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Del_Sub * oc,MI_DEL_SUB *dc,int cmdStatus=0);
	int CmdTransToInner(char * oc,MI_ADD_SUB_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Add_Sub * oc,MI_ADD_SUB *dc,int cmdStatus=0);
	int CmdTransToInner(char * oc,MI_UNBIND_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(char * oc,MI_UNBIND *dc,int cmdStatus=0);
	int CmdTransToInner(Bind_Receiver  *oc,MI_BIND *dc,int cmdStatus=0);
	int CmdTransToInner(Bind_Receiver_Resp *oc,MI_BIND_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Submit_Sm  *oc,MI_SUBMIT_SM *dc,int cmdStatus=0);
	int CmdTransToInner(Submit_Sm_Resp *oc,MI_SUBMIT_SM_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Submit_Sm  *oc,MI_DELIVER_SM *dc,int cmdStatus=0);
	int CmdTransToInner(Submit_Sm_Resp  *oc,MI_DELIVER_SM_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Query_Sm  *oc,MI_QUERY_SM_STATUS *dc,int cmdStatus=0);
	int CmdTransToInner(Query_Sm_Resp  *oc,MI_QUERY_SM_STATUS_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Cancel_Sm  *oc,MI_CANCEL_SM *dc,int cmdStatus=0);
	int CmdTransToInner(char  *oc,MI_CANCEL_SM_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Replace_Sm  *oc,MI_REPLACE_SM *dc,int cmdStatus=0);
	int CmdTransToInner(char  *oc,MI_REPLACE_SM_ACK *dc,int cmdStatus=0);
	int CmdTransToInner(Add_Sub * oc,MI_MOD_SUB *dc,int cmdStatus);	
	int CmdTransToInner(Replace_Sm_Ex * oc,MI_REPLACE_SM_EX *dc,int cmdStatus);
	int CmdTransToInner(Query_Sms * oc,MI_QUERY_SMS *dc,int cmdStatus);
	int CmdTransToInner(Query_Sms_Resp * oc,MI_QUERY_SMS_ACK *dc,int cmdStatus);
	int CmdTransToInner(Query_Sm_Details * oc,MI_QUERY_SM_DETAILS_EX *dc,int cmdStatus);
	int CmdTransToInner(Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_EX_ACK *dc,int cmdStatus);
    
    int CmdTransToInner(Submit_Sm * oc,MI_STATUS_REPORT *dc,int cmdStatus);

	int CmdTransToSmpp(Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_EX_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp (Query_Sm_Details * oc,MI_QUERY_SM_DETAILS_EX *dc,int& cmdStatus);
	int CmdTransToSmpp(Query_Sms_Resp * oc,MI_QUERY_SMS_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Query_Sms * oc,MI_QUERY_SMS *dc,int& cmdStatus);
	int CmdTransToSmpp(Replace_Sm_Ex * oc,MI_REPLACE_SM_EX *dc,int& cmdStatus);
	int CmdTransToSmpp(Add_Sub * oc,MI_MOD_SUB *dc,int& cmdStatus);
	int CmdTransToSmpp(Enquire_Sub_Resp * oc,MI_ENQUIRE_SUB_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Add_Sub * oc,MI_ADD_SUB *dc,int& cmdStatus);
	int CmdTransToSmpp(Del_Sub * oc,MI_DEL_SUB *dc,int& cmdStatus);
	int CmdTransToSmpp(char * oc,MI_ADD_SUB_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(char * oc,MI_UNBIND *dc,int& cmdStatus);
	int CmdTransToSmpp(char * oc,MI_UNBIND_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Bind_Receiver  *oc,MI_BIND *dc,int& cmdStatus);
	int CmdTransToSmpp(Bind_Receiver_Resp *oc,MI_BIND_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Submit_Sm  *oc,MI_SUBMIT_SM *dc,int& cmdStatus);
	int CmdTransToSmpp(Submit_Sm_Resp *oc,MI_SUBMIT_SM_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Submit_Sm  *oc,MI_DELIVER_SM *dc,int& cmdStatus);
	int CmdTransToSmpp(Submit_Sm_Resp  *oc,MI_DELIVER_SM_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Query_Sm  *oc,MI_QUERY_SM_STATUS *dc,int& cmdStatus);
	int CmdTransToSmpp(Query_Sm_Resp  *oc,MI_QUERY_SM_STATUS_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Cancel_Sm  *oc,MI_CANCEL_SM *dc,int& cmdStatus);
	int CmdTransToSmpp(char  *oc,MI_CANCEL_SM_ACK *dc,int& cmdStatus);
	int CmdTransToSmpp(Replace_Sm  *oc,MI_REPLACE_SM *dc,int& cmdStatus);
	int CmdTransToSmpp(char  *oc,MI_REPLACE_SM_ACK *dc,int& cmdStatus);

    int CmdTransToSmpp(Submit_Sm  *oc,MI_STATUS_REPORT *dc,int& cmdStatus);
    int CmdTransToSmpp(Pps_User_Check *oc, \
        MI_PPS_USER_CHECK *dc, int& cmdStatus);
    int CmdTransToSmpp(Pps_User_Check_Resp *oc, \
        MI_PPS_USER_CHECK_ACK *dc, int& cmdStatus);

    int CmdTransToSmpp(Pps_Result_Notify *oc, \
        MI_PPS_RESULT_NOTIFY *dc, int& cmdStatus);
    int CmdTransToSmpp(Pps_Result_Notify_Resp *oc, \
        MI_PPS_RESULT_NOTIFY_ACK *dc, int& cmdStatus);
    
    int CmdTransToInner(Pps_User_Check * oc, \
        MI_PPS_USER_CHECK *dc, int cmdStatus);
    int CmdTransToInner(Pps_User_Check_Resp * oc, \
        MI_PPS_USER_CHECK_ACK *dc, int cmdStatus);

    int CmdTransToInner(Pps_Result_Notify * oc, \
        MI_PPS_RESULT_NOTIFY *dc, int cmdStatus);
    int CmdTransToInner(Pps_Result_Notify_Resp * oc, \
        MI_PPS_RESULT_NOTIFY_ACK *dc, int cmdStatus);

	unsigned int HexStrToInt(char *str);
	unsigned int SMcopy(BYTE unicodeflag,char *deststr,char *orgstr);
	unsigned int SMcopy(char *deststr,char *orgstr,int UdLen);
	unsigned int IntToHexStr(char *str,int NUM);
	DWORD findSMPPCMD(DWORD innerID);
	DWORD findINNERCMD(DWORD smppID);
	void makeSMPPHEAD(Message_header *smppHEADER,MI_ONLYHEADER *innerHEADER,int smppMSGLEN,int cmdSTATUS);
    void makeINNERHEAD(Message_header *smppHEADER,\
        MI_ONLYHEADER *innerHEADER,int innerMSGLEN, \
        BOOLEAN bStatusReport = FALSE);
	DWORD CodeCopy(char *dest,char *org,int len);
	DWORD InnerToSmppErrMap(DWORD innerStatus,DWORD& smppStatus);
	DWORD SmppToInnerErrMap(DWORD smppStatus,DWORD& innerStatus);

	DWORD ChangeInnerTimeToSmppTime( char* buf,const char *cs);
	DWORD ChangeSmppTimeToInnerTime( char* csinnertime ,const char* cs);

    unsigned char ConvertSMPPStateToSMEIState(unsigned char ucState);
	unsigned char ConvertSMEIStateToSMPPState(unsigned char ucState);
	unsigned char ConvertSMPPErrCodeToSMEIErrCode(unsigned char ucErrCode);
	unsigned char ConvertSMEIErrCodeToSMPPErrCode(unsigned char ucErrCode);
};
#endif	//_SMPPANDINNERTRANS_H_INCLUDED_