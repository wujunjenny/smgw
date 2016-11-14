// SmsAndEICode.h: interface for the CSmsAndEICode class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SMSANDEICODE_H_INCLUDED_)
#define _SMSANDEICODE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "codeanddecode.h"
#include "SMEIMsg.h"
#include "NewMsgHead.h"

class CSmsAndEICode : public CCodeAndDecode
{
public:
	CSmsAndEICode();
	virtual ~CSmsAndEICode();

public:
    //从EI格式到SMS格式的转换
    int Decode(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    //从SMS到EI格式的转换
	int Code(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);

private:

	int DeCodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int DeCodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitService(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeClientBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeDeliverSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int DeCodeDeliverSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);

    int DeCodeStatusReport(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);

	int CodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int CodeClientBind(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeShakeHand(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeSubmitService(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeClientBindAck(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);

    int CodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int CodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);

	int CodeSubmitAddr(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeSubmitAddrAck(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    int CodeDeliverAddr(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    int CodeDeliverAddrAck(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
};



//add by wujun for infomaster
class CSmsAndSmppCode : public CCodeAndDecode
{
public:
	CSmsAndSmppCode();
	virtual ~CSmsAndSmppCode();

public:
    //从EI格式到SMS格式的转换
    int Decode(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    //从SMS到EI格式的转换
	int Code(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);

private:

	int DeCodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int DeCodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitService(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeClientBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeDeliverSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int DeCodeDeliverSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeSubmitSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);

	int DeCodeShReportSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeShReportSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int DeCodeStatusReport(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	//add by cj
	int DeCodeQueryUserStateAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodePaymntRequestAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int&nDestSize);
	int DeCodeFeePaymntRequest(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int&nDestSize);
	int DeCodeFeePaymntConfirmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);

	//end cj

    int  CodeShReportAddr(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int  CodeShReportAddrAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodeSubmitServiceAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int CodeClientBind(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeShakeHand(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeSubmitService(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeClientBindAck(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);

    int CodeClientUnBind(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int CodeClientUnBindAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    
	int CodeFwdAddr(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeSubmitAddr(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeSubmitAddrAck(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeFwdAddrAck(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    int DeCodeFwdSm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
    int DeCodeFwdSmAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodeDeliverAddr(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
    int CodeDeliverAddrAck(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int CodeQueryUserState(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodePaymntRequest(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodeFeePaymntConfirm(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodeFeePaymntRequestAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);

	//***SMGW40-01, 2004-12-3, jdz, add begin***//	
	int CodeAuthPriceReq(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeAuthPriceReqAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodeAuthPriceCnfmReq(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeAuthPriceCnfmReqAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	//***SMGW40-01, 2004-12-3, jdz, add end***//

	//SMGW42-55, 2005-7-12, jdz, modi begin//
	int CodeFeeCheckResultAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodeFeeCheckMsg(LPVOID pOrgBuff,int &nOrgSize,LPVOID pDestBuff,int& nDestSize);
	int DeCodeFeeCheckAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeFeeCheckResult(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	//SMGW42-55, 2005-7-12, jdz, modi end//
	
	//SMGW45-05, 2005-12-31, jdz add begin//
	int CodeQueryUserStateAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodeQueryUserState(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int CodePaymentRequestAck(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	int DeCodePaymntRequest(LPVOID pOrgBuff, int &nOrgSize, LPVOID pDestBuff, int &nDestSize);
	//SMGW45-05, 2005-12-31, jdz add end//
};


#endif // !defined(_SMSANDEICODE_H_INCLUDED_)
