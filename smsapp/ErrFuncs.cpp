
#include "stdafx.h"
#include "errfuncs.h"

//***SMGW25-16, 2004-05-25, jdz, add begin***//
#include "codeanddecode.h"
//***SMGW25-16, 2004-05-25, jdz, add end***//

DWORD ToSMGPErrorCode( DWORD wInCode );
DWORD ToCNGPErrorCode( DWORD wInCode);
DWORD ToCMPPErrorCode( DWORD wInCode );
DWORD ToSGIPErrorCode( DWORD wInCode );
DWORD ToSMPPErrorCode( DWORD wInCode );
DWORD ToPTOPErrorCode( DWORD wInCode );
DWORD ToSMGPPaymentRequestRespErrorCode(DWORD wInCode);

DWORD ToInCode( UC nStatusCode, DWORD wErrorCode )
{
	if(wErrorCode == 0)
	{
		return 0;
	}
	switch(nStatusCode)
	{
	case ERR_QUERY_USERSTATE_RESP:
		return wErrorCode;
		break;
	case ERR_PAYMENT_REQUEST_RESP:
		return 256 + wErrorCode;
		break;
	case ERR_SMGP_FWD_SM_RESP:
	case ERR_CNGP_FWD_SM_RESP:
		return 768 + wErrorCode;
		break;
	case ERR_SMGP_FWD_SM:
	case ERR_CNGP_FWD_SM:
		return 1024 + wErrorCode;
		break;
	case ERR_SMPP_SUBMIT_SM_RESP:
		return 1280 + wErrorCode;
		break;
	case ERR_SMGP_DELIVER_SM:
		return 768 + wErrorCode;
		break;
	case ERR_SMGP_DELIVER_SM_RESP:
	case ERR_CNGP_DELIVER_SM_RESP:
		return 768 + wErrorCode;
		break;
	case ERR_CMPP_DELIVER_SM_RESP:
		return 1536 + wErrorCode;
		break;
	case ERR_SGIP_DELIVER_SM_RESP:
		return 1792 + wErrorCode;
		break;
	case ERR_SMPP_DELIVER_SM_RESP:
		return 1280 + wErrorCode;
		break;
	case ERR_INTERNAL_FWD:
		return 512 + wErrorCode;
		break;
	case ERR_PTOP_FWD_SM_RESP:
		return 2816 + wErrorCode;
		break;
    case ERR_SMPP_DELIVER:
        return 2048 + wErrorCode;
		break;
		
	default:
		return 5000;
	}
}


DWORD ToOutCode( UC nProtocolID, DWORD wInCode )
{
    
	if (wInCode == 0)
		return 0;
	
	switch(nProtocolID)
	{
	case SMGP:
		return ToSMGPErrorCode(wInCode);
		break;
	case SMGP_PAYMENT_REQUEST_REST:
		return ToSMGPPaymentRequestRespErrorCode(wInCode);
	case CNGP:
		return ToCNGPErrorCode(wInCode);
		break;
	case CMPP:
		return ToCMPPErrorCode(wInCode);
		break;
	case SGIP:
		return ToSGIPErrorCode(wInCode);
		break;
	case SMPP:
		return ToSMPPErrorCode(wInCode);
		break;
    case PTOP:
		return ToPTOPErrorCode(wInCode);
		break;
	default:
		return 0;
	}
}


//SMGW45-05, 2005-12-31, jdz add begin//
DWORD ToSMGPPaymentRequestRespErrorCode( DWORD wInCode )
{
	if(wInCode == E_NOT_PREPAY_USER)
	{
		return 50;
	}
	
	if(wInCode >= 256 && wInCode < 511)
	{
		return wInCode - 256;
	}
	else
	{
		return 55;
	}
}
//SMGW45-05, 2005-12-31, jdz add end//


