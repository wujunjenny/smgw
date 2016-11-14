// SmapAndInnerTrans.cpp: implementation of the CSmapAndInnerTrans class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "msginner.h"
#include "SmapAndInnerTrans.h"
//使用返回码用 
#include "codeanddecode.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define ESME_RINVCMDID	0

CSmapAndInnerTrans::CSmapAndInnerTrans()
{
	for(int i=0;i<MAX_SMAP_CMD;i++)
	{
		m_CmdIdTrans[i][0]=0;
		m_CmdIdTrans[i][1]=0;
	}
	m_Max_Hex_Trans=SMAP_MESSAGE_ID_LEN-1;   // 8
	m_seqhead=0;
	m_referNum=0;
	//command id 从Smap定义到INNER定义变换表
	m_CmdIdTrans[0][0]=SMAP_BND;
	m_CmdIdTrans[0][1]=SMEI_BIND_RECEIVER;
	m_CmdIdTrans[1][0]=SMAP_BND_RESP;
	m_CmdIdTrans[1][1]=SMEI_BIND_RECEIVER_ACK;
	m_CmdIdTrans[2][0]=SMAP_BND;
	m_CmdIdTrans[2][1]=SMEI_BIND_TRANSMITTER;
	m_CmdIdTrans[3][0]=SMAP_BND_RESP;
	m_CmdIdTrans[3][1]=SMEI_BIND_TRANSMITTER_ACK;
	m_CmdIdTrans[27][0]=SMAP_BND_RESP;
	m_CmdIdTrans[27][1]=SMEI_BIND_RECEIVER_ACK;
	m_CmdIdTrans[4][0]=SMAP_UBD;
	m_CmdIdTrans[4][1]=SMEI_UNBIND;
	m_CmdIdTrans[5][0]=SMAP_UBD_RESP;
	m_CmdIdTrans[5][1]=SMEI_UNBIND_ACK;
	m_CmdIdTrans[6][0]=SMAP_SUB_SM	;
	m_CmdIdTrans[6][1]=SMEI_SUBMIT_SM;
	m_CmdIdTrans[7][0]=SMAP_SUB_SM_RESP;
	m_CmdIdTrans[7][1]=SMEI_SUBMIT_SM_ACK;
	m_CmdIdTrans[8][0]=SMAP_DELIVERY_SM;
	m_CmdIdTrans[8][1]=SMEI_DELIVER_SM;
	m_CmdIdTrans[9][0]=SMAP_DELIVERY_SM_RESP;
	m_CmdIdTrans[9][1]=SMEI_DELIVER_SM_ACK;
	m_CmdIdTrans[10][0]=0;
	m_CmdIdTrans[10][1]=0;
	m_CmdIdTrans[11][0]=0;
	m_CmdIdTrans[11][1]=0;	//0值无用
	m_CmdIdTrans[12][0]=SMAP_CANCEL_SM;
	m_CmdIdTrans[12][1]=SMEI_CANCEL_SM;
	m_CmdIdTrans[13][0]=SMAP_CANCEL_SM_RESP;
	m_CmdIdTrans[13][1]=SMEI_CANCEL_SM_ACK;

	m_CmdIdTrans[14][0]=SMAP_REPLACE_SM;
	m_CmdIdTrans[14][1]=SMEI_REPLACE_SM;
	m_CmdIdTrans[15][0]=SMAP_REPLACE_SM_RESP;
	m_CmdIdTrans[15][1]=SMEI_REPLACE_SM_ACK;


	m_CmdIdTrans[16][0]=SMAP_QRYLINK;
	m_CmdIdTrans[16][1]=SMEI_ENQUIRE_LINK;
	m_CmdIdTrans[17][0]=SMAP_QRYLINK_RESP;
	m_CmdIdTrans[17][1]=SMEI_ENQUIRE_LINK_ACK;

	
	m_CmdIdTrans[30][0]=SMAP_QUERY_SM_DETAILS;
	m_CmdIdTrans[30][1]=SMEI_QUERY_MSG_DETAILS;
	m_CmdIdTrans[31][0]=SMAP_QUERY_SM_DETAILS_RESP;
	m_CmdIdTrans[31][1]=SMEI_QUERY_MSG_DETAILS_ACK;
}
//完成从INNER流到Smap流的变化 
int CSmapAndInnerTrans::InnerToSmap(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int&  nDestSize)
{
	unsigned char *obuff=(unsigned char *)pOrgBuff;
	unsigned char *dbuff=(unsigned char *)pDestBuff;
	
	//Smap,inner头结构
	UL Sequence = 0;
	Smap_Message_Header *smapHEADER=(Smap_Message_Header *)pDestBuff;
	MI_ONLYHEADER *innerHEADER=(MI_ONLYHEADER *)pOrgBuff ;
	//把目标缓冲区清为全O

	memset(dbuff,0,nDestSize);
	int smapHEADSIZE=sizeof(Smap_Message_Header);  //Smap头长度
	int innerHEADSIZE=sizeof(MI_ONLYHEADER);   //INNER头长度
	int smapMSGLEN=0;	  //Smap 消息长度
	int innerMSGLEN=0; //inner 消息长度
	int traFlg=CODE_SUCCEED;
	int cmdSTATUS=0;
	
	//源缓冲区不够消息头长
	if(innerHEADSIZE>nOrgSize) return SMAP_CODE_NO_ENOUGHT_ORG_BUF;
	//Smap正文地址
	dbuff+=smapHEADSIZE;

	switch( innerHEADER->nCommand)
	{
		case SMEI_REPLACE_SM:
			{   
					//输入,输出结构形成
				Smap_Replace_Sm *oc=(Smap_Replace_Sm  *)dbuff;
				MI_REPLACE_SM *dc=(MI_REPLACE_SM *)obuff;
				smapMSGLEN=sizeof(Smap_Replace_Sm);
				smapMSGLEN+=smapHEADSIZE;
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
				//因为输入的ORGSIZE为UD中除去多余部分，可能没有结构长，以下去掉。
				
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				
				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}

				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_REPLACE_SM_ACK:
			{   
				//输入,输出结构形成
				Smap_Replace_Sm_Resp *oc=(Smap_Replace_Sm_Resp  *)dbuff;
				MI_REPLACE_SM_ACK *dc=(MI_REPLACE_SM_ACK *)obuff;
				smapMSGLEN=0;
				smapMSGLEN+=smapHEADSIZE;
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_ENQUIRE_LINK:
		case SMEI_UNBIND:   //UNBIND ,QUERY LINK 
			{
				//输入,输出结构形成
				char *oc=(char *)dbuff;
				MI_UNBIND *dc=(MI_UNBIND *)obuff;
			
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN=0;
				smapMSGLEN+=smapHEADSIZE;	
				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				
				//消息流转换
				//注：此处的oc Smap流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
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
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN=0;
				smapMSGLEN+=smapHEADSIZE;	
				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
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
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN=0;
				smapMSGLEN+=smapHEADSIZE;	
				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				
				//消息流转换
				//注：此处的oc Smap流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_BIND_RECEIVER:   //bind 此处未改
		case SMEI_BIND_TRANSMITTER:
			{
				//输入,输出结构形成
				Smap_Bind_Req *oc=(Smap_Bind_Req *)dbuff;
				MI_BIND		  *dc=(MI_BIND *)obuff;
			
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN=sizeof(Smap_Bind_Req);
				smapMSGLEN+=smapHEADSIZE;	
				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				//注：此处的oc Smap流的地址，目的地址，dc为源地址，INNER地址
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			};
			break;
		case SMEI_BIND_RECEIVER_ACK:
		case SMEI_BIND_TRANSMITTER_ACK:
			{
					//输入,输出结构形成
				Smap_Bind_Resp *oc=(Smap_Bind_Resp *)dbuff;
				MI_BIND_ACK 	*dc=(MI_BIND_ACK *)obuff;
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN=sizeof(Smap_Bind_Resp);
				smapMSGLEN+=smapHEADSIZE;

				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}

				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_SUBMIT_SM:
		case SMEI_DELIVER_SM:   //	注：DELIVE SM 在Smap和SUBMIT SM 结构相同
			{
					//输入,输出结构形成
				Smap_Submit_Sm *oc=(Smap_Submit_Sm *)dbuff;
				MI_SUBMIT_SM *dc=(MI_SUBMIT_SM *)obuff;
				
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(dc->DCS,dc->UDL);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN=sizeof(*dc)+iOctetLen-MAX_SM_LEN;
				//因为输入的ORGSIZE为UD中除去多余部分，可能没有结构长，以下去掉。
				
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF; 
					break;
				}
				 //end change
				smapMSGLEN=sizeof(Smap_Submit_Sm);
				smapMSGLEN+=smapHEADSIZE;

				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_SUBMIT_SM_ACK:
		case SMEI_DELIVER_SM_ACK:	//	注:DELIVE SM RESP 在Smap和SUBMIT SM RESP结构相同
			{
			    //输入,输出结构形成
				Smap_Submit_Sm_Resp *oc=(Smap_Submit_Sm_Resp *)dbuff;
				MI_SUBMIT_SM_ACK *dc=(MI_SUBMIT_SM_ACK *)obuff;

				innerMSGLEN=sizeof(*dc); 
      
				if( innerMSGLEN > nOrgSize)
				{
                    ASSERT(0);
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}

				smapMSGLEN=sizeof(Smap_Submit_Sm_Resp);
				smapMSGLEN+=smapHEADSIZE;

				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}

				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_QUERY_MSG_DETAILS:
			{   
				//输入,输出结构形成
				Smap_Query_Sm_Details *oc=(Smap_Query_Sm_Details *)dbuff;
				MI_QUERY_SM_DETAILS *dc=(MI_QUERY_SM_DETAILS *)obuff;
				smapMSGLEN=sizeof(Smap_Query_Sm_Details);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN+=smapHEADSIZE;

				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_QUERY_MSG_DETAILS_ACK:
			{   
					//输入,输出结构形成
				Smap_Query_Sm_Details_Resp *oc=(Smap_Query_Sm_Details_Resp *)dbuff;
				MI_QUERY_SM_DETAILS_ACK *dc=(MI_QUERY_SM_DETAILS_ACK *)obuff;
				smapMSGLEN=sizeof(Smap_Query_Sm_Details_Resp);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN+=smapHEADSIZE;

				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_CANCEL_SM:
			{   
					//输入,输出结构形成
				Smap_Cancel_Sm *oc=(Smap_Cancel_Sm *)dbuff;
				MI_CANCEL_SM *dc=(MI_CANCEL_SM *)obuff;
				smapMSGLEN=sizeof(Smap_Cancel_Sm);
				
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				smapMSGLEN+=smapHEADSIZE;

				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		case SMEI_CANCEL_SM_ACK:
			{   
					//输入,输出结构形成
				Smap_Cancel_Sm_Resp *oc=(Smap_Cancel_Sm_Resp  *)dbuff;
				MI_CANCEL_SM_ACK *dc=(MI_CANCEL_SM_ACK *)obuff;
				smapMSGLEN=0;
				smapMSGLEN+=smapHEADSIZE;
				innerMSGLEN=sizeof(*dc);
				if(innerMSGLEN>nOrgSize)
				{
					traFlg=SMAP_CODE_NO_ENOUGHT_ORG_BUF;
					break;
				}
				if((smapMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_CODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				//消息流转换
				traFlg=CmdTransToSmap(oc,dc,cmdSTATUS);
			}
			break;
		default:
			//转换协议不支持
			traFlg=CODE_NOT_SUPPORT;
			break;
	}	
	nDestSize=smapMSGLEN;      //设置返回消息流的总长度
	if(traFlg==CODE_SUCCEED)  //消息体成功转换，则进行消息头的转换
	{
		nOrgSize=innerMSGLEN;
		DWORD smapSTATUS=0;
		InnerToSmapErrMap(cmdSTATUS,smapSTATUS);
		makeSmapHEAD(smapHEADER,innerHEADER,smapMSGLEN,smapSTATUS);
		if( innerHEADER->nCommand == SMEI_DELIVER_SM)
		{
			smapHEADER->Sequence_No = (US)Sequence;	//为扩充不转换
		}
	}
	
	return traFlg;
};	






//完成从Smap流到INNER流的变化 
int CSmapAndInnerTrans::SmapToInner(LPVOID pOrgBuff,LPVOID pDestBuff,int&  nDestSize)
{
	unsigned char *obuff=(unsigned char *)pOrgBuff;
	unsigned char *dbuff=(unsigned char *)pDestBuff;
	
	//Smap,inner头结构
	Smap_Message_Header *smapHEADER=( Smap_Message_Header *)pOrgBuff;
	MI_ONLYHEADER *innerHEADER=(MI_ONLYHEADER *)pDestBuff;
	
	int smapHEADSIZE=sizeof(Smap_Message_Header);  //Smap头长度
	int innerHEADSIZE=sizeof(MI_ONLYHEADER);   //INNER头长度
	int innerMSGLEN=0;	//inner 消息长度
	int traFlg=DECODE_SUCCEED;  //消息体翻译成功与否的标志
	DWORD cmdSTATUS=0;	//无用
	
	int oPos=0,dPos=0;//源，目的BUFF读写位置

	//正文地址
	obuff+=smapHEADSIZE;
	
	switch( smapHEADER->Command_ID)
	{
		case SMAP_QUERY_SM_DETAILS:
			{
				//输入,输出结构形成
				Smap_Query_Sm_Details *oc=(Smap_Query_Sm_Details *)obuff;
				MI_QUERY_SM_DETAILS *dc=(MI_QUERY_SM_DETAILS *)dbuff;			
				innerMSGLEN=sizeof(MI_QUERY_SM_DETAILS);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case SMAP_QUERY_SM_DETAILS_RESP:
			{
				//输入,输出结构形成
				Smap_Query_Sm_Details_Resp *oc=(Smap_Query_Sm_Details_Resp *)obuff;
				MI_QUERY_SM_DETAILS_ACK *dc=(MI_QUERY_SM_DETAILS_ACK *)dbuff;			
				int iOctetLen = 0;
				iOctetLen = GetOctetLength(oc->data_coding,(UC)oc->sm_length);
				if( iOctetLen < 0)
				{
					ASSERT(0);
					traFlg=CODE_DCS_NOT_SUPPORT;
					break;
				}
				innerMSGLEN = sizeof(*dc) + iOctetLen - MAX_SM_LEN;

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case SMAP_QRYLINK_RESP:
			{
				char *oc=(char  *)obuff;
				MI_ENQUIRE_LINK_ACK *dc=(MI_ENQUIRE_LINK_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_ENQUIRE_LINK_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				traFlg=DECODE_SUCCEED;
			}
			break;
		case SMAP_UBD_RESP:
			{
				char *oc=(char  *)obuff;
				MI_UNBIND_ACK *dc=(MI_UNBIND_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_UNBIND_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case SMAP_QRYLINK:
		case SMAP_UBD:   //UNBIND ,	QRYLINK, NACK
			{
				char *oc=(char  *)obuff;
				MI_UNBIND *dc=(MI_UNBIND *)dbuff;
				innerMSGLEN=sizeof(MI_UNBIND);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case SMAP_BND:
			{
				//输入,输出结构形成
				Smap_Bind_Req *oc=(Smap_Bind_Req *)obuff;
				MI_BIND		  *dc=(MI_BIND *)dbuff;
			
				innerMSGLEN=sizeof(MI_BIND);
				
				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			};
			break;
		case SMAP_BND_RESP:
			{
					//输入,输出结构形成
				Smap_Bind_Resp *oc=(Smap_Bind_Resp *)obuff;
				MI_BIND_ACK 	*dc=(MI_BIND_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_BIND_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case SMAP_SUB_SM:
			{
					//输入,输出结构形成
				Smap_Submit_Sm *oc=(Smap_Submit_Sm *)obuff;
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
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
				ASSERT(innerMSGLEN < 2000);
			}
			break;
		case SMAP_DELIVERY_SM:
			{
					//输入,输出结构形成
				Smap_Submit_Sm *oc=(Smap_Submit_Sm *)obuff;
				MI_DELIVER_SM *dc=(MI_DELIVER_SM *)dbuff;
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
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
				ASSERT(innerMSGLEN < 2000);
			}
			break;
		case SMAP_SUB_SM_RESP:
		case SMAP_DELIVERY_SM_RESP:
			{
					//输入,输出结构形成
				Smap_Submit_Sm_Resp *oc=(Smap_Submit_Sm_Resp *)obuff;
				MI_SUBMIT_SM_ACK *dc=(MI_SUBMIT_SM_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_SUBMIT_SM_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case SMAP_CANCEL_SM:
			{   
					//输入,输出结构形成
				Smap_Cancel_Sm *oc=(Smap_Cancel_Sm *)obuff;
				MI_CANCEL_SM *dc=(MI_CANCEL_SM *)dbuff;
				innerMSGLEN=sizeof(MI_CANCEL_SM);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case SMAP_CANCEL_SM_RESP:
			{   
					//输入,输出结构形成
				Smap_Cancel_Sm_Resp *oc=(Smap_Cancel_Sm_Resp  *)obuff;
				MI_CANCEL_SM_ACK *dc=(MI_CANCEL_SM_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_CANCEL_SM_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		case SMAP_REPLACE_SM:    
			{   
				Smap_Replace_Sm *oc=(Smap_Replace_Sm  *)obuff;
				MI_REPLACE_SM *dc=(MI_REPLACE_SM *)dbuff;
				innerMSGLEN=sizeof(*dc)+GetOctetLength(0/*oc->data_coding*/,(UC)oc->sm_length)-MAX_SM_LEN;

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
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
		case SMAP_REPLACE_SM_RESP:
			{   
					//输入,输出结构形成
				Smap_Replace_Sm_Resp *oc=(Smap_Replace_Sm_Resp  *)obuff;
				MI_REPLACE_SM_ACK *dc=(MI_REPLACE_SM_ACK *)dbuff;
				innerMSGLEN=sizeof(MI_REPLACE_SM_ACK);
				

				if((innerMSGLEN)>nDestSize)  //目标缓冲区不够长
				{	
					 traFlg=SMAP_DECODE_NO_ENOUGHT_DEST_BUF;
					break;
				}
				innerMSGLEN-=innerHEADSIZE;
				//
				//消息流转换
				traFlg=CmdTransToInner(oc,dc,cmdSTATUS);
			}
			break;
		default:
			//转换协议不支持,COMMAND ID	错误
			traFlg=	ESME_RINVCMDID;
			break;
	}	
	nDestSize=innerMSGLEN+innerHEADSIZE;

	//nDestSize=innerMSGLEN;



	if(traFlg==DECODE_SUCCEED) //成功进行消息体的转换，则进行消息头的转换
	{
		makeINNERHEAD(smapHEADER,innerHEADER,innerMSGLEN);
	}
	return traFlg;
};


int CSmapAndInnerTrans::CmdTransToInner(char * oc,MI_UNBIND *dc,int cmdStatus)
{
	return TRANS_SUCCEED;
}

int CSmapAndInnerTrans::CmdTransToSmap(char * oc,MI_UNBIND *dc,int& cmdStatus)
{
	return TRANS_SUCCEED;
}

//-----
int CSmapAndInnerTrans::CmdTransToInner(char * oc,MI_UNBIND_ACK *dc,int cmdStatus)
{
	dc->Status=0;
	return TRANS_SUCCEED;
}
int CSmapAndInnerTrans::CmdTransToSmap(char * oc,MI_UNBIND_ACK *dc,int& cmdStatus)
{
	return TRANS_SUCCEED;
}



//oc 输入结构，DC输出结构，CMDSTATUS：消息状态（位于HEADER中)
int CSmapAndInnerTrans::CmdTransToInner(Smap_Replace_Sm_Resp* oc,MI_REPLACE_SM_ACK *dc,int cmdStatus)
{
	dc->Status=oc->result;
	return TRANS_SUCCEED;
}

int CSmapAndInnerTrans::CmdTransToSmap(Smap_Replace_Sm_Resp * oc,MI_REPLACE_SM_ACK *dc,int& cmdStatus)
{
	//message_id,errorcode无意义,smapdebug
	oc->message_id=0;
	if (dc->Status==0)
		oc->result=0;
	else	oc->result=1;	
	oc->errorcode=oc->result;
	return TRANS_SUCCEED;
}

//replace_sm

int CSmapAndInnerTrans::CmdTransToInner(Smap_Replace_Sm * oc,MI_REPLACE_SM *dc,int cmdStatus)
{
	dc->sm_ID=oc->message_id;

	if(!ConvertSmapAddrToInnerAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr))
		return TRANS_ADDR_ERROR;   

	*dc->Schedule=NULL;
	if (oc->validity_type==3)
		SMAP_TO_INNER_TIME_TRANS(oc->validity_period,dc->Expire);
	if(oc->validity_type==0)	*(dc->Expire)=NULL;
	else	
	{
		ASSERT(FALSE);
		return	TRANS_ERROR;
	}
	dc->SRR=0;
	dc->DCS=oc->data_coding;
	dc->UDL=(unsigned char )oc->sm_length;
	dc->Default_ID=0;

	unsigned short UDL;
	int retvalue=ConvertTextToData((char*)oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);	//smapdebug
	if(retvalue<0) 
	{
		TRACE("数据转换到DATA错误\n");
		return TRANS_ERROR;
	}
	dc->UDL=(unsigned char)oc->sm_length;

	return TRANS_SUCCEED;
}
int CSmapAndInnerTrans::CmdTransToSmap(Smap_Replace_Sm * oc,MI_REPLACE_SM *dc,int& cmdStatus)
{
	oc->message_id=(MESSAGE_ID_TYPE) dc->sm_ID;

	if(!ConvertInnerAddrToSmapAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		||!ConvertInnerAddrToSmapAddr(oc->destination_len,oc->destination_addr,
		ADDR_TON_INTERNATIONAL_NUMBER,1,""))
		return TRANS_ADDR_ERROR;

	oc->priority_flag=0;
	oc->service_type=SERVICE_TYPE;
	if(*(dc->Schedule)!=NULL) return TRANS_ERROR;
	if(*(dc->Expire)!=NULL)
	{
		oc->validity_type=3;
		INNER_TO_SMAP_TIME_TRANS(oc->validity_period,dc->Expire);
	}
	else	
		oc->validity_type=0;
	
	oc->data_coding = dc->DCS;
	//because replace sm have not UDHI,set it to 0
	int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,0/*dc->UDHI*/,(char*)oc->short_message, CODE_INTERFACE_SMPP);
	if(retvalue<0) 
	{
		TRACE("数据转换到TEXT错误\n");
		return TRANS_ERROR;
	}

	oc->sm_length=(UC)dc->UDL;

	return TRANS_SUCCEED;
}


int CSmapAndInnerTrans::CmdTransToInner(Smap_Query_Sm_Details * oc,MI_QUERY_SM_DETAILS *dc,int cmdStatus)
{
	/*dc->sm_ID=oc->message_id;
	if(!ConvertSmapAddrToInnerAddr(0,"",
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		|| !ConvertSmapAddrToInnerAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr) ;
		return TRANS_ADDR_ERROR;   
	dc->Len=MAX_MESSAGE_LEN;*/
	return TRANS_ERROR;
}
int CSmapAndInnerTrans::CmdTransToSmap (Smap_Query_Sm_Details * oc,MI_QUERY_SM_DETAILS *dc,int& cmdStatus)
{
	oc->message_id=(MESSAGE_ID_TYPE)dc->sm_ID;
	if(!ConvertInnerAddrToSmapAddr(oc->destination_len,oc->destination_addr,
		dc->TON,dc->NPI,dc->Addr))
		return TRANS_ADDR_ERROR;
	oc->service_type=SERVICE_TYPE;
	return TRANS_SUCCEED;
}

//---QUERY_SM_DETAILS_RESP
int CSmapAndInnerTrans::CmdTransToInner(Smap_Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_ACK *dc,int cmdStatus)
{
	dc->Status=oc->result;	
	dc->Service_type=SERVICE_TYPE;  //保留

	if(!ConvertSmapAddrToInnerAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		|| !ConvertSmapAddrToInnerAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr))
		return TRANS_ADDR_ERROR;   

	dc->PRI=oc->priority_flag;	
	dc->PID=0;
	dc->DCS=oc->data_coding;
	dc->SRR=0;
	*(dc->Schedule)=NULL;
	if (oc->validity_type==3)
		SMAP_TO_INNER_TIME_TRANS(oc->validity_period,dc->Expire);
	if(oc->validity_type==0)	*(dc->Expire)=NULL;
	else	
	{
		ASSERT(FALSE);
		return	TRANS_ERROR;
	}
	*(dc->Final)=NULL;
	dc->sm_ID=oc->message_id;
	dc->sm_SubID=0; 
	if(oc->errorcode==0)
		dc->sm_Status=1;
	else	dc->sm_Status=2;	//这里认为是等待下发不准确，smapdebug
	dc->Error_Code=oc->errorcode;	
	dc->ConFlag=0;
	
	unsigned short UDL;
	int retvalue=ConvertTextToData((char*)oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
	if(retvalue<0) 
	{
		TRACE("数据转换到DATA错误\n");
		return TRANS_ERROR;
	}
	dc->UDL=(unsigned char)oc->sm_length;
	
	return TRANS_SUCCEED;
}
int CSmapAndInnerTrans::CmdTransToSmap(Smap_Query_Sm_Details_Resp * oc,MI_QUERY_SM_DETAILS_ACK *dc,int& cmdStatus)
{
/*	cmdStatus=dc->Status;
	oc->service_type=m_serviceType;
	oc->priority_flag=dc->PRI;
	oc->data_coding=dc->DCS;

	if(!ConvertInnerAddrToSmapAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		||!ConvertInnerAddrToSmapAddr(oc->destination_len,oc->destination_addr,
		ADDR_TON_INTERNATIONAL_NUMBER,1,""))
		return TRANS_ADDR_ERROR;

	if(dc->Schedule!=NULL) return TRANS_ERROR;
	oc->validity_type=3;
	INNER_TO_SMAP_TIME_TRANS(oc->validity_period,dc->Expire);
	oc->message_id=(MESSAGE_ID_TYPE) dc->sm_ID;
	oc->errorcode=dc->Error_Code;

	int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,0,oc->short_message, CODE_INTERFACE_SMPP);
	if(retvalue<0) 
	{
		TRACE("数据转换到TEXT错误\n");
		return TRANS_ERROR;
	}
	oc->message_length=(UC)dc->UDL;*/

	return TRANS_ERROR;
}


int CSmapAndInnerTrans::CmdTransToInner(Smap_Cancel_Sm_Resp * oc,MI_CANCEL_SM_ACK *dc,int cmdStatus)
{
	dc->Status=oc->result;
	return TRANS_SUCCEED;
}
int CSmapAndInnerTrans::CmdTransToSmap(Smap_Cancel_Sm_Resp * oc,MI_CANCEL_SM_ACK *dc,int& cmdStatus)
{
	//message_id,errorcode无意义,smapdebug
	oc->message_id=0;
	if (dc->Status==0)
		oc->result=0;
	else	oc->result=1;	
	oc->errorcode=oc->result;
	return TRANS_SUCCEED;
}

//-----
int CSmapAndInnerTrans::CmdTransToInner(Smap_Cancel_Sm * oc,MI_CANCEL_SM *dc,int cmdStatus)
{
	dc->Service_type=SERVICE_TYPE;
	dc->sm_ID=oc->message_id;

	BYTE temp;
	if(!ConvertSmapAddrToInnerAddr(0,&temp,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		|| !ConvertSmapAddrToInnerAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr))
		return TRANS_ADDR_ERROR;   

	return TRANS_SUCCEED;
}
int CSmapAndInnerTrans::CmdTransToSmap(Smap_Cancel_Sm * oc,MI_CANCEL_SM *dc,int& cmdStatus)
{
	oc->message_id=(MESSAGE_ID_TYPE) dc->sm_ID;
	oc->service_type=SERVICE_TYPE;

	if(!ConvertInnerAddrToSmapAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr))
		return TRANS_ADDR_ERROR;
	return TRANS_SUCCEED;
}


//此Smap_Submit_Sm_Resp代表Delive_Sm_Resp,因为它的结构相同，而在INNER中结构不同
int CSmapAndInnerTrans::CmdTransToInner(Smap_Submit_Sm_Resp * oc,MI_DELIVER_SM_ACK *dc,int cmdStatus)
{
	
	dc->sm_ID=oc->message_id;
	dc->LinkNo=0;
	dc->Result=oc->result;
	dc->CS=0;
	dc->FCS=0;
	//其余为0值省略赋值
	return TRANS_SUCCEED;
}
//因DELIVER SM RESP在Smap中与SUBMIT SM RESP 结构相同，所以用SUBMIT SM RESP的结构代替DELIVER SM RESP的结构
int CSmapAndInnerTrans::CmdTransToSmap(Smap_Submit_Sm_Resp * oc,MI_DELIVER_SM_ACK *dc,int& cmdStatus)
{
	//message_id,errorcode无意义,smapdebug
	oc->message_id=(MESSAGE_ID_TYPE)dc->sm_ID;
	if (dc->Result==0)
		oc->result=0;
	else	oc->result=1;	
	oc->errorcode=oc->result;
	return TRANS_SUCCEED;
}

//----------
//因DELIVER SM在Smap中与SUBMIT SM结构相同，所以用SUBMIT SM的结构代替DELIVER SM 的结构
int CSmapAndInnerTrans::CmdTransToInner(Smap_Submit_Sm * oc,MI_DELIVER_SM *dc,int cmdStatus)
{
	dc->sm_ID=oc->message_id;
	dc->LinkNo=0;

	if(!ConvertSmapAddrToInnerAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		|| !ConvertSmapAddrToInnerAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr))
		return TRANS_ADDR_ERROR;   


	dc->SCTON=0;
	dc->SCNPI=0;
	CODECOPY(TRANS_ERROR,dc->SCAddr,"");
	
	dc->PRI=oc->priority_flag;
	dc->MMS=0;
	dc->RP=0;
	dc->UDHI=0;
	dc->SRI=0;
	dc->PID=0;
	dc->DCS=oc->data_coding;
	CODECOPY(TRANS_ERROR,dc->TimeStamp,"");	

	unsigned short UDL;
	int retvalue=ConvertTextToData((char*)oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
	//dc->UDL=(unsigned char)oc->sm_length;
	dc->UDL=(unsigned char)oc->sm_length;
	if(retvalue<0) 
	{
		TRACE("数据转换到DATA错误\n");
		return TRANS_ERROR;
	}

	return TRANS_SUCCEED;
}

//因DELIVER SM在Smap中与SUBMIT SM结构相同，所以用SUBMIT SM的结构代替DELIVER SM 的结构
int CSmapAndInnerTrans::CmdTransToSmap(Smap_Submit_Sm * oc,MI_DELIVER_SM *dc,int& cmdStatus)
{
	oc->message_id=(MESSAGE_ID_TYPE)dc->sm_ID;
	if(!ConvertInnerAddrToSmapAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		||!ConvertInnerAddrToSmapAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr))
		return TRANS_ADDR_ERROR;
	memset(oc->schedule_delivery_time,0,sizeof(oc->schedule_delivery_time));

	oc->message_id=(MESSAGE_ID_TYPE)dc->sm_ID;
	oc->service_type=SERVICE_TYPE;
	oc->priority_flag=dc->PRI;

	oc->validity_type=0;
	oc->data_coding=dc->DCS;
	int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,dc->UDHI,(char*)oc->short_message, CODE_INTERFACE_SMPP);
	
	if(retvalue<0) 
	{
		TRACE("数据转换到TEXT错误\n");
		return TRANS_ERROR;
	}
	oc->sm_length=(UC)dc->UDL;

	return TRANS_SUCCEED;
}


int CSmapAndInnerTrans::CmdTransToInner(Smap_Submit_Sm_Resp * oc,MI_SUBMIT_SM_ACK *dc,int cmdStatus)
{
	
	dc->sm_ID=oc->message_id;
	dc->LinkNo=0;
	dc->MR=0;
	dc->Result=oc->result;
	dc->FCS=0;
	return TRANS_SUCCEED;
}

int CSmapAndInnerTrans::CmdTransToSmap(Smap_Submit_Sm_Resp * oc,MI_SUBMIT_SM_ACK *dc,int& cmdStatus)
{
	//errorcode无意义,smapdebug
	oc->message_id=(MESSAGE_ID_TYPE)dc->sm_ID;
	if (dc->Result==0)
		oc->result=0;
	else	oc->result=1;	
	oc->errorcode=oc->result;
	return TRANS_SUCCEED;
}


//------------
int CSmapAndInnerTrans::CmdTransToInner(Smap_Submit_Sm * oc,MI_SUBMIT_SM *dc,int cmdStatus)
{
	dc->sm_ID=oc->message_id;
	dc->LinkNo=0;

	if(!ConvertSmapAddrToInnerAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		|| !ConvertSmapAddrToInnerAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr))
		return TRANS_ADDR_ERROR;   

	dc->PRI=oc->priority_flag;
	dc->RD=0;
	dc->RP=0;
	dc->UDHI=0;
	dc->SRR=0;
	dc->MR=0;
	dc->PID=0;
	dc->DCS=oc->data_coding;
	SMAP_TO_INNER_TIME_TRANS(oc->schedule_delivery_time,dc->Schedule);
	if (oc->validity_type==3)
		SMAP_TO_INNER_TIME_TRANS(oc->validity_period,dc->Expire);
	if(oc->validity_type==0)	*(dc->Expire)=NULL;
	else	
	{
		ASSERT(FALSE);
		return	TRANS_ERROR;
	}
	dc->Default_ID=0;
	dc->UDL=oc->sm_length;

	ASSERT(dc->UDL<=SMAP_SHORT_MESSAGE_LEN);

	unsigned short UDL;
	int retvalue=ConvertTextToData((char*)oc->short_message,oc->data_coding,(UC*)dc->UD,UDL, CODE_INTERFACE_SMPP);
	if(retvalue<0) 
	{
		TRACE("数据转换到DATA错误\n");
		return TRANS_ERROR;
	}
	return TRANS_SUCCEED;
}

int CSmapAndInnerTrans::CmdTransToSmap(Smap_Submit_Sm * oc,MI_SUBMIT_SM *dc,int& cmdStatus)
{ 
	if(!ConvertInnerAddrToSmapAddr(oc->orig_len,oc->orig_addr,
		dc->OrgTON,dc->OrgNPI,dc->OrgAddr) 
		||!ConvertInnerAddrToSmapAddr(oc->destination_len,oc->destination_addr,
		dc->DestTON,dc->DestNPI,dc->DestAddr))
		return TRANS_ADDR_ERROR;
	INNER_TO_SMAP_TIME_TRANS(oc->schedule_delivery_time,dc->Schedule);

	oc->message_id=(MESSAGE_ID_TYPE)dc->sm_ID;
	oc->service_type=SERVICE_TYPE;
	oc->priority_flag=dc->PRI;

	if(*(dc->Expire)!=NULL)
	{
		oc->validity_type=3;
		INNER_TO_SMAP_TIME_TRANS(oc->validity_period,dc->Expire);
	}
	else	oc->validity_type=0;
	oc->data_coding=dc->DCS;
	int retvalue=ConvertDataToText((UC*)dc->UD,(unsigned short)dc->UDL,dc->DCS,dc->UDHI,(char*)oc->short_message, CODE_INTERFACE_SMPP);
	
	if(retvalue<0) 
	{
		TRACE("数据转换到TEXT错误\n");
		return TRANS_ERROR;
	}
	oc->sm_length=(UC)dc->UDL;

	return TRANS_SUCCEED;
}


//-----------
int CSmapAndInnerTrans::CmdTransToInner(Smap_Bind_Resp * oc,MI_BIND_ACK *dc,int cmdStatus)
{
	dc->Status=oc->result;
	strcpy((char*) dc->System_id,"");
	return TRANS_SUCCEED;
}
int CSmapAndInnerTrans::CmdTransToSmap(Smap_Bind_Resp * oc,MI_BIND_ACK *dc,int& cmdStatus)
{
	if (dc->Status==0)
		oc->result=0;
	else	oc->result=1;	
	oc->interface_version=SYSTEM_VERSION;	//用缺省版本，smapdebug
	return TRANS_SUCCEED;
}

int CSmapAndInnerTrans::CmdTransToInner(Smap_Bind_Req * oc,MI_BIND *dc,int cmdStatus)
{
	CString str;
	str.Format("%d",oc->system_id);
	CodeCopy((char*) dc->System_id,(char*)(LPCTSTR)str,SMAP_SYSTEM_ID_LEN);
	memcpy(dc->Password, oc->password,SMAP_PASSWORD_LEN);
	strcpy((char*)dc->System_type,"");
	dc->Interface_version=(unsigned char)oc->interface_version;

	BYTE temp;
	if(!ConvertSmapAddrToInnerAddr(0,&temp,
		dc->Addr_ton,dc->Addr_npi,NULL))
		return TRANS_ADDR_ERROR;

	strcpy(dc->Address_range,"");
	return TRANS_SUCCEED;

}

int CSmapAndInnerTrans::CmdTransToSmap(Smap_Bind_Req * oc,MI_BIND *dc,int& cmdStatus)
{
	char* stopstr;
	oc->system_id=(BYTE) strtoul(dc->System_id,&stopstr,10);
	CodeCopy((char*)oc->password,dc->Password,SMAP_PASSWORD_LEN);
	oc->interface_version=(char)dc->Interface_version;

	return TRANS_SUCCEED;

}
//根据Smap COMMMAND ID 从转换表查出inner COMMAND ID
DWORD CSmapAndInnerTrans::findINNERCMD(DWORD SmapID)
{

	DWORD innerID=NO_INNER_CMD;
	for(int i=0;i<MAX_SMAP_CMD;i++)
		if(SmapID==m_CmdIdTrans[i][0])
			{
				innerID=m_CmdIdTrans[i][1];
				break;
			}

	return innerID;
}
//根据INNER COMMAND ID 从转换表查出Smap COMMAND ID
DWORD CSmapAndInnerTrans::findSmapCMD(DWORD innerID)
{
	DWORD SmapID=0;
	for(int i=0;i<MAX_SMAP_CMD;i++)
	{
		if(innerID==m_CmdIdTrans[i][1])
		{
			SmapID=m_CmdIdTrans[i][0];
			break;
		}
	}
	return SmapID;
}
//从INNER头结构，以及从INNER消息体中提取的cmdStatus生成Smap消息头
void CSmapAndInnerTrans::makeSmapHEAD(Smap_Message_Header *smapHEADER,MI_ONLYHEADER *innerHEADER,int smapMSGLEN,int cmdSTATUS)
{
	//这里不能设置消息长度
	smapHEADER->Command_ID = (BYTE)findSmapCMD(innerHEADER->nCommand);	//为扩充不转换
	ASSERT(EXTDATA_SIZE>=sizeof(smapHEADER->Sequence_No));
	DWORD *pSeq = (DWORD*)(innerHEADER->sExtData);	//SMAPDEBUG
	smapHEADER->Sequence_No = (WORD)*pSeq;
	m_seqhead = (WORD)(*pSeq /(256*256));	//SMAPDEBUG
}

void CSmapAndInnerTrans::makeINNERHEAD(Smap_Message_Header *smapHEADER,MI_ONLYHEADER *innerHEADER,int innerMSGLEN)
{
	innerHEADER->nCommand=findINNERCMD(smapHEADER->Command_ID);
	DWORD* ptr=(DWORD *)innerHEADER->sExtData;
	ASSERT(EXTDATA_SIZE>=sizeof(DWORD));
	DWORD seq=smapHEADER->Sequence_No+m_seqhead*256*256;
	*ptr=seq;
	innerHEADER->nLen=innerMSGLEN;
}

unsigned int CSmapAndInnerTrans:: SMcopy(char *deststr,char *orgstr,int UdLen)
{
	if(UdLen>SMAP_SHORT_MESSAGE_LEN)  //消息体过长
	{
		return TRANS_ERROR;
	}
	memcpy(deststr,orgstr,UdLen);
	return TRANS_SUCCEED;
}

unsigned int CSmapAndInnerTrans:: SMcopy(BYTE unicodeflag,char *deststr,char *orgstr)
{
	unicodeflag=unicodeflag&DCSMASK;
	int maxlen,i;
	maxlen=SMAP_SHORT_MESSAGE_LEN;
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
	if(i>=maxlen)
	{
		return TRANS_ERROR;
	}
	return TRANS_SUCCEED;
}

unsigned int CSmapAndInnerTrans::IntToHexStr(char *str,int num)
{
	sprintf(str,"%x",num);
	return true;
}

//COPY以目标缓冲区长度的字符串，超长返回错误
DWORD CSmapAndInnerTrans::CodeCopy(char *dest,char *org,int len)
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
//以下两方法为错误映射表，现不提供功能实现
DWORD CSmapAndInnerTrans::InnerToSmapErrMap(DWORD innerStatus,DWORD& smapSTATUS)
{
	smapSTATUS=innerStatus;
	return 0;
}

DWORD CSmapAndInnerTrans::SmapToInnerErrMap(DWORD smapSTATUS,DWORD& innerStatus)
{
	innerStatus=smapSTATUS;
	return 0;
}
//把STR中16进制字符串转换为整数
unsigned int CSmapAndInnerTrans::HexStrToInt(char *str)
{
	int rv=0,sv;
	CString temp;
	temp=str;
	int len;
	
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


//Smap格式时间YYMMDDhhmmss(BCD code)到INNER格式时间"YYYY/MM/DD HH:MM:SS"的转换
DWORD CSmapAndInnerTrans::ChangeSmapTimeToInnerTime(const BYTE* cs, char* csinnertime )
{
		BOOL ok=false;
		for(int i=0;i<SMAP_TIME_LEN;i++)
			if (cs[i]!=0) ok=true;
		if(!ok)
		{
			*csinnertime=NULL;
			return TRANS_SUCCEED;
		}
		COleDateTime time;

		time.SetStatus(COleDateTime::valid);
		int nYear=(cs[0]>>4)+ (cs[0]&0x0f)*10;
		int nMonth = (cs[1]>>4)+ (cs[1]&0x0f)*10;
		int nDay = (cs[2]>>4) + (cs[2]&0x0f)*10;
		int nHour = (cs[3]>>4) + (cs[3]&0x0f)*10;
		int nMinute = (cs[4]>>4) + (cs[4]&0x0f)*10;
		int nSecond = (cs[5]>>4) + (cs[5]&0x0f)*10;
		
		if (nYear>70)
			nYear=1900+nYear;
		else	nYear=2000+nYear;
		DWORD retVALUE=time.SetDateTime(nYear, nMonth, nDay, nHour, nMinute, nSecond);
		
		if(retVALUE!=0)  //时间转换有错，即原格式有错
		{
			return TRANS_TIME_ERROR;
		}
	
	sprintf(csinnertime, "%04d/%02d/%02d %02d:%02d:%02d", time.GetYear(), time.GetMonth(),
			time.GetDay(), time.GetHour(), time.GetMinute(), time.GetSecond());

	return TRANS_SUCCEED;
}


//INNER格式时间（"YYYY/MM/DD HH:MM:SS"到Smap时间格式YYMMDDhhmmss(BCD code) 的转换
DWORD  CSmapAndInnerTrans::ChangeInnerTimeToSmapTime( BYTE* buf,const char *cs)
{
	COleDateTime time;
	int len=strlen(cs);
	if(len!=(MAX_TIMESTAMP_LEN-1))   //INNER时间字符串长度
	{
		if(len==0)
		{
			memset(buf,0,SMAP_SCHEDULE_DELIVERY_TIME_LEN);         //没有时间参数
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
	else 
	{
		buf[0]=ConvertCharToBCD(time.GetYear() %100);
		buf[1]=ConvertCharToBCD(time.GetMonth());
		buf[2]=ConvertCharToBCD(time.GetDay());
		buf[3]=ConvertCharToBCD(time.GetHour());
		buf[4]=ConvertCharToBCD(time.GetMinute());
		buf[5]=ConvertCharToBCD(time.GetSecond());
	}
	return TRANS_SUCCEED;
}

UC CSmapAndInnerTrans::ConvertCharToBCD(char nCh)
{
	//if beyond BCD range
	if(nCh > 99 || nCh < -99)
		return CONVERT_CHAR_BCD_ERROR;

	UC nBCD;
	if(nCh >= 0)
	{
		nBCD = nCh / 10;
		nBCD |= (nCh % 10) << 4;
	}
	else
	{
		nBCD = (-nCh / 10) | 0x8;
		nBCD |= (-nCh % 10) << 4;
	}

	return nBCD;
}

BOOL  CSmapAndInnerTrans::ConvertInnerAddrToSmapAddr(BYTE& oclen,BYTE* ocaddr,
													 BYTE dcTON,BYTE dcNPI,char* dcAddr)
{
	ASSERT(strlen(dcAddr)+3<=SMAP_ADDR_LEN);
	BOOL ok=false;
	if (dcTON==ADDR_TON_NATIONAL_NUMBER)
	{
		int len=strlen(COUNTRY_CODE);
		oclen=strlen(dcAddr)+len;
		if(ocaddr!=NULL && oclen<=SMAP_ADDR_LEN)
		{memcpy(ocaddr,COUNTRY_CODE,len);
		memcpy(ocaddr+len,dcAddr,strlen(dcAddr));
		ok=true;}
		
	}
	else
	{
		oclen=(BYTE) strlen(dcAddr);
		if(ocaddr!=NULL && oclen<=SMAP_ADDR_LEN)
		{
			memcpy(ocaddr,dcAddr,oclen);
			ok=true;
		}
	}
	return ok;
}

BOOL  CSmapAndInnerTrans::ConvertSmapAddrToInnerAddr(BYTE oclen,BYTE* ocaddr,
													 BYTE &dcTON,BYTE &dcNPI,char* dcAddr)
{
	dcTON=ADDR_TON_INTERNATIONAL_NUMBER;	
	dcNPI=ADDR_NPI_ISDN_NUMBERING_PLAN;

	ASSERT(oclen<=SMAP_ADDR_LEN);
	if(oclen>SMAP_ADDR_LEN) return false;
	if(dcAddr!=NULL)
	{
		memcpy(dcAddr,ocaddr,oclen);
		dcAddr[oclen+1]=NULL;
	}
	return true;
}

