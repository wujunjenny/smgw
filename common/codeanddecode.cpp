#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
#include "stdafx.h"
#include "codeanddecode.h"
#include <locale>
#include <codecvt>
static UC ASCII[]={
	0x40,0x9c,0x24,0x9d,0x8a,0x82,0x97,0x8d,0x95,0x80,0x0a,0xed,0xec,0x0d,0x8f,0x86,
	0x7f,0x5f,0xe8,0xe2,0x5e,0xea,0xe3,0xe7,0xe4,0xe9,0xf0,0x01,0x92,0x91,0xe1,0x90,
	0x20,0x21,0x22,0x23,0xf8,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0xad,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x8e,0x99,0xa5,0x9a,0x15,
	0xa8,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x84,0x94,0xa4,0x81,0x85};

static UC Default[]={
	0x20,0x1b,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x0a,0x20,0x20,0x0d,0x20,0x20,
	0x20,0x20,0x20,0x20,0x20,0x5f,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
	0x20,0x21,0x22,0x23,0x02,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,
	0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,
	0x00,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,
	0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,0x5a,0x20,0x20,0x20,0x14,0x11,
	0x20,0x61,0x62,0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,0x6e,0x6f,
	0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,0x79,0x7a,0x20,0x20,0x20,0x20,0x10,
	0x09,0x7e,0x05,0x20,0x7b,0x7f,0x0f,0x20,0x20,0x20,0x04,0x20,0x20,0x07,0x5b,0x0e,
	0x1f,0x1d,0x1c,0x20,0x7c,0x08,0x20,0x06,0x20,0x5c,0x5e,0x20,0x01,0x03,0x20,0x20,
	0x20,0x20,0x20,0x20,0x7d,0x5d,0x20,0x20,0x60,0x20,0x20,0x20,0x20,0x40,0x20,0x20,
	0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
	0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
	0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
	0x20,0x1e,0x13,0x16,0x18,0x20,0x20,0x17,0x12,0x19,0x15,0x20,0x0c,0x0b,0x20,0x20,
	0x1a,0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x24,0x20,0x20,0x20,0x20,0x20,0x20,0x20};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCodeAndDecode::CCodeAndDecode()
{
	m_szCountryCode[0]=NULL;
}

CCodeAndDecode::~CCodeAndDecode()
{

}
int CCodeAndDecode::Code(LPVOID pOrgBuff, int& nOrgsize, LPVOID pDestBuff, int& nDestSize)
{
	return CODE_FAILED;
}

int CCodeAndDecode::Decode(LPVOID pOrgBuff, int& nOrgsize, LPVOID pDestBuff, int& nDestSize)
{
	return CODE_FAILED;
}
int CCodeAndDecode::ConvertTextToDataEx(LPCSTR pString, int nStrLen, UC nDCS, \
               UC* pData,US& nLen, CODE_INTERFACE CodeInterface)
{
    int nResult;

    nLen = 0;

	//DCS value is 00xxxxxx
	if(nDCS <= 0x3F)
	{
		//now it isn't support compress DCS
		if(nDCS & 0x20)
			return DCS_NOT_SUPPORT;

		switch((nDCS & 0x0C) >> 2)
		{
		case 0://Default alphabet
			//changed by wujun
			ConvertTextTo8BitsEx(pString, nStrLen, pData,nLen);
			break;
		case 1://8 bit
            return ConvertTextTo8BitsEx(pString, nStrLen, pData,nLen);
			break;
		case 2://UCS2
			if (CodeInterface == CODE_INTERFACE_SMPP)
			{
				nLen=wcslen((wchar_t* )pString);
				if(nLen > MAX_UD_LEN-1)
					nLen = MAX_UD_LEN-1;
				memcpy(pData,pString,MAX_UD_LEN-1);
				return nLen;
			}
			nResult = ConvertTextToUCS2(pString,pData,nLen);
			if( nResult == CONVERT_TEXT_UCS2_ERROR)
				return CONVERT_TEXT_DATA_ERROR;
			else
				return nResult;
			break;
		case 3://reserved
			return DCS_NOT_SUPPORT;
			break;
		}
	}

	if(nDCS >= 0x40 && nDCS <= 0xBF)
		return DCS_NOT_SUPPORT;

	if(nDCS >= 0xC0 && nDCS <= 0xDF)
		return ConvertTextTo8BitsEx(pString, nStrLen, pData,nLen);

	if(nDCS >= 0xE0 && nDCS <= 0xEF)
	{
		if(ConvertTextToUCS2(pString,pData,nLen) == CONVERT_TEXT_UCS2_ERROR)
			return CONVERT_TEXT_DATA_ERROR;
	}

	if(nDCS >= 0xF0 && nDCS <= 0xFF)
	{
		if(nDCS & 0x4)
			return ConvertTextTo8BitsEx(pString, nStrLen, pData,nLen);

		return ConvertTextTo8BitsEx(pString, nStrLen, pData,nLen);
	}

	return DCS_NOT_SUPPORT;
}

