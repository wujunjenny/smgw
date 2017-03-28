#pragma once
#include "log.h"
#include <unordered_map>
#include <string>
#include <memory>
#include <sstream>
#include <list>
#include <vector>
#include <ctime>
#include <tuple>
#include <cassert>

namespace LONGSM
{
	using namespace std;
#define MAX_SUBSM_COUNT	30

#ifndef FIELD_OFFSET
	#define FIELD_OFFSET(type,field) ((long)(int*)&(((type*)0)->field))
#endif
	const int max_sm_size = 140;
	const bool max_sign = false;
	template <typename T>
	class release_deleter {
	public:
		release_deleter():_brelease(false){};
		void release() { _brelease = true; };
		void operator()(T* ptr){if(!_brelease) delete ptr;};
	protected:
		bool _brelease;
	};


	template <class T>
	T*  release(shared_ptr<T> shptr){  auto ret = shptr.get();

										auto deletor =  get_deleter<release_deleter<T>>(shptr);
										deletor ->release();
										return ret; };


#pragma pack(push,1)
	struct sub_sm_header_tag
	{
		unsigned char tag_type;
		unsigned char tag_len;
		unsigned char _tag_value;
		const char* getvalue(){return (const char*)&_tag_value;};
		int getvaluelen(){ return tag_len;};
		int gettotallen() { return tag_len+FIELD_OFFSET(sub_sm_header_tag,_tag_value);};
		static int getbaseLen(){ return FIELD_OFFSET(sub_sm_header_tag,_tag_value);};
	};
	struct sub_long_sm_tag
	{
		unsigned char tag_ref;
		unsigned char tag_total;
		unsigned char tag_index;		
		static int gettaglen(){return sizeof(sub_long_sm_tag);};
	};
#pragma pack(pop)

	int GetLongSMInfo(const void* pcontent,int& ref,int& index,int& total);
	int addsigntail(vector<char> &value,bool bs,const char* psign,int signlen);
	
	//insert dest At index whith vector s ,cut off at smmax,put left at lf
	int insertAt(vector<char> &dest,int index,int smmax,vector<char> &s,vector<char>& lf);


	template<class _CSM>
	class Clongsmdata :public _CSM
	{
	public:
		Clongsmdata():bvalid(false),_ref(0),_total(0),_nrcv(0){ VLOG(5)<<"Clongsmdata():["<<this<<"]";};
		
		Clongsmdata(sm::gw_shortmsg* ppb):_CSM(ppb){
			VLOG(5)<<"Clongsmdata():["<<this<<"] make by gw_shortmsg = "<<ppb;
			auto &map = ppb->subsms().subcontents();
			for(auto itr = map.begin();itr!=map.end();itr++)
			{
				if(itr==map.begin())
				{
					makelongsm(itr->second.data(),itr->second.size(),itr->first.c_str());
				}
				else
				{
					combinesubsm(itr->second.data(),itr->second.size(),itr->first.c_str());
				}
			}
		};
		
		Clongsmdata(_CSM* pCopy):_CSM(pCopy),bvalid(false),_ref(0),_total(0),_nrcv(0){  VLOG(5)<<"Clongsmdata(_CSM* pCopy):["<<this<<"]["<<pCopy<<"]";};
		virtual ~Clongsmdata() { VLOG(5)<<"~Clongsmdata():"<<this;};
		int makelongsm(void* pcontent,int len,const char* msgid);//delay construct long smdata
		int combinesubsm(void* pcontent,int len,const char* msgid);
		vector<pair<vector<char>,string>>& getsubcontents() { return subcontents;};
		vector<char> getalldata(){ 
			vector<char> result;
			for(int i=0;i<(int)subcontents.size();i++)
			{
				int headlen = subcontents[i].first[0]+1;
				int subsize = subcontents[i].first.size();
				result.insert(result.end(),&subcontents[i].first[headlen],&subcontents[i].first[0]+subsize);
			}
			return move(result);
		};