DWORD ToSMGPErrorCode( DWORD wInCode )
{
   	if( wInCode == 513 )
		return 128;
	if( wInCode == 514 )
		return 39;
	
	if( wInCode == 518 )
		return 129;
	if( wInCode == 519 )
		return 130;
	if( wInCode == 520 )
		return 131;
	if( wInCode == 521 )
		return 132;
	if( wInCode == 522 )
		return 133;
	if( wInCode == 523 )
		return 134;
	if( wInCode == 524 )
		return 32;
	if( wInCode == 525 )
		return 33;
	if( wInCode == 526 )
		return 135;
	//	if( wInCode == 527 )
	//		return 30;
	if( wInCode == 528 )
		return 136;
	if( wInCode == 529 )
		return 137;
	if( wInCode == 533 )
		return 138;
	
    if( wInCode == 534 )
		return 139;
	
    if( wInCode == 535 )
		return 140;
	
	if( wInCode == 530 )
		return 141;
	
	if( wInCode == E_ERROR_QUEUE_FULL )
		return 142;
	
    if ( wInCode == E_ERROR_LOGINIP)
		return 20;
	
	if (wInCode == E_ACCOUNTNAMEERR)
		return 21;
    
	if (wInCode == E_ACCOUNTPASSWORDERR)
		return 21;
	
	if (wInCode == E_BEYONDMAXIFNUM)
		return 2;
    
	if (wInCode == E_ERROR_LOGINOTHER )
		return 143;
	
		
	if (wInCode == E_ERROR_SMGWNOERR )
		return 144;

	//SMGW42-42, 2005-7-4, jdz, add begin//
	if (wInCode == E_FEEMONTHMSG_CANNOTSEND )
		return 145;

	if (wInCode == E_REPEATFEEMONTH )
		return 146;

	if (wInCode == E_NOTACTIVEFEEMONTHMSG )
		return 147;

	if (wInCode == E_ERROR_FILTER )
		return 148;

	if (wInCode == E_ERROR_CPSTATUS_STOP )
		return 149;
	//SMGW42-42, 2005-7-4, jdz, add end//
	
	//***SMGW40-01, 2004-12-23, jdz, add begin***//
	if(wInCode >= 700 && wInCode <= 767)
	{//订购关系处理错误码，将内部错误码(700~767)转换为协议中错误码(150~217)
		return wInCode - 550;
	}
	//***SMGW40-01, 2004-12-23, jdz, add end***//
	
	//SMGW45-19, 2006-2-14, ZF modi begin//
	if(wInCode >= 0 && wInCode <= 255)
	{
		if(wInCode >= 0 && wInCode <= 255)
		{
			switch(wInCode)
			{
			case 0:
				wInCode = E_SUCCESS;
				break;
			case 1:
				wInCode = 51;
				break;
			case 2:
				wInCode = 50;
				break;
			case 3:
				break;
			case 4:
				wInCode = 54;
				break;
			case 5:
				wInCode = 52;
				break;
			default:
				break;
			}
			
			return wInCode;
		}
	}
	//SMGW45-19, 2006-2-14, ZF modi end//

    return 255;
}


DWORD ToCNGPErrorCode( DWORD wInCode )
{
   	if( wInCode == 513 )
		return 128;
	if( wInCode == 514 )
		return 39;
	
	if( wInCode == 518 )
		return 129;
	if( wInCode == 519 )
		return 130;
	if( wInCode == 520 )
		return 131;
	if( wInCode == 521 )
		return 132;
	if( wInCode == 522 )
		return 133;
	if( wInCode == 523 )
		return 134;
	if( wInCode == 524 )
		return 32;
	if( wInCode == 525 )
		return 33;
	if( wInCode == 526 )
		return 135;
	//	if( wInCode == 527 )
	//		return 30;
	if( wInCode == 528 )
		return 136;
	if( wInCode == 529 )
		return 137;
	if( wInCode == 533 )
		return 138;
	
    if( wInCode == 534 )
		return 139;
	
    if( wInCode == 535 )
		return 140;
	
	if( wInCode == 530 )
		return 141;

	if( wInCode == E_ERROR_QUEUE_FULL )
		return 142;
	
	
	if ( wInCode == E_ERROR_LOGINIP)
		return 20;
	
	
	if (wInCode == E_ACCOUNTNAMEERR)
		return 21;
    
	if (wInCode == E_ACCOUNTPASSWORDERR)
		return 21;
	
	if (wInCode == E_BEYONDMAXIFNUM)
		return 2;
    
	if (wInCode == E_ERROR_LOGINOTHER )
		return 143;
		
	if (wInCode == E_ERROR_SMGWNOERR )
		return 144;

	//SMGW42-42, 2005-7-4, jdz, add begin//
	if (wInCode == E_FEEMONTHMSG_CANNOTSEND )
		return 145;

	if (wInCode == E_REPEATFEEMONTH )
		return 146;

	if (wInCode == E_NOTACTIVEFEEMONTHMSG )
		return 147;

	if (wInCode == E_ERROR_FILTER )
		return 148;

	if (wInCode == E_ERROR_CPSTATUS_STOP )
		return 149;
	//SMGW42-42, 2005-7-4, jdz, add end//


	//***SMGW40-01, 2004-12-23, jdz, add begin***//
	if(wInCode >= 700 && wInCode <= 767)
	{//订购关系处理错误码，将内部错误码(700~767)转换为协议中错误码(150~217)
		return wInCode - 550;
	}
	//***SMGW40-01, 2004-12-23, jdz, add end***//	

	//SMGW45-19, 2006-2-14, ZF modi begin//
	if(wInCode >= 0 && wInCode <= 255)
	{
		switch(wInCode)
		{
		case 0:
			wInCode = E_SUCCESS;
			break;
		case 1:
			wInCode = 41;
			break;
		case 2:
			wInCode = 43;
			break;
		case 3:
			break;
		case 4:
			wInCode = 42;
			break;
		case 5:
			wInCode = 40;
			break;
		default:
			break;
		}

		return wInCode;
	}
	//SMGW45-19, 2006-2-14, ZF modi end//
	
	return 255;
}