int CCodeAndDecode::ConvertTextToData(LPCSTR pString,UC nDCS,UC* pData,US& nLen, CODE_INTERFACE CodeInterface)
{
	int nResult;

    nLen = 0;

	if(nDCS <= 0x3F)
	{
		if(nDCS & 0x20)
			return DCS_NOT_SUPPORT;

		switch((nDCS & 0x0C) >> 2)
		{
		case 0:
			return ConvertTextTo8Bits(pString, pData,nLen);
			break;
		case 1:
			return ConvertTextTo8Bits(pString,pData,nLen);
			break;
		case 2:
			if (CodeInterface == CODE_INTERFACE_SMPP)
			{
				nLen=wcslen((wchar_t* )pString);
				if(nLen > MAX_UD_LEN-1)
					nLen = MAX_UD_LEN-1;
				memcpy(pData,pString,MAX_UD_LEN-1);
				return nLen;
			}
			nResult = ConvertTextToUCS2(pString,pData,nLen);
			if( nResult == CONVERT_TEXT_UCS2_ERROR)
				return CONVERT_TEXT_DATA_ERROR;
			else
				return nResult;
			break;
		case 3://reserved
			return DCS_NOT_SUPPORT;
			break;
		}
	}

	if(nDCS >= 0x40 && nDCS <= 0xBF)
		return DCS_NOT_SUPPORT;

	if(nDCS >= 0xC0 && nDCS <= 0xDF)
		//changged by wujun
		return ConvertTextTo8Bits(pString, pData,nLen);

	if(nDCS >= 0xE0 && nDCS <= 0xEF)
	{
        nResult = ConvertTextToUCS2(pString,pData,nLen);
		if(nResult == CONVERT_TEXT_UCS2_ERROR)
			return CONVERT_TEXT_DATA_ERROR;
        else
            return nResult; 
	}

	if(nDCS >= 0xF0 && nDCS <= 0xFF)
	{
		if(nDCS & 0x4)
			return ConvertTextTo8Bits(pString,pData,nLen);

		return ConvertTextTo8Bits(pString, pData,nLen);
	}

	return DCS_NOT_SUPPORT;
}

int CCodeAndDecode::ConvertTextToDefault(LPCSTR pString,UC* pData,US& nLen)
{
	int nPos=0,nCount=0,nBitPos=0;
	while(*pString && nPos < MAX_MESSAGE_UD_LEN-1)
	{
		switch(nBitPos)
		{
		case 0:
            pData[nPos] = Default[(UC)(*pString)];
			break;
		case 1:
            pData[nPos] |= (Default[(UC)(*pString)] & 0x01) << 7;
			nPos++;
            pData[nPos] = Default[(UC)(*pString)] >> 1;
			break;
		case 2://bit 7-6 is null
            pData[nPos] |= (Default[(UC)(*pString)] & 0x03) << 6;
			nPos++;
            pData[nPos] = Default[(UC)(*pString)] >> 2;
			break;
		case 3://bit 7-5 is null
            pData[nPos] |= (Default[(UC)(*pString)] & 0x07) << 5;
			nPos++;
            pData[nPos] = Default[(UC)(*pString)] >> 3;
			break;
		case 4://bit 7-4 is null
            pData[nPos] |= (Default[(UC)(*pString)] & 0x0F) << 4;
			nPos++;
            pData[nPos] = Default[(UC)(*pString)] >> 4;
			break;
		case 5://bit 7-3 is null
            pData[nPos] |= (Default[(UC)(*pString)] & 0x1F) << 3;
			nPos++;
            pData[nPos] = Default[(UC)(*pString)] >> 5;
			break;
		case 6://bit 7-2 is null
            pData[nPos] |= (Default[(UC)(*pString)] & 0x3F) << 2;
			nPos++;
            pData[nPos] = Default[(UC)(*pString)] >> 6;
			break;
		case 7://bit 7-1 is null
            pData[nPos] |= (Default[(UC)(*pString)] & 0x7F) << 1;
			nPos++;
            pData[nPos] = Default[(UC)(*pString)] >> 7;
			break;
		}

		nBitPos = (nBitPos+1) % 8;
		pString++,nCount++;
	}
	nLen = nCount;

	if(nBitPos)
		return nPos+1;
	else
		return nPos;
}
int CCodeAndDecode::ConvertTextTo8BitsEx(LPCSTR pString, int nStrLen, UC* pData,US& nLen)
{
	//fill user data length
	nLen=nStrLen;

	//if the SM is NULL,set lengh = 0
	if(!nLen)
		return 0;

	if(nLen > MAX_MESSAGE_UD_LEN-1)
		nLen = MAX_MESSAGE_UD_LEN-1;

	memcpy(pData,pString,nLen);

	return nLen;
}

