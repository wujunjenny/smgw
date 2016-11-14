#include "stdafx.h"
#include "MobilePoolMng.h"
#include <sstream>
#include <rapidjson\document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/filewritestream.h>
#include <rapidjson\writer.h>
#include "OmcManager.h"


MobileAddrPoolMng::MobileAddrPoolMng(void)
{
	::InitializeCriticalSection(&m_Lock);

	this->LoadConfig();
	this->LoadFromFile();
	this->LoadNet2AddrTable();
	m_lastCheckTime = time(nullptr);
	FushLimitCount();
}


MobileAddrPoolMng::~MobileAddrPoolMng(void)
{
	::DeleteCriticalSection(&m_Lock);
}


int MobileAddrPoolMng::SaveToFile()
{
		AUTO_LOCK(&m_Lock);

		FILE* fp = fopen("MobileAddrPoolData.json","wb");
		if(fp == NULL)
			return -1;
		char writeBuff[65536];
		rapidjson::FileWriteStream os(fp,writeBuff,sizeof(writeBuff));
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

		writer.StartObject();
		writer.Key("PoolArray");
		writer.StartArray();
		auto itr = m_mobileaddrpool.begin();
		while(itr!= m_mobileaddrpool.end())
		{
			writer.StartObject();

			if(itr->second.key.size()>0)
			{
				writer.Key("key");
				writer.String(itr->second.key.c_str());
			}

			if(itr->second.mobile_addr_pool.size()>0)
			{
				writer.Key("mobileAddrPool");
				writer.StartArray();
				auto itr_pool =  itr->second.mobile_addr_pool.begin();
				for(;itr_pool!=itr->second.mobile_addr_pool.end();)
				{
					writer.String(itr_pool->c_str());
					itr_pool++;
				}
				writer.EndArray();
			}

			writer.EndObject();
			itr++;
		}	
		writer.EndArray();
		
		writer.Key("SPSwitchArray");
		writer.StartArray();
		auto itr_enable = m_spEnableMap.begin();
		while(itr_enable!= m_spEnableMap.end())
		{
			writer.StartObject();
			writer.Key("SPID");
			writer.String(itr_enable->first.c_str());
			writer.Key("enable");
			writer.Int(itr_enable->second);
			writer.EndObject();
			itr_enable++;
		}
		writer.EndArray();
		writer.EndObject();		
		fclose(fp);

		return 0;
}

int MobileAddrPoolMng::LoadFromFile()
{
	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("MobileAddrPoolData.json","rb");
	if(fp == NULL)
		return FALSE;
	char readBuff[65536];
	rapidjson::FileReadStream is(fp,readBuff,sizeof(readBuff));
	rapidjson::Document reader;
	reader.ParseStream(is);
	fclose(fp);
	if(reader.HasParseError())
	{
		return -1;
	}

	if(reader.HasMember("PoolArray"))
	{
		m_mobileaddrpool.clear();
		rapidjson::Value& _array = reader["PoolArray"];
		auto itr = _array.Begin();
		while (itr!=_array.End())
		{
			std::string s_key;
			std::pair<std::string,_POOL_> data;
			if(itr->HasMember("key"))
			{
				s_key = (*itr)["key"].GetString();
				data.first = s_key;
				data.second.key = s_key;
			}

			if(itr->HasMember("mobileAddrPool"))
			{
				auto itr_pool = (*itr)["mobileAddrPool"].Begin();
				while(itr_pool!=(*itr)["mobileAddrPool"].End())
				{
					data.second.mobile_addr_pool.push_back(itr_pool->GetString());
					itr_pool++;
				}
			}

			auto rt = m_mobileaddrpool.insert(data);
			if(rt.second == true)
			{
				auto bok = m_FuzzyTree.AddKey(rt.first->first.c_str(),&(rt.first->second));
				if(bok==FALSE)
				{
					//严重错误;
				}
			}
			itr++;
		}

	}

	if(reader.HasMember("SPSwitchArray"))
	{
		rapidjson::Value& _array = reader["SPSwitchArray"];
		auto itr = _array.Begin();
		while (itr!=_array.End())
		{
			if(!itr->HasMember("SPID"))
			{
				continue;
			}
			if(!itr->HasMember("enable"))
			{
				continue;
			}
			
			SetSPAddrPoolEnable( (*itr)["SPID"].GetString(),(*itr)["enable"].GetInt()==1);
			itr++;
		}
	}
	return 0;
}

