#ifndef _TLV_H_INCLUDED_
#define _TLV_H_INCLUDED_

//#include "define.h"

//changed by wj for long msg trans
#define STREAM_LEN_MAX 4096  //2048

class CTLV  
{
private:
	struct STLV{
		DWORD	dwTag;
		DWORD	dwLen;
		BYTE*	bstrValue;
		STLV*	pNext;
	};
	STLV*	m_pSTLV;
public:
	int FromStream(const BYTE*	pStream, const DWORD& dwStreamLength, DWORD& dwStreamPosition);
	const BYTE* GetItem(DWORD dwTag, DWORD& dwLen);
	int DeleteItem(DWORD dwTag);
	int AddItem(DWORD dwTag, DWORD dwLen, const BYTE* bstrValue);
	int SetItem(DWORD dwTag, DWORD dwLen, const BYTE* pValue);

	//***SMGW40-01, 2004-12-23, jdz, modi begin***//
	long int GetLongIntItemValue(DWORD dwTag);
	bool AddLongIntItem(DWORD dwTag, const long int bstrValue);
	bool SetLongIntItem(DWORD dwTag, const long int bstrValue);
	//***SMGW40-01, 2004-12-23, jdz, modi end***//

	int ToStream(BYTE *pStream, DWORD& dwStreamLength);
	CTLV();
	//add by wzy for ÏûÏ¢²ð·Ö
	CTLV( const CTLV& tlv );
	//end

	//***SMGW35-12, 2004-09-28,jdz add begin***//
	DWORD GetTLVLength();
	//***SMGW35-12, 2004-09-28,jdz add end***//

	~CTLV();
};

#endif //_TLV_H_INCLUDED_