		vector<_CSM*> GetAllSubSM();
		bool iscomplete(){ return (_total==_nrcv && _total>0);};
		int getsubcount(){ return _nrcv;};
		virtual wstring GetAllContentNW() override { 
			auto v = getalldata();
			auto dcs = GetMsgFormat();
			if((dcs&0x0C)==0x08)
			{
				CCodeAndDecode code;
				wstring s;
				s.resize(v.size()/2);
				memcpy(&s[0],&v[0],v.size());
				return s;
			}
			CCodeAndDecode code;
			string s(&v[0],v.size());
			return code.ConvertWStringToNWString(code.ConvertStringToWString(s));
		};
		virtual int GetSubMsgCount() override { return _nrcv;};
		virtual int AddSign(int head,const char* psign ,int len) override;
		virtual sm::gw_shortmsg GetPBPack() override
		{
			sm::gw_shortmsg pb = _CSM::GetPBPack();
			auto psms = pb.mutable_subsms();
			auto psubcontents = psms->mutable_subcontents();
			for(auto itr = subcontents.begin();itr != subcontents.end();itr++)
			{
				//psubcontents->insert(make_pair(itr->second,string(itr->first.begin(),itr->first.end())));
				auto& v = psubcontents->at(itr->second);
				v.replace(v.begin(),v.end(),itr->first.begin(),itr->first.end());
			}
			return move(pb);
		}
	protected:
		bool bvalid;
		int _ref;
		int _total;
		int _nrcv;
		//int _index;
		vector<pair<vector<char>,string>> subcontents;
	};

	template<class _CSM>
	int Clongsmdata<_CSM>::makelongsm(void* pcontent,int len,const char* msgid)
	{
		//int ref = 0,index=0,total=0;
		int index;
		if(auto rt = GetLongSMInfo(pcontent,_ref,index,_total)<0)
		{
			//no long msg tag
			LOG(WARNING) << "GetLongSMInfo error ="<< rt ;
			return -1;
		}

		if(_total <= 1 || index > _total)
		{
			LOG(WARNING) << "GetLongSMInfo error total =" << _total <<";index = "<<index;
			return -2;
		}
		VLOG(5)<<"make first Lsm smid="<<msgid
				<<"; orgaddr="<<GetOrgAddr()
				<<"; destaddr="<<GetDestAddr()
				<<"; ref="<<_ref
				<<"; index="<<index
				<<"; total="<<_total;
		subcontents.resize(_total);
		subcontents[index-1]=make_pair<vector<char>,string>(vector<char>((char*)pcontent,(char*)pcontent+len),msgid);
		_nrcv = 1;
		m_bReturnFrist = true;
		//m_bFromHttp = true;
		//GetSMData()->SMLength = 0;
		return 0;
	}

	template<class _CSM>
	int  Clongsmdata<_CSM>::combinesubsm(void* pcontent,int len,const char* msgid)
	{

		int ref = 0,index=0,total=0;
		if(auto rt= GetLongSMInfo(pcontent,ref,index,total)<0)
		{
			//no long msg tag
			LOG(WARNING) << "GetLongSMInfo error ="<< rt <<" id=["<<msgid<<"]";
			return -1;
		}

		if(ref!=_ref||total!=_total||index>total||index < 1)
		{
			LOG(WARNING) << "GetLongSMInfo error total =" << _total <<";index = "<<index<<" id=["<<msgid<<"]";
			//erro long head info no match
			return -2;
		}

		VLOG(6)<<" total = "<<total
			   <<"; ref = "<<ref
			   <<"; _total = "<<_total
			   <<"; index = "<<index;
			   			   			   			    
		if(subcontents[index-1].first.size())
		{
			LOG(WARNING) << "combinesubsm error duplicate index = "<<index<<" id=["<<msgid<<"]";
			return -3;
		}

		subcontents[index-1]=make_pair<vector<char>,string>(vector<char>((char*)pcontent,(char*)pcontent+len),msgid);
		_nrcv++;

		if(_nrcv==_total)
		{
			VLOG(5) << "combinesubsm complete ok total = "<<_total 
					<< ";  ref="<<_ref 
					<<"; destaddr = "<< GetDestAddr() 
					<<"; orgaddr="<<GetOrgAddr()<<";"
					<<" id=["<<msgid<<"]";

			return 1;
		}
		return 0;
	};