int MobileAddrPoolMng::SetMobileAddrPool(int nettype,std::string key,TYPE_MOBILE_ADDR_POOL& pools)
{
	AUTO_LOCK(&m_Lock);
	if(nettype == 0)
	{
		auto itr = this->m_netparamlist.begin();
		int rt = 0;
		for(;itr!= m_netparamlist.end();)
		{
			rt = _SetMobileAddrPool(itr->nettype,key,pools,*itr);
			itr++;
		}
		return rt;
	}
	_NET_PARAM_ param;
	param.count = -1;
	param.nettype = nettype;
	auto itr_param = this->m_netparamlist.begin();
	for(;itr_param!=m_netparamlist.end();)
	{
		if(itr_param->nettype==nettype)
		{
			param = *itr_param;
			break;		
		}
		itr_param++;
	}

	return _SetMobileAddrPool(nettype,key,pools,param);
	
}


int MobileAddrPoolMng::_SetMobileAddrPool(int nettype,std::string key,TYPE_MOBILE_ADDR_POOL& pools,_NET_PARAM_& param)
{
	std::stringstream s_key;
	s_key << nettype;
	s_key << "@";
	s_key << key;

	auto itr = m_mobileaddrpool.insert(std::make_pair<std::string,_POOL_>(s_key.str(),_POOL_()));
	_POOL_& pool = itr.first->second;
	if(itr.second == true)
	{
		itr.first->second.key = itr.first->first;

		TYPE_MOBILE_ADDR_POOL& addrpool =  pool.mobile_addr_pool;
		addrpool.insert(addrpool.begin(),pools.begin(),pools.end());
		auto rt = m_FuzzyTree.AddKey(s_key.str().c_str(),&(itr.first->second));
		if(rt==FALSE)
		{
			//严重错误;
		}
	}
	else
	{
		TYPE_MOBILE_ADDR_POOL& addrpool =  pool.mobile_addr_pool;
		addrpool.clear();
		addrpool.insert(addrpool.begin(),pools.begin(),pools.end());
	}

	pool.runtime_pool.clear();
	auto itr_mobile = pools.begin();
	for(;itr_mobile!= pools.end();)
	{
		_RUNTIME_DATA run_data;
		run_data.mobile_addr = *itr_mobile;
		run_data.count = param.count;
		pool.runtime_pool.push_back(run_data);
		itr_mobile++;
	}

	return 0;
}


int MobileAddrPoolMng::GetPhoneNumber(int smcount,int nettype,std::string key,std::string& outPhoneNumber)
{
	if(this->m_bActivePool==false || this->m_nLocalNetType== nettype)
		return 1;

	AUTO_LOCK(&m_Lock);
	std::stringstream s_key;
	s_key << nettype;
	s_key << "@";
	s_key << key;

	CFuzzyResult result;
	auto  rt = m_FuzzyTree.FuzzySearch(s_key.str().c_str(),&result);
	if(rt)
	{
		auto pos = result.GetHeadPosition();
		while(pos!=NULL)
		{
			HANDLE item = result.GetNext(pos);
			_POOL_* pPool = (_POOL_*)result.GetValue(item);
			if(pPool->mobile_addr_pool.size()==0)
				return -1;
			if(pPool->runtime_pool.size()==0)
				continue;
			auto itr = pPool->runtime_pool.begin();
			outPhoneNumber = itr->mobile_addr;

			if(itr->count == -1)
			{
				return 0;
			}

			itr->count-= smcount;
			if(itr->count<= 0)
			{
				pPool->runtime_pool.pop_front();
			}
			return 0;
		}

		return 1;
	}
	return -1;
}


