
#if !defined(_CODEANDDECODE_H_INCLUDED_)
#define _CODEANDDECODE_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//////////////////////////////////////////////////////////////////////
// codeanddecode.h: interface for the CCodeAndDecode class.
//

#include "SMCMacro.h"
#include <string>
#define UDH_DEFAULT_LENGTH (6+1)

#ifdef _LONG_SHORT_MESSAGE
	#define	MAX_MESSAGE_UD_LEN MAX_LONG_UD_LEN
	#define MAX_MESSAGE_SM_LEN	MAX_LONG_SM_LEN	
#else
	#define MAX_MESSAGE_UD_LEN MAX_UD_LEN
	#define	MAX_MESSAGE_SM_LEN	MAX_SM_LEN
#endif

//convert error
#define DCS_NOT_SUPPORT				-2
#define VPF_NOT_SUPPORT				-2
#define CONVERT_TEXT_DATA_ERROR		-1
#define CONVERT_TEXT_UCS2_ERROR		-1
#define CONVERT_DATA_TEXT_ERROR		-1
#define CONVERT_UCS2_TEXT_ERROR		-1
#define CONVERT_STRING_ADDR_ERROR	-1
#define CONVERT_ADDR_STRING_ERROR	-1

#define CONVERT_CHAR_BCD_ERROR		255
#define CONVERT_UC_BCD_ERROR		255

//short message const 
#define ADDR_ZONE_CODE			"86"
#define ADDR_DEFAULT_TON		1
#define ADDR_DEFAULT_NPI		1

#define TIMESTAMP_FORMAT_LEN	7
#define TP_VP_RELATIVE_LEN		1
#define TP_VP_ABSOLUTE_LEN		7

enum ADDR_TON_VALUES
{
	ADDR_TON_UNKNOWN=0,
	ADDR_TON_INTERNATIONAL_NUMBER,
	ADDR_TON_NATIONAL_NUMBER,
	ADDR_TON_NETWORK_SPECIFIC_NUMBER,
	ADDR_TON_SUBSCRIBER_NUMBER,
	ADDR_TON_ALPHANUMERIC,
	ADDR_TON_ABBREVIATED_NUMBER,
	ADDR_TON_EXTENSION
};

enum CODE_INTERFACE
{
	CODE_INTERFACE_SMEI = 0,
	CODE_INTERFACE_SMPP,
};
//去掉一个字符串不成功返回码
#define TRIMLEFT_ERROR		-1
enum ADDR_NPI_VALUES
{
	ADDR_NPI_UNKNOWN=0,
	ADDR_NPI_ISDN_NUMBERING_PLAN,
	ADDR_NPI_DATA_NUMBERING_PLAN=3,
	ADDR_NPI_TELEX_NUMBERING_PLAN,
	ADDR_NPI_NATIONAL_NUMBERING_PLAN=8,
	ADDR_NPI_PRIVATE_NUMBERING_PLAN,
	ADDR_NPI_ERMES_NUMBERING_PLAN,
	ADDR_NPI_EXTENSION
};

enum TP_VPF_VALUES
{
	TP_VPF_NOT_PRESENT=0,
	TP_VPF_RESERVED,
	TP_VPF_RELATIVE,
	TP_VPF_ABSOLUTE
};

enum TP_DCS_TYPE
{
	DCS_DEFAULT=1,
	DCS_8BIT,
	DCS_UCS2
};

enum CODE_RESULT
{
	CODE_SUCCEED=0,
	CODE_FAILED,
	CODE_NO_ENOUGH_ORG_BUF,		//源缓冲不够
	CODE_NO_ENOUGH_DEST_BUF,	//目的缓冲不够
	CODE_PROTOCOL_ERROR,		//协议错
	CODE_NOT_SUPPORT,			//协议不支持
	CODE_ADDR_ERROR,			//地址的TON,NPI不支持或为保留值
	CODE_TIMESTAMP_ERROR,		//时间格式不正确
	CODE_ORG_UD_TOO_LARGE,		//源用户数据太大
	CODE_DCS_NOT_SUPPORT,		//编码方案不支持
};

enum DECODE_RESULT
{
	DECODE_SUCCEED=0,
	DECODE_FAILED,
	DECODE_NO_ENOUGH_ORG_BUF,	//源缓冲不够
	DECODE_NO_ENOUGH_DEST_BUF,	//目的缓冲不够
	DECODE_PROTOCOL_ERROR,		//协议错
	DECODE_NOT_SUPPORT,			//协议不支持
	DECODE_MTI_ERROR,			//SMSMap发送的消息中的MTI值不正确
	DECODE_ADDR_ERROR,			//地址的TON,NPI不支持或为保留值
	DECODE_TIMESTAMP_ERROR,		//时间格式不正确
	DECODE_PI_NOT_SUPPORT,		//PI不支持扩展(Extension=1)
	DECODE_DCS_NOT_SUPPORT,		//解码方案不支持
	DECODE_SMPP_HEADER_ERROR    //SMPP头有错（1。COMMAND LENGTH 小于包头长
								//       2。COMMAND LENGTH 大于SMPP最大包长）。
};

