#ifndef _TLV_H_INCLUDED_
#define _TLV_H_INCLUDED_
#include <vector>
#include <string>
#include <cassert>
#include "log.h"
//#include "define.h"

//changed by wj for long msg trans
#define STREAM_LEN_MAX 4096  //2048

namespace old_tlv {
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
	//add by wzy for 消息拆分
	CTLV( const CTLV& tlv );
	//end

	//***SMGW35-12, 2004-09-28,jdz add begin***//
	DWORD GetTLVLength();
	//***SMGW35-12, 2004-09-28,jdz add end***//

	~CTLV();
};
}

namespace tlv {

class CTLV
{
public:
	int FromStream(const BYTE*	pStream, const DWORD& dwStreamLength, DWORD& dwStreamPosition)
	{
			unsigned short wTagTemp=0, wLenTemp=0;
			while(dwStreamPosition < dwStreamLength)
			{
				if(dwStreamLength -dwStreamPosition < sizeof(wTagTemp)+sizeof(wLenTemp))
					return -1;

				memcpy(&wTagTemp, pStream +dwStreamPosition, sizeof(wTagTemp));
				memcpy(&wLenTemp, pStream +dwStreamPosition+ sizeof(wTagTemp) , sizeof(wLenTemp));
				wTagTemp = ntohs(wTagTemp);
				wLenTemp = ntohs(wLenTemp);
		
				if( dwStreamPosition + wLenTemp + sizeof(wTagTemp) + sizeof(wLenTemp) > dwStreamLength ) 
					return -1;
				//AddItem((DWORD)wTagTemp, (DWORD)wLenTemp, pStream +dwStreamPosition +sizeof(wLenTemp) +sizeof(wTagTemp) );
				v_tlv.push_back(std::make_pair(wTagTemp,
								std::string((char*)pStream +dwStreamPosition +sizeof(wLenTemp) +sizeof(wTagTemp),
								wLenTemp))
								);
				dwStreamPosition += wLenTemp + sizeof(wLenTemp) + sizeof(wTagTemp);
			}
			return 0;
	};

	const BYTE* GetItem(DWORD dwTag, DWORD& dwLen)
	{
		for(int i = 0;i<v_tlv.size();i++)
		{
			if(v_tlv[i].first == dwTag)
			{
				dwLen = v_tlv[i].second.size();
				return (const BYTE*)v_tlv[i].second.data();
			}

		}
		dwLen = 0;
		return nullptr;
	};

	int DeleteItem(DWORD dwTag)
	{
		for( auto itr = v_tlv.begin();itr!=v_tlv.end(); )
		{
			if(itr->first == dwTag )
			{
				itr = v_tlv.erase(itr);
			}
			else
			{
				itr++;
			}
		}
		return 0;
	}

	int AddItem(DWORD dwTag, DWORD dwLen, const BYTE* bstrValue)
	{
		v_tlv.push_back(std::make_pair((unsigned short)dwTag,std::string((const char*)bstrValue,dwLen)));
		return 0;
	};

	int SetItem(DWORD dwTag, DWORD dwLen, const BYTE* pValue)
	{
		for(int i = 0;i<v_tlv.size();i++)
		{
			if(v_tlv[i].first == dwTag)
			{
				v_tlv[i].second.assign((const char*)pValue,(const char*)pValue+dwLen);
				return 0;
			}

		}
		v_tlv.push_back(std::make_pair((unsigned short)dwTag,std::string((const char*)pValue,dwLen)));
		return 0;
	}

	//***SMGW40-01, 2004-12-23, jdz, modi begin***//
	long int GetLongIntItemValue(DWORD dwTag)
	{
		for(int i = 0;i<v_tlv.size();i++)
		{
			if(v_tlv[i].first == dwTag)
			{
				assert( v_tlv[i].second.size()==sizeof(int));
				auto pv = v_tlv[i].second.data();
				
				int* ip = (int* )pv;
				
				return ntohl(*ip);
			}

		}
		return 0;
	}

	bool AddLongIntItem(DWORD dwTag, const long int bstrValue)
	{
		int nvalue = htonl(bstrValue);
		v_tlv.push_back(std::make_pair((unsigned short)dwTag,std::string((const char*)&nvalue,sizeof(nvalue))));
		return true;
	};

	bool SetLongIntItem(DWORD dwTag, const long int bstrValue)
	{
		int ivalue = htonl(bstrValue);
		const char* pValue = (char*)&ivalue;
		int dwLen =sizeof(int);

		for(int i = 0;i<v_tlv.size();i++)
		{
			if(v_tlv[i].first == dwTag)
			{
				v_tlv[i].second.assign((const char*)pValue,(const char*)pValue+dwLen);
				return true;
			}

		}
		v_tlv.push_back(std::make_pair((unsigned short)dwTag,std::string((const char*)pValue,dwLen)));
		return true;
	}
	//***SMGW40-01, 2004-12-23, jdz, modi end***//

	int ToStream(BYTE *pStream, DWORD& dwStreamLength)
	{
			unsigned short wPosTemp = 0;//(unsigned short)dwStreamLength;
			unsigned short wTemp;
			if(GetTLVLength() > STREAM_LEN_MAX)
			{
				LOG(ERROR)<<"TLV len="<<GetTLVLength()<<"; > maxlen" << STREAM_LEN_MAX;
				return -1;
			}

			for(int i = 0;i<v_tlv.size();i++)
			{
				auto& p = v_tlv[i];

				//判断是否超出目标缓冲区
				//if( wPosTemp + p.second.size() + 2*sizeof(wTemp) > STREAM_LEN_MAX) 
				//{
				//	TRACE("TLV数据入缓存时，缓存区溢出\n");
				//	ASSERT(0);
				//	return -1;
				//}

				wTemp = htons(p.first);
				memcpy(pStream + wPosTemp, &wTemp, sizeof(wTemp) );
				wPosTemp += sizeof(wTemp);

				wTemp = htons((unsigned short)p.second.size());
				memcpy(pStream + wPosTemp, &wTemp, sizeof(wTemp) );
				wPosTemp += sizeof(wTemp);

				memcpy(pStream + wPosTemp, p.second.data(), p.second.size());
				wPosTemp += p.second.size();
			}

			dwStreamLength = wPosTemp;
			return 0;
	}

	CTLV(){};
	//add by wzy for 消息拆分
	CTLV( const CTLV& tlv ){ v_tlv = tlv.v_tlv;};
	//end
	~CTLV(){};

	DWORD GetTLVLength()
	{
		int len = 0;
		for(int i =0;i<v_tlv.size();i++)
		{
			len  += v_tlv[i].second.size()+ 2*sizeof(unsigned short);
		}
		return len;
	}

	int GetCount() {return v_tlv.size();};

	std::pair<unsigned short,std::string> GetTlv(int index)
	{ if( index>=0 && index < v_tlv.size()) 
		return v_tlv[index];
	  return default_v;
	};
protected:
	std::vector<std::pair<unsigned short,std::string>> v_tlv;
	static std::pair<unsigned short,std::string> default_v;
};

}

using namespace tlv;

#endif //_TLV_H_INCLUDED_