	template<class _CSM>
	vector<_CSM*> Clongsmdata<_CSM>::GetAllSubSM()
	{
		vector<_CSM*> v;
		v.resize(subcontents.size());
		for(int i=0;i<_total;i++)
		{
			if(subcontents[i].first.size()==0)
			{
				v[i]=nullptr;
				continue;
			}
			v[i] = new _CSM(this);
			v[i]->SetSourceID((char*)subcontents[i].second.c_str());
			v[i]->SetSmid((char*)subcontents[i].second.c_str());
			memcpy(v[i]->GetSMData()->SMUD,&subcontents[i].first[0],subcontents[i].first.size());
			v[i]->GetSMData()->SMLength = subcontents[i].first.size();
		}
		return std::move(v);
	}

	template<class _CSM>
	int Clongsmdata<_CSM>::AddSign(int header,const char* psign ,int len)
	{
		vector<char> vsign(psign,psign+len);
		auto dcs = GetMsgFormat();
		if((dcs&0x0C)==8)
		{//unicode
			CCodeAndDecode code;
			std::wstring wzSign = code.ConvertStringToWString(std::string(psign,len));
			std::wstring nwzSign = code.ConvertWStringToNWString(wzSign);				
			vsign.resize(nwzSign.size()*2);
			memcpy(&vsign[0],&nwzSign[0],vsign.size());
		}



		vector<char> v_lef;
		if(header == 0) //add to tail
		{
			VLOG(5)<<"add sign to tail ["<< string(psign,len)<<"]" ;
			assert(subcontents.size()==_total);
			auto& tail = subcontents[_total-1];
			if(tail.first.size()==0)
			{
				LOG(WARNING)<<"long sm tail subconent is null add fail msgid=["<<tail.second<<"]";
				return -1;
			}
			
			return addsigntail(tail.first,max_sign,&vsign[0],vsign.size());

		}
		else
		{
			VLOG(5)<<"add sign to head ["<< string(psign,len)<<"]";
			if(_nrcv!=_total || _total < 1)
			{
				LOG(ERROR)<<"add sign to long sm is not a complete sm ";
				return -1;
			}
			vector<char> s = std::move(vsign);
			for(int i = 0;i< subcontents.size();i++)
			{
				vector<char> &v = subcontents[i].first;
				if(v.size()==0||v.size()<v[0])
				{
					LOG(ERROR)<<"add sign to long sm is not a complete sm ";
					return -1;
				}
				vector<char> l;
				if(auto rt = insertAt(v,v[0]+1,max_sm_size,s,l)!=0)
				{

					LOG(ERROR)<<"add sign to long sm  erro = "<<rt;
					return rt;
				}
				s=std::move(l);
				VLOG(5)<<"add signed to long sm index = "<<i+1<<" left="<<s.size();
				if(s.size()==0)//leftsize ==0 not need to shift
					return 0;
			}
		}
		return 0;
	}



	template<class _CSM>
	class CLongSMTable
	{
	public:
		CLongSMTable(void) { m_nsecond_timeout = 1000; };
		~CLongSMTable(void) {};
		typedef list<Clongsmdata<_CSM> *> smlist;
		typedef Clongsmdata<_CSM> LSM;
		int CacheSubSM(_CSM* pSubSM);
		int GetBadSM(smlist& outlist);
		int GetCompleteList(smlist& outlist);
		LSM* GetCompleteSM(){ 
			if(m_complete_list.size()==0) return nullptr;
			auto itr = m_complete_list.begin(); 
			auto ret = itr->release();
			m_complete_list.pop_front();
			return ret;
		};
		int GetTimeOut(){ return m_nsecond_timeout;};
		void SetTimeOut(int timesec){ m_nsecond_timeout = timesec;};
	protected:

