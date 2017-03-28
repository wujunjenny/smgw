#include "stdafx.h"
#include <string.h>
//#include <winsock2.h>
#include "log.h"
#include "tlv.h"

namespace old_tlv {

CTLV::CTLV() : m_pSTLV(NULL)
{
	
}

//add by wzy for Message div
CTLV::CTLV( const CTLV& tlv )
{
	this->m_pSTLV = NULL;
	STLV* pSTLV = tlv.m_pSTLV;
	while( pSTLV )
	{
		AddItem( pSTLV ->dwTag, pSTLV->dwLen, pSTLV->bstrValue );
		pSTLV = pSTLV->pNext;
	}
	return;
}
//end
CTLV::~CTLV()
{
	STLV* pSTLV = m_pSTLV;
	while(pSTLV)
	{
		m_pSTLV = m_pSTLV ->pNext;
		delete[] pSTLV ->bstrValue;
		delete pSTLV;
		pSTLV = m_pSTLV;
	}
}

int CTLV::AddItem(DWORD dwTag, DWORD dwLen, const BYTE* bstrValue)
{
	STLV* plastSTLV = m_pSTLV;
	STLV* pnewSTLV = new STLV;

	if( pnewSTLV == NULL )
		return -1;

	pnewSTLV ->dwTag = dwTag;
	pnewSTLV ->dwLen = dwLen;
	pnewSTLV ->bstrValue = new BYTE[dwLen+1];
	memset(pnewSTLV ->bstrValue,0,dwLen+1);
	memcpy(pnewSTLV ->bstrValue, bstrValue, dwLen);
	pnewSTLV ->pNext = NULL;

	while(plastSTLV && plastSTLV ->pNext )
		plastSTLV = plastSTLV ->pNext;
	
	if( plastSTLV == NULL)
		m_pSTLV = pnewSTLV;
	else
		plastSTLV ->pNext = pnewSTLV;

	return 0;
}

/*
描述：设定一个TLV字段
by:wujun
*/

int CTLV::SetItem(DWORD dwTag, DWORD dwLen, const BYTE* pValue)
{
	STLV* pSTLV = m_pSTLV;
	STLV* pPreSTLV = NULL;
	while(1)
	{
		if(pSTLV == NULL)
		{
			//if search fail,add new item
			STLV* pnewSTLV = new STLV;
			pnewSTLV ->dwTag = dwTag;
			pnewSTLV ->dwLen = dwLen;
			pnewSTLV ->bstrValue = new BYTE[dwLen+1];
			memset(pnewSTLV ->bstrValue,0,dwLen+1);
			memcpy(pnewSTLV ->bstrValue, pValue, dwLen);
			pnewSTLV ->pNext = NULL;
			if(pPreSTLV == NULL)
			{
				m_pSTLV = pnewSTLV;
			}
			else
			{
				pPreSTLV->pNext = pnewSTLV;
			}

			return NULL;
		}

		if(pSTLV->dwTag == dwTag)
		{
			if(pValue == pSTLV->bstrValue)
			{
				ASSERT(0);
				return NULL;
			}

			break;
		}

		pPreSTLV = pSTLV ;
		pSTLV = pSTLV->pNext;
	}
	
	if(dwLen <= pSTLV->dwLen)
	{
		pSTLV->dwLen = dwLen;
		memset(pSTLV->bstrValue,0,dwLen+1);
		memcpy(pSTLV->bstrValue,pValue,dwLen);
	}
	else
	{
		pSTLV->dwLen =dwLen;
		delete[] pSTLV->bstrValue ;
		pSTLV->bstrValue = new BYTE[dwLen+1];
		memset(pSTLV->bstrValue,0,dwLen+1);
		memcpy(pSTLV->bstrValue,pValue,dwLen);
	}

	return NULL;
};


//函数描述: 取得一个TLV字段
//
//入口参数: dwTag: TLV字段的Tag值
//
//返回值: value的地址
//        dwLen: 引用对象 返回TLV字段的value的长度  

const BYTE* CTLV::GetItem(DWORD dwTag, DWORD& dwLen)
{
	STLV* pSTLV = m_pSTLV;
	while(1)
	{
		if(pSTLV == NULL)
		{
			return NULL;
		}
		if(pSTLV->dwTag == dwTag)
			break;
		pSTLV = pSTLV->pNext;
	}
	dwLen = pSTLV->dwLen;
	return pSTLV->bstrValue;
}

//函数返回成功与否 0 成功
//函数在目标缓冲区足够的情况下返回成功 0
//否则返回 -1 失败 并不改变原来的缓冲区内容
int CTLV::ToStream(BYTE *pStream, DWORD& dwStreamLength)
{
	WORD wPosTemp = (WORD)dwStreamLength;
	WORD	wTemp;
	STLV* pSTLV = m_pSTLV;
	while(pSTLV!=NULL)
	{
		//判断是否超出目标缓冲区
		if( wPosTemp + pSTLV->dwLen + 2*sizeof(wTemp) > STREAM_LEN_MAX) 
		{
			TRACE("TLV数据入缓存时，缓存区溢出\n");
			ASSERT(0);
			return -1;
		}

		wTemp = htons((WORD)pSTLV ->dwTag);
		memcpy(pStream + wPosTemp, &wTemp, sizeof(wTemp) );
		wPosTemp += sizeof(wTemp);

		wTemp = htons((WORD)pSTLV ->dwLen);
		memcpy(pStream + wPosTemp, &wTemp, sizeof(wTemp) );
		wPosTemp += sizeof(wTemp);

		memcpy(pStream + wPosTemp, pSTLV->bstrValue, pSTLV->dwLen);
		wPosTemp += (WORD)(pSTLV->dwLen);

		pSTLV = pSTLV ->pNext;
	}

	dwStreamLength = wPosTemp;
	return 0;
}



//函数返回成功与否 0 成功

//函数在缓冲区长度和TLV内容匹配情况下返回0
//否则返回 -1 失败 并在CTLV中加入匹配的成员
int CTLV::FromStream(const BYTE *pStream, const DWORD &dwStreamLength, DWORD &dwStreamPosition)
{
	WORD wTagTemp=0, wLenTemp=0;
	while(dwStreamPosition < dwStreamLength)
	{
		memcpy(&wTagTemp, pStream +dwStreamPosition, sizeof(wTagTemp));
		memcpy(&wLenTemp, pStream +dwStreamPosition+ sizeof(wTagTemp) , sizeof(wLenTemp));
		wTagTemp = ntohs(wTagTemp);
		wLenTemp = ntohs(wLenTemp);
		
		if( dwStreamPosition + wLenTemp + sizeof(wTagTemp) + sizeof(wLenTemp) > dwStreamLength ) 
			return -1;

		AddItem((DWORD)wTagTemp, (DWORD)wLenTemp, pStream +dwStreamPosition +sizeof(wLenTemp) +sizeof(wTagTemp) );

		dwStreamPosition += wLenTemp + sizeof(wLenTemp) + sizeof(wTagTemp);
	}
	return 0;
}

//***SMGW35-12, 2004-09-28,jdz add begin***//
DWORD CTLV::GetTLVLength()
{
	DWORD dwLen = 0;
	STLV* pSTLV = m_pSTLV;
	while(pSTLV != NULL)
	{
		dwLen += pSTLV->dwLen + sizeof(WORD) + sizeof(WORD);
		pSTLV = pSTLV ->pNext;
	}
	return dwLen;
}
//***SMGW35-12, 2004-09-28,jdz add end***//


//***SMGW40-01, 2004-12-23, jdz, modi begin***//	  

//从TLV中获取一个long int 型数据的值
long int CTLV::GetLongIntItemValue(DWORD dwTag)
{
	STLV* pSTLV = m_pSTLV;
	while(1)
	{
		if(pSTLV == NULL)
		{
			return 0;
		}

		if(pSTLV->dwTag == dwTag)
		{
			break;
		}

		pSTLV = pSTLV->pNext;
	}

	long int temp;
	memcpy(&temp, pSTLV->bstrValue, sizeof(long int));
	long int temp2 = ntohl(temp);

	return temp2;
}


//增加一个long int 型数据到TLV中
bool CTLV::AddLongIntItem(DWORD dwTag, const long int bstrValue)
{
	STLV* plastSTLV = m_pSTLV;
	STLV* pnewSTLV = new STLV;

	if( pnewSTLV == NULL )
		return false;

	pnewSTLV->dwTag = dwTag;
	pnewSTLV->dwLen = sizeof(long int);
	pnewSTLV->bstrValue = new BYTE[sizeof(long int)+1];
	memset(pnewSTLV->bstrValue, 0, sizeof(long int)+1);
	long int temp = htonl(bstrValue);
	//(*(long int*)pnewSTLV->bstrValue) = temp;
	memcpy(pnewSTLV->bstrValue, &temp, sizeof(long int));
	pnewSTLV ->pNext = NULL;

	while(plastSTLV && plastSTLV ->pNext )
		plastSTLV = plastSTLV ->pNext;
	
	if( plastSTLV == NULL)
		m_pSTLV = pnewSTLV;
	else
		plastSTLV ->pNext = pnewSTLV;

	return true;
}	  


//重新设置一个long int 型数据到TLV中
bool CTLV::SetLongIntItem(DWORD dwTag, const long int bstrValue)
{
	STLV* pSTLV = m_pSTLV;
	STLV* pPreSTLV = NULL;
	while(1)
	{
		if(pSTLV == NULL)
		{
			//if search fail,add new item
			STLV* pnewSTLV = new STLV;
			if( pnewSTLV == NULL )
				return false;

			pnewSTLV->dwTag = dwTag;
			pnewSTLV->dwLen = sizeof(long int);
			pnewSTLV->bstrValue = new BYTE[sizeof(long int)+1];
			memset(pnewSTLV->bstrValue, 0, sizeof(long int)+1);
			long int temp = htonl(bstrValue);
			//(*(long int*)pnewSTLV->bstrValue) = temp;
			memcpy(pnewSTLV->bstrValue, &temp, sizeof(long int));
			pnewSTLV ->pNext = NULL;
			
			if(pPreSTLV == NULL)
			{
				m_pSTLV = pnewSTLV;
			}
			else
			{
				pPreSTLV->pNext = pnewSTLV;
			}

			return true;
		}

		if(pSTLV->dwTag == dwTag)
		{
			break;
		}

		pPreSTLV = pSTLV;
		pSTLV = pSTLV->pNext;
	}
	
	if(sizeof(long int) <= pSTLV->dwLen)
	{
		pSTLV->dwLen = sizeof(long int);
		
		memset(pSTLV ->bstrValue, 0, sizeof(long int)+1);
		long int temp = htonl(bstrValue);
		memcpy(pSTLV ->bstrValue, &temp, sizeof(long int));
	}
	else
	{
		pSTLV->dwLen = sizeof(long int);
		delete[] pSTLV->bstrValue;
		pSTLV->bstrValue = new BYTE[sizeof(long int)+1];
		memset(pSTLV->bstrValue, 0, sizeof(long int)+1);
		long int temp = htonl(bstrValue);
		memcpy(pSTLV ->bstrValue, &temp, sizeof(long int));
	}

	return true;
}


};
//***SMGW40-01, 2004-12-23, jdz, modi end***//



namespace tlv
{

		
	std::pair<unsigned short,std::string> CTLV::default_v(0,"");

};