int CCodeAndDecode::ConvertTextTo8Bits(LPCSTR pString,UC* pData,US& nLen)
{
	nLen=strlen(pString);

	if(!nLen)
		return 0;

	if(nLen > MAX_MESSAGE_UD_LEN-1)
		nLen = MAX_MESSAGE_UD_LEN-1;

	memcpy(pData,pString,nLen);

	return nLen;
}

int CCodeAndDecode::ConvertTextToUCS2(LPCSTR pString,UC* pData,US& nLen)
{
	int nCount=strlen(pString);
	
	char Buff[MAX_MESSAGE_UD_LEN*2];
	
	if(!nCount)
	{
		*pData = 0;
		return 1;
	}

	if(nCount > (MAX_MESSAGE_UD_LEN-1))
		nCount = (MAX_MESSAGE_UD_LEN-1);

	nCount = MultiByteToWideChar(CP_ACP,0,pString,nCount,(LPWSTR)Buff,(MAX_MESSAGE_UD_LEN*2));
	if(!nCount)
		return CONVERT_TEXT_UCS2_ERROR;
	
	if(nCount>(MAX_MESSAGE_UD_LEN-1) / 2)
		nCount = (MAX_MESSAGE_UD_LEN-1) / 2;
	for(int i=0;i<nCount;i++)
	{
		pData[2*i]=Buff[2*i+1];
		pData[2*i+1]=Buff[2*i];
	}
	nLen = 2*nCount;
	return 2*nCount;
}

int CCodeAndDecode::ConvertDataToText(UC* pData,US nLen,UC nDCS,UC nUDHI,char* pString, CODE_INTERFACE CodeInterface)
{
	if(nUDHI)
	{
        // 如果是连续短消息，并且长度正确
        if ((nLen >= 6) && (*(pData+1) == 0))
        {
            nLen-=6;
		    pData+=6;
        }
        else
        {
            //如果不是连续短消息，或者长度不正确
            memcpy(pString, pData, nLen);
            // 保证即使调用strlen也可以不越界
            pString[nLen] = 0;  
            return nLen;
        }
	}

	int nResult;
	if(nDCS <= 0x3F)
	{
		if(nDCS & 0x20)
			return DCS_NOT_SUPPORT;

		//get Bits 3 and 2
		switch((nDCS & 0x0C) >> 2)
		{
		case 0://Default alphabet
			return Convert8BitsToText(pData,nLen,pString);
			break;
		case 1://8 bit
			return Convert8BitsToText(pData,nLen,pString);
			break;
		case 2://UCS2
			if (CodeInterface == CODE_INTERFACE_SMPP)
			{
				if(!nLen)
				{
					*pString = '\0';
					return 0;
				}
				if(nLen > MAX_UD_LEN-1)
					nLen = MAX_UD_LEN-1;
				memcpy(pString,pData,nLen);
				pString[nLen] = 0;
				return nLen;
			}
			nResult = ConvertUCS2ToText(pData,nLen,pString);
			if( nResult == CONVERT_UCS2_TEXT_ERROR)
				return CONVERT_DATA_TEXT_ERROR;
			else
				return nResult;
			break;
		case 3://reserved
			return DCS_NOT_SUPPORT;
			break;
   case 15:
    if(nLen > MAX_UD_LEN-1)
					nLen = MAX_UD_LEN-1;
				
    memcpy(pString,pData,nLen);
		return  nLen;
		break; 
		}
	}

	if(nDCS >= 0x40 && nDCS <= 0xBF)
		return DCS_NOT_SUPPORT;

	if(nDCS >= 0xC0 && nDCS <= 0xDF)
		//changed by wujun
		return Convert8BitsToText(pData,nLen,pString);
		//return ConvertDefaultToText(pData,nLen,pString);

	if(nDCS >= 0xE0 && nDCS <= 0xEF)
	{
        nResult = ConvertUCS2ToText(pData,nLen,pString);
		if(nResult == CONVERT_UCS2_TEXT_ERROR)
			return CONVERT_DATA_TEXT_ERROR;
        else
            return nResult;
	}

	if(nDCS >= 0xF0 && nDCS <= 0xFF)
	{
		if(nDCS & 0x4)
			return Convert8BitsToText(pData,nLen,pString);

			return Convert8BitsToText(pData,nLen,pString);
			
	}

	return DCS_NOT_SUPPORT;
}