DWORD ToCMPPErrorCode( DWORD wInCode )
{	
	if (wInCode ==  513 )
		return 8;
	
	if (wInCode ==  514 )
		
		return  11;
	
	if (wInCode ==  515 )
		
		return 17;
	
	if (wInCode ==  519 )
		return 18;
	
	
	if (wInCode ==  522 )
		return 19;
	
	if (wInCode ==  523 )
		return 7;
	
	if (wInCode ==  524 )
		return 20;
	
	if (wInCode ==  525 )
		return 5;
	
	if (wInCode ==  526 )
		return 21;
	
	if (wInCode ==  528 )
		return 22;
	
	if (wInCode ==  529 )
		return 23;
	if (wInCode ==  533 )
		return  24;
	
	if (wInCode ==  534 )
		return  25;
	
	if (wInCode ==  535 )
		return  26;
	
	if( wInCode == 530 )
		return 27;

	if( wInCode == E_ERROR_QUEUE_FULL )
		return 28;
	
	
	if ( wInCode == E_ERROR_LOGINIP)
		return 20;
	
	
	if (wInCode == E_ACCOUNTNAMEERR)
		return 2;
    
	if (wInCode == E_ACCOUNTPASSWORDERR)
		return 3;
	
	if (wInCode == E_BEYONDMAXIFNUM)
		return 5;
    
	if (wInCode == E_ERROR_LOGINOTHER )
		return 8;
	
		
	if (wInCode == E_ERROR_SMGWNOERR )
		return 29;
	

	if(wInCode >= 510 && wInCode <= 630)
	{//订购关系处理错误码，将内部错误码(510~630) 
		return wInCode;
	}

	if(wInCode >= 700 && wInCode <= 767)
	{//订购关系处理错误码，将内部错误码(700~767)转换为协议中错误码(150~217)
		return wInCode - 550;
	}
	return  255;	
}

DWORD ToSGIPErrorCode( DWORD wInCode )
{
	
    if (wInCode ==  2049)
		return 25;
	
	if (wInCode ==  2050)
		return 28;
	if (wInCode ==  2051)
		return 31;
	if (wInCode ==  2052)
		return 29;
	if (wInCode ==  2053)
		return  34;
	if (wInCode ==  2054)
		return 32;
	
	if (wInCode ==  2055)
		return 26;
	
	if (wInCode ==  2056)
		return 30;
	
	if (wInCode ==  2057)
		return 35;
	
	if (wInCode ==  2058)
		return 33;
	
	
	if (wInCode ==  513 )
		
		return 60;
	
	if (wInCode ==  514 )
		
		return 23;
	
	if (wInCode ==  515 )
		
		return 61;
	
	if (wInCode ==  519 )
		return 62;
	
	
	if (wInCode ==  522 )
		return 63;
	
	if (wInCode ==  523 )
		return 64;
	
	if (wInCode ==  524 )
		return 65;
	
	if (wInCode ==  525 )
		return 66;
	
	if (wInCode ==  526 )
		return 36;
	
	if (wInCode ==  528 )
		return 24;
	
	if (wInCode ==  529 )
		return 67;
	if (wInCode ==  533 )
		return  68;
	
	if (wInCode ==  534 )
		return  69;
	
	if (wInCode ==  535 )
		return  70;
	
	if( wInCode == 530 )
		return 71;

	if( wInCode == E_ERROR_QUEUE_FULL )
		return 72;
		
	if (wInCode == E_ERROR_SMGWNOERR )
		return 73;
	
	return 80;
	
	
	
	
	
}


