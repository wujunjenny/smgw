
#ifndef _PTOPANDSMPP_H_INCLUDED_
#define _PTOPANDSMPP_H_INCLUDED_


//SMPP压缩与解压的的类
class	CPtopAndSmpp :public CCodeAndDecode
{
public:
    CPtopAndSmpp();
    ~CPtopAndSmpp(); 
	//smpp解压
	int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	//smpp压缩
	int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
private:  
	//编码，解码中用的子程序
	// 用来存放关状态值   
	int m_iStateCount;
	int m_iReportDarkFlag;  
    
	BOOL  PutLocalMsgID(char * pSrcID,UC * pDestID,int MaxLen);
    BOOL PutMsgID(char * pSrcID,UC * pDestID,int MaxLen);
	BOOL GetMsgID(UC * pSrcID,char * pDestID,int MaxLen);

    BOOL  PutLocalMsgID12(char * pSrcID,UC * pDestID,int MaxLen);
	BOOL  GetMsgID12(UC * pSrcID,char * pDestID,int MaxLen); 
	BOOL  PutMsgID12(char * pSrcID,UC * pDestID,int MaxLen);

	int fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize);

	int fnPutBindReceiver(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutBindReceiverResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutFwdSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutFwdSmResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
    int fnPutTerminateResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);	

	int fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);

	int fnGetBindReceiver(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBindReceiverResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
    int fnGetFwdSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetFwdSmResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetTerminateResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);


	int fnPutToOctCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);
	int fnPutToUNCode(char  **cppDestBuff,int& nDestSize,char *cpOrgValid,int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,long int  nOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,unsigned char      cOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,char      *cpOrgValid, int size);
	int fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,long int  &nDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,unsigned char      &cDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,char      *cpDestValid, int size);
	int fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,char *cpDestValid,int size);
    int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt, UL &nDestValid,  int size);
    
    int fnGetActiveTestResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
    int fnPutActiveTestResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
};


#endif	//_PTOPANDSMPP_H_INCLUDED_