int CCodeAndDecode::ConvertDefaultToText(UC* pData,US nLen,char* pString)
{
	if(!nLen)
	{
		*pString = '\0';
		return 0;
	}

	if(nLen > MAX_MESSAGE_SM_LEN-1)
		nLen = MAX_MESSAGE_SM_LEN-1;

	int nCount=0,nBitPos=0;
	while(nLen)
	{
		switch(nBitPos)
		{
		case 0:
			*pString = ASCII[*pData & 0x7F];
			break;
		case 1:
			*pString = ASCII[(*pData >> 7) | ((*(pData+1) & 0x3F) << 1)];
			pData++,nCount++;
			break;
		case 2:
			*pString = ASCII[(*pData >> 6) | ((*(pData+1) & 0x1F) << 2)];
			pData++,nCount++;
			break;
		case 3:
			*pString = ASCII[(*pData >> 5) | ((*(pData+1) & 0xF) << 3)];
			pData++,nCount++;
			break;
		case 4:
			*pString = ASCII[(*pData >> 4) | ((*(pData+1) & 0x7) << 4)];
			pData++,nCount++;
			break;
		case 5:
			*pString = ASCII[(*pData >> 3) | ((*(pData+1) & 0x3) << 5)];
			pData++,nCount++;
			break;
		case 6:
			*pString = ASCII[(*pData >> 2) | ((*(pData+1) & 0x1) << 6)];
			pData++,nCount++;
			break;
		case 7:
			*pString = ASCII[*pData >> 1];
			pData++,nCount++;
			break;
		}

		nBitPos = (nBitPos+1) % 8;
		pString++,nLen--;
	}
	*pString = 0;

	return nCount + 1;
}

int CCodeAndDecode::ConvertDefaultWithUDHIToText(UC* pData,US nLen,char* pString)
{
	if(!nLen)
	{
		*pString = '\0';
		return 0;
	}
	
	if(nLen > MAX_MESSAGE_SM_LEN-1)
		nLen = MAX_MESSAGE_SM_LEN-1;
	int nCount=0,nBitPos=0;
	pData=pData+(UDH_DEFAULT_LENGTH-UDH_LENGTH); //add fill bits
	
    if (nLen < 2)
        return CONVERT_DATA_TEXT_ERROR;
    else
        nLen -= 2;
	while(nLen)
	{
		switch(nBitPos)
		{
		case 0:
			*pString = ASCII[*pData & 0x7F];
			break;
		case 1:
			*pString = ASCII[(*pData >> 7) | ((*(pData+1) & 0x3F) << 1)];
			pData++,nCount++;
			break;
		case 2://char is position at bit 6-4(next octet)
			*pString = ASCII[(*pData >> 6) | ((*(pData+1) & 0x1F) << 2)];
			pData++,nCount++;
			break;
		case 3://char is position at bit 5-3(next octet)
			*pString = ASCII[(*pData >> 5) | ((*(pData+1) & 0xF) << 3)];
			pData++,nCount++;
			break;
		case 4://char is position at bit 4-2(next octet)
			*pString = ASCII[(*pData >> 4) | ((*(pData+1) & 0x7) << 4)];
			pData++,nCount++;
			break;
		case 5://char is position at bit 3-1(next octet)
			*pString = ASCII[(*pData >> 3) | ((*(pData+1) & 0x3) << 5)];
			pData++,nCount++;
			break;
		case 6://char is position at bit 2-0(next octet)
			*pString = ASCII[(*pData >> 2) | ((*(pData+1) & 0x1) << 6)];
			pData++,nCount++;
			break;
		case 7://char is position at bit 1-7(next octet)
			*pString = ASCII[*pData >> 1];
			pData++,nCount++;
			break;
		}

		nBitPos = (nBitPos+1) % 8;
		pString++,nLen--;
	}
	
	*pString = 0;

	return nCount + 1;
}

int CCodeAndDecode::Convert8BitsToText(UC* pData,US nLen,char* pString)
{
	if(!nLen)
	{
		*pString = '\0';
		return 0;
	}

	if(nLen > MAX_MESSAGE_UD_LEN-1)
		nLen = MAX_MESSAGE_UD_LEN-1;

	//copy user data to string
	memcpy(pString,pData,nLen);
	pString[nLen] = 0;

	return nLen;
}

int CCodeAndDecode::ConvertUCS2ToText(UC* pData,US nLen,char* pString)
{

    char sTmpBuff[MAX_MESSAGE_UD_LEN];
    memset(sTmpBuff, 0, sizeof(sTmpBuff));

    if(!nLen)
	{
		*pString = '\0';
		return 0;
	}

	if(nLen > MAX_MESSAGE_UD_LEN-1)
		nLen = MAX_MESSAGE_UD_LEN-1;

    memcpy(sTmpBuff, pData, nLen);

	for(int i=0;i<nLen/2;i++)
	{
		UC nTemp = sTmpBuff[2*i];
		sTmpBuff[2*i] = sTmpBuff[2*i+1];
		sTmpBuff[2*i+1] = nTemp;
	}

	int nCount = nLen / 2;

	//convert unicode char to multibyte
	nCount=WideCharToMultiByte(CP_ACP,0,(LPCWSTR)sTmpBuff,nCount,pString,MAX_MESSAGE_UD_LEN-1,NULL,NULL);

	if(!nCount)
	{
		*pString = '\0';
		return CONVERT_UCS2_TEXT_ERROR;
	}

    if (nCount >= MAX_MESSAGE_UD_LEN)
    {
        nCount = MAX_MESSAGE_UD_LEN - 1;
    }
	pString[nCount] = 0;
	return nLen;
}