int MobileAddrPoolMng::LoadConfig()
{
	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("MobileAddrPoolConfig.json","rb");
	if(fp == NULL)
		return FALSE;
	char readBuff[65536];
	rapidjson::FileReadStream is(fp,readBuff,sizeof(readBuff));
	rapidjson::Document reader;
	reader.ParseStream(is);
	fclose(fp);
	if(reader.HasParseError())
	{
		return -1;
	}

	bool active = true;
	if(reader.HasMember("ActivePool"))
	{
		active = reader["ActivePool"].GetBool();	
	}
	m_bActivePool = active;


	int LocalNetType = 0;
	if(reader.HasMember("LocalNetType"))
	{
		LocalNetType = reader["LocalNetType"].GetInt();	
	}
	m_nLocalNetType = LocalNetType;

	int nTimeMinuteToFlush = 120;
	if(reader.HasMember("TimeForResetCount"))
	{
		nTimeMinuteToFlush = reader["TimeForResetCount"].GetInt();	
	}
	m_nCycle_minute = nTimeMinuteToFlush;



	int startpos = 0;
	if(reader.HasMember("ExParamStartPos"))
	{
		startpos = reader["ExParamStartPos"].GetInt();	
	}
	m_nExtParam_start_pos = startpos;

	int exlen = 0;
	if(reader.HasMember("ExParamlen"))
	{
		exlen = reader["ExParamlen"].GetInt();	
	}
	m_nExtParam_len = exlen;

	if(reader.HasMember("NetTypes"));
	{
		m_mobileaddrpool.clear();
		rapidjson::Value& _array = reader["NetTypes"];
		auto itr = _array.Begin();
		while (itr!=_array.End())
		{
			int nettype = 0;
			_NET_PARAM_ param;
			if(itr->HasMember("NetType"))
			{
				nettype = (*itr)["NetType"].GetInt();
				param.nettype = nettype;
			}

			std::string s_des;
			if(itr->HasMember("Description"))
			{
				s_des = (*itr)["Description"].GetString();
				param.description = s_des;
			}

			int nCount = -1;
			if(itr->HasMember("Count"))
			{
				nCount = (*itr)["Count"].GetInt();
				param.count = nCount;
			}
			
			this->m_netparamlist.push_back(param);

			itr++;
		}

	}

	return 0;
}

int MobileAddrPoolMng::SaveConfig()
{
		AUTO_LOCK(&m_Lock);
		FILE* fp = fopen("MobileAddrPoolConfig.json","wb");
		if(fp == NULL)
			return -1;
		char writeBuff[65536];
		rapidjson::FileWriteStream os(fp,writeBuff,sizeof(writeBuff));
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

		writer.StartObject();
		writer.Key("ActivePool");
		writer.Bool(this->m_bActivePool);

		writer.Key("LocalNetType");
		writer.Int(this->m_nLocalNetType);

		writer.Key("TimeForResetCount");
		writer.Int(this->m_nCycle_minute);

		writer.Key("ExParamStartPos");
		writer.Int(this->m_nExtParam_start_pos);

		writer.Key("ExParamlen");
		writer.Int(this->m_nExtParam_len);

		writer.Key("NetTypes");
		writer.StartArray();
		auto itr = this->m_netparamlist.begin();
		for(;itr!=m_netparamlist.end();)
		{
			writer.StartObject();
			writer.Key("NetType");
			writer.Int(itr->nettype);

			writer.Key("Count");
			writer.Int(itr->count);
			
			writer.Key("Description");
			writer.String(itr->description.c_str());

			writer.EndObject();
			itr++;
		}
		writer.EndArray();

		writer.EndObject();		
		fclose(fp);
		return 0;
}
void MobileAddrPoolMng::ClearMobileAddrPool()
{
	AUTO_LOCK(&m_Lock);
	this->m_mobileaddrpool.clear();	
	this->m_spEnableMap.clear();
	this->m_FuzzyTree.RemoveAll();
}


int MobileAddrPoolMng::SetNetParam(int nettype,int limit)
{
	AUTO_LOCK(&m_Lock);
	auto itr = this->m_netparamlist.begin();
	for(;itr!=m_netparamlist.end();)
	{
		if(itr->nettype == nettype)
		{
			itr->count = limit;
			return 0;
		}
		itr++;
	}
	_NET_PARAM_ param;
	param.nettype = nettype;
	param.count = limit;
	param.description ="UNKNOWN";
	m_netparamlist.push_back(param);
	return 1;
}


int MobileAddrPoolMng::GetNetType(LPCTSTR phoneNumber)
{
	AUTO_LOCK(&m_Lock);
	CFuzzyResult result;

	auto rt = m_NetTypeFuzzyTree.FuzzySearch(phoneNumber,&result);
	if(rt)
	{
		auto pos = result.GetHeadPosition();
		if(pos)
		{
			auto item = result.GetNext(pos);
			auto rtv = result.GetValue(item);
			return (int)rtv;
		}
	}
	return 0;
}

