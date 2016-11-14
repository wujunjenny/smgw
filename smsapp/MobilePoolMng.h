#pragma once
#include <list>
#include <vector>
#include <string>
#include <unordered_map>
#include <rapidjson\document.h>
#include "MOHUTree.h"

class MobileAddrPoolMng
{
public:
	typedef std::list<std::string> TYPE_EXPARAMLIST;
	typedef std::list<std::string> TYPE_MOBILE_ADDR_POOL;
	struct _RUNTIME_DATA
	{
		std::string mobile_addr;
		int count;
	};
	typedef std::list<_RUNTIME_DATA> TYPE_RUNTIME_POOL;
	struct _POOL_
	{
		std::string key;
		TYPE_MOBILE_ADDR_POOL mobile_addr_pool;
		TYPE_RUNTIME_POOL runtime_pool;
	};
	struct _NET_PARAM_
	{
		int nettype;
		int count;//limited count by unit of time
		std::string description;
	};

	MobileAddrPoolMng(void);
	~MobileAddrPoolMng(void);
	int SaveToFile();
	int LoadFromFile();
	int LoadConfig();
	int SaveConfig();
	int SaveNet2AddrTable();
	int LoadNet2AddrTable();
//
	int GetPhoneNumber(int smcount,int nettype,std::string key,std::string& outPhoneNumber);
	int FushLimitCount();
// add by wj 
	bool IsSPAddrPoolEnable(LPCTSTR pSPID);
	bool SetSPAddrPoolEnable(LPCTSTR pSPID,bool bEnable);

	void ClearMobileAddrPool();
	int SetMobileAddrPool(int nettype,std::string key,TYPE_MOBILE_ADDR_POOL& pools);
	int SetNetParam(int nettype,int limit);

	int GetNetType(LPCTSTR phoneNumber);
	int AddKeyForNetType(LPCTSTR phoneNumber ,int nettype);
	int ClearNetTypeTable();


	int GetExParamStartPos();
	int GetExParamLen();

	bool CheckFlush();
protected:

	time_t m_lastCheckTime;
	int m_nCycle_minute;

	bool m_bActivePool;
	int m_nLocalNetType;

	int m_nExtParam_start_pos;
	int m_nExtParam_len;

	std::unordered_map<std::string,_POOL_> m_mobileaddrpool;
	std::unordered_map<std::string,bool> m_spEnableMap;
	std::list<_NET_PARAM_> m_netparamlist;
	CFuzzyTree m_FuzzyTree;

	CFuzzyTree m_NetTypeFuzzyTree;
	std::unordered_map<std::string,int> m_AddrToNettypeTable;

	CRITICAL_SECTION m_Lock;

	int _SetMobileAddrPool(int nettype,std::string key,TYPE_MOBILE_ADDR_POOL& pools,_NET_PARAM_& param);


};


inline int MobileAddrPoolMng::GetExParamStartPos()
{
	return m_nExtParam_start_pos;
};
inline int MobileAddrPoolMng::GetExParamLen()
{
	return m_nExtParam_len;
};



class AddrConvertTable
{
public:
	AddrConvertTable();
	~AddrConvertTable();
	int Convert(LPCTSTR pDstAddr,LPCTSTR pOrgAddr,int InOut,LPCTSTR pActName,std::string& soutDst,std::string& soutOrg);
	int LoadAddrConvertTable();
	int SaveAddrCouvertTable();
	int SetTable(rapidjson::Value& reader,std::string& error);
	int BackupData();
protected:
	int SetRecord(LPCTSTR pActName,LPCTSTR pInOut,LPCTSTR pDO,LPCTSTR pRule,LPCTSTR pReplace);
	void Clear();

	struct ConvertData
	{
		std::string accountname;
		char   inout;
		char   ds;
		std::string rule;
		std::string replace;
	};

	
	struct AccountData
	{
		std::string accountname;
		std::unordered_map<std::string,ConvertData> table;
		CFuzzyTree FuzzyTree;
	};

	std::unordered_map<std::string,AccountData> m_AccountTable;

	CRITICAL_SECTION m_Lock;
	
};


class DcsConvertTable
{
public:
	DcsConvertTable();
	~DcsConvertTable();
	int IsConvert(LPCTSTR pActName);
	int LoadDcsConvertTable();
	int SaveDcsConvertTable();
	int SetTable(rapidjson::Value& reader,std::string& error);
	int BackupData();
protected:
	int SetRecord(LPCTSTR pActName,int nConvert);
	void Clear();

	std::unordered_map<std::string,int> m_AccountTable;

	CRITICAL_SECTION m_Lock;
	
};