int CCodeAndDecode::ConvertStringToSmppAddr(LPCSTR pAddr,UC nTON,UC nNPI,UC* pSMPP)
{
	int nCount=0;
	US len;
	nCount=strlen(pAddr);
	switch(nTON)
	{
	case ADDR_TON_UNKNOWN:
	case ADDR_TON_INTERNATIONAL_NUMBER:
    case ADDR_TON_NATIONAL_NUMBER:
	case ADDR_TON_NETWORK_SPECIFIC_NUMBER:
	case ADDR_TON_SUBSCRIBER_NUMBER:
	case ADDR_TON_ABBREVIATED_NUMBER:
		
		nCount=strlen(pAddr);
		memcpy(pSMPP,pAddr,nCount+1);

		break;
	case ADDR_TON_ALPHANUMERIC:
		if(nNPI != 0)
			return CONVERT_STRING_ADDR_ERROR;
		len=strlen(pAddr);
		nCount = ConvertTextToDefault(pAddr,pSMPP,len);
		break;
	default:
        nCount=strlen(pAddr);
		memcpy(pSMPP,pAddr,nCount+1);
	}
	
	return nCount;
}

int CCodeAndDecode::ConvertStringToAddr(LPCSTR pAddr,UC nTON,UC nNPI,UC* pBCD)
{
	int nCount=0;
	US len;
	//we are now only support international number,
	//because interal protocol use international number
	switch(nTON)
	{
	case ADDR_TON_UNKNOWN:
	case ADDR_TON_NATIONAL_NUMBER:
	case ADDR_TON_INTERNATIONAL_NUMBER:
	case ADDR_TON_NETWORK_SPECIFIC_NUMBER:
	case ADDR_TON_SUBSCRIBER_NUMBER:
	case ADDR_TON_ABBREVIATED_NUMBER:
		
		nCount = ConvertStringToBCD(pAddr,&pBCD[2],MAX_ADDR_LEN-1);
		
		pBCD[0] = nCount;

		break;
	case ADDR_TON_ALPHANUMERIC:
		if(nNPI != 0)
			return CONVERT_STRING_ADDR_ERROR;
		len=pBCD[0];
		nCount = ConvertTextToDefault(pAddr,&pBCD[2],len);
		pBCD[0]=(UC)len;
		break;
	default:
		return CONVERT_STRING_ADDR_ERROR;
	}
	
	pBCD[1] = (nTON << 4) | (nNPI) | 0x80;

	if(nCount % 2)
		return (nCount/2+1) + 2;
	else
		return nCount/2 + 2;
}

#define MGTS_TEST	//for test

#ifdef MGTS_TEST
#define ADDR_ZONE_CODE_BCD	0x68	//for test
#endif

int CCodeAndDecode::ConvertAddrToString(UC* pBCD,UC& nTON,UC& nNPI,char* pAddr)
{
#ifndef MGTS_TEST
	if(!(pBCD[1] & 0x80))
	{
		*pAddr = '\0';
		return CONVERT_ADDR_STRING_ERROR;
	}
#endif
	

	nTON = (pBCD[1] & 0x70) >> 4;
	nNPI = pBCD[1] & 0x0F;

	int nCount=0;
	switch(nTON)
	{
	case ADDR_TON_NATIONAL_NUMBER:

		
		nCount = ConvertBCDToString(&pBCD[2],pBCD[0],pAddr);
		break;
	case ADDR_TON_UNKNOWN:
	case ADDR_TON_INTERNATIONAL_NUMBER:
	case ADDR_TON_NETWORK_SPECIFIC_NUMBER:
	case ADDR_TON_SUBSCRIBER_NUMBER:
	case ADDR_TON_ABBREVIATED_NUMBER:
		
		nCount = ConvertBCDToString(&pBCD[2],pBCD[0],pAddr);
		break;
	case ADDR_TON_ALPHANUMERIC:
		if(nNPI != 0)
			return CONVERT_ADDR_STRING_ERROR;
		
		nCount = ConvertDefaultToText(&pBCD[2],pBCD[0],pAddr);
		break;
	default:
		*pAddr = '\0';
		return CONVERT_ADDR_STRING_ERROR;
	}
	

	return nCount+2;
}

int CCodeAndDecode::ConvertSmppAddrToString(UC* pSMPP,UC nTON,UC nNPI,char* pAddr)
{

	int nCount=0;
	int len=TrimLeft((char*)pSMPP);
	switch(nTON)
	{
	case ADDR_TON_NATIONAL_NUMBER:

		nCount=strlen((char*)pSMPP);
		memcpy(pAddr,pSMPP,nCount+1);
		break;
	case ADDR_TON_UNKNOWN: 
	case ADDR_TON_INTERNATIONAL_NUMBER:
	case ADDR_TON_NETWORK_SPECIFIC_NUMBER:
	case ADDR_TON_SUBSCRIBER_NUMBER:
	case ADDR_TON_ABBREVIATED_NUMBER:
		
		nCount = len;
		
		

		memcpy(pAddr,pSMPP,nCount+1);
		break;
	case ADDR_TON_ALPHANUMERIC:
		if(nNPI != 0)
			return CONVERT_ADDR_STRING_ERROR;
		len=len*8/7;
		nCount = ConvertDefaultToText(pSMPP,len,pAddr);
		break;
	default:
        nCount = len;
        memcpy(pAddr,pSMPP,nCount+1);
        break;
	}
	

	return nCount;
}

