
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

#include "stdafx.h"
#include "msginner.h"
#include "SmppAndInnerTrans.h"
//使用返回码用 
#include "codeanddecode.h"
#define UCS2_END_LEN	0
#define TEXT_END_LEN	0


CSmppAndInnerTrans::CSmppAndInnerTrans()
{
	for(int i=0;i<MAX_SMPP_CMD;i++)
	{
		m_CmdIdTrans[i][0]=0;
		m_CmdIdTrans[i][1]=0;
	}
	m_Max_Hex_Trans=MESSAGE_ID_LEN-1;   // 8
	//command id 从SMPP定义到INNER定义变换表
	m_CmdIdTrans[0][0]=ESME_BNDRCV;
	m_CmdIdTrans[0][1]=SMEI_BIND_RECEIVER;
	m_CmdIdTrans[1][0]=ESME_BNDRCV_RESP;
	m_CmdIdTrans[1][1]=SMEI_BIND_RECEIVER_ACK;
	m_CmdIdTrans[2][0]=ESME_BNDTRN;
	m_CmdIdTrans[2][1]=SMEI_BIND_TRANSMITTER;
	m_CmdIdTrans[3][0]=ESME_BNDTRN_RESP;
	m_CmdIdTrans[3][1]=SMEI_BIND_TRANSMITTER_ACK;
	m_CmdIdTrans[27][0]=ESME_BNDRCV_RESP;
	m_CmdIdTrans[27][1]=SMEI_BIND_RECEIVER_ACK;
	m_CmdIdTrans[4][0]=ESME_UBD;
	m_CmdIdTrans[4][1]=SMEI_UNBIND;
	m_CmdIdTrans[5][0]=ESME_UBD_RESP;
	m_CmdIdTrans[5][1]=SMEI_UNBIND_ACK;
	m_CmdIdTrans[6][0]=ESME_SUB_SM	;
	m_CmdIdTrans[6][1]=SMEI_SUBMIT_SM;
	m_CmdIdTrans[7][0]=ESME_SUB_SM_RESP;
	m_CmdIdTrans[7][1]=SMEI_SUBMIT_SM_ACK;
	m_CmdIdTrans[8][0]=ESME_DELIVER_SM;
	m_CmdIdTrans[8][1]=SMEI_DELIVER_SM;
	m_CmdIdTrans[9][0]=ESME_DELIVER_SM_RESP;
	m_CmdIdTrans[9][1]=SMEI_DELIVER_SM_ACK;
	m_CmdIdTrans[10][0]=ESME_QUERY_SM;
	m_CmdIdTrans[10][1]=SMEI_QUERY_SM_STATUS;
	m_CmdIdTrans[11][0]=ESME_QUERY_SM_RESP;
	m_CmdIdTrans[11][1]=SMEI_QUERY_SM_STATUS_ACK;
	m_CmdIdTrans[12][0]=ESME_CANCEL_SM;
	m_CmdIdTrans[12][1]=SMEI_CANCEL_SM;
	m_CmdIdTrans[13][0]=ESME_CANCEL_SM_RESP;
	m_CmdIdTrans[13][1]=SMEI_CANCEL_SM_ACK;
	//判断是否为扩展的SMPP协议
//#ifndef _SMPP_EX_
	m_CmdIdTrans[14][0]=ESME_REPLACE_SM;
	m_CmdIdTrans[14][1]=SMEI_REPLACE_SM;
	m_CmdIdTrans[15][0]=ESME_REPLACE_SM_RESP;
	m_CmdIdTrans[15][1]=SMEI_REPLACE_SM_ACK;


//#else
	m_CmdIdTrans[32][0]=ESME_REPLACE_SM_EX;
	m_CmdIdTrans[32][1]=SMEI_REPLACE_SM_EX;
	m_CmdIdTrans[33][0]=ESME_REPLACE_SM_EX_RESP;
	m_CmdIdTrans[33][1]=SMEI_REPLACE_SM_EX_ACK;
//#endif
	m_CmdIdTrans[16][0]=ESME_QRYLINK;
	m_CmdIdTrans[16][1]=SMEI_ENQUIRE_LINK;
	m_CmdIdTrans[17][0]=ESME_QRYLINK_RESP;
	m_CmdIdTrans[17][1]=SMEI_ENQUIRE_LINK_ACK;
	m_CmdIdTrans[18][0]=ESME_NACK;
	m_CmdIdTrans[18][1]=SMEI_NACK;
	//SUB FOUNCTION
	m_CmdIdTrans[19][0]=ESME_ADD_SUB;
	m_CmdIdTrans[19][1]=SMEI_ADD_SUB;
	m_CmdIdTrans[20][0]=ESME_ADD_SUB_RESP;
	m_CmdIdTrans[20][1]=SMEI_ADD_SUB_ACK;
	m_CmdIdTrans[21][0]=ESME_DEL_SUB;
	m_CmdIdTrans[21][1]=SMEI_DEL_SUB;
	m_CmdIdTrans[22][0]=ESME_DEL_SUB_RESP;
	m_CmdIdTrans[22][1]=SMEI_DEL_SUB_ACK;
	m_CmdIdTrans[23][0]=ESME_MOD_SUB;
	m_CmdIdTrans[23][1]=SMEI_MOD_SUB;
	m_CmdIdTrans[24][0]=ESME_MOD_SUB_RESP;
	m_CmdIdTrans[24][1]=SMEI_MOD_SUB_ACK;
	m_CmdIdTrans[25][0]=ESME_QRY_SUB;
	m_CmdIdTrans[25][1]=SMEI_ENQUIRE_SUB;
	m_CmdIdTrans[26][0]=ESME_QRY_SUB_RESP;
	m_CmdIdTrans[26][1]=SMEI_ENQUIRE_SUB_ACK;
	
	m_CmdIdTrans[28][0]=ESME_QUERY_SMS;
	m_CmdIdTrans[28][1]=SMEI_QUERY_MSGS;
	m_CmdIdTrans[29][0]=ESME_QUERY_SMS_RESP;
	m_CmdIdTrans[29][1]=SMEI_QUERY_MSGS_ACK;

	m_CmdIdTrans[30][0]=ESME_QUERY_SM_DETAILS;
	m_CmdIdTrans[30][1]=SMEI_QUERY_MSG_DETAILS_EX;
	m_CmdIdTrans[31][0]=ESME_QUERY_SM_DETAILS_RESP;
	m_CmdIdTrans[31][1]=SMEI_QUERY_MSG_DETAILS_EX_ACK;

    m_CmdIdTrans[36][0]=ESME_PPS_USER_CHECK;
	m_CmdIdTrans[36][1]=SMEI_PPS_USER_CHECK;
    m_CmdIdTrans[37][0]=ESME_PPS_USER_CHECK_RESP;
	m_CmdIdTrans[37][1]=SMEI_PPS_USER_CHECK_ACK;
    m_CmdIdTrans[38][0]=ESME_PPS_RESULT_NOTIFY;
	m_CmdIdTrans[38][1]=SMEI_PPS_RESULT_NOTIFY;
    m_CmdIdTrans[39][0]=ESME_PPS_RESULT_NOTIFY_RESP;
	m_CmdIdTrans[39][1]=SMEI_PPS_RESULT_NOTIFY_ACK;
}
//完成从INNER流到SMPP流的变化 
int CSmppAndInnerTrans::InnerToSmpp(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	unsigned char *obuff=(unsigned char *)pOrgBuff;
	unsigned char *dbuff=(unsigned char *)pDestBuff;
	
	UL Sequence = 0;
	Message_header *smppHEADER=(Message_header *)pDestBuff;
	MI_ONLYHEADER *innerHEADER=(MI_ONLYHEADER *)pOrgBuff ;
	//把目标缓冲区清为全O

	memset(dbuff,0,nDestSize);
	int smppHEADSIZE=sizeof(Message_header);  //smpp头长度
	int innerHEADSIZE=sizeof(MI_ONLYHEADER);   //INNER头长度
	int smppMSGLEN;	  //SMPP 消息长度
	int innerMSGLEN=0; //inner 消息长度
	int traFlg=CODE_SUCCEED;
	int cmdSTATUS=0;

    BOOLEAN bStatusReportFlag = FALSE;
	
	//源缓冲区不够消息头长
	if(innerHEADSIZE>nOrgSize) return ESME_CODE_NO_ENOUGHT_ORG_BUF;
	//smpp正文地址
	dbuff+=smppHEADSIZE;

	switch( innerHEADER->nCommand)
	{
//判断是否为扩展的SMPP协议
//#ifdef	_SMPP_EX_
		
		case SMEI_REPLACE_SM_EX:    //此处REPLACE SM 对应于SMPP的REPLACE_SM_EX
			{
				//输入,输出结构形成
				Replace_Sm_Ex *oc=(Replace_Sm_Ex *)dbuff;
				MI_REPLACE_SM_EX *dc=(MI_REPLACE_SM_EX *)obuff;			
				smppMSGLEN=sizeof(Replace_Sm_Ex);
				
				
				//innerMSGLEN=sizeof(*dc);
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(dc->DCS,dc->UDL);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN=sizeof(*dc) + iOctetLen - MAX_SM_LEN;
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
//#else
		case SMEI_REPLACE_SM:
			{   
					//输入,输出结构形成
				Replace_Sm *oc=(Replace_Sm  *)dbuff;
				MI_REPLACE_SM *dc=(MI_REPLACE_SM *)obuff;
				smppMSGLEN=sizeof(Replace_Sm);
				smppMSGLEN+=smppHEADSIZE;
				//innerMSGLEN=sizeof(*dc);

				int iOctetLen = 0;
				iOctetLen = GetOctetLength(dc->DCS,dc->UDL);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN=sizeof(*dc)+iOctetLen-MAX_SM_LEN;
				
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				
				if((smppMSGLEN)>nDestSize)
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}

				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
//#endif
		case SMEI_QUERY_MSGS:
			{
				//输入,输出结构形成
				Query_Sms *oc=(Query_Sms *)dbuff;
				MI_QUERY_SMS *dc=(MI_QUERY_SMS *)obuff;			
				smppMSGLEN=sizeof(Query_Sms);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_QUERY_MSGS_ACK:
			{
				//输入,输出结构形成
				Query_Sms_Resp *oc=(Query_Sms_Resp *)dbuff;
				MI_QUERY_SMS_ACK *dc=(MI_QUERY_SMS_ACK *)obuff;			
				smppMSGLEN=sizeof(Query_Sms_Resp);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_QUERY_MSG_DETAILS:
			{
				//输入,输出结构形成
				Query_Sm_Details *oc=(Query_Sm_Details *)dbuff;
				MI_QUERY_SM_DETAILS_EX *dc=(MI_QUERY_SM_DETAILS_EX *)obuff;			
				smppMSGLEN=sizeof(Query_Sm_Details);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_QUERY_MSG_DETAILS_EX_ACK:
			{
				//输入,输出结构形成
				Query_Sm_Details_Resp *oc=(Query_Sm_Details_Resp *)dbuff;
				MI_QUERY_SM_DETAILS_EX_ACK *dc=(MI_QUERY_SM_DETAILS_EX_ACK *)obuff;			
				smppMSGLEN=sizeof(Query_Sm_Details_Resp);

				//innerMSGLEN=sizeof(*dc)+GetOctetLength(dc->DCS,dc->UDL)-MAX_SM_LEN;
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(dc->DCS,dc->UDL);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN=sizeof(*dc)+iOctetLen-MAX_SM_LEN;

				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;

		case SMEI_DEL_SUB:   
		case SMEI_ENQUIRE_SUB:
			{
				//输入,输出结构形成
				Del_Sub *oc=(Del_Sub *)dbuff;
				MI_DEL_SUB	  *dc=(MI_DEL_SUB *)obuff;			
				smppMSGLEN=sizeof(Del_Sub);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_ADD_SUB_ACK: 
		case SMEI_DEL_SUB_ACK:
		case SMEI_MOD_SUB_ACK:
			{
				//输入,输出结构形成
				char  *oc=(char  *)dbuff;
				MI_ADD_SUB_ACK	*dc=(MI_ADD_SUB_ACK *)obuff;
			
				smppMSGLEN=0;
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_ENQUIRE_SUB_ACK:
			{
				//输入,输出结构形成
				Enquire_Sub_Resp *oc=(Enquire_Sub_Resp *)dbuff;
				MI_ENQUIRE_SUB_ACK	  *dc=(MI_ENQUIRE_SUB_ACK *)obuff;
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN=sizeof(Enquire_Sub_Resp);
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;

		case SMEI_ADD_SUB:
//		case SMEI_MOD_SUB:
			{
				//输入,输出结构形成
				Add_Sub *oc=(Add_Sub *)dbuff;
				MI_ADD_SUB	  *dc=(MI_ADD_SUB *)obuff;
			
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}

				smppMSGLEN=sizeof(Add_Sub);
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_MOD_SUB:
			{
				//输入,输出结构形成,在SMPP中ADD SUB 和 MOD SUB 的结构是相同的，所以使用相同的
				//结构名，代替Mod_Sub
				Add_Sub *oc=(Add_Sub *)dbuff;
				MI_MOD_SUB	  *dc=(MI_MOD_SUB *)obuff;
			
				innerMSGLEN=sizeof(MI_MOD_SUB);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}

				smppMSGLEN=sizeof(Add_Sub);
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
			
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_ENQUIRE_LINK:
		case SMEI_NACK:
		case SMEI_UNBIND:   //UNBIND ,QUERY LINK ,NAK
			{
				//输入,输出结构形成
				char *oc=(char *)dbuff;
				MI_UNBIND *dc=(MI_UNBIND *)obuff;
			
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN=0;
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				
				//消息流转换
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_ENQUIRE_LINK_ACK:   //UNBIND ACK,QUERY LINK ACK
			{
				//输入,输出结构形成
				char *oc=(char *)dbuff;
				MI_ENQUIRE_LINK_ACK *dc=(MI_ENQUIRE_LINK_ACK *)obuff;
			
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN=0;
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				
				traFlg=CODE_SUCCEED;
			};
			break;
		case SMEI_UNBIND_ACK:
			{
				char *oc=(char *)dbuff;
				MI_UNBIND_ACK *dc=(MI_UNBIND_ACK *)obuff;
			
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN=0;
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				
				//消息流转换
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_BIND_RECEIVER:   //bind 此处未改
		case SMEI_BIND_TRANSMITTER:
			{
				//输入,输出结构形成
				Bind_Receiver *oc=(Bind_Receiver *)dbuff;
				MI_BIND		  *dc=(MI_BIND *)obuff;
			
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN=sizeof(Bind_Receiver);
				smppMSGLEN+=smppHEADSIZE;	
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//昨日做到此-----------------------
				//消息流转换
				//注：此处的oc SMPP流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_BIND_RECEIVER_ACK:
		case SMEI_BIND_TRANSMITTER_ACK:
			{
					//输入,输出结构形成
				Bind_Receiver_Resp *oc=(Bind_Receiver_Resp *)dbuff;
				MI_BIND_ACK 	*dc=(MI_BIND_ACK *)obuff;
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN=sizeof(Bind_Receiver_Resp);
				smppMSGLEN+=smppHEADSIZE;

				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}

				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_SUBMIT_SM:
			{
					//输入,输出结构形成
				Submit_Sm *oc=(Submit_Sm *)dbuff;
				MI_SUBMIT_SM *dc=(MI_SUBMIT_SM *)obuff;
				
				//innerMSGLEN=sizeof(*dc);
				//innerMSGLEN=sizeof(*dc)+GetOctetLength(dc->DCS,dc->UDL)-MAX_SM_LEN;

				int iOctetLen = 0;
				iOctetLen = GetOctetLength(dc->DCS,dc->UDL);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN=sizeof(*dc)+iOctetLen-MAX_SM_LEN;
				
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF; 
					break;
				}
				smppMSGLEN=sizeof(Submit_Sm);
				smppMSGLEN+=smppHEADSIZE;

				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_SUBMIT_SM_ACK:
			{
			    //输入,输出结构形成
				Submit_Sm_Resp *oc=(Submit_Sm_Resp *)dbuff;
				MI_SUBMIT_SM_ACK *dc=(MI_SUBMIT_SM_ACK *)obuff;

				innerMSGLEN=sizeof(*dc); 

				if( innerMSGLEN > nOrgSize)
				{
                    ASSERT(0);
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}

				smppMSGLEN=sizeof(Submit_Sm_Resp);
				smppMSGLEN+=smppHEADSIZE;

				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}

				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_DELIVER_SM:   //	注：DELIVE SM 在SMPP和SUBMIT SM 结构相同
			{
					//输入,输出结构形成
				Submit_Sm *oc=(Submit_Sm *)dbuff;
				MI_DELIVER_SM *dc=(MI_DELIVER_SM *)obuff;
				smppMSGLEN=sizeof(Submit_Sm);
				
				//innerMSGLEN=sizeof(*dc);
				//innerMSGLEN=sizeof(*dc)+GetOctetLength(dc->DCS,dc->UDL)-MAX_SM_LEN;
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(dc->DCS,dc->UDL);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN=sizeof(*dc)+iOctetLen-MAX_SM_LEN;
				
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				
				smppMSGLEN+=smppHEADSIZE;
				
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
				Sequence = dc->sm_ID;
			}
			break;
        case SMEI_STATUS_REPORT:   //	注：DELIVE SM 在SMPP和SUBMIT SM 结构相同
			{
				//输入,输出结构形成
				Submit_Sm *oc=(Submit_Sm *)dbuff;
				MI_STATUS_REPORT *dc=(MI_STATUS_REPORT *)obuff;
				smppMSGLEN=sizeof(Submit_Sm);
				
				//innerMSGLEN=sizeof(*dc);
				//innerMSGLEN=sizeof(*dc)+GetOctetLength(dc->DCS,dc->UDL)-MAX_SM_LEN;
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(dc->byDCS,dc->ulUDL);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN=sizeof(*dc)+iOctetLen-MAX_SM_LEN;
				
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				
				smppMSGLEN+=smppHEADSIZE;
				
				if((smppMSGLEN)>nDestSize)
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
				Sequence = dc->sm_ID;
			}
			break;
		//	注：DELIVE SM RESP 在SMPP和SUBMIT SM RESP结构相同
        //end add
		//	注：DELIVE SM RESP 在SMPP和SUBMIT SM RESP结构相同
		case SMEI_DELIVER_SM_ACK:
			{   
					//输入,输出结构形成
				Submit_Sm_Resp *oc=(Submit_Sm_Resp *)dbuff;
				MI_DELIVER_SM_ACK *dc=(MI_DELIVER_SM_ACK *)obuff;
				smppMSGLEN=sizeof(Submit_Sm_Resp);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;

				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
                Sequence = dc->sm_ID;
			}
			break;
		case SMEI_QUERY_SM_STATUS:
			{   
				//输入,输出结构形成
				Query_Sm *oc=(Query_Sm *)dbuff;
				MI_QUERY_SM_STATUS *dc=(MI_QUERY_SM_STATUS *)obuff;
				smppMSGLEN=sizeof(Query_Sm);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;

				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_QUERY_SM_STATUS_ACK:  
			{   
					//输入,输出结构形成
				Query_Sm_Resp *oc=(Query_Sm_Resp *)dbuff;
				MI_QUERY_SM_STATUS_ACK *dc=(MI_QUERY_SM_STATUS_ACK *)obuff;
				smppMSGLEN=sizeof(Query_Sm_Resp);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;

				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_CANCEL_SM:
			{   
					//输入,输出结构形成
				Cancel_Sm *oc=(Cancel_Sm *)dbuff;
				MI_CANCEL_SM *dc=(MI_CANCEL_SM *)obuff;
				smppMSGLEN=sizeof(Cancel_Sm);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smppMSGLEN+=smppHEADSIZE;

				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_CANCEL_SM_ACK:
			{   
					//输入,输出结构形成
				char *oc=(char  *)dbuff;
				MI_CANCEL_SM_ACK *dc=(MI_CANCEL_SM_ACK *)obuff;
				smppMSGLEN=0;
				smppMSGLEN+=smppHEADSIZE;
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_REPLACE_SM_ACK:
			{   
				//输入,输出结构形成
				char *oc=(char  *)dbuff;
				MI_REPLACE_SM_ACK *dc=(MI_REPLACE_SM_ACK *)obuff;
				smppMSGLEN=0;
				smppMSGLEN+=smppHEADSIZE;
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				//traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
                cmdSTATUS = dc->Status;
	            traFlg = TRANS_SUCCEED;
			}
			break;
        case SMEI_REPLACE_SM_EX_ACK:
			{   
				//输入,输出结构形成
				char *oc=(char  *)dbuff;
				MI_REPLACE_SM_EX_ACK *dc=(MI_REPLACE_SM_EX_ACK *)obuff;
				smppMSGLEN = smppHEADSIZE;
				innerMSGLEN = sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=ESME_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				if((smppMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换

                cmdSTATUS = dc->Status;
	            traFlg = TRANS_SUCCEED;
				//traFlg=CmdTransToSmpp(oc,dc,cmdSTATUS);
			}
			break;

        case SMEI_PPS_USER_CHECK:   //PPS用户鉴权
        {   
            //输入,输出结构形成
            Pps_User_Check *oc = (Pps_User_Check *)dbuff;
            MI_PPS_USER_CHECK *dc = (MI_PPS_USER_CHECK *)obuff;
            smppMSGLEN = sizeof(Pps_User_Check);

            innerMSGLEN = sizeof(*dc);
            if (innerMSGLEN > nOrgSize)
            {
	            traFlg = ESME_CODE_NO_ENOUGHT_ORG_BUF;
	            break;
            }
            smppMSGLEN += smppHEADSIZE;

            if ((smppMSGLEN) > nDestSize)  //目标缓冲区不够长
            {	
	             traFlg = ESME_CODE_NO_ENOUGHT_DEST_BUF;
	            break;
            }
            //消息流转换
            traFlg = CmdTransToSmpp(oc, dc, cmdSTATUS);
			}
			break;

        case SMEI_PPS_USER_CHECK_ACK:   //用户鉴权响应
        {   
            //输入,输出结构形成
            Pps_User_Check_Resp *oc = (Pps_User_Check_Resp *)dbuff;
            MI_PPS_USER_CHECK_ACK *dc = (MI_PPS_USER_CHECK_ACK *)obuff;
            smppMSGLEN = sizeof(Pps_User_Check_Resp);

            innerMSGLEN = sizeof(*dc);
            if (innerMSGLEN > nOrgSize)
            {
	            traFlg = ESME_CODE_NO_ENOUGHT_ORG_BUF;
	            break;
            }
            smppMSGLEN += smppHEADSIZE;

            if ((smppMSGLEN) > nDestSize)  //目标缓冲区不够长
            {	
	             traFlg = ESME_CODE_NO_ENOUGHT_DEST_BUF;
	            break;
            }
            //消息流转换
            traFlg = CmdTransToSmpp(oc, dc, cmdSTATUS);
			}
			break;


//短消息结果通知
#define SMEI_PPS_RESULT_NOTIFY_ACK	    0X81F5
        case SMEI_PPS_RESULT_NOTIFY:   //短消息结果通知
        {   
            //输入,输出结构形成
            Pps_Result_Notify *oc = (Pps_Result_Notify *)dbuff;
            MI_PPS_RESULT_NOTIFY *dc = (MI_PPS_RESULT_NOTIFY *)obuff;
            smppMSGLEN = sizeof(Pps_Result_Notify);

            innerMSGLEN = sizeof(*dc);
            if (innerMSGLEN > nOrgSize)
            {
	            traFlg = ESME_CODE_NO_ENOUGHT_ORG_BUF;
	            break;
            }
            smppMSGLEN += smppHEADSIZE;

            if ((smppMSGLEN) > nDestSize)  //目标缓冲区不够长
            {	
	             traFlg = ESME_CODE_NO_ENOUGHT_DEST_BUF;
	            break;
            }
            //消息流转换
            traFlg = CmdTransToSmpp(oc, dc, cmdSTATUS);
			}
			break;

        case SMEI_PPS_RESULT_NOTIFY_ACK:   //短消息结果通知
        {   
            //输入,输出结构形成
            Pps_Result_Notify_Resp *oc = (Pps_Result_Notify_Resp *)dbuff;
            MI_PPS_RESULT_NOTIFY_ACK *dc = (MI_PPS_RESULT_NOTIFY_ACK *)obuff;
            smppMSGLEN = sizeof(Pps_Result_Notify_Resp);

            innerMSGLEN = sizeof(*dc);
            if (innerMSGLEN > nOrgSize)
            {
	            traFlg = ESME_CODE_NO_ENOUGHT_ORG_BUF;
	            break;
            }
            smppMSGLEN += smppHEADSIZE;

            if ((smppMSGLEN) > nDestSize)  //目标缓冲区不够长
            {	
	             traFlg = ESME_CODE_NO_ENOUGHT_DEST_BUF;
	            break;
            }
            //消息流转换
            traFlg = CmdTransToSmpp(oc, dc, cmdSTATUS);
			}
			break;
        //////////////////////////////////////////////////
        //end add
        //////////////////////////////////////////////////
		default:
			//转换协议不支持
			traFlg=CODE_NOT_SUPPORT;
			break;
	}	
	nDestSize=smppMSGLEN;      //设置返回消息流的总长度
	if(traFlg==CODE_SUCCEED)  //消息体成功转换，则进行消息头的转换
	{
		nOrgSize=innerMSGLEN;
		DWORD smppStatus=0;
		InnerToSmppErrMap(cmdSTATUS,smppStatus);
		makeSMPPHEAD(smppHEADER,innerHEADER,smppMSGLEN,cmdSTATUS);
		if( (innerHEADER->nCommand == SMEI_DELIVER_SM) ||
            (innerHEADER->nCommand == SMEI_DELIVER_SM_ACK) ||
            (innerHEADER->nCommand == SMEI_STATUS_REPORT))
		{
			smppHEADER->Sequence_No = Sequence;
		}
	}
	
	return traFlg;
};	






//完成从SMPP流到INNER流的变化 
int CSmppAndInnerTrans::SmppToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize)
{
	unsigned char *obuff=(unsigned char *)pOrgBuff;
	unsigned char *dbuff=(unsigned char *)pDestBuff;
	
	//SMPP,inner头结构
	Message_header *smppHEADER=( Message_header *)pOrgBuff;
	MI_ONLYHEADER *innerHEADER=(MI_ONLYHEADER *)pDestBuff;
	
	int smppHEADSIZE=sizeof(Message_header);  //smpp头长度
	int innerHEADSIZE=sizeof(MI_ONLYHEADER);   //INNER头长度
	int innerMSGLEN;	//inner 消息长度
	int traFlg=DECODE_SUCCEED;  //消息体翻译成功与否的标志
	DWORD cmdSTATUS;
	
	int oPos=0,dPos=0;//源，目的BUFF读写位置

    BOOLEAN bStatusReportFlag = FALSE;

	//正文地址
	obuff+=smppHEADSIZE;
	
	//从SMPP消息头读取命令状态，只值只有反馈消息中有用
	DWORD smppSTATUS=smppHEADER->Command_Status;
	//SMPP和INNER错误类型的翻译
	SmppToInnerErrMap(smppSTATUS,cmdSTATUS);
	switch( smppHEADER->Command_ID)
	{
		case ESME_REPLACE_SM_EX:
			{
				//输入,输出结构形成
				Replace_Sm_Ex *oc=(Replace_Sm_Ex *)obuff;
				MI_REPLACE_SM_EX *dc=(MI_REPLACE_SM_EX *)dbuff;			
				
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(oc->data_coding, (UC)oc->sm_length);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN = sizeof(*dc) + iOctetLen - MAX_SM_LEN;

				if((innerMSGLEN) > nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_QUERY_SMS:
			{
				//输入,输出结构形成
				Query_Sms *oc=(Query_Sms *)obuff;
				MI_QUERY_SMS *dc=(MI_QUERY_SMS *)dbuff;			
				innerMSGLEN=sizeof(MI_QUERY_SMS);
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_QUERY_SMS_RESP:
			{
				//输入,输出结构形成
				Query_Sms_Resp *oc=(Query_Sms_Resp *)obuff;
				MI_QUERY_SMS_ACK *dc=(MI_QUERY_SMS_ACK *)dbuff;			
				innerMSGLEN=sizeof(MI_QUERY_SMS_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_QUERY_SM_DETAILS:
			{
				//输入,输出结构形成
				Query_Sm_Details *oc=(Query_Sm_Details *)obuff;
				MI_QUERY_SM_DETAILS_EX *dc=(MI_QUERY_SM_DETAILS_EX *)dbuff;
				innerMSGLEN=sizeof(MI_QUERY_SM_DETAILS_EX);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_QUERY_SM_DETAILS_RESP:
			{
				//输入,输出结构形成
				Query_Sm_Details_Resp *oc=(Query_Sm_Details_Resp *)obuff;
				MI_QUERY_SM_DETAILS_EX_ACK *dc=(MI_QUERY_SM_DETAILS_EX_ACK *)dbuff;			
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(oc->data_coding,(UC)oc->message_length);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN = sizeof(*dc) + iOctetLen - MAX_SM_LEN;

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_DEL_SUB:   
		case ESME_QRY_SUB:
			{
				//输入,输出结构形成
				Del_Sub *oc=(Del_Sub *)obuff;
				MI_DEL_SUB		  *dc=(MI_DEL_SUB *)dbuff;			
				innerMSGLEN=sizeof(MI_DEL_SUB);
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_ADD_SUB_RESP: 
		case ESME_DEL_SUB_RESP:
		case ESME_MOD_SUB_RESP:
			{
				char *oc=(char  *)obuff;
				MI_ADD_SUB_ACK *dc=(MI_ADD_SUB_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_ADD_SUB_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;

		case ESME_QRY_SUB_RESP:
			{
					//输入,输出结构形成
				Enquire_Sub_Resp *oc=(Enquire_Sub_Resp *)obuff;
				MI_ENQUIRE_SUB_ACK *dc=(MI_ENQUIRE_SUB_ACK *)dbuff;			
				innerMSGLEN=sizeof(MI_ENQUIRE_SUB_ACK);
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;

		case ESME_ADD_SUB:
			{
					//输入,输出结构形成
				Add_Sub *oc=(Add_Sub *)obuff;
				MI_ADD_SUB		  *dc=(MI_ADD_SUB *)dbuff;			
				innerMSGLEN=sizeof(MI_ADD_SUB);
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_MOD_SUB:
			{
					//输入,输出结构形成
				Add_Sub *oc=(Add_Sub *)obuff;
				MI_MOD_SUB		  *dc=(MI_MOD_SUB *)dbuff;			
				innerMSGLEN=sizeof(MI_MOD_SUB);
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;

		//-------------
		case ESME_QRYLINK_RESP:
			{
				char *oc=(char  *)obuff;
				MI_ENQUIRE_LINK_ACK *dc=(MI_ENQUIRE_LINK_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_ENQUIRE_LINK_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				traFlg=DECODE_SUCCEED;
			}
			break;
		case ESME_UBD_RESP:
			{
				char *oc=(char  *)obuff;
				MI_UNBIND_ACK *dc=(MI_UNBIND_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_UNBIND_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_QRYLINK:
		case ESME_NACK:
		case ESME_UBD:   //UNBIND ,	QRYLINK, NACK
			{
				char *oc=(char  *)obuff;
				MI_UNBIND *dc=(MI_UNBIND *)dbuff;
				innerMSGLEN=sizeof(MI_UNBIND);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_BNDRCV:   //bind
		case ESME_BNDTRN:
			{
				//输入,输出结构形成
				Bind_Receiver *oc=(Bind_Receiver *)obuff;
				MI_BIND		  *dc=(MI_BIND *)dbuff;
			
				innerMSGLEN=sizeof(MI_BIND);
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case ESME_BNDTRN_RESP:
		case ESME_BNDRCV_RESP:
			{
					//输入,输出结构形成
				Bind_Receiver_Resp *oc=(Bind_Receiver_Resp *)obuff;
				MI_BIND_ACK 	*dc=(MI_BIND_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_BIND_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_SUB_SM:
			{
				Submit_Sm *oc=(Submit_Sm *)obuff;
				MI_SUBMIT_SM *dc=(MI_SUBMIT_SM *)dbuff;
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(oc->data_coding, (UC)oc->sm_length);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN = sizeof(*dc) + iOctetLen - MAX_SM_LEN;
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
				ASSERT(innerMSGLEN < 2000);
			}
			break;
		case ESME_SUB_SM_RESP:
			{
				Submit_Sm_Resp *oc=(Submit_Sm_Resp *)obuff;
				MI_SUBMIT_SM_ACK *dc=(MI_SUBMIT_SM_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_SUBMIT_SM_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_DELIVER_SM:   //	注：DELIVE SM 在SMPP和SUBMIT SM 结构相同
			{
                //输入,输出结构形成
				Submit_Sm *oc=(Submit_Sm *)obuff;
                if (oc->esm_class & ESM_CLASS_STATUS_REPORT) //此DELIVER 消息为状态报告
                {
                    MI_STATUS_REPORT *dc = (MI_STATUS_REPORT *)dbuff;
                    bStatusReportFlag = TRUE;
				    int iOctetLen = 0;
				    iOctetLen = GetOctetLength(oc->data_coding, (UC)oc->sm_length);
				    if( iOctetLen < 0)
				    {
					    ASSERT(0);
					    traFlg=CODE_DCS_NOT_SUPPORT;
					    break;
				    }
				    innerMSGLEN = sizeof(*dc) + iOctetLen - MAX_SM_LEN;
				    
				    if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				    {	
					     traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					    break;
				    }
				    innerMSGLEN-=innerHEADSIZE; 
				    //消息流转换
				    traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
				    dc->sm_ID = smppHEADER->Sequence_No;				    
                }
                else
                {
				    MI_DELIVER_SM *dc=(MI_DELIVER_SM *)dbuff;
                    bStatusReportFlag = FALSE;
				    int iOctetLen = 0;
				    iOctetLen = GetOctetLength(oc->data_coding, (UC)oc->sm_length);
				    if( iOctetLen < 0)
				    {
					    ASSERT(0);
					    traFlg=CODE_DCS_NOT_SUPPORT;
					    break;
				    }
				    innerMSGLEN = sizeof(*dc) + iOctetLen - MAX_SM_LEN;
				    
				    if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				    {	
					     traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					    break;
				    }
				    innerMSGLEN-=innerHEADSIZE; 
				    //
				    //消息流转换
				    traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
				    dc->sm_ID = smppHEADER->Sequence_No;
				    /*
				    if(traFlg==TRANS_SUCCEED)
				    {
					    innerMSGLEN=innerMSGLEN-SHORT_MESSAGE_LEN+(oc->sm_length);
				    }
				    */
                }                				
			}
			break;
		//	注：DELIVE SM RESP 在SMPP和SUBMIT SM RESP结构相同
		case ESME_DELIVER_SM_RESP:
			{   
					//输入,输出结构形成
				Submit_Sm_Resp *oc=(Submit_Sm_Resp *)obuff;
				MI_DELIVER_SM_ACK *dc=(MI_DELIVER_SM_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_DELIVER_SM_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);

                dc->sm_ID = smppHEADER->Sequence_No;
			}
			break;
		case ESME_QUERY_SM:
			{   
					//输入,输出结构形成
				Query_Sm *oc=(Query_Sm *)obuff;
				MI_QUERY_SM_STATUS *dc=(MI_QUERY_SM_STATUS *)dbuff;
				innerMSGLEN=sizeof(MI_QUERY_SM_STATUS);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_QUERY_SM_RESP:
			{   
					//输入,输出结构形成
				Query_Sm_Resp *oc=(Query_Sm_Resp *)obuff;
				MI_QUERY_SM_STATUS_ACK *dc=(MI_QUERY_SM_STATUS_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_QUERY_SM_STATUS_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_CANCEL_SM:
			{   
					//输入,输出结构形成
				Cancel_Sm *oc=(Cancel_Sm *)obuff;
				MI_CANCEL_SM *dc=(MI_CANCEL_SM *)dbuff;
				innerMSGLEN=sizeof(MI_CANCEL_SM);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_CANCEL_SM_RESP:
			{   
					//输入,输出结构形成
				char *oc=(char  *)obuff;
				MI_CANCEL_SM_ACK *dc=(MI_CANCEL_SM_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_CANCEL_SM_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case ESME_REPLACE_SM:    
			{   
					//输入,输出结构形成
				Replace_Sm *oc=(Replace_Sm  *)obuff;
				MI_REPLACE_SM *dc=(MI_REPLACE_SM *)dbuff;
				innerMSGLEN=sizeof(*dc)+GetOctetLength(0/*oc->data_coding*/,(UC)oc->sm_length)-MAX_SM_LEN;

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
				/*
				if(traFlg==TRANS_SUCCEED)
				{
					innerMSGLEN=innerMSGLEN-SHORT_MESSAGE_LEN+(oc->sm_length);
				}
				*/
			}
			break;
		case ESME_REPLACE_SM_RESP:
		case ESME_REPLACE_SM_EX_RESP:
			{   
			    //输入,输出结构形成
				char *oc=(char  *)obuff;
				MI_REPLACE_SM_ACK *dc=(MI_REPLACE_SM_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_REPLACE_SM_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
        
        case ESME_PPS_USER_CHECK:
            {
                //输入,输出结构形成
				Pps_User_Check *pOrgStru = (Pps_User_Check  *)obuff;
				MI_PPS_USER_CHECK *pDestStru = (MI_PPS_USER_CHECK *)dbuff;
				innerMSGLEN = sizeof(MI_PPS_USER_CHECK);				

				if( (innerMSGLEN) > nDestSize)  //目标缓冲区不够长
				{	
					 traFlg = ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN -= innerHEADSIZE;
				//
				//消息流转换
				traFlg = CmdTransToInner(pOrgStru, pDestStru, cmdSTATUS);
            }
            
            break;

        case ESME_PPS_USER_CHECK_RESP:
            {
                //输入,输出结构形成
				Pps_User_Check_Resp *pOrgStru = (Pps_User_Check_Resp  *)obuff;
				MI_PPS_USER_CHECK_ACK *pDestStru = (MI_PPS_USER_CHECK_ACK *)dbuff;
				innerMSGLEN = sizeof(MI_PPS_USER_CHECK_ACK);				

				if( (innerMSGLEN) > nDestSize)  //目标缓冲区不够长
				{	
					 traFlg = ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN -= innerHEADSIZE;
				//
				//消息流转换
				traFlg = CmdTransToInner(pOrgStru, pDestStru, cmdSTATUS);
            }            
            break;

        case ESME_PPS_RESULT_NOTIFY:
            {
                //输入,输出结构形成
				Pps_Result_Notify *pOrgStru = (Pps_Result_Notify  *)obuff;
				MI_PPS_RESULT_NOTIFY *pDestStru = (MI_PPS_RESULT_NOTIFY *)dbuff;
				innerMSGLEN = sizeof(MI_PPS_RESULT_NOTIFY);				

				if( (innerMSGLEN) > nDestSize)  //目标缓冲区不够长
				{	
					 traFlg = ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN -= innerHEADSIZE;
				//
				//消息流转换
				traFlg = CmdTransToInner(pOrgStru, pDestStru, cmdSTATUS);
            }            
            break;

        case ESME_PPS_RESULT_NOTIFY_RESP:
            {
                //输入,输出结构形成
				Pps_Result_Notify_Resp *pOrgStru = (Pps_Result_Notify_Resp  *)obuff;
				MI_PPS_RESULT_NOTIFY_ACK *pDestStru = (MI_PPS_RESULT_NOTIFY_ACK *)dbuff;
				innerMSGLEN = sizeof(MI_PPS_RESULT_NOTIFY_ACK);				

				if( (innerMSGLEN) > nDestSize)  //目标缓冲区不够长
				{	
					 traFlg = ESME_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN -= innerHEADSIZE;
				//
				//消息流转换
				traFlg = CmdTransToInner(pOrgStru, pDestStru, cmdSTATUS);
            }            
            break;

        ///////////////////////////////////////////////////
        //end add
        ///////////////////////////////////////////////////
		default:
			//转换协议不支持,COMMAND ID	错误
			traFlg=	ESME_RINVCMDID;
			break;
	}	
	nDestSize=innerMSGLEN+innerHEADSIZE;

	//nDestSize=innerMSGLEN;

	if(traFlg==DECODE_SUCCEED) //成功进行消息体的转换，则进行消息头的转换
	{
        makeINNERHEAD(smppHEADER, innerHEADER, innerMSGLEN, bStatusReportFlag);
	}
	return traFlg;
};

int CSmppAndInnerTrans::CmdTransToInner(Enquire_Sub_Resp * oc,MI_ENQUIRE_SUB_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	dc->TON=oc->sub_ton;
	dc->NPI=oc->sub_npi;
	int ret=ConvertSmppAddrToString((UC*)oc->sub_ID,dc->TON,dc->NPI,dc->SubID);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}
	CODECOPY(TRANS_ERROR,dc->SubName,oc->sub_name);
	CODECOPY(TRANS_ERROR,dc->SubAddress,oc->sub_address);
	dc->MSType=oc->MS_type;	
	dc->Service=oc->service_level;
	dc->bSM=oc->SM_allowed;
	dc->OCOS=oc->OCOS;
	dc->TCOS=oc->TCOS;
	CODECOPY(TRANS_ERROR,dc->Password,oc->password);
	SMPP_TO_INNER_TIME_TRANS(dc->RegDateTime,oc->reg_datetime);
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Enquire_Sub_Resp * oc,MI_ENQUIRE_SUB_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;

	oc->sub_ton=dc->TON;
	oc->sub_npi=dc->NPI;
	int ret=ConvertStringToSmppAddr(dc->SubID,dc->TON,dc->NPI,(UC*)oc->sub_ID);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}

	CODECOPY(TRANS_ERROR,oc->sub_name,dc->SubName);
	CODECOPY(TRANS_ERROR,oc->sub_address,dc->SubAddress);	
	oc->MS_type=dc->MSType;

	oc->service_level=dc->Service;
	oc->SM_allowed=dc->bSM;
	oc->OCOS=(UC)dc->OCOS;
	oc->TCOS=(UC)dc->TCOS;
	CODECOPY(TRANS_ERROR,oc->password,dc->Password);
	INNER_TO_SMPP_TIME_TRANS(oc->reg_datetime,dc->RegDateTime);
	return TRANS_SUCCEED;
}

//---
int CSmppAndInnerTrans::CmdTransToInner(Add_Sub * oc,MI_ADD_SUB *dc,int cmdStatus)
{
	dc->TON=oc->sub_ton;
	dc->NPI=oc->sub_npi;
	int ret=ConvertSmppAddrToString((UC*)oc->sub_ID,dc->TON,dc->NPI,dc->SubID);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}

	CODECOPY(TRANS_ERROR,dc->SubName,oc->sub_name);
	CODECOPY(TRANS_ERROR,dc->SubAddress,oc->sub_address);
	dc->MSType=oc->MS_type;

	dc->Service=oc->service_level;
	dc->bSM=oc->SM_allowed;
	dc->OCOS=oc->OCOS;
	dc->TCOS=oc->TCOS;
	CODECOPY(TRANS_ERROR,dc->Password,oc->password);
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Add_Sub * oc,MI_ADD_SUB *dc,int& cmdStatus)
{
	oc->sub_ton=dc->TON;
	oc->sub_npi=dc->NPI;
	int ret=ConvertStringToSmppAddr(dc->SubID,dc->TON,dc->NPI,(UC*)oc->sub_ID);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}
	CODECOPY(TRANS_ERROR,oc->sub_name,dc->SubName);
	CODECOPY(TRANS_ERROR,oc->sub_address,dc->SubAddress);
	oc->MS_type=dc->MSType;

	oc->service_level=dc->Service;
	oc->SM_allowed=dc->bSM;
	oc->OCOS=(UC)dc->OCOS;
	oc->TCOS=(UC)dc->TCOS;
	CODECOPY(TRANS_ERROR,oc->password,dc->Password);
	
	return TRANS_SUCCEED;
}

//---在此处,用SMPP中的ADD SUB代替MOD SUB ,两者在SMPP中的结构是相同的

int CSmppAndInnerTrans::CmdTransToInner(Add_Sub * oc,MI_MOD_SUB *dc,int cmdStatus)
{

	dc->TON=oc->sub_ton;
	dc->NPI=oc->sub_npi;
	int ret=ConvertSmppAddrToString((UC*)oc->sub_ID,dc->TON,dc->NPI,dc->SubID);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}

	CODECOPY(TRANS_ERROR,dc->SubName,oc->sub_name);
	CODECOPY(TRANS_ERROR,dc->SubAddress,oc->sub_address);
	dc->MSType=oc->MS_type;
	
	dc->Service=oc->service_level;
	dc->bSM=oc->SM_allowed;
	dc->OCOS=oc->OCOS;
	dc->TCOS=oc->TCOS;
	CODECOPY(TRANS_ERROR,dc->Password,oc->password);
	return TRANS_SUCCEED;
}
//在此处,用SMPP中的ADD SUB代替MOD SUB ,两者在SMPP中的结构是相同的
int CSmppAndInnerTrans::CmdTransToSmpp(Add_Sub * oc,MI_MOD_SUB *dc,int& cmdStatus)
{ 

	oc->sub_ton=dc->TON;
	oc->sub_npi=dc->NPI;
	int ret=ConvertStringToSmppAddr(dc->SubID,dc->TON,dc->NPI,(UC*)oc->sub_ID);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}

	CODECOPY(TRANS_ERROR,oc->sub_name,dc->SubName);
	CODECOPY(TRANS_ERROR,oc->sub_address,dc->SubAddress);
	oc->MS_type=dc->MSType;
		
	oc->service_level=dc->Service;
	oc->SM_allowed=dc->bSM;
	oc->OCOS=(UC)dc->OCOS;
	oc->TCOS=(UC)dc->TCOS;
	CODECOPY(TRANS_ERROR,oc->password,dc->Password);
	
	return TRANS_SUCCEED;
}
//-----
int CSmppAndInnerTrans::CmdTransToInner(char * oc,MI_ADD_SUB_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(char * oc,MI_ADD_SUB_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	return TRANS_SUCCEED;
}
//----
int CSmppAndInnerTrans::CmdTransToInner(Del_Sub * oc,MI_DEL_SUB *dc,int cmdStatus)
{
	int ret=ConvertSmppAddrToString((UC*)oc->sub_ID,0/*dc->TON*/,1/*dc->NPI*/,dc->SubID);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}

	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Del_Sub * oc,MI_DEL_SUB *dc,int& cmdStatus)
{
	oc->sub_ton=0;
	oc->sub_npi=1;
	int ret=ConvertStringToSmppAddr(dc->SubID,0/*dc->TON*/,1/*dc->NPI*/,(UC*)oc->sub_ID);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->sub_ID))
	{
		return TRANS_ADDR_ERROR;
	}
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(char * oc,MI_UNBIND *dc,int cmdStatus)
{
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToSmpp(char * oc,MI_UNBIND *dc,int& cmdStatus)
{
	return TRANS_SUCCEED;
}

//-----
int CSmppAndInnerTrans::CmdTransToInner(char * oc,MI_UNBIND_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(char * oc,MI_UNBIND_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	return TRANS_SUCCEED;
}



//oc 输入结构，DC输出结构，CMDSTATUS：消息状态（位于HEADER中)
int CSmppAndInnerTrans::CmdTransToInner(char * oc,MI_REPLACE_SM_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToSmpp(char * oc,MI_REPLACE_SM_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	return TRANS_SUCCEED;
}

//------
int CSmppAndInnerTrans::CmdTransToInner(Replace_Sm * oc,MI_REPLACE_SM *dc,int cmdStatus)
{
	dc->sm_ID=HexStrToInt(oc->original_message_id);
	dc->OrgTON=oc->orig_addr_ton;
	
	dc->OrgNPI=oc->orig_addr_npi;

	int ret=ConvertSmppAddrToString((UC *)oc->originating_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->OrgAddr))
	{
		return TRANS_ADDR_ERROR;   
	}

	SMPP_TO_INNER_TIME_TRANS(dc->Schedule,oc->schedule_delivery_time);
	SMPP_TO_INNER_TIME_TRANS(dc->Expire,oc->validity_period);
	dc->SRR=0;
	dc->UDL=(unsigned char )oc->sm_length;
	dc->DCS=0;

	unsigned short UDL;
    int retvalue=ConvertTextToDataEx(oc->short_message, oc->sm_length, dc->DCS, \
        (UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
	if(retvalue<0) 
	{
		TRACE("数据转换到DATA错误\n");
		return TRANS_ERROR;
	}
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Replace_Sm * oc,MI_REPLACE_SM *dc,int& cmdStatus)
{
	IntToHexStr(oc->original_message_id,dc->sm_ID);
	oc->orig_addr_ton=dc->OrgTON;
	oc->orig_addr_npi=dc->OrgNPI;
	int ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->originating_addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->originating_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	INNER_TO_SMPP_TIME_TRANS(oc->schedule_delivery_time,dc->Schedule);
	INNER_TO_SMPP_TIME_TRANS(oc->validity_period,dc->Expire);

    if( dc->UDL >= MAX_SM_LEN)
    {
        ASSERT(0);
        dc->UDL = MAX_SM_LEN - 1;
    }

	int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,0/*dc->UDHI*/,oc->short_message, CODE_INTERFACE_SMPP);
	if(retvalue<0) 
	{
		TRACE("数据转换到TEXT错误\n");
		return TRANS_ERROR;
	}

    oc->sm_length=(UC)dc->UDL;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Replace_Sm_Ex * oc,MI_REPLACE_SM_EX *dc,int cmdStatus)
{
	dc->sm_ID=HexStrToInt(oc->original_message_id);
	dc->OrgTON=oc->orig_addr_ton;

	dc->OrgNPI=oc->orig_addr_npi;

	int ret=ConvertSmppAddrToString((UC *)oc->originating_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->OrgAddr))
	{
		return TRANS_ADDR_ERROR;   
	}

	SMPP_TO_INNER_TIME_TRANS(dc->Schedule,oc->schedule_delivery_time);
	SMPP_TO_INNER_TIME_TRANS(dc->Expire,oc->validity_period);
    
	dc->DCS=oc->data_coding;
	dc->UDL=(unsigned char )oc->sm_length;

    // 兼容PHASE II+
    if (oc->esm_class & ESM_CLASS_UDHI)
        dc->UDHI = 1;
    else
        dc->UDHI = 0;

	//CODECOPY(TRANS_ERROR,dc->UD,oc->short_message);  
	unsigned short UDL;
    if (dc->UDHI)
    {
        int iOctLen = GetOctetLength(oc->data_coding, oc->sm_length);
        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(dc->UD, oc->short_message, iOctLen);
    }
    else
    {
	    //int retvalue=ConvertTextToData(oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
        int retvalue=ConvertTextToDataEx(oc->short_message, oc->sm_length, \
            oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
	    //dc->UDL=(unsigned char)oc->sm_length;
	    if(retvalue<0) 
	    {
		    TRACE("数据转换到DATA错误\n");
		    return TRANS_ERROR;
	    }
    }

    dc->UDL=(unsigned char)oc->sm_length;
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Replace_Sm_Ex * oc,MI_REPLACE_SM_EX *dc,int& cmdStatus)
{
	IntToHexStr(oc->original_message_id,dc->sm_ID);
	oc->orig_addr_ton=dc->OrgTON;
	oc->orig_addr_npi=dc->OrgNPI;
	int ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->originating_addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->originating_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	INNER_TO_SMPP_TIME_TRANS(oc->schedule_delivery_time,dc->Schedule);
	INNER_TO_SMPP_TIME_TRANS(oc->validity_period,dc->Expire);
	
	//oc->sm_length=(char)dc->UDL;
	oc->data_coding = dc->DCS;
	//because replace sm have not UDHI,set it to 0
    
    if (dc->UDHI)
    {
        oc->esm_class = ESM_CLASS_UDHI;

        int iOctLen = GetOctetLength(dc->DCS, dc->UDL);
        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(oc->short_message, dc->UD, iOctLen);
    }
    else
    {
        oc->esm_class = 0;

	    int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,0/*dc->UDHI*/,oc->short_message, CODE_INTERFACE_SMPP);
	    if(retvalue<0) 
	    {
		    TRACE("数据转换到TEXT错误\n");
		    return TRANS_ERROR;
	    }
    }

    oc->sm_length=(UC)dc->UDL;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Query_Sms * oc,MI_QUERY_SMS *dc,int cmdStatus)
{
	dc->Direction=oc->direction;
	dc->TON=oc->ton; 
	dc->NPI=oc->npi; 

	int ret=ConvertSmppAddrToString((UC *)oc->addr,dc->TON,dc->NPI,dc->Addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->Addr))
	{
		return TRANS_ADDR_ERROR;   
	}
	SMPP_TO_INNER_TIME_TRANS(dc->DateTime,oc->start_time);
	SMPP_TO_INNER_TIME_TRANS(dc->FinaTime,oc->final_time);

	dc->Number=oc->number;
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Query_Sms * oc,MI_QUERY_SMS *dc,int& cmdStatus)
{
	oc->direction=dc->Direction;		
	oc->ton=dc->TON; 
	oc->npi=dc->NPI; 
	int ret=ConvertStringToSmppAddr(dc->Addr,dc->TON,dc->NPI,(UC*)oc->addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->addr))
	{
		return TRANS_ADDR_ERROR;
	} 
	INNER_TO_SMPP_TIME_TRANS(oc->start_time,dc->DateTime);
	INNER_TO_SMPP_TIME_TRANS(oc->final_time,dc->FinaTime);
	oc->number=dc->Number;	
	return TRANS_SUCCEED;
}
//---QUERY_SMS_RESP
int CSmppAndInnerTrans::CmdTransToInner(Query_Sms_Resp * oc,MI_QUERY_SMS_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	dc->Direction=oc->direction;	
	dc->Number=oc->number; 

	for(int i=0;i<oc->number;i++)
	{
		dc->sm_ID[i]=HexStrToInt(oc->sm_ID[i]);
	}
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Query_Sms_Resp * oc,MI_QUERY_SMS_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	oc->direction=dc->Direction;	
	oc->number=dc->Number; 

    if (oc->number > MAX_SM_NUM)
    {
        ASSERT(0);
        return TRANS_ERROR;
    }
	for(int i=0;i<dc->Number;i++)
	{
		IntToHexStr(oc->sm_ID[i],dc->sm_ID[i]);
	}
	return TRANS_SUCCEED;
}

//---query_sm_details
int CSmppAndInnerTrans::CmdTransToInner(Query_Sm_Details * oc,MI_QUERY_SM_DETAILS_EX *dc,int cmdStatus)
{
	dc->sm_ID=HexStrToInt(oc->sm_ID);
	dc->TON=oc->originating_ton; 
	dc->NPI=oc->originating_npi; 
	int ret=ConvertSmppAddrToString((UC *)oc->originating_addr,dc->TON,dc->NPI,dc->Addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->Addr))
	{
		return TRANS_ADDR_ERROR;   
	}
	dc->Len=(UC)oc->sm_length;
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp (Query_Sm_Details * oc,MI_QUERY_SM_DETAILS_EX *dc,int& cmdStatus)
{
	IntToHexStr(oc->sm_ID,dc->sm_ID);
	oc->originating_ton=dc->TON; 
	oc->originating_npi=dc->NPI; 
	int ret=ConvertStringToSmppAddr(dc->Addr,dc->TON,dc->NPI,(UC*)oc->originating_addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->originating_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	oc->sm_length=(UC ) dc->Len;
	return TRANS_SUCCEED;
}

//---QUERY_SM_DETAILS_RESP
int CSmppAndInnerTrans::CmdTransToInner(Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_EX_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;	
	dc->Service_type=NULL;  //保留
	dc->OrgTON=oc->originating_ton;	
	dc->OrgNPI=oc->originating_npi;	
	int ret=ConvertSmppAddrToString((UC *)oc->originating_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->OrgAddr))
	{
		return TRANS_ADDR_ERROR;   
	}
	dc->DestTON=oc->dest_ton;	
	dc->DestNPI=oc->dest_npi;	

	ret=ConvertSmppAddrToString((UC *)oc->dest_addr,dc->DestTON,dc->DestNPI,dc->DestAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->DestAddr))
	{
		return TRANS_ADDR_ERROR;   
	}

	dc->PRI=oc->priority_flag;	
    dc->RD = oc->RD;
    dc->RP = oc->RP;
    dc->UDHI = oc->UDHI;
	dc->SRR=oc->registered_delivery_flag;
    dc->MR = oc->MR;
    dc->PID=oc->protocol_ID;
	dc->DCS=oc->data_coding;
    
	SMPP_TO_INNER_TIME_TRANS(dc->Schedule,oc->schedule_delivery_time);
	SMPP_TO_INNER_TIME_TRANS(dc->Expire,oc->validity_period);
	SMPP_TO_INNER_TIME_TRANS(dc->Final,oc->final_date);
	dc->sm_ID=HexStrToInt(oc->message_id);
	dc->sm_SubID=0; 
	//dc->sm_Status=oc->message_status;
	dc->sm_Status = ConvertSMPPStateToSMEIState(oc->message_status);
	//dc->Error_Code=oc->GSM_code; 
	dc->Error_Code = ConvertSMPPErrCodeToSMEIErrCode(oc->GSM_code); 
	dc->ConFlag=0;
	
	unsigned short UDL;

    if (dc->UDHI)
    {
        int iOctLen = GetOctetLength(oc->data_coding, oc->message_length);
        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(dc->UD, oc->short_message, iOctLen);
    }
    else
    {
	    //int retvalue=ConvertTextToData(oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
        int retvalue=ConvertTextToDataEx(oc->short_message, oc->message_length, \
            oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
	    if(retvalue<0) 
	    {
		    TRACE("数据转换到DATA错误\n");
		    return TRANS_ERROR;
	    }
    }

    dc->UDL = (unsigned char)oc->message_length;
	
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_EX_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	memset(oc->service_type,0,SERVICE_TYPE_LEN);
	oc->originating_ton=dc->OrgTON;
	oc->originating_npi=dc->OrgNPI;

	int ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->originating_addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->originating_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	oc->dest_ton=dc->DestTON;
	oc->dest_npi=dc->DestNPI;
	ret=ConvertStringToSmppAddr(dc->DestAddr,dc->DestTON,dc->DestNPI,(UC*)oc->dest_addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->originating_addr))
	{
		return TRANS_ADDR_ERROR;
	}

	oc->priority_flag=dc->PRI;
    oc->RD = dc->RD;
    oc->RP = dc->RP;
    oc->UDHI = dc->UDHI;
    oc->registered_delivery_flag=dc->SRR;
    oc->MR = dc->MR;
	oc->protocol_ID=dc->PID;
	oc->data_coding=dc->DCS;

	INNER_TO_SMPP_TIME_TRANS(oc->schedule_delivery_time,dc->Schedule);
	INNER_TO_SMPP_TIME_TRANS(oc->validity_period,dc->Expire);
	INNER_TO_SMPP_TIME_TRANS(oc->final_date,dc->Final);
	IntToHexStr(oc->message_id,dc->sm_ID);
	oc->message_status = ConvertSMEIStateToSMPPState(dc->sm_Status);
	oc->GSM_code = ConvertSMEIErrCodeToSMPPErrCode(dc->Error_Code);

    if( dc->UDL >= MAX_SM_LEN)
    {
        ASSERT(0);
        dc->UDL = MAX_SM_LEN - 1;
    }

    if (dc->UDHI)
    {
        int iOctLen = GetOctetLength(dc->DCS, dc->UDL);
        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(oc->short_message, dc->UD, iOctLen);
    }
    else
    {

	    //oc->message_length=dc->UDL;
	    //because replace sm have not UDHI,set it to 0
	    int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,0/*dc->UDHI*/,oc->short_message, CODE_INTERFACE_SMPP);
	    if(retvalue<0) 
	    {
		    TRACE("数据转换到TEXT错误\n");
		    return TRANS_ERROR;
	    }
    }

    oc->message_length = (UC)dc->UDL;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(char * oc,MI_CANCEL_SM_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(char * oc,MI_CANCEL_SM_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Cancel_Sm * oc,MI_CANCEL_SM *dc,int cmdStatus)
{
	//在SMPP中此值强制为NULL;
	dc->Service_type=NULL; //oc->service_type[0];
	dc->sm_ID=HexStrToInt(oc->original_message_id);
	dc->OrgNPI=oc->source_addr_npi;
	dc->OrgTON=oc->source_addr_ton;
	//CODECOPY(TRANS_ERROR,dc->OrgAddr,oc->source_addr);
	int ret=ConvertSmppAddrToString((UC *)oc->source_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->OrgAddr))
	{
		return TRANS_ADDR_ERROR;   
	}

	dc->DestTON=oc->dest_addr_ton;
	dc->DestNPI=oc->dest_addr_npi;
	//CODECOPY(TRANS_ERROR,dc->DestAddr,oc->destination_addr);
	ret=ConvertSmppAddrToString((UC *)oc->destination_addr,dc->DestTON,dc->DestNPI,dc->DestAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->source_addr))
	{
		return TRANS_ADDR_ERROR;   
	}
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Cancel_Sm * oc,MI_CANCEL_SM *dc,int& cmdStatus)
{
	//在SMPP中此值强制为NULL;
	oc->service_type[0]=dc->Service_type;
	IntToHexStr(oc->original_message_id,dc->sm_ID);
	oc->source_addr_ton=dc->OrgTON;
	oc->source_addr_npi=dc->OrgNPI;
	//CODECOPY(TRANS_ERROR,oc->source_addr,dc->OrgAddr);
	int ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->source_addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->source_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	oc->dest_addr_ton=dc->DestTON;
	oc->dest_addr_npi=dc->DestNPI;
	//CODECOPY(TRANS_ERROR,oc->destination_addr,dc->DestAddr);
	ret=ConvertStringToSmppAddr(dc->DestAddr,dc->DestTON,dc->DestNPI,(UC*)oc->destination_addr);
	if(ret==CONVERT_STRING_ADDR_ERROR||ret>=sizeof(oc->destination_addr))
	{
		return TRANS_ADDR_ERROR;
	}

	return TRANS_SUCCEED;
}

//-----
int CSmppAndInnerTrans::CmdTransToInner(Query_Sm_Resp * oc,MI_QUERY_SM_STATUS_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	dc->sm_ID=HexStrToInt(oc->original_message_id);
	SMPP_TO_INNER_TIME_TRANS(dc->Final_date,oc->final_date);
	//dc->SM_Status=oc->message_status;
	dc->SM_Status = ConvertSMPPStateToSMEIState(oc->message_status);
	//dc->Error_Code=oc->GSM_code;
	dc->Error_Code = ConvertSMPPErrCodeToSMEIErrCode(oc->GSM_code);
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Query_Sm_Resp * oc,MI_QUERY_SM_STATUS_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	IntToHexStr(oc->original_message_id,dc->sm_ID);
	INNER_TO_SMPP_TIME_TRANS(oc->final_date,dc->Final_date);
	//oc->message_status=dc->SM_Status;
	oc->message_status = ConvertSMEIStateToSMPPState(dc->SM_Status);
	//oc->GSM_code=dc->Error_Code;
	oc->GSM_code = ConvertSMEIErrCodeToSMPPErrCode(dc->Error_Code);
	return TRANS_SUCCEED;
}

//-----
int CSmppAndInnerTrans::CmdTransToInner(Query_Sm * oc,MI_QUERY_SM_STATUS *dc,int cmdStatus)
{
	dc->sm_ID=HexStrToInt(oc->original_message_id);
	dc->OrgTON=oc->originating_ton;
	dc->OrgNPI=oc->originating_npi;
	//CODECOPY(TRANS_ERROR,dc->OrgAddr,oc->originating_addr);
	int ret=ConvertSmppAddrToString((UC *)oc->originating_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->originating_addr))
	{
		return TRANS_ADDR_ERROR;   
	}

	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Query_Sm * oc,MI_QUERY_SM_STATUS *dc,int& cmdStatus)
{
	IntToHexStr(oc->original_message_id,dc->sm_ID);
	oc->originating_ton=dc->OrgTON;
	oc->originating_npi=dc->OrgNPI;
	//CODECOPY(TRANS_ERROR,oc->originating_addr,dc->OrgAddr);
	int ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->originating_addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->originating_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	return TRANS_SUCCEED;
}

//------
//此Submit_Sm_Resp代表Delive_Sm_Resp,因为它的结构相同，而在INNER中结构不同
int CSmppAndInnerTrans::CmdTransToInner(Submit_Sm_Resp * oc,MI_DELIVER_SM_ACK *dc,int cmdStatus)
{
	
	dc->sm_ID=HexStrToInt(oc->Message_id);
	dc->LinkNo=0;
	dc->Result=cmdStatus;
	dc->CS=0;
	dc->FCS=0;

	return TRANS_SUCCEED;
}
//因DELIVER SM RESP在SMPP中与SUBMIT SM RESP 结构相同，所以用SUBMIT SM RESP的结构代替DELIVER SM RESP的结构
int CSmppAndInnerTrans::CmdTransToSmpp(Submit_Sm_Resp * oc,MI_DELIVER_SM_ACK *dc,int& cmdStatus)
{
	
	IntToHexStr(oc->Message_id,dc->sm_ID);
	cmdStatus=dc->Result;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Submit_Sm * oc,MI_STATUS_REPORT *dc,int cmdStatus)
{
	dc->LinkNo=0;
	dc->DestTON=oc->dest_addr_ton;
	dc->DestNPI=oc->dest_addr_npi;
	int ret=ConvertSmppAddrToString((UC*)oc->destination_addr,dc->DestTON,dc->DestNPI,dc->DestAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->destination_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	dc->OrgTON=oc->source_addr_ton;
	dc->OrgNPI=oc->source_addr_npi;

	ret=ConvertSmppAddrToString((UC *)oc->source_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->source_addr))
	{
		return TRANS_ADDR_ERROR;   
	}

	dc->SCTON=0;
	dc->SCNPI=0;
	CODECOPY(TRANS_ERROR,dc->SCAddr,"");
	
	dc->PRI=oc->priority_flag;
	dc->byDCS=oc->data_coding;
    
    if (oc->sm_length >= MAX_MESSAGE_SM_LEN)
    {
        ASSERT(0);
        return TRANS_ERROR;
    }
	unsigned short UDL;
    int retvalue=ConvertTextToDataEx(oc->short_message, oc->sm_length, \
        oc->data_coding,(UC*)dc->sUD,UDL, CODE_INTERFACE_SMPP);

    dc->ulUDL=(unsigned char)oc->sm_length;      
	if(retvalue<0) 
	{
		TRACE("数据转换到DATA错误\n");
		return TRANS_ERROR;
	}

	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToInner(Submit_Sm * oc,MI_DELIVER_SM *dc,int cmdStatus)
{
	dc->LinkNo=0;
	dc->DestTON=oc->dest_addr_ton;
	dc->DestNPI=oc->dest_addr_npi;
	int ret=ConvertSmppAddrToString((UC*)oc->destination_addr,dc->DestTON,dc->DestNPI,dc->DestAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->destination_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	dc->OrgTON=oc->source_addr_ton;
	dc->OrgNPI=oc->source_addr_npi;
	//CODECOPY(TRANS_ERROR,dc->OrgAddr,oc->source_addr);
	ret=ConvertSmppAddrToString((UC *)oc->source_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->source_addr))
	{
		return TRANS_ADDR_ERROR;   
	}

	dc->SCTON=0;
	dc->SCNPI=0;
	CODECOPY(TRANS_ERROR,dc->SCAddr,"");
	
	dc->PRI=oc->priority_flag;
	dc->MMS=0;
	dc->RP=0;
	dc->SRI=0;
	dc->PID=oc->protocol_ID;
	dc->DCS=oc->data_coding;
	CODECOPY(TRANS_ERROR,dc->TimeStamp,"");	

    if(oc->esm_class & ESM_CLASS_UDHI)
        dc->UDHI = INNER_UDHI_FLAG;
    else
        dc->UDHI = 0;

    if (oc->esm_class & ESM_CLASS_REPLYPATH)
        dc->RP = 1;
    
    if(INNER_UDHI_FLAG == dc->UDHI) //此时消息透传
    {
        int iOctLen = GetOctetLength(oc->data_coding, oc->sm_length);
        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(dc->UD, oc->short_message, iOctLen);
        dc->UDL = oc->sm_length;
    }
    else
    {
        if (oc->sm_length >= MAX_MESSAGE_SM_LEN)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
	    unsigned short UDL;
	    //int retvalue=ConvertTextToData(oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
        int retvalue=ConvertTextToDataEx(oc->short_message, oc->sm_length, \
            oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
	    //dc->UDL=(unsigned char)oc->sm_length;

        dc->UDL=(unsigned char)oc->sm_length;

	    if(retvalue<0) 
	    {
		    TRACE("数据转换到DATA错误\n");
		    return TRANS_ERROR;
	    }
    }

	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToSmpp(Submit_Sm * oc,MI_STATUS_REPORT *dc,int& cmdStatus)
{
	oc->dest_addr_ton=dc->DestTON;
	oc->dest_addr_npi=dc->DestNPI;
	int ret=ConvertStringToSmppAddr(dc->DestAddr,dc->DestTON,dc->DestNPI,(UC*)oc->destination_addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->destination_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	oc->source_addr_ton=dc->OrgTON;
	oc->source_addr_npi=dc->OrgNPI;
	ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->source_addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->source_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	
	oc->priority_flag=dc->PRI;

	oc->data_coding=dc->byDCS;

    if( dc->ulUDL >= MAX_SM_LEN)
    {
        ASSERT(0);
        dc->ulUDL = MAX_SM_LEN - 1;
    }

    oc->esm_class = ESM_CLASS_STATUS_REPORT;
    
    
	int retvalue = ConvertDataToText((UC*)dc->sUD, (unsigned short)dc->ulUDL, dc->byDCS, \
        0, oc->short_message, CODE_INTERFACE_SMPP);
	
	if(retvalue<0) 
	{
		TRACE("数据转换到TEXT错误\n");
		return TRANS_ERROR;
	}
    oc->sm_length=(UC)dc->ulUDL;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToSmpp(Submit_Sm * oc,MI_DELIVER_SM *dc,int& cmdStatus)
{
	oc->dest_addr_ton=dc->DestTON;
	oc->dest_addr_npi=dc->DestNPI;
	int ret=ConvertStringToSmppAddr(dc->DestAddr,dc->DestTON,dc->DestNPI,(UC*)oc->destination_addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->destination_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	oc->source_addr_ton=dc->OrgTON;
	oc->source_addr_npi=dc->OrgNPI;
	ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->source_addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->source_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	
	oc->priority_flag=dc->PRI;

	oc->protocol_ID=dc->PID;
	oc->data_coding=dc->DCS;
    
    if( dc->UDL >= MAX_SM_LEN)
    {
        ASSERT(0);
        dc->UDL = MAX_SM_LEN - 1;
    }

    if(INNER_UDHI_FLAG == dc->UDHI)
        oc->esm_class = ESM_CLASS_UDHI;
    else
        oc->esm_class = 0;

    if (dc->RP)
        oc->esm_class |= ESM_CLASS_REPLYPATH;

    if( INNER_UDHI_FLAG == dc->UDHI) //此时消息透传
    {
        int iOctLen = GetOctetLength(dc->DCS, dc->UDL);

        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(oc->short_message, dc->UD, iOctLen);
    }
    else
    {
	    int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,dc->UDHI,oc->short_message, CODE_INTERFACE_SMPP);
	    
	    if(retvalue<0) 
	    {
		    TRACE("数据转换到TEXT错误\n");
		    return TRANS_ERROR;
	    }
    }

    oc->sm_length=(UC)dc->UDL;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Submit_Sm_Resp * oc,MI_SUBMIT_SM_ACK *dc,int cmdStatus)
{

	dc->sm_ID=HexStrToInt(oc->Message_id);
	dc->LinkNo=0;
	dc->MR=0;
	dc->Result=cmdStatus;
	dc->FCS=0;

	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Submit_Sm_Resp * oc,MI_SUBMIT_SM_ACK *dc,int& cmdStatus)
{

	IntToHexStr(oc->Message_id,dc->sm_ID);
	cmdStatus=dc->Result;
	return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Submit_Sm * oc,MI_SUBMIT_SM *dc,int cmdStatus)
{
	dc->sm_ID=0;
	dc->LinkNo=0;
	dc->OrgTON=oc->source_addr_ton;
	dc->OrgNPI=oc->source_addr_npi;

	int ret=ConvertSmppAddrToString((UC *)oc->source_addr,dc->OrgTON,dc->OrgNPI,dc->OrgAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->OrgAddr))
	{
		return TRANS_ADDR_ERROR;   
	}

	dc->DestTON=oc->dest_addr_ton;
	dc->DestNPI=oc->dest_addr_npi;
	ret=ConvertSmppAddrToString((UC*)oc->destination_addr,dc->DestTON,dc->DestNPI,dc->DestAddr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>(int)(sizeof(oc->destination_addr)))
	{
		return TRANS_ADDR_ERROR;   
	}


	dc->PRI=oc->priority_flag;
	dc->RD=oc->replace_if_present_flag;
    dc->SRR=oc->registered_delivery_flag;


	dc->PID=oc->protocol_ID;
	dc->RP=0;
	dc->MR=0;
	dc->DCS=oc->data_coding;
	SMPP_TO_INNER_TIME_TRANS(dc->Schedule,oc->schedule_delivery_time);
	SMPP_TO_INNER_TIME_TRANS(dc->Expire,oc->validity_period);
	dc->Default_ID=(unsigned char)oc->sm_default_msg_id;
    dc->UDL=(unsigned char)oc->sm_length;

	ASSERT(dc->UDL<=MAX_SM_LEN);
	if(dc->UDL>MAX_SM_LEN)  return TRANS_ERROR;

	unsigned short UDL;

    if(((oc->esm_class) & ESM_CLASS_UDHI) != 0)
        dc->UDHI = INNER_UDHI_FLAG;
    else
        dc->UDHI = 0;

    if (oc->esm_class & ESM_CLASS_REPLYPATH)
        dc->RP = 1;
    
    if( INNER_UDHI_FLAG == dc->UDHI)
    {
        int iOctLen = GetOctetLength(oc->data_coding, oc->sm_length);

        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(dc->UD, oc->short_message, iOctLen);
        dc->UDL = oc->sm_length;
    }
    else
    {
	    //int retvalue=ConvertTextToData(oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
        int retvalue=ConvertTextToDataEx(oc->short_message, oc->sm_length, \
            oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
        //dc->UDL=(unsigned char)oc->sm_length;
	    if(retvalue<0) 
	    {
		    TRACE("数据转换到DATA错误\n");
		    return TRANS_ERROR;
	    }
    }

	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Submit_Sm * oc,MI_SUBMIT_SM *dc,int& cmdStatus)
{ 
	oc->source_addr_ton=dc->OrgTON;
	oc->source_addr_npi=dc->OrgNPI;
	//CODECOPY(TRANS_ERROR,oc->source_addr,dc->OrgAddr);
	int ret=ConvertStringToSmppAddr(dc->OrgAddr,dc->OrgTON,dc->OrgNPI,(UC*)oc->source_addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->source_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	oc->dest_addr_ton=dc->DestTON;
	oc->dest_addr_npi=dc->DestNPI;
	ret=ConvertStringToSmppAddr(dc->DestAddr,dc->DestTON,dc->DestNPI,(UC*)oc->destination_addr);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->destination_addr))
	{
		return TRANS_ADDR_ERROR;
	}
	oc->priority_flag=dc->PRI;

	oc->replace_if_present_flag=dc->RD;
    oc->registered_delivery_flag=dc->SRR;

	oc->protocol_ID=dc->PID;

	oc->data_coding=dc->DCS;
	INNER_TO_SMPP_TIME_TRANS(oc->schedule_delivery_time,dc->Schedule);
	INNER_TO_SMPP_TIME_TRANS(oc->validity_period,dc->Expire);
	oc->sm_default_msg_id=(char)dc->Default_ID;

    if( dc->UDL >= MAX_SM_LEN)
    {
        ASSERT(0);
        dc->UDL = MAX_SM_LEN - 1;
    }

    if (dc->RP)
        oc->esm_class = ESM_CLASS_REPLYPATH;
    else
        oc->esm_class = 0;

    if( INNER_UDHI_FLAG == dc->UDHI) //此时消息透传
    {
        oc->esm_class |= ESM_CLASS_UDHI;

        int iOctLen = GetOctetLength(dc->DCS, dc->UDL);

        if (iOctLen < 0)
        {
            ASSERT(0);
            return TRANS_ERROR;
        }
        memcpy(oc->short_message, dc->UD, iOctLen);
    }
    else
    {
	    int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,dc->UDHI,oc->short_message, CODE_INTERFACE_SMPP);
	    if(retvalue<0) 
	    {
		    TRACE("数据转换到TEXT错误\n");
		    return TRANS_ERROR;
	    }
    }
    
    oc->sm_length=(UC)dc->UDL;

	return TRANS_SUCCEED;
}


//-----------
int CSmppAndInnerTrans::CmdTransToInner(Bind_Receiver_Resp * oc,MI_BIND_ACK *dc,int cmdStatus)
{
	dc->Status=cmdStatus;
	CODECOPY(TRANS_ERROR,dc->System_id,oc->system_id);
	return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Bind_Receiver_Resp * oc,MI_BIND_ACK *dc,int& cmdStatus)
{
	cmdStatus=dc->Status;
	CODECOPY(TRANS_ERROR,oc->system_id,dc->System_id);
	return TRANS_SUCCEED;
}

//-------
int CSmppAndInnerTrans::CmdTransToInner(Bind_Receiver * oc,MI_BIND *dc,int cmdStatus)
{

	CODECOPY(TRANS_ERROR,dc->System_id,oc->system_id);
	CODECOPY(TRANS_ERROR,dc->Password, oc->password);
	CODECOPY(TRANS_ERROR,dc->System_type,oc->system_type); 	
	dc->Interface_version=(unsigned char)oc->interface_version;
	dc->Addr_ton=oc->addr_ton;
	dc->Addr_npi=oc->addr_npi;
	//CODECOPY(TRANS_ERROR,dc->Address_range,oc->address_range);
	//从SMPP地址格式到INNER格式的转换
	int ret=ConvertSmppAddrToString((UC *)oc->address_range,dc->Addr_ton,dc->Addr_npi,dc->Address_range);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(dc->Address_range))
	{
		return TRANS_ADDR_ERROR;   
	}
	return TRANS_SUCCEED;

}

int CSmppAndInnerTrans::CmdTransToSmpp(Bind_Receiver * oc,MI_BIND *dc,int& cmdStatus)
{

	CODECOPY(TRANS_ERROR,oc->system_id,dc->System_id);
	CODECOPY(TRANS_ERROR, oc->password,dc->Password);
	CODECOPY(TRANS_ERROR,oc->system_type,dc->System_type);
	oc->interface_version=(char)dc->Interface_version;
	oc->addr_ton=dc->Addr_ton;
	oc->addr_npi=dc->Addr_npi;
//	CODECOPY(TRANS_ERROR,oc->address_range,dc->Address_range);
	int ret=ConvertStringToSmppAddr(dc->Address_range,dc->Addr_ton,dc->Addr_npi,(UC*)oc->address_range);
	if(ret==CONVERT_ADDR_STRING_ERROR||ret>=sizeof(oc->address_range))
	{
		return TRANS_ADDR_ERROR;   
	}
	return TRANS_SUCCEED;

}
//根据SMPP COMMMAND ID 从转换表查出inner COMMAND ID
DWORD CSmppAndInnerTrans::findINNERCMD(DWORD smppID)
{

	DWORD innerID=NO_INNER_CMD;
	for(int i=0;i<MAX_SMPP_CMD;i++)
	{
		if(smppID==m_CmdIdTrans[i][0])
		{
			innerID=m_CmdIdTrans[i][1];
			break;
		}
	}
	return innerID;
}
//根据INNER COMMAND ID 从转换表查出SMPP COMMAND ID
DWORD CSmppAndInnerTrans::findSMPPCMD(DWORD innerID)
{
	DWORD smppID=0;
	for(int i=0;i<MAX_SMPP_CMD;i++)
	{
		if(innerID==m_CmdIdTrans[i][1])
		{
			smppID=m_CmdIdTrans[i][0];
			break;
		}
	}
	return smppID;
}

void CSmppAndInnerTrans::makeSMPPHEAD(Message_header *smppHEADER,MI_ONLYHEADER *innerHEADER,int smppMSGLEN,int cmdSTATUS)
{
	smppHEADER->Command_Length=smppMSGLEN;
    if (SMEI_STATUS_REPORT == innerHEADER->nCommand)
    {
        smppHEADER->Command_ID = ESME_DELIVER_SM;
    }
    else
    {
        smppHEADER->Command_ID=findSMPPCMD(innerHEADER->nCommand);
    }
    //end add
	char *ptr;
	ptr=(char *)&(smppHEADER->Sequence_No);
	for(int i=0;i<sizeof(int);i++)
	{
		ptr[i]=innerHEADER->sExtData[i];
	}
	smppHEADER->Command_Status=cmdSTATUS;
}

void CSmppAndInnerTrans::makeINNERHEAD(Message_header *smppHEADER,\
                                       MI_ONLYHEADER *innerHEADER,int innerMSGLEN,
                                       BOOLEAN bStatusReport)
{
	char *ptr;

    if (bStatusReport)
    {
        innerHEADER->nCommand = SMEI_STATUS_REPORT;
    }
    else
    {
	    innerHEADER->nCommand=findINNERCMD(smppHEADER->Command_ID);
    }
	ptr=(char *)&(smppHEADER->Sequence_No);
    int iIntSize = sizeof(int);
    for(int i=0;i<iIntSize;i++)
	{
		innerHEADER->sExtData[i]=ptr[i];
	}
	innerHEADER->nLen=innerMSGLEN;
	ASSERT(innerMSGLEN<2000);
}
unsigned int CSmppAndInnerTrans:: SMcopy(char *deststr,char *orgstr,int UdLen)
{
	if(UdLen>SHORT_MESSAGE_LEN)
	{
		return TRANS_ERROR;
	}
	memcpy(deststr,orgstr,UdLen);
	return TRANS_SUCCEED;
}

unsigned int CSmppAndInnerTrans:: SMcopy(BYTE unicodeflag,char *deststr,char *orgstr)
{
	unicodeflag=unicodeflag&DCSMASK;
	int maxlen,i;
	maxlen=SHORT_MESSAGE_LEN;
	//此消息是UNICODE码
	if(unicodeflag==DCS_UNICODE)
	{
		unsigned short *destus=(unsigned short *)deststr;
		unsigned short *orgus=(unsigned short *)orgstr;
		maxlen=maxlen/2;
		for(i=0;i<maxlen;i++)
		{
			destus[i]=orgus[i];
			if(orgus[i]==NULL)
				break;
		}
	}
	else
	{
		for(i=0;i<maxlen;i++)
		{
			deststr[i]=orgstr[i];
			if(orgstr[i]==NULL)
				break;
		}
	}
	if(i>=maxlen)  //到尾还没有结束符
	{
		return TRANS_ERROR;
	}
	return TRANS_SUCCEED;
}

//整数转换成16进制字符串 IE:0x12AB 变为 "12AB"
unsigned int CSmppAndInnerTrans::IntToHexStr(char *str,int num)
{
	sprintf(str,"%x",num);
	return true;
}

DWORD CSmppAndInnerTrans::CodeCopy(char *dest,char *org,int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		dest[i]=org[i];
		if(org[i]=='\0') break;
	}
	if(i>=len) return TRANS_ERROR;   //到最大长度，还没有结束符
	else	return TRANS_SUCCEED;
}

DWORD CSmppAndInnerTrans::InnerToSmppErrMap(DWORD innerStatus,DWORD& smppStatus)
{
	//以后此句为表映射
	if(innerStatus!=0) 
		smppStatus=ESME_RUNKNOWNERR;
	else
		smppStatus=ESME_ROK;
	smppStatus=innerStatus;
	return 0;
}

DWORD CSmppAndInnerTrans::SmppToInnerErrMap(DWORD smppStatus,DWORD& innerStatus)
{
	//以后此句为表映射
	if(smppStatus!=ESME_ROK)
		innerStatus=DECODE_PROTOCOL_ERROR;
	else
		innerStatus=0;
	innerStatus=smppStatus;
	return 0;
}

unsigned int CSmppAndInnerTrans::HexStrToInt(char *str)
{
	int rv=0,sv;
	CString temp;

	int len;
	
    char sTmpHex[MESSAGE_ID_LEN];
    memset(sTmpHex, 0, sizeof(sTmpHex));
    strncpy(sTmpHex, str, MESSAGE_ID_LEN - 1);

    temp = sTmpHex;

	temp.MakeLower();
	len=temp.GetLength();
	if(len>m_Max_Hex_Trans) len=m_Max_Hex_Trans;

	for(int i=0;i<len;i++)
	{
		if( (temp[i]>='0'&&temp[i]<='9')||(temp[i]>='a'&&temp[i]<='f'))
		{
			if(temp[i]>='a'&&temp[i]<='f')
			{
				sv=temp[i]-'a'+10;
			}
			else
				sv=temp[i]-'0';
		}
		else continue;
		rv=(rv<<4)+sv;
	}
	return rv;
}


//以下两个变量为转换时间中时区转换用，避免多次重复计算
static long g_bias = 0x7fffffff;
static char g_nnp[4];



//SMPP格式时间"YYMMDDhhmmsstnnp"到INNER格式时间"YYYY/MM/DD HH:MM:SS"的转换
DWORD CSmppAndInnerTrans::ChangeSmppTimeToInnerTime( char* csinnertime ,const char* cs/*"YYMMDDhhmmsstnnp"*/)
{
	COleDateTime time;
	int len=strlen(cs);
	if( len < 12 )
	{

		if(len==0) 
		{
			csinnertime[0]='\0';
			return TRANS_SUCCEED;
		}
		else  return TRANS_ERROR;

		//time.SetStatus(COleDateTime::null);
	}
	else {
		time.SetStatus(COleDateTime::valid);
		int nYear = (cs[0]-'0') * 10 + cs[1] - '0';
		int nMonth = (cs[2] - '0') * 10 + cs[3] - '0';
		int nDay = (cs[4] - '0') * 10 + cs[5] - '0';
		int nHour = (cs[6] - '0') * 10 + cs[7] - '0';
		int nMinute = (cs[8] - '0') * 10 + cs[9] - '0';
		int nSecond = (cs[10] - '0') * 10 + cs[11] - '0';
		if( nYear > 97 )
			nYear += 1900;
		else nYear += 2000;
		DWORD retVALUE=time.SetDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
		
		if(retVALUE!=0)  //时间转换有错，即原格式有错
		{
			return TRANS_TIME_ERROR;
		}
		if( strlen(cs) == 16 )
		{
			if( g_bias == 0x7FFFFFFF )
			{
				TIME_ZONE_INFORMATION timezone;
				GetTimeZoneInformation(&timezone);
				g_bias = timezone.Bias;
				int nn = g_bias/15;
				if( nn < 0 )
				{
					nn = 0-nn;
					sprintf(g_nnp, "%02d+", nn);
				}
				else {
					sprintf(g_nnp, "%02d-", nn);
				}
			}
			long timeAdvanced = ((cs[13] - '0')*10 + cs[14] - '0') * 15; // 
			if( cs[15] == '+' )
				timeAdvanced = 0 - timeAdvanced;
			timeAdvanced -= g_bias;
			COleDateTimeSpan timeSpan(0, timeAdvanced/60, timeAdvanced%60, 0);
			time += timeSpan;
			sprintf(csinnertime, "%04d/%02d/%02d %02d:%02d:%02d", time.GetYear(), time.GetMonth(),
			time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

		}
	}
	return TRANS_SUCCEED;
}


//INNER格式时间（"YYYY/MM/DD HH:MM:SS"到SMPP时间格式（GSM3.40)中的定义"YYMMDDhhmmsstnnp" 的转换
DWORD  CSmppAndInnerTrans::ChangeInnerTimeToSmppTime( char* buf,const char *cs)
{
	COleDateTime time;
	int len=strlen(cs);
	if(len!=(MAX_TIMESTAMP_LEN-1))   //INNER时间字符串长度
	{
		if(len==0)
		{
			strcpy(buf,"");         //没有时间参数
			return TRANS_SUCCEED;
		}
		else return TRANS_TIME_ERROR;
	}
	int nYear = (cs[0]-'0') * 1000 + (cs[1] - '0')*100+(cs[2]-'0')*10+cs[3]-'0';
	int nMonth = (cs[5] - '0') * 10 + cs[6] - '0';
	int nDay = (cs[8] - '0') * 10 + cs[9] - '0';
	int nHour = (cs[11] - '0') * 10 + cs[12] - '0';
	int nMinute = (cs[14] - '0') * 10 + cs[15] - '0';
	int nSecond = (cs[17] - '0') * 10 + cs[18] - '0';
	
	DWORD retVALUE=time.SetDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
	if( retVALUE!=0)
	{
		return TRANS_TIME_ERROR;
	}
	else {
		if( g_bias == 0x7FFFFFFF )
		{
			TIME_ZONE_INFORMATION timezone;
			GetTimeZoneInformation(&timezone);
			g_bias = timezone.Bias;
			int nn = g_bias/15;
			if( nn < 0 )
			{
				nn = 0-nn;
				sprintf(g_nnp, "%02d+", nn);
			}
			else {
				sprintf(g_nnp, "%02d-", nn);
			}
		}
		int year=time.GetYear();
		year=year%100;
		sprintf(buf, "%02d%02d%02d%02d%02d%02d0%s", year, time.GetMonth(),
			time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond(), g_nnp);
	}
	return TRANS_SUCCEED;
}

unsigned char CSmppAndInnerTrans::ConvertSMPPStateToSMEIState(unsigned char ucState)
{
    return ucState;

	unsigned char ucRet;
	switch(ucState)
	{
	case SM_STATE_EN_ROUTE:	/* default state for messages in transit */
		//ucRet = SM_RES_WORK;
		ucRet = SMS_WORK; 
		break;

	case SM_STATE_DELIVERED:/* message is delivered */
		ucRet = SMS_DELIVER_SUCCESS;
		break;

	case SM_STATE_EXPIRED:/* validity period expired */
		ucRet = SMS_TIMEOUT;
		break;

	case SM_STATE_DELETED:/* message has been deleted */
		ucRet = SMS_DELETED;
		break;

	case SM_STATE_UNDELIVERABLE:/* undeliverable */
		ucRet = SMS_DELIVER_FAIL;
		break;

	case SM_STATE_ACCEPTED:/* message is in accepted state */
		ucRet = SMS_WAIT_DELIVER_FOR_ERROR;
		break;

	case SM_STATE_INVALID:/* message is in invalid state */
		ucRet = SMS_INVALID;
		break;
	default:
		ucRet = SMS_INVALID;
		break;
	}
	return ucRet;
}

unsigned char CSmppAndInnerTrans::ConvertSMEIStateToSMPPState(unsigned char ucState)
{
    return ucState;

	unsigned char ucRet;
	switch(ucState)
	{
	case SMS_DELIVER_SUCCESS:
		ucRet = SM_STATE_DELIVERED;
		break;

	case SMS_WORK: // 工作态
		ucRet = SM_STATE_EN_ROUTE;
		break;

	case SMS_DELIVER_FAIL:
		ucRet = SM_STATE_UNDELIVERABLE;
		break;

	case SMS_WAIT_DELIVER_FOR_ERROR: // 因提交失败而等待
	case SMS_WAIT_DELIVER_FOR_NORMAL: // 因定时短讯而等待
	case SMS_WAIT_ALERT: // 等待ALERT
		ucRet = SM_STATE_ACCEPTED;
		break;

	case SMS_DELETED: //短消息已经被删除
		ucRet = SM_STATE_DELETED;
		break;

	case SMS_TIMEOUT: //短消息超时
		ucRet = SM_STATE_EXPIRED;
		break;

	case SMS_INVALID:
	default:
		ucRet = SM_STATE_INVALID;
	}

	return ucRet;
}

unsigned char CSmppAndInnerTrans::ConvertSMPPErrCodeToSMEIErrCode(unsigned char ucErrCode)
{
	return ucErrCode;
}

unsigned char CSmppAndInnerTrans::ConvertSMEIErrCodeToSMPPErrCode(unsigned char ucErrCode)
{
		return ucErrCode;
}

int CSmppAndInnerTrans::CmdTransToSmpp(Pps_User_Check *oc, \
    MI_PPS_USER_CHECK *dc, int& cmdStatus)
{
    oc->byMOMTFlag = dc->byMOMTFlag;
    strncpy(oc->sDestinationAddress, dc->sDestinationAddress, MAX_ADDR_LEN - 1);
    strncpy(oc->sSMSCAddress, dc->sSMSCAddress, MAX_ADDR_LEN - 1);
    strncpy(oc->sSourceAddress, dc->sSourceAddress, MAX_ADDR_LEN - 1);
    oc->ulServiceID = dc->ulServiceID;
    oc->ulSmID = dc->ulSmID;
    oc->ulSmLength = dc->ulSmLength;
    oc->ulVersion = dc->ulVersion;
    return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToSmpp(Pps_User_Check_Resp *oc, \
    MI_PPS_USER_CHECK_ACK *dc, int& cmdStatus)
{
    oc->byNotifyMode = dc->byNotifyMode;
    oc->ulOperationResult = dc->ulOperationResult;
    return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToSmpp(Pps_Result_Notify *oc, \
    MI_PPS_RESULT_NOTIFY *dc, int& cmdStatus)
{
    oc->byMOMTFlag = dc->byMOMTFlag;
    strncpy(oc->sDestinationAddress, dc->sDestinationAddress, MAX_ADDR_LEN - 1);
    strncpy(oc->sSMSCAddress, dc->sSMSCAddress, MAX_ADDR_LEN - 1);
    strncpy(oc->sSourceAddress, dc->sSourceAddress, MAX_ADDR_LEN - 1);
    oc->ulSendResult = dc->ulSendResult;
    oc->ulServiceID = dc->ulServiceID;
    oc->ulSmID = dc->ulSmID;
    oc->ulSmLength = dc->ulSmLength;
    oc->ulVersion = dc->ulVersion;

    return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToSmpp(Pps_Result_Notify_Resp *oc, \
    MI_PPS_RESULT_NOTIFY_ACK *dc, int& cmdStatus)
{
    oc->ulOperationResult = dc->ulOperationResult;

    return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Pps_User_Check * oc, \
    MI_PPS_USER_CHECK *dc, int cmdStatus)
{        
    dc->ulVersion = oc->ulVersion;
    strncpy(dc->sSMSCAddress, oc->sSMSCAddress, MAX_ADDR_LEN);
    strncpy(dc->sSourceAddress, oc->sSourceAddress, MAX_ADDR_LEN);
    strncpy(dc->sDestinationAddress, oc->sDestinationAddress, MAX_ADDR_LEN);
    dc->byMOMTFlag = oc->byMOMTFlag;
    dc->ulSmID = oc->ulSmID;
    dc->ulSmLength = oc->ulSmLength;
    dc->ulServiceID = oc->ulServiceID;
    return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Pps_User_Check_Resp * oc, \
    MI_PPS_USER_CHECK_ACK *dc, int cmdStatus)
{
    dc->ulOperationResult = oc->ulOperationResult;
    dc->byNotifyMode = oc->byNotifyMode;
	dc->ulStatus = cmdStatus;
    return TRANS_SUCCEED;
}

int CSmppAndInnerTrans::CmdTransToInner(Pps_Result_Notify * oc, \
    MI_PPS_RESULT_NOTIFY *dc, int cmdStatus)
{
    dc->byMOMTFlag = oc->byMOMTFlag;
    strncpy(dc->sDestinationAddress, oc->sDestinationAddress, \
        MAX_ADDR_LEN - 1);
    strncpy(oc->sSMSCAddress, dc->sSMSCAddress, MAX_ADDR_LEN - 1);
    strncpy(oc->sSourceAddress, dc->sSourceAddress, MAX_ADDR_LEN - 1);
    dc->ulSendResult = oc->ulSendResult;
    dc->ulServiceID = oc->ulServiceID;
    dc->ulSmID = oc->ulSmID;
    dc->ulSmLength = oc->ulSmLength;
    dc->ulVersion = oc->ulVersion;

    return TRANS_SUCCEED;
}
int CSmppAndInnerTrans::CmdTransToInner(Pps_Result_Notify_Resp * oc, \
    MI_PPS_RESULT_NOTIFY_ACK *dc, int cmdStatus)
{
    dc->ulOperationResult = oc->ulOperationResult;
	dc->ulStatus = cmdStatus;
    return TRANS_SUCCEED;
}