DWORD ToSMPPErrorCode( DWORD wInCode )
{
	// add  by lzh  ACK 的错误码映射
	if (wInCode ==  513 )
		return 22;
	
	if (wInCode ==  514 )
		
		return  23;
	
	if (wInCode ==  515 )
		
		return 24;
	
	if (wInCode ==  519 )
		return 25;
	
	if (wInCode ==  522 )
		return 26;
	
	if (wInCode ==  523 )
		return 16;
	
	if (wInCode ==  524 )
		return 27;
	
	if (wInCode ==  525 )
		return 28;
	
	if (wInCode ==  526 )
		return 29;
	
	if (wInCode ==  528 )
		return 30;
	
	if (wInCode ==  529 )
		return 31;
	if (wInCode ==  533 )
		return  32;
	
	if (wInCode ==  534 )
		return  33;
	
	if (wInCode ==  535 )
		return  34; 
	
	if( wInCode == 530 )
		return 38;

	if( wInCode == E_ERROR_QUEUE_FULL )
		return 39;
	 
	if ( wInCode == E_ERROR_LOGINIP)
		return 35;
	
	if (wInCode == E_ACCOUNTNAMEERR)
		return 14;
    
	if (wInCode == E_ACCOUNTPASSWORDERR)
		return 13;
	
	if (wInCode == E_BEYONDMAXIFNUM)
		return 36;
    
	if (wInCode == E_ERROR_LOGINOTHER )
		return 37;
		
	//***SMGW25-16, 2004-05-25, jdz, add begin***//
	if(wInCode == E_ERROR_MSG_FORMAT)
	{
		return DECODE_NOT_SUPPORT;
	}
	//***SMGW25-16, 2004-05-25, jdz, add end***//

	//***SMGW40-01, 2004-12-23, jdz, add begin***//
	if(wInCode >= 700 && wInCode <= 767)
	{//订购关系处理错误码，将内部错误码(700~767)转换为协议中错误码(104~171)
		return wInCode - 596;
	}
	//***SMGW40-01, 2004-12-23, jdz, add end***//

	//SMGW45-19, 2006-2-14, zf modi begin//
	if(wInCode >= 0 && wInCode <= 255)
	{
		return wInCode;
	}
	//SMGW45-19, 2006-2-14, zf modi end//
	
	return 255;
	
}

DWORD ToPTOPErrorCode( DWORD wInCode )
{
	
	

	if (wInCode ==  513 )
		return 8;
	
	if (wInCode ==  514 )
		
		return  9;
	
	if (wInCode ==  515 )
		
		return 17;
	
	if (wInCode ==  519 )
		return 18;
	
	
	if (wInCode ==  522 )
		return 19;
	
	if (wInCode ==  523 )
		return 11;
	
	if (wInCode ==  524 )
		return 20;
	
	if (wInCode ==  525 )
		return 5;
	
	if (wInCode ==  526 )
		return 21;
	
	if (wInCode ==  528 )
		return 22;
	
	if (wInCode ==  529 )
		return 23;
	if (wInCode ==  533 )
		return  24;
	
	if (wInCode ==  534 )
		return  25;
	
	if (wInCode ==  535 )
		return  26;
	
	if( wInCode == 530 )
		return 27;

	if( wInCode == E_ERROR_QUEUE_FULL )
		return 28;
	
	
	if ( wInCode == E_ERROR_LOGINIP)
		return 9;
	
	
	if (wInCode == E_ACCOUNTNAMEERR)
		return 2;
    
	if (wInCode == E_ACCOUNTPASSWORDERR)
		return 3;
	
	if (wInCode == E_BEYONDMAXIFNUM)
		return 5;
    
	if (wInCode == E_ERROR_LOGINOTHER )
		return 8;

	
	if (wInCode == E_ERROR_SMGWNOERR )
		return 29;
	
	return  255;
}