int CCodeAndDecode::ConvertStringToBCD(LPCSTR pString,UC* pBCD,US maxlen)
{
	UC nCount=0,nPos=0,nBCD;
	while(nPos < maxlen)
	{
		if(pString[nPos])
		{
			nBCD = pString[nPos] - '0';
			if(nBCD > 9 )
			{
				if('*' == pString[nPos])
					nBCD = 10;
				else
					nBCD = 11;
			}

			if(nPos%2 == 0)
			{
				pBCD[nCount] = nBCD << 4;
			}
			else
			{
				pBCD[nCount] |= nBCD; 
				nCount++;
			}
		}
		else
		{
			//low byte set
			if(nPos%2 == 0)
				pBCD[nCount] = 0xFF;
			else
				pBCD[nCount] |= 0xF0;
			break;
		}

		nPos++;
	}

	return nPos;
}

int CCodeAndDecode::ConvertStringToBCDA(LPCSTR pString,UC* pBCD,US maxlen)
{
	UC nCount=0,nPos=0;
	while(nPos < maxlen)
	{
		if(pString[nPos])
		{
			if(nPos%2 == 0)
				pBCD[nCount] = (pString[nPos] - '0') ;
			else
			{
				pBCD[nCount] |= (pString[nPos]-'0') << 4; 
				nCount++;
			}
		}
		else
		{
			if(nPos%2 == 0)
				pBCD[nCount] = 0xFF;
			break;
		}

		nPos++;
	}

	if(nPos%2)
		pBCD[nCount] |= 0xF0;

	return nPos;
}

int CCodeAndDecode::ConvertBCDToStringA(UC* pBCD,US nLen,char* pString)
{
	static char code[]="0123456789*#####";
	for(US i=0;i<(nLen+1)/2;i++)
	{
		pString[2*i]=code[pBCD[i] & 0x0F];
		pString[2*i+1]=code[(pBCD[i] >> 4) & 0x0F];
	}
	pString[nLen]=0;
	return (nLen+1)/2;
}

//changed by wujun for decode map
int CCodeAndDecode::ConvertBCDToString(UC* pBCD,US nLen,char* pString)
{
	static char code[]="0123456789*#####";
	for(US i=0;i<(nLen+1)/2;i++)
	{
		pString[2*i]=code[(pBCD[i] >> 4) & 0x0F]; 
		pString[2*i+1]=code[pBCD[i] & 0x0F];
	}
	pString[nLen]=0;
	return (nLen+1)/2;
}

UC CCodeAndDecode::ConvertUCToBCD(UC value)
{
	if(value >= 100)
		return CONVERT_UC_BCD_ERROR;

	UC nBCD;
	nBCD = value / 10;
	nBCD |= (value % 10) << 4;
	return nBCD;
}

UC CCodeAndDecode::ConvertBCDToUC(UC nBCD)
{
	return (nBCD & 0x0F) * 10 + (nBCD >> 4);
}

int CCodeAndDecode::ConvertOleTimeToTimeStamp(LPCSTR pString,UC* pStamp)
{
	COleDateTime mStamp;
	if(!mStamp.ParseDateTime((LPCTSTR)pString))
		return FALSE;

	pStamp[0] = ConvertUCToBCD(UC((mStamp.GetYear())%100));
	pStamp[1] = ConvertUCToBCD(UC(mStamp.GetMonth()));
	pStamp[2] = ConvertUCToBCD(UC(mStamp.GetDay()));
	pStamp[3] = ConvertUCToBCD(UC(mStamp.GetHour()));
	pStamp[4] = ConvertUCToBCD(UC(mStamp.GetMinute()));
	pStamp[5] = ConvertUCToBCD(UC(mStamp.GetSecond()));

	if(pStamp[0] == CONVERT_CHAR_BCD_ERROR ||
	   pStamp[1] == CONVERT_CHAR_BCD_ERROR ||
	   pStamp[2] == CONVERT_CHAR_BCD_ERROR ||
	   pStamp[3] == CONVERT_CHAR_BCD_ERROR ||
	   pStamp[4] == CONVERT_CHAR_BCD_ERROR ||
	   pStamp[5] == CONVERT_CHAR_BCD_ERROR)
		return FALSE;

	pStamp[6] = ConvertCharToBCD(0);
	if(pStamp[6] == CONVERT_CHAR_BCD_ERROR)
		return FALSE;

	return TRUE;
}

