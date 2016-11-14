// SmapCompAndUnComp.h: interface for the CSmapCompAndUnComp class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_SMAPCOMPANDUNCOMP_H_INCLUDED_)
#define _SMAPCOMPANDUNCOMP_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Smap压缩与解压的的类
class	CSmapCompAndUnComp
{
public:
	//Smap解压
	int Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
	//Smap压缩
	int Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize);
private:   //编码，解码中用的子程序
	int fnPutToOctCode(char  **cppDestBuff,int& nDestSize,unsigned char *cpOrgValid,int size);
	int fnPutToUNCode(char  **cppDestBuff,int& nDestSize,unsigned char *cpOrgValid,int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,WORD  nOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,BYTE  cOrgValid,  int size);
	int fnPutToCode(char **cppDestBuff ,int& nDestSize,unsigned char *cpOrgValid, int size);
	int fnPutMessageHeader(char **cppDestBuff,int& nDestSize,char **cpOrgValid,int& nOrgSize);
	int fnPutBind(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutBindResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutSubmitSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutGeneralResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmDetails(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutQuerySmDetailsResp(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutCancelSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);
	int fnPutReplaceSm(char **cppDestBuff,int& nDestSize,char **cppOrgValid,int& nOrgSize);

	int fnGetFromOctCode(char  **cppOrgBuff,int& nDeCnt,unsigned char *cpDestValid,int size);
	int fnGetFromUNCode(char  **cppOrgBuff,int& nDeCnt,unsigned char *cpDestValid,int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,WORD  &nDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,BYTE  &cDestValid,  int size);
	int fnGetFromCode(char **cppOrgBuff ,int& nDeCnt,unsigned char *cpDestValid, int size);
	int fnGetMessageHeader(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBind(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize); 
	int fnGetBindResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetSubmitSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetGeneralResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmDetails(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetQuerySmDetailsResp(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetCancelSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
	int fnGetReplaceSm(char **cppOrgBuff,int& nDeCnt,char **cppDestValid,int& nDestSize);
};

#endif // !defined(_SMAPCOMPANDUNCOMP_H_INCLUDED_)
