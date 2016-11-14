
#ifndef _SMGPANDSMPP_H_INCLUDED_
#define _SMGPANDSMPP_H_INCLUDED_

//SMPP压缩与解压的的类
class	CSmgpAndSmpp :public CCodeAndDecode
{
public:
    CSmgpAndSmpp();
    ~CSmgpAndSmpp(); 

	virtual int MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen );
	//smpp解压
	int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	//smpp压缩
	int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
private:  
	//编码，解码中用的子程序
	// 用来存放关状态值   
	int m_iStateCount;
	int m_iReportDarkFlag;  

	int fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);
	int fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);
	int fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,long int  nOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,unsigned char      cOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,char      *cpOrgValid, int size);
	int fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize);
	int fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);

    int fnGetDeliverSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetDeliverSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);

	int fnPutFwdSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
    int fnGetFwdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnPutFwdSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnGetFwdSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnPutQuerySm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutAddSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutEnquireSubResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutDelSub(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutReplaceSmEx(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySms(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnGetReplaceSmEx(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySms(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);


	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,long int  &nDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,unsigned char      &cDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,char      *cpDestValid, int size);
	int fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);

	int fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetAddSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetEnquireSubResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetDelSub(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);

	int fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
    int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt, UL &nDestValid,  int size);
    
    int fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
    int fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
    int fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
    int fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
    int fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);	

	//预付费
	int fnPutQueryUserStateSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutPaymntRequestSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutPaymntRequestSm20(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnGetQueryUserStateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetPaymntRequestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnDecodeStatReport(Sm_Report *smppReport);
	int fnGetQueryUserState(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetPaymentRequest(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetPaymentRequest20(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnPutQueryUserStateAck(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutPaymentRequestAck(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
};


#endif	//_SMGPANDSMPP_H_INCLUDED_