int CCodeAndDecode::ConvertTimeStampToOleTime(UC* pStamp,char* pString)
{
	int nYear   = ConvertBCDToUC(pStamp[0]) + 1900;
	int nMonth  = ConvertBCDToUC(pStamp[1]);
	int nDay    = ConvertBCDToUC(pStamp[2]);
	int nHour   = ConvertBCDToUC(pStamp[3]);
	int nMinute = ConvertBCDToUC(pStamp[4]);
	int nSecond = ConvertBCDToUC(pStamp[5]);

	COleDateTime mCurTime=COleDateTime::GetCurrentTime();
	int nCurYear = mCurTime.GetYear();
	while(nYear < nCurYear)
		nYear += 100;


	COleDateTime mStamp;
	mStamp.SetDateTime(nYear,nMonth,nDay,nHour,nMinute,nSecond);

    if(mStamp.GetStatus() != COleDateTime::valid){
         mStamp = COleDateTime::GetCurrentTime();
    }
	strcpy(pString,mStamp.Format("%Y/%m/%d %H:%M:%S"));
	return TP_VP_ABSOLUTE_LEN;
}

int CCodeAndDecode::ConvertPeriodToOleTime(UC* pPeriod,UC nFormat,char* pString)
{
	if(nFormat > TP_VPF_ABSOLUTE)
		return VPF_NOT_SUPPORT;

	//format=1 is reserved
	if(nFormat == TP_VPF_RESERVED)
		return VPF_NOT_SUPPORT;

	if(nFormat == TP_VPF_NOT_PRESENT)
	{
		*pString = '\0';
		return 0;
	}

	if(nFormat == TP_VPF_ABSOLUTE)
		return ConvertTimeStampToOleTime(pPeriod,pString);

	COleDateTimeSpan mSpan;
	if(*pPeriod >=0 && *pPeriod <= 143)
	{
		int nMinute = (*pPeriod + 1) * 5;
		mSpan.SetDateTimeSpan(0,0,nMinute,0);
	}
	else if(*pPeriod >=144 && *pPeriod <= 167)
	{
		int nMinute = (*pPeriod - 143) * 30;
		mSpan.SetDateTimeSpan(0,12,nMinute,0);
	}
	else if(*pPeriod >=168 && *pPeriod <= 196)
	{
		int nDay = (*pPeriod - 166);
		mSpan.SetDateTimeSpan(nDay,0,0,0);
	}
	else if(*pPeriod >=197 && *pPeriod <= 255)
	{
		int nDay = (*pPeriod - 192) * 7;
		mSpan.SetDateTimeSpan(nDay,0,0,0);
	}

	COleDateTime mTime=COleDateTime::GetCurrentTime();
	mTime += mSpan;

    if(mTime.GetStatus() != COleDateTime::valid){
         mTime = COleDateTime::GetCurrentTime();
    }
	strcpy(pString,mTime.Format("%Y/%m/%d %H:%M:%S"));
	
	return TP_VP_RELATIVE_LEN;
}