class CCodeAndDecode  
{
public:
	CCodeAndDecode();
	virtual ~CCodeAndDecode();

public:

	// *** SMGW25-50,2004-07-30,YinBiaozheng add begin *** //
	virtual int MakeMsgID( LPVOID pOrgMsgID, UC orglen, LPVOID pDestMsgID, UC& destlen )
	{
		ASSERT(0);
		destlen = 0;
		return -1;
	};
	// *** SMGW25-50,2004-07-30,YinBiaozheng add end *** //
	virtual int Decode(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int& nDestSize);

	virtual int Code(LPVOID pOrgBuff,int& nOrgSize,LPVOID pDestBuff,int& nDestSize);

public:
	int ConvertTextToData(LPCSTR pString,UC nDCS,UC* pData,US& nLen, CODE_INTERFACE CodeInterface = CODE_INTERFACE_SMEI);
	int ConvertTextToDataEx(LPCSTR pString, int nStrLen, UC nDCS,UC* pData,US& nLen, CODE_INTERFACE CodeInterface = CODE_INTERFACE_SMEI);

	int ConvertDataToText(UC* pData,US nLen,UC nDCS,UC nUDHI,char* pString, CODE_INTERFACE CodeInterface = CODE_INTERFACE_SMEI);

	int ConvertStringToAddr(LPCSTR pAddr,UC nTON,UC nNPI,UC* pBCD);

	int ConvertAddrToString(UC* pBCD,UC& nTON,UC& nNPI,char* pAddr);
	
	int ConvertStringToBCD(LPCSTR pString,UC* pBCD,US maxlen);

	//保留函数，原函数备份，现未用
	int ConvertStringToBCDA(LPCSTR pString,UC* pBCD,US maxlen);
	

	int ConvertBCDToString(UC* pBCD,US nLen,char* pString);
    //保留函数，原函数备份，现未用
	int ConvertBCDToStringA(UC* pBCD,US nLen,char* pString);

	UC ConvertUCToBCD(UC value);

	UC ConvertBCDToUC(UC nBCD);

	int ConvertOleTimeToTimeStamp(LPCSTR pString,UC* pStamp);

	int ConvertTimeStampToOleTime(UC* pStamp,char* pString);

	int ConvertPeriodToOleTime(UC* pPeriod,UC nFormat,char* pString);

	UC ConvertCharToBCD(char nCh);

	char ConvertBCDToChar(UC nBCD);

	int GetDCSType(UC nDCS);

	int GetOctetLength(UC nDCS,int nLen);
//convert octet len to 3.40 message length

	int GetUDLLength(UC nDCS,int nLen);
public:
	virtual int GetCommandID(LPVOID pOrgBuff, int iOrgSize, UL &ulCommandID);

	int ConvertTextToDefault(LPCSTR pString,UC* pData,US& nLen);

	int ConvertTextTo8Bits(LPCSTR pString,UC* pData,US& nLen);

	int ConvertTextTo8BitsEx(LPCSTR pString, int nStrLen, UC* pData,US& nLen);

	int ConvertTextToUCS2(LPCSTR pString,UC* pData,US& nLen);

	int ConvertDefaultToText(UC* pData,US nLen,char* pString);

	int ConvertDefaultWithUDHIToText(UC* pData,US nLen,char* pString);

	int Convert8BitsToText(UC* pData,US nLen,char* pString);

	int ConvertUCS2ToText(UC* pData,US nLen,char* pString);
	//add by wj
	int HexstrToBinstr( char* pHex,int Hexlen,char* pBin,int binlen);
	int BinstrToHexstr( char* pBin,int Binlen,char* pHex,int Hexlen);
	//end

	std::wstring ConvertUTF8ToWString(std::string& sUtf8);
	std::string ConvertWStringToUTF8(std::wstring& ws);
	std::string ConvertWStringToString(std::wstring& ws);
	std::wstring ConvertStringToWString(std::string s);
	std::wstring ConvertWStringToNWString(std::wstring wslocal);
	std::wstring ConvertNWStringToWString(std::wstring wsnet);


	int TrimLeft(char *pzSTR,int maxlen=0x7FFFFFFF);

	int ConvertSmppAddrToString(UC* pSMPP,UC nTON,UC nNPI,char* pAddr);

	int ConvertStringToSmppAddr(LPCSTR pAddr,UC nTON,UC nNPI,UC* pSMPP);
	char m_szCountryCode[MAX_COUNTRYCODE_LEN]; //国家代码，现未用，保留
};

#endif // !defined(_CODEANDDECODE_H_INCLUDED_)
