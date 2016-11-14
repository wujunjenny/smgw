
#if !defined(ERRFUNCS_H)
#define ERRFUNCS_H

#include "smcmacro.h"
#define ERR_QUERY_USERSTATE_RESP	1
#define ERR_PAYMENT_REQUEST_RESP	2
#define ERR_SMGP_FWD_SM_RESP		3
#define ERR_CNGP_FWD_SM_RESP		4
#define ERR_SMGP_FWD_SM				5
#define ERR_CNGP_FWD_SM				6
#define ERR_SMPP_SUBMIT_SM_RESP		7
#define ERR_SMGP_DELIVER_SM			8
#define ERR_SMGP_DELIVER_SM_RESP	9
#define ERR_CNGP_DELIVER_SM_RESP	10
#define ERR_CMPP_DELIVER_SM_RESP	11
#define ERR_SGIP_DELIVER_SM_RESP	12
#define ERR_SMPP_DELIVER_SM_RESP	13
#define ERR_INTERNAL_FWD			14
#define ERR_PTOP_FWD_SM_RESP		15
#define ERR_SMPP_DELIVER            16

#define SMGP	1
#define CNGP	2
#define CMPP	3
#define SGIP	4
#define SMPP	5
#define PTOP	6
#define SMGP_PAYMENT_REQUEST_REST	7

DWORD ToInCode( UC nStatusCode, DWORD wErrorCode );
DWORD ToOutCode( UC nProtocolID, DWORD wInCode );
#endif