UC CCodeAndDecode::ConvertCharToBCD(char nCh)
{
	//if beyond BCD range
	if(nCh > 79 || nCh < -79)
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

char CCodeAndDecode::ConvertBCDToChar(UC nBCD)
{
	char nCh = (nBCD & 0x7) * 10 + (nBCD >> 4);
	if(nBCD & 0x8)
		return -nCh;
	else
		return nCh;
}

int CCodeAndDecode::GetDCSType(UC nDCS)
{
	//DCS value is 00xxxxxx
	if(nDCS <= 0x3F)
	{
		//now it isn't support compress DCS
		if(nDCS & 0x20)
			return DCS_NOT_SUPPORT;

		//get Bits 3 and 2
		switch((nDCS & 0x0C) >> 2)
		{
		case 0://Default alphabet
			return DCS_DEFAULT;
			break;
		case 1://8 bit
			return DCS_8BIT;
			break;
		case 2://UCS2
			return DCS_UCS2;
		case 3://reserved
			return DCS_NOT_SUPPORT;
			break;
		}
	}

	if(nDCS >= 0x40 && nDCS <= 0xBF)
		return DCS_NOT_SUPPORT;

	if(nDCS >= 0xC0 && nDCS <= 0xDF)
		return DCS_DEFAULT;

	if(nDCS >= 0xE0 && nDCS <= 0xEF)
		return DCS_UCS2;

	if(nDCS >= 0xF0 && nDCS <= 0xFF)
	{
		if(nDCS & 0x4)
			return DCS_8BIT;

		return DCS_DEFAULT;
	}

	return DCS_NOT_SUPPORT;
}
int CCodeAndDecode::GetUDLLength(UC nDCS,int nLen)
{
	//get UD truly length
	int nUDSize;

	switch(GetDCSType(nDCS))
	{
	//if DCS is default code scheme,UDL is number of character
	//now it must get octet length
	case DCS_DEFAULT:
		nUDSize = 8 * nLen / 7;
		break;
	case DCS_8BIT:
		nUDSize = nLen;
		break;
	case DCS_UCS2:
		nUDSize = nLen;
		break;
	default:
		return DCS_NOT_SUPPORT;
		break;
	}

	return nUDSize;
}
int CCodeAndDecode::GetOctetLength(UC nDCS,int nLen)
{
	int nUDSize;
	switch(GetDCSType(nDCS))
	{
	case DCS_DEFAULT:
		nUDSize = 7 * nLen / 8;
		if(7 * nLen % 8)
			nUDSize ++;
		break;
	case DCS_8BIT:
		nUDSize = nLen;
		break;
	case DCS_UCS2:
		nUDSize = nLen;
		break;
	default:
		return DCS_NOT_SUPPORT;
		break;
	}

	return nUDSize;
}
int CCodeAndDecode::TrimLeft(char *pzSTR,int maxlen)
{
	int len=strlen(pzSTR);

	char *tempbuff=new char[len+1];
	if(!tempbuff)
		return 0;
	int i=0;
	int k=0;
	for(i=0;i<len;i++)
	{
		if(pzSTR[i]!=' ') break;
	}
	for(k=0;i<len;i++,k++)
	{
		tempbuff[k]=pzSTR[i];
	}
	if(k==0) 
	{
		pzSTR[0]=NULL;
		delete tempbuff;
		return 0;
	}
	tempbuff[k]=NULL;
	memset(pzSTR,0,len);
	memcpy(pzSTR,tempbuff,k);
	
	delete tempbuff;
	return k;
}
int CCodeAndDecode::GetCommandID(LPVOID pOrgBuff, int iOrgSize, UL &ulCommandID)
{
    return 0;
}


int CCodeAndDecode::HexstrToBinstr( char* pHex,int Hexlen,char* pBin,int binlen)
{
	int count = 0;
	int i;
	for( i = 0 ;i<Hexlen;i++)
	{
		char c = pHex[i];
		char v;
		if(c>='0'&&c<='9')
		{
			v = c-'0';
		}
		else if(c>='a'&&c<='f')
		{
			v = c-'a'+10;
		}
		else if(c>='A'&&c<='F')
		{
			v = c-'A'+10;
		}
		else
		{
			return i%2 ? count+1 : count ;
		}
		if(i%2)
		{
			pBin[count] += v;
			count++;
			if(count >= binlen )
			{
				return count;
			}

			
		}
		else
		{
			pBin[count] = v<<4;
		}

	}
	return i%2 ? count+1 : count ;
};


int CCodeAndDecode::BinstrToHexstr( char* pBin,int Binlen,char* pHex,int Hexlen)
{
	char* index = "0123456789ABCDEF";
	int i;
	int nHex = 0;
	if(2*Binlen>Hexlen+1)
	{
		return 0;
	}
	for(i = 0;i<Binlen;i++)
	{
		char h = ((unsigned char)pBin[i])>>4;
		pHex[nHex]=index[h];
		nHex++;
		h = pBin[i]&0x0F;
		pHex[nHex]=index[h];
		nHex++;
	}
	pHex[nHex] = 0;
	return nHex;
}


std::wstring CCodeAndDecode::ConvertUTF8ToWString(std::string& sUtf8)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> cv;

	return cv.from_bytes(sUtf8);
}

std::string CCodeAndDecode::ConvertWStringToUTF8(std::wstring& ws)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>,wchar_t> cv;
	return cv.to_bytes(ws);
}

std::string CCodeAndDecode::ConvertWStringToString(std::wstring& ws)
{
	std::locale sys_local("");
	typedef std::codecvt<wchar_t, char, mbstate_t> CodecvtFacet;
	const CodecvtFacet& cvt = std::use_facet<CodecvtFacet>(sys_local);
	std::wstring_convert<CodecvtFacet> cv((CodecvtFacet*)&std::use_facet<CodecvtFacet>(sys_local));
	std::string retstr;
	try{
		retstr = cv.to_bytes(ws);
	}
	catch(...)
	{
	}
	return	retstr;
}

std::wstring CCodeAndDecode::ConvertStringToWString(std::string s)
{
	std::locale sys_local("");
	typedef std::codecvt<wchar_t, char, mbstate_t> CodecvtFacet;
	const CodecvtFacet& cvt = std::use_facet<CodecvtFacet>(sys_local);
	std::wstring_convert<CodecvtFacet> cv((CodecvtFacet*)&std::use_facet<CodecvtFacet>(sys_local));
	return cv.from_bytes(s);
}

std::wstring CCodeAndDecode::ConvertWStringToNWString(std::wstring wslocal)
{
	std::wstring nws = wslocal;
	for(int i=0;i<nws.size();i++ )
	{
		nws[i] = htons(nws[i]);
	}
	return nws;
}

std::wstring CCodeAndDecode::ConvertNWStringToWString(std::wstring wsnet)
{
	std::wstring wslocal = wsnet;
	for(int i=0;i<wslocal.size();i++ )
	{
		wslocal[i] = ntohs(wslocal[i]);
	}
	return wslocal;
}