		typedef unique_ptr<Clongsmdata<_CSM>> _smptr;
		typedef shared_ptr<_smptr> _sh_ptr;
		typedef weak_ptr<_smptr> _wk_ptr;
		typedef unordered_map<string,_sh_ptr> _map_type;
		typedef typename _map_type::const_iterator _map_iterator;
		
		typedef tuple<time_t,_wk_ptr,_map_iterator> _tuple;
		typedef pair<time_t,_map_iterator> _pair2;

		//list<_tuple> _timeout_list;
		list<pair<_wk_ptr,_pair2>> _timeout_list;
		list<_smptr> m_complete_list;	
		unordered_map<string,_sh_ptr> m_sm_map;
		//list<pair<_wk_ptr,time_t>> m_timeout_list;

		int m_nsecond_timeout;
	};

	template<class _CSM>
	int CLongSMTable<_CSM>::CacheSubSM(_CSM* pSubSM)
	{
		
		char* pcontent = pSubSM->GetMsgContent();
		int len = pSubSM->GetSMLength();
		if(len < 1+sub_sm_header_tag::getbaseLen()+sub_long_sm_tag::gettaglen())
		{
			//erro len of smhead
			LOG(WARNING)<<"error smhead len= "<<len;
			return -1;
		}

		int headlen = (unsigned char)pcontent[0];

		if(len < headlen)
		{
			//smlen <smheadlen
			LOG(WARNING)<<"error smhead headlen= "<< headlen
						<<"smlen = "<<len;
			return -11;			
		}


		int ref = 0,index=0,total=0;
		if(auto rt = GetLongSMInfo(pcontent,ref,index,total)<0)
		{
			//no long msg tag
			LOG(WARNING)<<"error smhead has no LSM tag rt= " << rt;
			return -2;
		}
		//add for test 
		//ref = ref%2 ? ref :index;

		if( index <= 0 ||index > total || total > MAX_SUBSM_COUNT  )
		{
			LOG(WARNING)<<"error  LSM tag  index=" << index
						<<";total="<<total<<";";
			//error index or total
			return -3;
		}
		
		stringstream key;
		key << pSubSM->GetOrgAddr() <<"."<<pSubSM->GetDestAddr()<<"."<<ref;	
		auto rt = m_sm_map.emplace(make_pair(key.str(),nullptr));
		if(rt.second)
		{
			VLOG(5)<<"new lsm key=["<<key.str()<<"]";
			_smptr  pNew (new Clongsmdata<_CSM>(pSubSM));
			rt.first->second = make_shared<_smptr> (std::move(pNew));

			auto ret = (*rt.first->second)->makelongsm(pcontent,len,pSubSM->GetSmid());
			if(ret ==0)
			{
				VLOG(5)<<"add timeout ["<<key.str()<<"]";
				//make_tuple<_tuple>(time(nullptr),rt.first->second,rt.first);
				_timeout_list.push_back( make_pair(rt.first->second,make_pair(time(nullptr),rt.first)));

			}
			return ret;
			
		}
		else
		{
			VLOG(5)<<"find lsm key=["<<key.str()<<"] combine it";
			//has rcv lsm,this is a part of lsm . save content with 
			auto sm_sh_ptr = rt.first->second;
			auto ret = (*sm_sh_ptr)->combinesubsm(pcontent,len,pSubSM->GetSmid());

			if(ret==1)
			{
				VLOG(5)<<"combine complete  lsm key=["<<key.str()<<"] put to ok list";
				
				m_complete_list.push_back(move(*sm_sh_ptr));
				m_sm_map.erase(rt.first);
			}
			return ret;
		}		
		return 0;
	};

