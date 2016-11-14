#ifndef _SMPPANDINNERTRANS_H_INCLUDED_
#define _SMPPANDINNERTRANS_H_INCLUDED_


/*-----------------------------------------------------

int CSmppAndInnerTrans.SmppToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int  nDestSize)

 ��ɴ�SMPP����INNER���ı仯
 ���룺LPVOID pOrgBuff :����SMPP�ṹ����
	   LPVOID pDestBuff:���������
	   nDestSize:�������������
 ����ֵ��TRANS_SUCCEED (ֵΪ0) ����ȷ�ֻ���pDestBuff�з���INNER�ṹ��,nDestSize:INNER
		�ṹ���ĳ���
 ������ �޶�ӦINNER�������������

int CSmppAndInnerTrans.InnerToSmpp(LPVOID pOrgBuff int &nOrgSize,LPVOID pDestBuff,int  nDestSize)

 ��ɴ�INNER����SMPP���ı仯
 ���룺LPVOID pOrgBuff :����INNER�ṹ����
	   LPVOID pDestBuff:���������
	   nOrgSize:���뻺�����ĳ���
	   nDestSize:�������������
 ����ֵ��TRANS_SUCCEED (ֵΪ0) ����ȷ�ֻ���pDestBuff�з���SMPP�ṹ����nDestSize:SMPP
		�ṹ���ĳ��ȣ�nOrgSize:��ʹ�ù��Ļ���������
		 ������ �޶�ӦSMPP�������������

-------------------------------------------------------*/

#include "smppdef.h"    //SMPPͷ�ļ�,�ṹ�������ȶ���
#include "msgpure.h"   //inner ����Ϣ��ʽ����
#include "msginner.h"   //inner ��Ϣ�ṹ����
#include "codeanddecode.h"

//GSM,INNER��UDHI��־��λ
#define INNER_UDHI_FLAG  1
//Aldiscon����Ϣ���ĵ�UDHI��־����λ��ESM_FLAG�ĵ�6BIT�ϡ�
//#define ALSDISCON_UDHI  0X40

// ���¶���ESM_CLASS��λ����
#define ESM_CLASS_REPLYPATH         0X80
#define ESM_CLASS_UDHI              0X40
#define ESM_CLASS_RCPT              0X20
#define ESM_CLASS_MESSAGEINGMODE    0X03

#define ESM_CLASS_STATUS_REPORT 0X04  //XX0001XX
#define ESM_CLASS_STATUS_REPORT_SEC 0X08
#define ESM_CLASS_STATUS_REPORT_MASK 0X6C //XX1111XX

//��HEADER�ж�ȡEXTDATA����
#define EXTDATA_SIZE	4
//���SMPP������Ŀ

#define MAX_SMPP_CMD	40

//SMPP������û����Ӧ��INNER����
#define NO_INNER_CMD	-1
//��ʾSMPP��INNER�ṹ�໥ת���ɹ�,��ֵ����CODE ��DECODE�ɹ�ֵ���
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
//INNER��SMPPʱ��ת����
#define INNER_TO_SMPP_TIME_TRANS(csSmpp,csInner) { \
	DWORD retVALUE=ChangeInnerTimeToSmppTime(csSmpp,csInner);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}
//INNER��SMPP֮��ʱ��ת����
#define SMPP_TO_INNER_TIME_TRANS(csInner,csSmpp) { \
	DWORD retVALUE=ChangeSmppTimeToInnerTime(csInner,csSmpp);   \
	if(retVALUE!=TRANS_SUCCEED) return retVALUE;   \
	}

class CSmppAndInnerTrans:CCodeAndDecode
{
public:
	//smpp�ṹ����INNER�ṹ��֮���ת��
	int SmppToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize);
	//INNER�ṹ����SMPP��֮���ת��
	int InnerToSmpp(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize);
	//��ʼ��COMMAND IDת����
	CSmppAndInnerTrans();

private:
	//command id ��SMPP���嵽INNER����任��
	DWORD m_CmdIdTrans[MAX_SMPP_CMD][2];
	//��HEX�ַ����У������Ҫת�����ַ�����
	int m_Max_Hex_Trans;


	//oc ����ṹ��dc����ṹ��cmdStaus����Ϣ״̬��λ��HEADER��)
	//���1��������Ϊchar * oc,���SMPP�ṹ����Ϣ��
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