int MobileAddrPoolMng::AddKeyForNetType(LPCTSTR phoneNumber ,int nettype)
{
	AUTO_LOCK(&m_Lock);
	auto rt = m_NetTypeFuzzyTree.AddKey(phoneNumber,(HANDLE)nettype);

	if(rt)
	{
		m_AddrToNettypeTable.insert(std::make_pair<std::string,int>(phoneNumber,nettype));
		return 0;
	}
	return -1;
}

int MobileAddrPoolMng::ClearNetTypeTable()
{
		AUTO_LOCK(&m_Lock);
		m_NetTypeFuzzyTree.RemoveAll();
		m_AddrToNettypeTable.clear();
		return 0;
}


int MobileAddrPoolMng::SaveNet2AddrTable()
{
		AUTO_LOCK(&m_Lock);
		FILE* fp = fopen("Addr2NettypeTable.json","wb");
		if(fp == NULL)
			return -1;
		char writeBuff[65536];
		rapidjson::FileWriteStream os(fp,writeBuff,sizeof(writeBuff));
		rapidjson::Writer<rapidjson::FileWriteStream> writer(os);

		writer.StartArray();
		auto itr = m_AddrToNettypeTable.begin();
		for(;itr!=m_AddrToNettypeTable.end();)
		{
			writer.StartObject();
			writer.Key("NetType");
			writer.Int(itr->second);
			
			writer.Key("AddressSeg");
			writer.String(itr->first.c_str());

			writer.EndObject();
			itr++;
		}
		writer.EndArray();
		fclose(fp);
		return 0;

}

int MobileAddrPoolMng::LoadNet2AddrTable()
{
	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("Addr2NettypeTable.json","rb");
	if(fp == NULL)
		return FALSE;
	char readBuff[65536];
	rapidjson::FileReadStream is(fp,readBuff,sizeof(readBuff));
	rapidjson::Document reader;
	reader.ParseStream(is);
	fclose(fp);
	if(reader.HasParseError())
	{
		return -1;
	}
	auto itr_array = reader.Begin();
	for(;itr_array!=reader.End();)
	{
		auto itr_nettype = itr_array->FindMember("NetType");
		auto itr_addr = itr_array->FindMember("AddressSeg");
		if(itr_nettype!=itr_array->MemberEnd()&&itr_addr!=itr_array->MemberEnd())
		{
			AddKeyForNetType(itr_addr->value.GetString(),itr_nettype->value.GetInt());
		}
		itr_array++;
	}
	return 0;
}


int MobileAddrPoolMng::FushLimitCount()
{
	AUTO_LOCK(&m_Lock);
	auto itr = this->m_mobileaddrpool.begin();
	for(;itr!=m_mobileaddrpool.end();)
	{
		int net = atoi(itr->first.c_str());
		int limit = -1;
		auto netparam = m_netparamlist.begin();
		for( ;netparam!= m_netparamlist.end(); )
		{
			if(netparam->nettype == net)
			{
				limit = netparam->count;
				break;
			}
			netparam++;
		}

		itr->second.runtime_pool.clear();
		auto itr_mobile = itr->second.mobile_addr_pool.begin();
		for(;itr->second.mobile_addr_pool.end()!=itr_mobile;)
		{
			_RUNTIME_DATA data;
			data.count = limit;
			data.mobile_addr = itr_mobile->c_str();
			itr->second.runtime_pool.push_back(data);
			itr_mobile++;
		}
		itr++;
	}

	m_lastCheckTime = time(nullptr);
	return 0;
}

bool MobileAddrPoolMng::CheckFlush()
{
	time_t now = time(nullptr);
	if( m_lastCheckTime + m_nCycle_minute*60 < now)
	{
			APP_BEGIN_LOG(5);
			CString s;
			CString snow = asctime(localtime(&now));
			CString slast = asctime(localtime(&m_lastCheckTime));
			s.Format("MobileAddrPoolMng::CheckFlush now[%s] last[%s] cycle[%d]minute start renew",
				(LPCTSTR)snow,
				(LPCTSTR)slast,
				m_nCycle_minute
				);
			APP_DEBUG_LOG((LPCTSTR)s);				
			APP_END_LOG;

		FushLimitCount();
		return true;
	}

	return false;
}


