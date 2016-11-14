#ifndef _SMAPDEF_H_INCLUDED_
#define _SMAPDEF_H_INCLUDED_
#include "codeanddecode.h"

#define MAX_SMAP_PACKET_LENGTH  (0x200)
//DCS掩码
#define DCSMASK	0XCC
//UNICODE编码方式
#define DCS_UNICODE	0X08

//MESSAGE ERROR CODE 

//为内部格式新增加的错误类型

//解码源缓冲不够
#define SMAP_DECODE_NO_ENOUGHT_ORG_BUF		DECODE_NO_ENOUGH_ORG_BUF	
//解码目的缓冲不够
#define SMAP_DECODE_NO_ENOUGHT_DEST_BUF		DECODE_NO_ENOUGH_DEST_BUF
//编码源缓冲不够
#define SMAP_CODE_NO_ENOUGHT_ORG_BUF		CODE_NO_ENOUGH_ORG_BUF	
//编码目的缓冲不够
#define SMAP_CODE_NO_ENOUGHT_DEST_BUF		CODE_NO_ENOUGH_DEST_BUF


#define MESSAGE_ID_TYPE		WORD	


// Smap Command id 
#define SMAP_BND			0X00000010
#define SMAP_BND_RESP		0X00000011
#define SMAP_UBD   			0X00000012 
#define SMAP_UBD_RESP		0X00000013
#define SMAP_QRYLINK		0X00000014
#define SMAP_QRYLINK_RESP	0X00000015
#define SMAP_SUB_SM			0X00000020
#define SMAP_SUB_SM_RESP	0X00000021
#define SMAP_DELIVERY_SM	0X00000051
#define SMAP_DELIVERY_SM_RESP 0X00000052
#define SMAP_QUERY_SM_DETAILS		0X00000028
#define SMAP_QUERY_SM_DETAILS_RESP	0X00000029
#define SMAP_CANCEL_SM		0X00000024
#define SMAP_CANCEL_SM_RESP	0X00000025
#define SMAP_REPLACE_SM		0X00000026
#define SMAP_REPLACE_SM_RESP 0X00000027


#define SMAP_SYSTEM_ID_LEN					3
#define SMAP_PASSWORD_LEN					6
#define SMAP_ADDR_LEN						16
#define SMAP_ORIG_ADDR_LEN					SMAP_ADDR_LEN
#define SMAP_DESTINATION_ADDR_LEN			SMAP_ADDR_LEN
#define	SMAP_TIME_LEN						6 
#define SMAP_VALIDITY_PERIOD_LEN			SMAP_TIME_LEN 
#define SMAP_SCHEDULE_DELIVERY_TIME_LEN		SMAP_TIME_LEN 
#define SMAP_SHORT_MESSAGE_LEN			   160 
#define SMAP_MESSAGE_ID_LEN					 2


typedef struct tagSmap_Message_header
{
	BYTE Command_Length;
	BYTE Command_ID;
	WORD Sequence_No;
	WORD Reserved;
} Smap_Message_Header;

typedef struct tagSmap_General_Resp
{
	MESSAGE_ID_TYPE message_id;
	BYTE result;
	BYTE errorcode;
}Smap_General_Resp;

typedef struct tagSmap_Bind
{
	BYTE system_id;
	BYTE password[SMAP_PASSWORD_LEN];
	WORD interface_version;
} Smap_Bind_Req;

typedef struct tagSmap_Bind_Resp
{
	BYTE result;
	WORD interface_version;
}Smap_Bind_Resp;

//下列字段地址，内容数据长度是可变，而非固定，按最大字节数分配，转换时要注意

typedef struct tagSmap_Submit_Sm
{
	BYTE destination_len;
	BYTE destination_addr[SMAP_DESTINATION_ADDR_LEN];
	BYTE orig_len;
	BYTE orig_addr[SMAP_ORIG_ADDR_LEN];
	BYTE schedule_delivery_time[SMAP_SCHEDULE_DELIVERY_TIME_LEN];
	BYTE service_type;
	MESSAGE_ID_TYPE message_id;
	BYTE priority_flag;
	BYTE validity_type;
	BYTE validity_period[SMAP_VALIDITY_PERIOD_LEN];
	BYTE data_coding;
	BYTE sm_length;
	BYTE short_message[SMAP_SHORT_MESSAGE_LEN];
}Smap_Submit_Sm;

#define Smap_Submit_Sm_Resp		Smap_General_Resp

typedef struct tagSmap_Query_Sm
{
	BYTE destination_len;
	BYTE destination_addr[SMAP_DESTINATION_ADDR_LEN];
	BYTE service_type;
	MESSAGE_ID_TYPE message_id;
}Smap_Query_Sm_Details;

typedef struct tagSmap_Query_Sm_Resp
{
	BYTE result;
	BYTE errorcode;
	BYTE destination_len;
	BYTE destination_addr[SMAP_DESTINATION_ADDR_LEN];
	BYTE orig_len;
	BYTE orig_addr[SMAP_ORIG_ADDR_LEN];
	MESSAGE_ID_TYPE message_id;
	BYTE priority_flag;
	BYTE validity_type;
	BYTE validity_period[SMAP_VALIDITY_PERIOD_LEN];
	BYTE retry_count;
	BYTE data_coding;
	BYTE submit_time[SMAP_TIME_LEN];
	BYTE last_deliver_time[SMAP_TIME_LEN];
	BYTE query_time[SMAP_TIME_LEN];
	BYTE last_error;
	BYTE sm_length;
	BYTE short_message[SMAP_SHORT_MESSAGE_LEN];
}Smap_Query_Sm_Details_Resp;

typedef struct tagSmap_Cancel_Sm
{
	BYTE destination_len;
	BYTE destination_addr[SMAP_DESTINATION_ADDR_LEN];
	MESSAGE_ID_TYPE message_id;
	BYTE service_type;
}Smap_Cancel_Sm;

#define Smap_Cancel_Sm_Resp	Smap_General_Resp

typedef struct tagSmap_Replace_Sm
{
	MESSAGE_ID_TYPE message_id;
	BYTE destination_len;
	BYTE destination_addr[SMAP_DESTINATION_ADDR_LEN];
	BYTE orig_len;
	BYTE orig_addr[SMAP_ORIG_ADDR_LEN];
	BYTE priority_flag;
	BYTE service_type;
	BYTE validity_type;
	BYTE validity_period[SMAP_VALIDITY_PERIOD_LEN];
	BYTE data_coding;
	BYTE sm_length;
	BYTE short_message[SMAP_SHORT_MESSAGE_LEN];
}	Smap_Replace_Sm;

#define Smap_Replace_Sm_Resp	Smap_General_Resp

#endif	//_SMAPDEF_H_INCLUDED_



