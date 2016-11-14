
#ifndef _CNGPANDSMPP_H_INCLUDED_
#define _CNGPANDSMPP_H_INCLUDED_


//CNGP到SMPP的解码和SMPP到CNGP的编码类：
class CCngpAndSmpp : public CCodeAndDecode
{
public:
    CCngpAndSmpp();
    ~CCngpAndSmpp(); 
	virtual int MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen );
	//CNGP到SMPP的解码
	int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	//SMPP到CNGP的编码
	int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
private:  
	// 用来存放关状态值   
	int m_iStateCount;
	int m_iReportDarkFlag;  

	//编码，解码中用的子程序

	//消息头解码、编码
	int fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize);

	//消息内容解码、编码
	//Login
	int fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	//Submit
	int fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetSubmitSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutSubmitSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	//Deliver
    int fnGetDeliverSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetDeliverSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
    int fnPutDeliverSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
    int fnPutDeliverSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	//Forward
    int fnGetFwdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetFwdSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnPutFwdSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutFwdSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	//ActiveTest
    int fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
    int fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	//Exit
	int fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
    int fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);	



	int fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
	int fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,long int &nDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,unsigned char &cDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,char      *cpDestValid, int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt, UL &nDestValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,long int  nOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,unsigned char      cOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,char      *cpOrgValid, int size);
	int fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
	int fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);

	//预付费
	int fnPutQueryUserStateSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutPaymntRequestSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnGetQueryUserStateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetPaymntRequestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	// SMGW431-08, 2005-11-01, wzy add begin //
	int fnDecodeStatReport(Sm_Report *smppReport);
	// SMGW431-08, 2005-11-01, wzy add end //

};


#endif	//_CNGPANDSMPP_H_INCLUDED_