AddrConvertTable::AddrConvertTable()
{
	::InitializeCriticalSection(&m_Lock);
	LoadAddrConvertTable();
	BackupData();

}

int AddrConvertTable::LoadAddrConvertTable()
{
	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("AddrConvertTable.json","rb");
	if(fp == NULL)
		return FALSE;
	char readBuff[65536];
	rapidjson::FileReadStream is(fp,readBuff,sizeof(readBuff));
	rapidjson::Document reader;
	reader.ParseStream(is);
	fclose(fp);
	if(reader.HasParseError())
	{
		return -1;
	}
	if(!reader.IsArray())
	{
		return -1;
	}

	Clear();
	auto Itr_Act = reader.Begin();
	while(Itr_Act!=reader.End())
	{

		auto Itr_Act_Name = Itr_Act->FindMember("AccountName");
		std::string actname;
		if(Itr_Act_Name==Itr_Act->MemberEnd())
		{
			continue;
		}
		if(!Itr_Act_Name->value.IsString())
		{
			continue;
		}
		actname = Itr_Act_Name->value.GetString();

		auto Itr_ConvertTable = Itr_Act->FindMember("ConvertTable");
		if(Itr_ConvertTable==Itr_Act->MemberEnd())
		{
			continue;
		}
		if(!Itr_ConvertTable->value.IsArray())
		{
			continue;
		}

		auto Itr_record = Itr_ConvertTable->value.Begin();
		while(Itr_record != Itr_ConvertTable->value.End())
		{
			auto InOut = Itr_record->FindMember("InOut");
			if(InOut==Itr_record->MemberEnd())
			{
				continue;
			}
			auto DS = Itr_record->FindMember("DestOrg");
			if(DS==Itr_record->MemberEnd())
			{
				continue;
			}
			auto rule = Itr_record->FindMember("Rule");
			if(rule==Itr_record->MemberEnd())
			{
				continue;
			}
			auto replace = Itr_record->FindMember("Replace");
			if(replace==Itr_record->MemberEnd())
			{
				continue;
			}
			
			SetRecord(actname.c_str(),InOut->value.GetString(),DS->value.GetString(),rule->value.GetString(),replace->value.GetString());

			Itr_record++;
		}

		Itr_Act++;
	}

	return 0;
}

int AddrConvertTable::SaveAddrCouvertTable()
{
	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("AddrConvertTable.json","wb");
	if(fp == NULL)
		return -1;
	char writeBuff[65536];
	rapidjson::FileWriteStream os(fp,writeBuff,sizeof(writeBuff));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	writer.StartArray();
	auto itr = this->m_AccountTable.begin();
	while(itr!=m_AccountTable.end())
	{
		writer.StartObject();
		writer.Key("AccountName");
		writer.String(itr->second.accountname.c_str());

		writer.Key("ConvertTable");
		writer.StartArray();
		auto itr_table  = itr->second.table.begin();
		while(itr_table !=itr->second.table.end())
		{
			writer.StartObject();
			std::string tmp;
			writer.Key("InOut");//In Out
			if(itr_table->second.inout=='I')
			{
				writer.String("IN");
			}
			else if(itr_table->second.inout=='O')
			{
				writer.String("OUT");
			}
			else
			{
				tmp.push_back(itr_table->second.inout);
				writer.String(tmp.c_str());
			}

			writer.Key("DestOrg");// DestAddr   OrgAddr
			tmp.clear();
			if(itr_table->second.ds == 'D')
			{
				writer.String("DestAddr");
			}
			else if(itr_table->second.ds == 'O')
			{
				writer.String("OrgAddr");
			}
			else
			{
				tmp.push_back(itr_table->second.ds);
				writer.String(tmp.c_str());
			}

			writer.Key("Rule");
			writer.String(itr_table->second.rule.c_str());

			writer.Key("Replace");
			writer.String(itr_table->second.replace.c_str());
			writer.EndObject();
			itr_table++;
		}
		writer.EndArray();
		writer.EndObject();
		itr++;
	}
	writer.EndArray();
	fclose(fp);
	return 0;
}


AddrConvertTable::~AddrConvertTable()
{
	SaveAddrCouvertTable();
	::DeleteCriticalSection(&m_Lock);
}