	template<class _CSM>
	int  CLongSMTable<_CSM>::GetCompleteList(smlist& outlist)
	{

		for(auto itr=m_complete_list.begin();itr!=m_complete_list.end();)
		{
			//m_complete_list.swap(outlist);
			outlist.push_back(itr->release());
			itr = m_complete_list.erase(itr);
		}

		return 0;
	}

	template<class _CSM>
	int  CLongSMTable<_CSM>::GetBadSM(smlist& outlist)
	{
		auto itr = _timeout_list.begin();
		for(;itr!= _timeout_list.end();)
		{
			
			auto pt = itr->first.lock();
			if(pt==nullptr || (*pt)->iscomplete())
			{
				itr = _timeout_list.erase(itr);
				continue;
			}
			else
			{
				auto now = time(nullptr);
				if(now - itr->second.first > m_nsecond_timeout)
				{
					VLOG(5)<<"timeout key=["<<itr->second.second->first<<"]  mapsize = " << m_sm_map.size();
					m_sm_map.erase(itr->second.second);
					itr = _timeout_list.erase(itr);
					outlist.push_back(pt->release());
					continue;
				}
				else
				{
					break;
				}
			}

		}
		return 0;
	}


    inline int GetLongSMInfo(const void* pcontent,int& ref,int& index,int& total)
	{
		unsigned char* pheads = (unsigned char*)pcontent;
		int headlen =  ((unsigned char*)pcontent)[0];
		int tagbegin = 1;

		while(tagbegin < headlen)
		{
			sub_sm_header_tag* ptag = (sub_sm_header_tag*)&pheads[tagbegin];
			if(ptag->tag_type==0)
			{

				if(ptag->tag_len != sub_long_sm_tag::gettaglen())
				{
					return -2;
				}

				sub_long_sm_tag* pinfo = (sub_long_sm_tag*)ptag->getvalue();

				ref = pinfo->tag_ref;
				total = pinfo->tag_total;
				index = pinfo->tag_index;
				//ok
				return 0;
			}
			tagbegin+=ptag->gettotallen();
		}

		return -1;
	}


	inline int addsigntail(vector<char> &value,bool bs,const char* psign,int signlen)
	{
		if(signlen > max_sm_size || signlen <0)
		{
			LOG(ERROR)<<"signlen > max_sm_len  ["<<signlen <<"]["<< max_sm_size<<"]";
			return -1;
		}
		int sz = value.size();
		if(sz==0)
		{
			LOG(ERROR)<<"subsm len ==0 is not a complete sm";
			return -2;
		}
		if(sz+signlen>max_sm_size)
		{
			if(bs)//save sign an cut off content
			{
				value.resize(max_sm_size);
				memcpy(&value[max_sm_size-signlen],psign,signlen);
			}
			else
			{
				int cp = max_sm_size -sz;
				value.resize(max_sm_size);
				if(cp>0)
				{
					memcpy(&value[sz],psign,cp);
				}
			}

		}
		else
		{
			VLOG(7)<<"add signed   orgsize= "<<value.size()<<"; signlen="<<signlen;
			int tail = value.size();
			value.resize(tail+signlen);
			memcpy(&value[tail],psign,signlen);
		}
		return 0;
	}
	
	//insert dest At index whith vector s ,cut off at smmax,put left at lf
	inline int insertAt(vector<char> &dest,int index,int smmax,vector<char> &s,vector<char>& lf)
	{
		if(dest.size() < index)
		{
			LOG(ERROR)<<"not a complete sm size < index ["<<dest.size()<<"]["<<index<<"]";
			return -1;
		}

		dest.insert(dest.begin()+index,s.begin(),s.end());
		if(dest.size()>smmax)
		{
			lf.insert(lf.begin(),dest.begin()+smmax,dest.end());
			dest.resize(smmax);
		}
		return 0;
	}


}