int AddrConvertTable::BackupData()
{

	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("AddrConvertTableBack.json","wb");
	if(fp == NULL)
		return -1;
	char writeBuff[65536];
	rapidjson::FileWriteStream os(fp,writeBuff,sizeof(writeBuff));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	writer.StartArray();
	auto itr = this->m_AccountTable.begin();
	while(itr!=m_AccountTable.end())
	{
		auto itr_table  = itr->second.table.begin();
		while(itr_table !=itr->second.table.end())
		{
			writer.StartObject();
			writer.Key("Key");
			writer.String(itr_table->first.c_str());
			writer.Key("AccountName");
			writer.String(itr_table->second.accountname.c_str());
			writer.Key("DestOrOrg");// DestAddr   OrgAddr
			std::string tmp;
			tmp.push_back(itr_table->second.ds);
			writer.String(tmp.c_str());
			writer.Key("IO");//In Out
			tmp.clear();
			tmp.push_back(itr_table->second.inout);
			writer.String(tmp.c_str());

			writer.Key("Rule");
			writer.String(itr_table->second.rule.c_str());

			writer.Key("Replace");
			writer.String(itr_table->second.replace.c_str());


			writer.EndObject();
			itr_table++;
		}
		itr++;
	}
	writer.EndArray();
	fclose(fp);
	return 0;
}


int AddrConvertTable::Convert(LPCTSTR pDstAddr,LPCTSTR pOrgAddr,int InOut,LPCTSTR pActName,std::string& soutDst,std::string& soutOrg)
{
	AUTO_LOCK(&m_Lock);
		CString info;
		info.Format("AddrConvert:dest[%s],org[%s],account[%s],inout[%d]",pDstAddr,pOrgAddr,pActName,InOut);
		GetSmsApp()->WriteTestRecord(info, 5);
	auto itr = m_AccountTable.find(pActName);
	if(itr == m_AccountTable.end())
	{
		//info.Format("AddrConvert:error!,can not find account");
		//GetSmsApp()->WriteTestRecord(info, 5);
		return -1;
	}

	auto& table = itr->second;
	std::string dst;
	if(InOut == 0)
		dst+="O";
	else
		dst+="I";
	dst+="D";
	dst+=pDstAddr;
	{
		CFuzzyResult result;
		table.FuzzyTree.FuzzySearch(dst.c_str(),&result);
		if(result.GetCount()>0)
		{
			auto pos = result.GetHeadPosition();
			auto hItem = result.GetNext(pos);
			ConvertData* pData = (ConvertData*)result.GetValue(hItem);
			if(pData->replace[0]=='P')
			{
				int len = result.GetMatchLen(hItem);
				soutDst=dst.replace(0,len,&pData->replace[1]);
			}
			else if(pData->replace[0]=='A')
			{
				soutDst = &pData->replace[1];
			}
			else
			{
				soutDst = pDstAddr;
				int len = result.GetMatchLen(hItem);
				soutDst=dst.replace(0,len,pData->replace);

			}
		}
		else
		{
			soutDst.clear();
			info.Format("AddrConvert :dest can not find key[%s]",dst.c_str());
				GetSmsApp()->WriteTestRecord(info, 5);
			
		}
	}


	std::string src;
	if(InOut == 0)
		src+="O";
	else
		src+="I";
	src+="O";
	src+=pOrgAddr;
	CFuzzyResult result;
	table.FuzzyTree.FuzzySearch(src.c_str(),&result);
	if(result.GetCount()>0)
	{
		auto pos = result.GetHeadPosition();
		auto hItem = result.GetNext(pos);
		ConvertData* pData = (ConvertData*)result.GetValue(hItem);
		if(pData->replace[0]=='P')
		{
			int len = result.GetMatchLen(hItem);
			soutOrg=src.replace(0,len,&pData->replace[1]);
		}
		else if(pData->replace[0]=='A')
		{
			soutOrg = &pData->replace[1];
		}
		else
		{
			int len = result.GetMatchLen(hItem);
			soutOrg= src.replace(0,len,pData->replace);
		}
	}
	else
	{
		soutOrg.clear();
		info.Format("AddrConvert :org can not find key[%s]",src.c_str());
				GetSmsApp()->WriteTestRecord(info, 5);
	}
	info.Format("AddrConvert do:repdest[%s],reporg[%s]",soutDst.c_str(),soutOrg.c_str());
	GetSmsApp()->WriteTestRecord(info, 5);
	return 0;
}


int AddrConvertTable::SetRecord(LPCTSTR pActName,LPCTSTR pInOut,LPCTSTR pDO,LPCTSTR pRule,LPCTSTR pReplace)
{
	auto data = std::make_pair<std::string,AccountData>(std::string(pActName),AccountData());

	auto _ret = this->m_AccountTable.insert(data);

	{
		char IO = 0;
		char DO = 0;
		_ret.first->second.accountname = pActName;
		std::string key;
		if(stricmp(pInOut,"IN")==0)
		{
			key.push_back('I');
			IO='I';
		}
		else if (stricmp(pInOut,"OUT")==0)
		{
			key.push_back('O');
			IO='O';
		}
		else
		{
			//error value
			return -2;
		}
		
		if(stricmp(pDO,"DestAddr")==0)
		{
			key.push_back('D');
			DO = 'D';
		}
		else if(stricmp(pDO,"OrgAddr")==0)
		{
			key.push_back('O');
			DO = 'O';
		}
		else
		{
			//error value
			return -3;
		}
		
		key+=pRule;

		auto record = std::make_pair<std::string,ConvertData>(key,ConvertData());
		auto _ret2 = _ret.first->second.table.insert(record);
		if(_ret2.second)
		{
			ConvertData& Data = _ret2.first->second;
			_ret2.first->second.accountname = pActName;
			_ret2.first->second.inout = IO;
			_ret2.first->second.ds =DO;
			_ret2.first->second.rule = pRule;
			_ret2.first->second.replace = pReplace;
			auto ret_add = _ret.first->second.FuzzyTree.AddKey(key.c_str(),&Data);
			if(!ret_add)
				return -4;
		}
		else
		{
			ConvertData& Data = _ret2.first->second;
			_ret2.first->second.accountname = pActName;
			_ret2.first->second.inout = IO;
			_ret2.first->second.ds =DO;
			_ret2.first->second.rule = pRule;
			_ret2.first->second.replace = pReplace;
			auto ret_delete = _ret.first->second.FuzzyTree.DeleteKey(key.c_str());
			auto ret_add = _ret.first->second.FuzzyTree.AddKey(key.c_str(),&Data);
			if(!ret_delete||!ret_add)
				return -5;
		}
	}
	return 0;
}

void AddrConvertTable::Clear()
{
	m_AccountTable.clear();
}


int AddrConvertTable::SetTable(rapidjson::Value& reader,std::string& error)
{
	AUTO_LOCK(&m_Lock);
	if(!reader.IsArray())
	{
		error = "format error not []";
		return -1;
	}

	Clear();
	auto Itr_Act = reader.Begin();
	while(Itr_Act!=reader.End())
	{

		auto Itr_Act_Name = Itr_Act->FindMember("AccountName");
		std::string actname;
		if(Itr_Act_Name==Itr_Act->MemberEnd())
		{	error += "[AccountName not Found]";	
			continue;
		}
		if(!Itr_Act_Name->value.IsString())
		{
			error += "[AccountName is not String]";
			continue;
		}
		actname = Itr_Act_Name->value.GetString();

		auto Itr_ConvertTable = Itr_Act->FindMember("ConvertTable");
		if(Itr_ConvertTable==Itr_Act->MemberEnd())
		{
			error += "[ConvertTable not Found]";
			continue;
		}
		if(!Itr_ConvertTable->value.IsArray())
		{
			error += "[ConvertTable is not Array]";
			continue;
		}

		auto Itr_record = Itr_ConvertTable->value.Begin();
		while(Itr_record != Itr_ConvertTable->value.End())
		{
			auto InOut = Itr_record->FindMember("InOut");
			if(InOut==Itr_record->MemberEnd())
			{
				error += "[InOut is not Found]";
				continue;
			}
			auto DS = Itr_record->FindMember("DestOrg");
			if(DS==Itr_record->MemberEnd())
			{
				error += "[DestOrg is not Found]";
				continue;
			}
			auto rule = Itr_record->FindMember("Rule");
			if(rule==Itr_record->MemberEnd())
			{
				error += "[Rule is not Found]";
				continue;
			}
			auto replace = Itr_record->FindMember("Replace");
			if(replace==Itr_record->MemberEnd())
			{
				error += "[Replace is not Found]";
				continue;
			}
			
			auto rt = SetRecord(actname.c_str(),InOut->value.GetString(),DS->value.GetString(),rule->value.GetString(),replace->value.GetString());

			if(rt!=0)
			{
				CString estr;
				estr.Format("[erro=(%d) accountname(%s) InOut(%s) DestOrg(%s) Rule(%s) Replace(%s)]",rt,actname.c_str(),
					InOut->value.GetString(),
					DS->value.GetString(),
					rule->value.GetString(),
					replace->value.GetString()
					);
				error+=estr;
			}
			Itr_record++;
		}

		Itr_Act++;
	}
	SaveAddrCouvertTable();
	BackupData();
	return 0;

}




DcsConvertTable::DcsConvertTable()
{
	::InitializeCriticalSection(&m_Lock);
	LoadDcsConvertTable();

}

DcsConvertTable::~DcsConvertTable()
{
	SaveDcsConvertTable();
	::DeleteCriticalSection(&m_Lock);
}

int DcsConvertTable::IsConvert(LPCTSTR pActName)
{
	AUTO_LOCK(&m_Lock);
	auto itr_ret = m_AccountTable.find(pActName);
	if(itr_ret!=m_AccountTable.end())
	{
		return itr_ret->second;
	}
	return -1;
}

int DcsConvertTable::LoadDcsConvertTable()
{
	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("DcsConvertTable.json","rb");
	if(fp == NULL)
		return FALSE;
	char readBuff[65536];
	rapidjson::FileReadStream is(fp,readBuff,sizeof(readBuff));
	rapidjson::Document reader;
	reader.ParseStream(is);
	fclose(fp);
	if(reader.HasParseError())
	{
		return -1;
	}
	if(!reader.IsArray())
	{
		return -1;
	}

	auto Itr_Act = reader.Begin();
	while(Itr_Act!=reader.End())
	{

		auto Itr_Act_Name = Itr_Act->FindMember("AccountName");
		std::string actname;
		if(Itr_Act_Name==Itr_Act->MemberEnd())
		{
			continue;
		}
		if(!Itr_Act_Name->value.IsString())
		{
			continue;
		}
		actname = Itr_Act_Name->value.GetString();

		auto Itr_convert = Itr_Act->FindMember("Convert");
		if(Itr_convert==Itr_Act->MemberEnd())
		{
			continue;
		}
		if(!Itr_convert->value.IsInt())
		{
			continue;
		}

		auto item = std::make_pair<std::string,int>(std::string(Itr_Act_Name->value.GetString()),Itr_convert->value.GetInt());
		auto insert_ret = this->m_AccountTable.insert(item);
		if(insert_ret.second == false)
		{
		}
		Itr_Act++;
	}

	return 0;

}
	
int DcsConvertTable::SaveDcsConvertTable()
{
	AUTO_LOCK(&m_Lock);
	FILE* fp = fopen("DcsConvertTable.json","wb");
	if(fp == NULL)
		return -1;
	char writeBuff[65536];
	rapidjson::FileWriteStream os(fp,writeBuff,sizeof(writeBuff));
	rapidjson::Writer<rapidjson::FileWriteStream> writer(os);
	writer.StartArray();
	auto itr = this->m_AccountTable.begin();
	while(itr!=m_AccountTable.end())
	{
		writer.StartObject();
		writer.Key("AccountName");
		writer.String(itr->first.c_str());

		writer.Key("Convert");
		writer.Int(itr->second);
		writer.EndObject();
		itr++;
	}
	writer.EndArray();
	fclose(fp);
	return 0;

}



bool MobileAddrPoolMng::IsSPAddrPoolEnable(LPCTSTR pSPID)
{
	AUTO_LOCK(&m_Lock);
	auto itr = m_spEnableMap.find(pSPID);

	if(itr!=m_spEnableMap.end())
	{
		return itr->second;
	}

	return false;
}

bool MobileAddrPoolMng::SetSPAddrPoolEnable(LPCTSTR pSPID,bool bEnable)
{
	AUTO_LOCK(&m_Lock);
	auto rt = m_spEnableMap.insert(std::make_pair<std::string,bool>(std::string(pSPID),bEnable));

	if(rt.second == false)
	{
		rt.first->second = bEnable;
	}
	return true;
}
