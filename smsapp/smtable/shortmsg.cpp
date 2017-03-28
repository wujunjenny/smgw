#include "stdafx.h"
#include "Smcmacro.h"
#include "smppdef.h"
#include "shortmsg.h"
#include "CmdDef.h"
#include "SmsApp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//#define MAX_EXT_DATA_LEN  (MAX_SMS_SM_LEN-sizeof(smpp_Message_header)-sizeof(smpp_Submit_Sm))
int  extern  G_APP_STATUS_REPORT_SUPPORT;

//extern void WriteSysErrorLog(char *pStr);

#define COPY_FROM_STRING(dest,str)	\
	do{  if(str.size())strncpy(dest,str.data(),min(str.size(),sizeof(dest)-1));else dest[0]=0;}while(0)


#include "SmppAndInnerTrans.h"

//#define __DEBUG_SHORTMSG__

typedef struct tag_ShortMsg
{
	//内部号码的备份：为号码出变换备份
	char m_CalledNo[MAX_MSISDN_LEN + 1];
	char m_CallingNo[MAX_MSISDN_LEN + 1];
	char m_FeeCallNo[MAX_MSISDN_LEN + 1];
	//消息的提交时间，以毫秒保存
	DWORD m_nSubmitTime;
	//当前处理的步骤
	int m_DealStep;	
	//发送接口ID
	UL m_ulSenderID;
	//接收接口ID
	UL m_ulRecverID;
	// 消息的序列号
	UL m_ulSequenceID;
	// 消息的命令ID
	UL m_ulCmdID;
	//消息发送时的命令ID
	UL	m_SendCmdID;
	//重发次数
	int m_nReSubmitCount;
	//是否是状态报告消息
	//UC m_IsReport;
	//消息结构
	SM_STRUCT m_SMData;
	//消息是否发送的标记
	UC m_SendMsgFlag;
	//Tlv数据总长度
	DWORD m_nExtDataSize;
	//Tlv数据
	char m_ExtData[STREAM_LEN_MAX + 1]; 
}SHORTMSG;

class CTlv_Ref
{
public:
	CTlv_Ref(CTLV* pTlv) { m_pTlv = pTlv;};
	inline	CTLV* GetTlv() { return m_pTlv;};
	inline void SetTlv(CTLV* pNewTlv) { m_pTlv = pNewTlv;};
protected:
	CTLV* m_pTlv;
};

class char_ptr_out
{
public:
	char_ptr_out(const char* buff,int sz)
	{
		m_pBuff = (char*)buff;
		m_size = sz;
	};
	const char* data() { return m_pBuff;};
	int size(){ return m_size; };
protected:
	char* m_pBuff;
	int m_size;
};

class char_ptr_in
{
public:
	char_ptr_in(char* buff,int sz)
	{
		m_pBuff = buff;
		m_size = sz;
		m_write_pos = 0;
	};
	inline	char* write_data() { return m_pBuff+m_write_pos;};
	inline	char* data() { return m_pBuff;};
	inline  int write_size(int sz){ m_write_pos+=sz; return m_write_pos; };
	inline  int buffsize() { return m_size; };
	inline  int datasize() { return m_write_pos;};
protected:
	char* m_pBuff;
	int m_write_pos;
	int m_size;
};


class lit_stream
{
public:
	enum
	{
		stream_out = 0,
		stream_in = 1
	};

	lit_stream(char* buff,int size,int in_out)
	{
		buffsize = 0;
		rd_pos = 0;
		wr_pos = 0;
		auto pt = GetWriterPtr(size);
		memcpy(pt,buff,size);
		wr_pos = size;
	};
		
	lit_stream(int in_out)
	{
		buffsize = 0;
		rd_pos = 0;
		wr_pos = 0;
	};
	lit_stream& operator<< (int value);
	lit_stream& operator<< (bool value);
	lit_stream& operator<< (DWORD value);
	lit_stream& operator<< (char value);
	lit_stream& operator<< (unsigned char value);
	lit_stream& operator<< (char_ptr_out value);
	lit_stream& operator<< (CTlv_Ref value);
	lit_stream& operator<< (const std::string value);

	int streamout_block (char* pData,int size);
	inline	int GetWriteSize() { return wr_pos;};
	
	lit_stream& operator>> (int& value);
	lit_stream& operator>> (DWORD& value);
	lit_stream& operator>> (bool& value);
	lit_stream& operator>> (char& value);
	lit_stream& operator>> (unsigned char& value);
	lit_stream& operator>> (std::string& value);
	lit_stream& operator>> (char_ptr_in &value);
	lit_stream& operator>> (CTlv_Ref& value);
	int streamin_block (char* pData,int size);
	inline  int GetReadSize() { return rd_pos;};

	char* GetDataClone(int& len);
	const char* GetData(int& len) { len = wr_pos-rd_pos; return &m_data[rd_pos];};
protected:
	//in
	inline	char* GetReadPtr() { return &m_data[rd_pos];};
	//out
	inline	char* GetWriterPtr(int _size = 0) 
	{
		while(wr_pos+_size > m_data.size())
		{
			m_data.resize(m_data.size()+4096);
		}
		return &m_data[wr_pos];
	};
	std::vector<char> m_data;
	int buffsize;
	int rd_pos;
	int wr_pos;
};

lit_stream& lit_stream::operator<< (int value)
{
	int* ref = (int*)GetWriterPtr(sizeof(int));
	*ref = htonl(value); 
	wr_pos+=sizeof(int);
	return *this;
}


lit_stream& lit_stream::operator>> (int &value)
{
	int* ref = (int*)GetReadPtr();
	value = ntohl(*ref);
	rd_pos+=sizeof(int);
	return *this;
}


lit_stream& lit_stream::operator<< (bool value)
{
	char* ref = GetWriterPtr(sizeof(char));
	*ref = value; 
	wr_pos+=sizeof(char);
	return *this;
}


lit_stream& lit_stream::operator>> (bool &value)
{
	char* ref = GetReadPtr();
	value = *ref; 
	rd_pos+=sizeof(char);
	return *this;
}

lit_stream& lit_stream::operator<< (char value)
{
	char* ref = GetWriterPtr(sizeof(char));
	*ref = value; 
	wr_pos+=sizeof(char);
	return *this;
}

lit_stream& lit_stream::operator>> (char &value)
{
	char* ref = GetReadPtr();
	value = *ref ; 
	rd_pos+=sizeof(char);
	return *this;
}


lit_stream& lit_stream::operator<< (unsigned char value)
{
	char* ref = GetWriterPtr(sizeof(char));
	*ref = value; 
	wr_pos+=sizeof(char);
	return *this;
}


lit_stream& lit_stream::operator>> (unsigned char &value)
{
	char* ref = GetReadPtr();
	value = *ref; 
	rd_pos+=sizeof(char);
	return *this;
}


lit_stream& lit_stream::operator<< (DWORD value)
{
	DWORD* ref = (DWORD*)GetWriterPtr(sizeof(DWORD));
	*ref = htonl(value); 
	wr_pos+=sizeof(DWORD);
	return *this;
}

lit_stream& lit_stream::operator>> (DWORD &value)
{
	DWORD* ref = (DWORD*)GetReadPtr();
	value = ntohl(*ref); 
	rd_pos+=sizeof(DWORD);
	return *this;
}


lit_stream& lit_stream::operator<< (char_ptr_out value)
{
	*this << value.size();
	memcpy(GetWriterPtr(value.size()),value.data(),value.size());
	wr_pos+=value.size();
	return *this;
}

lit_stream& lit_stream::operator>> (char_ptr_in &value)
{
	int sz;
	*this >> sz;
	memcpy(value.data(),GetReadPtr(),min(value.buffsize(),sz));

	value.write_size(sz); 
	rd_pos+=sz;
	return *this;
}


lit_stream& lit_stream::operator<< (CTlv_Ref value)
{

	DWORD size;
	
	size = value.GetTlv() ? value.GetTlv()->GetTLVLength():0;

	*this << size;

	if(size)
	{
		char* pData = GetWriterPtr(size);
		size = 0;
		value.GetTlv()->ToStream((BYTE*)pData,size);
		wr_pos+=size;
	}

	return *this;
}


lit_stream& lit_stream::operator>> (CTlv_Ref &value)
{
	DWORD size,size1=0;

	*this >> size;
	if(value.GetTlv()==nullptr)
	{
		value.SetTlv(new CTLV);
	}

	char* pData = GetReadPtr();
	value.GetTlv()->FromStream((BYTE*)pData,size,size1);
	ASSERT(size==size1);
	rd_pos+=size;
	return *this;
}


lit_stream& lit_stream::operator<< (const std::string value)
{
	*this << (int)value.size();
	memcpy(GetWriterPtr(value.size()),&value[0],value.size());
	wr_pos+=value.size();
	return *this;
}

lit_stream& lit_stream::operator>> (std::string& value)
{
	int sz;
	*this >> sz;
	value.resize(sz);
	memcpy(&value[0],GetReadPtr(),sz);
	rd_pos+=sz;
	return *this;
}




int CShortMsg::NewToStream( BYTE*& pbyStream, DWORD& dwLen)
{
	lit_stream out(0);
	out << char_ptr_out(m_CalledNo,sizeof(m_CalledNo));
	out << char_ptr_out(m_CallingNo,sizeof(m_CallingNo));
	out << char_ptr_out(m_FeeCallNo,sizeof(m_FeeCallNo));
	out << m_nSubmitTime;
	out << m_DealStep;
	out << m_ulSenderID;
	out << m_ulRecverID;
	out << m_ulSequenceID;
	out << m_ulCmdID;
	out << m_SendCmdID;
	out << m_nReSubmitCount;
	//out << m_IsReport;
	out << m_SendMsgFlag;

	//out << m_bFromHttp ;
	out << m_bSndToFee ;
	out << m_sendorgaddr ;
	out << m_sourceipaddr ;
	out << m_sourcecodetype ;
	out << m_bReturnFrist ;
	out << m_senddestaddr ;
	out << m_sendsevicecode ;
	out << m_sendspcode ;
	out << m_orgservicecode ;
	out << m_orgspcode ;
	out << m_bTransferInner ;
	int id_count = m_msgidlist.size();
	out << id_count;
	auto itr = m_msgidlist.begin();
	for(int i=0;i<id_count;i++)
	{
		out << *itr;
		itr++;
	}

	int w_count = m_wlongmsg.size()*sizeof(std::wstring::value_type);

	out << char_ptr_out((char*)&m_wlongmsg[0],w_count);

	//if(m_pSMData == nullptr)
	//	m_pSMData = new SM_STRUCT;
	//out.streamout_block((char*)m_pSMData,sizeof(SM_STRUCT));
	if(m_pSMData == nullptr)
		out << char_ptr_out(nullptr,0); 
	else
		out << char_ptr_out((char*)m_pSMData,sizeof(SM_STRUCT)); 
	CTlv_Ref tlv(m_cTlv);
	out << tlv;	

	dwLen = out.GetWriteSize();
	pbyStream = (BYTE*) new char[dwLen];
	int sz;
	memcpy(pbyStream,out.GetData(sz),dwLen);
	return 0;
}


int CShortMsg::NewFromStream( const BYTE* pbyStream, const DWORD& dwLen)
{
	lit_stream in((char*)pbyStream,dwLen,1);
	char_ptr_in in_CalledNo(m_CalledNo,sizeof(m_CalledNo));
	in >> in_CalledNo;
	char_ptr_in in_CallingNo(m_CallingNo,sizeof(m_CallingNo));
	in >> in_CallingNo;
	char_ptr_in in_FeeCallNo(m_FeeCallNo,sizeof(m_FeeCallNo));
	in >> in_FeeCallNo;
	in >> m_nSubmitTime;
	in >> m_DealStep;
	in >> m_ulSenderID;
	in >> m_ulRecverID;
	in >> m_ulSequenceID;
	in >> m_ulCmdID;
	in >> m_SendCmdID;
	in >> m_nReSubmitCount;
	//in >> m_IsReport;
	in >> m_SendMsgFlag;

	//in >> m_bFromHttp ;
	in >> m_bSndToFee ;
	in >>m_sendorgaddr ;
	in >>m_sourceipaddr ;
	in >>m_sourcecodetype ;
	in >>m_bReturnFrist ;
	in >>m_senddestaddr ;
	in >>m_sendsevicecode ;
	in >>m_sendspcode ;
	in >>m_orgservicecode ;
	in >>m_orgspcode ;
	in >>m_bTransferInner ;

	int id_count;
	in >> id_count;
	for(int i=0;i<id_count;i++)
	{
		std::string s;
		in >> s;
		m_msgidlist.push_back(std::move(s));
	}

	int w_count = 4096;
	m_wlongmsg.resize(w_count);
	char_ptr_in ptr((char*)&m_wlongmsg[0],w_count);
	in >> ptr;
	m_wlongmsg.resize(ptr.datasize()/2);

	if(m_pSMData == nullptr)
		m_pSMData = new SM_STRUCT;
	//in.streamin_block((char*)m_pSMData,sizeof(SM_STRUCT));
	char_ptr_in in_pSMData((char*)m_pSMData,sizeof(SM_STRUCT));
	in >> in_pSMData;
	if(m_cTlv)
		delete m_cTlv;
	m_cTlv = new CTLV;
	CTlv_Ref tlv(m_cTlv);
	in >> tlv;	
	m_cTlv= tlv.GetTlv();

	return in.GetReadSize();
}

//add by wj
CShortMsg::CShortMsg(sm::gw_shortmsg* pPB)
{
	m_pSMData = new SM_STRUCT; 
	memset(m_pSMData, 0, sizeof(SM_STRUCT));
	
	//strncpy(m_pSMData->RcvMSISDN ,pPB->smdata().rcvaddr().data(),min(sizeof(m_pSMData->RcvMSISDN)-1,pPB->smdata().rcvaddr().size()));
	COPY_FROM_STRING(m_pSMData->RcvMSISDN,pPB->smdata().rcvaddr());
	m_pSMData->RcvTon = pPB->smdata().rcvton();
	m_pSMData->RcvNpi = pPB->smdata().rcvnpi();
	//strncpy(m_pSMData->SndMSISDN ,pPB->smdata().sndaddr().data(),sizeof(m_pSMData->SndMSISDN)-1);
	COPY_FROM_STRING(m_pSMData->SndMSISDN,pPB->smdata().sndaddr());
	m_pSMData->SndTon = pPB->smdata().sndton();
	m_pSMData->SndNpi = pPB->smdata().sndnpi();
	m_pSMData->DCS = pPB->smdata().dcs();
	m_pSMData->pid = pPB->smdata().pid();
	m_pSMData->pri = pPB->smdata().pri();
	m_pSMData->EmsClass = pPB->smdata().ems_class();
	//strncpy(m_pSMData->ExpireTime,pPB->smdata().expiretime().c_str(),sizeof(m_pSMData->ExpireTime)-1);
	COPY_FROM_STRING(m_pSMData->ExpireTime,pPB->smdata().expiretime());
	//strncpy(m_pSMData->ScheduleTime,pPB->smdata().scheduletime().c_str(),sizeof(m_pSMData->ScheduleTime)-1);
	COPY_FROM_STRING(m_pSMData->ScheduleTime,pPB->smdata().scheduletime());	
	m_pSMData->SMLength = (std::min)((int)pPB->smdata().content().size(),(int)sizeof(m_pSMData->SMUD)-1);
	memcpy(m_pSMData->SMUD,pPB->smdata().scheduletime().data(),m_pSMData->SMLength);
	m_pSMData->ReportType = pPB->smdata().reporttype();

	this->m_bReturnFrist = pPB->reply_end();
	//strncpy(m_CalledNo,pPB->org_destaddr().data(),sizeof(m_CalledNo));
	COPY_FROM_STRING(m_CalledNo,pPB->org_destaddr());
	COPY_FROM_STRING(m_CallingNo,pPB->org_orgaddr());
	COPY_FROM_STRING(m_FeeCallNo,pPB->org_feeaddr());
	m_orgservicecode = pPB->org_servicecode();
	m_orgspcode = pPB->org_spcode();
	m_senddestaddr = pPB->dst_destaddr();
	m_sendorgaddr = pPB->dst_orgaddr();
	m_sendsevicecode = pPB->dst_servicecode();
	m_sendspcode = pPB->dst_spcode();
	m_DealStep = pPB->step();
	m_nReSubmitCount = pPB->snd_count();
	m_SendMsgFlag = pPB->snd_flag();
	m_SendCmdID = pPB->snd_cmd();
	m_ulCmdID = pPB->cmd();
	m_nSubmitTime = pPB->submit_time();

	m_cTlv = new CTLV;
	if(pPB->tlvs().size())
	{	
	
		for(int i = 0;i<pPB->tlvs().size();i++)
		{
			auto& tr = pPB->tlvs(i);
			m_cTlv->AddItem(tr.tlv_type(),tr.bvalue().size(),(BYTE*)tr.bvalue().data());
		}
		VLOG(5)<<"CShortMsg(sm::gw_shortmsg*) tlv count="<<pPB->tlvs().size();
	}

	if(pPB->lsm_case()==pPB->kSupersms)
	{
		auto& lsm = pPB->supersms();
		
		this->m_wlongmsg.resize(lsm.longcontent().size()/2);

		memcpy(&m_wlongmsg[0],lsm.longcontent().data(),lsm.longcontent().size());

		VLOG(5)<<"CShortMsg(sm::gw_shortmsg*) longsm len="<<lsm.longcontent().size();

		for(int i=0;i<lsm.subids().size();i++)
		{
			auto& id = lsm.subids(i);

			assert(id.length()>0);
			this->m_msgidlist.push_back(std::move(id));
		}
		VLOG(5)<<"CShortMsg(sm::gw_shortmsg*) subsm count="<<lsm.subids().size();

	}

}

sm::gw_shortmsg CShortMsg::GetPBPack()
{
	sm::gw_shortmsg pb;
	if(this->m_pSMData)
	{
		auto psm_data = pb.mutable_smdata();
		psm_data->set_rcvaddr(m_pSMData->RcvMSISDN,strnlen(m_pSMData->RcvMSISDN,sizeof(m_pSMData->RcvMSISDN)-1));
		psm_data->set_rcvnpi(m_pSMData->RcvNpi);
		psm_data->set_rcvton(m_pSMData->RcvTon);
		psm_data->set_sndaddr(m_pSMData->SndMSISDN,strnlen(m_pSMData->SndMSISDN,sizeof(m_pSMData->SndMSISDN)-1));
		psm_data->set_sndnpi(m_pSMData->SndNpi);
		psm_data->set_sndton(m_pSMData->SndTon);
		psm_data->set_ems_class(m_pSMData->EmsClass);
		psm_data->set_pid(m_pSMData->pid);
		psm_data->set_pri(m_pSMData->pri);
		psm_data->set_dcs(m_pSMData->DCS);
		psm_data->set_expiretime(m_pSMData->ExpireTime,strnlen(m_pSMData->ExpireTime,sizeof(m_pSMData->ExpireTime)-1));
		psm_data->set_scheduletime(m_pSMData->ScheduleTime,strnlen(m_pSMData->ScheduleTime,sizeof(m_pSMData->ScheduleTime)-1));
		psm_data->set_reporttype(m_pSMData->ReportType);
		psm_data->set_content(m_pSMData->SMUD,m_pSMData->SMLength);
	}


	pb.set_reply_end(m_bReturnFrist);
	pb.set_org_orgaddr(m_CalledNo,strnlen(m_CalledNo,sizeof(m_CalledNo)-1));
	pb.set_org_orgaddr(m_CallingNo,strnlen(m_CallingNo,sizeof(m_CallingNo)-1));
	pb.set_org_orgaddr(m_FeeCallNo,strnlen(m_FeeCallNo,sizeof(m_FeeCallNo)-1));
	pb.set_org_servicecode(m_orgservicecode);
	pb.set_org_spcode(m_orgspcode);
	pb.set_dst_destaddr(m_senddestaddr);
	pb.set_dst_orgaddr(m_sendorgaddr);
	pb.set_dst_servicecode(m_sendsevicecode);
	pb.set_dst_spcode(m_sendsevicecode);
	pb.set_dst_spcode(m_sendspcode);
	pb.set_step(m_DealStep);
	pb.set_cmd(m_ulCmdID);
	pb.set_snd_cmd(m_SendCmdID);
	pb.set_snd_flag(m_SendMsgFlag);
	pb.set_submit_time(m_nSubmitTime);
	pb.set_snd_count(m_nReSubmitCount);

	if(m_cTlv)
	{
		for(int i =0;i<m_cTlv->GetCount();i++)
		{
			auto& pair = m_cTlv->GetTlv(i);
			if(pair.second.size())
			{
				auto p = pb.add_tlvs();
				p->set_tlv_type(pair.first);
				p->set_bvalue(pair.second);
			}
		}
		//pb.ConsumedEntireMessage();
	}
	if(m_wlongmsg.size()>0)
	{
		auto plsm = pb.mutable_supersms();
		plsm->set_longcontent(m_wlongmsg.data(),m_wlongmsg.size()*2);
		for(auto itr = m_msgidlist.begin();itr!= m_msgidlist.end();itr++)
		{
			plsm->add_subids(*itr);
		}
	}
	return std::move(pb);
}




CShortMsg::CShortMsg()
{	
	memset(m_CalledNo, 0, sizeof(m_CalledNo));
	memset(m_CallingNo, 0, sizeof(m_CallingNo));
	m_cTlv = NULL;
	m_DealStep = 0;
	memset(m_FeeCallNo, 0, sizeof(m_FeeCallNo));
	//m_IsReport = 0;
	m_nReSubmitCount = 0;
	m_nSubmitTime = GetTickCount();
	
	m_pSMData = new SM_STRUCT;					
	memset(m_pSMData, 0, sizeof(SM_STRUCT));

	m_SendCmdID = 0;
	m_SendMsgFlag = 1;
	m_ulCmdID = 0;
	m_ulRecverID = 0;
	m_ulSenderID = 0;
	m_ulSequenceID = 0;
	//m_bFromHttp = false;
	m_bReturnFrist = false;
	m_bSndToFee = false;
	m_bTransferInner = false;
#ifdef __DEBUG_SHORTMSG__
	APP_BEGIN_LOG(0)
	CString s;
	s.Format("CShortMsg construct :[%x] ",this);
	APP_DEBUG_LOG((LPCTSTR)s);
	APP_END_LOG
#endif
};
//modify by gyx 2015/2/13
void CShortMsg::SetEmsID(UC EmsID )
{
	m_pSMData->EmsClass = m_pSMData->EmsClass | EmsID ;
}
//end

CShortMsg::CShortMsg(LPVOID pMsg)
{  

	memset(m_CalledNo, 0, sizeof(m_CalledNo));
	memset(m_CallingNo, 0, sizeof(m_CallingNo));
	m_cTlv = NULL;
	m_DealStep = 0;
	memset(m_FeeCallNo, 0, sizeof(m_FeeCallNo));
	//m_IsReport = 0;
	m_nReSubmitCount = 0;
	m_nSubmitTime = GetTickCount();;

	m_pSMData = new SM_STRUCT;					
	memset(m_pSMData, 0, sizeof(SM_STRUCT));

	m_SendCmdID = 0;
	m_SendMsgFlag = 1;
	m_ulCmdID = 0;
	m_ulRecverID = 0;
	m_ulSenderID = 0;
	m_ulSequenceID = 0;	
	//m_bFromHttp = false;
	m_bReturnFrist = false;
	m_bSndToFee = false;
	m_bTransferInner = false;
//	Test();


	char buf[100];
	memset(buf, 0, sizeof(buf));

	tagSmsHead * pSmsHead=(tagSmsHead *)pMsg;
	switch(pSmsHead->nCmdID)
	{
	case SMS_SUBMIT_ADDR:
	case SMS_DELIVER_ADDR:
	case SMS_STATUS_REPORT:
	case SMS_FWD_ADDR:
	case SMS_SHREPORT_ADDR:
	case SMS_AUTHPRICEREQ:
	case SMS_AUTHPRICECNFMREQ:
		{
			tagSmsSubmitAddr* pSM=(tagSmsSubmitAddr*)pMsg;
			m_ulSequenceID = pSM->nSequenceID;
			m_ulSenderID = pSM->nSenderID;
			m_ulCmdID = pSmsHead->nCmdID;
			
			switch(pSM->byUDHI)
			{
			case UDHI_TYPE_NO:
				{
					m_pSMData->RcvNpi = pSM->byDestNPI;
					m_pSMData->RcvTon = pSM->byDestTON;
					strcpy(m_pSMData ->RcvMSISDN,pSM->sDestAddr);
					
					m_pSMData->SndNpi = pSM->byOrgNPI;
					m_pSMData->SndTon = pSM->byOrgTON;
					strcpy(m_pSMData->SndMSISDN,pSM->sOrgAddr);
					
					m_pSMData->EmsClass = 0;
					m_pSMData->pid = pSM->byPID;
					m_pSMData->pri = pSM->byPri;
					
					m_pSMData->DCS = pSM->byDCS;
					
					m_pSMData->SMLength = MAX_SM_LEN;
					CCodeAndDecode tool;
					tool.ConvertTextToDataEx((char*)pSM->UD,pSM->ulUDL,pSM->byDCS,(UC*)m_pSMData->SMUD,*(US*)(&(m_pSMData->SMLength)));
					
					break;
				}				

			case UDHI_TYPE_SMPP:
				{
					smpp_Submit_Sm* pSmppSub = (smpp_Submit_Sm*)(pSM->UD+sizeof(smpp_Message_header));

					m_pSMData->RcvNpi = pSmppSub->dest_addr_npi;
					m_pSMData->RcvTon = pSmppSub ->dest_addr_ton;
					strcpy(m_pSMData ->RcvMSISDN,pSmppSub->destination_addr);
					
					m_pSMData->SndNpi = pSmppSub->source_addr_npi;
					m_pSMData->SndTon = pSmppSub->source_addr_ton;
					strcpy(m_pSMData->SndMSISDN,pSmppSub->source_addr);
					
					/*if (pSmppSub->esm_class & ESM_CLASS_STATUS_REPORT)
					{
						m_IsReport=1;
					}*/
					
					m_pSMData->EmsClass = pSmppSub->esm_class;
					m_pSMData->pid = pSmppSub->protocol_ID;
					m_pSMData->pri = pSmppSub->priority_flag;
					
					m_pSMData->DCS = pSmppSub->data_coding;
					
					strcpy(m_pSMData->ExpireTime,pSmppSub->validity_period);
					strcpy(m_pSMData->ScheduleTime,pSmppSub->schedule_delivery_time);  
					strcpy(m_pSMData->ServiceType,pSmppSub->service_type);  
					
					m_pSMData->ReportType=pSmppSub->registered_delivery_flag;  
					
					if(pSmppSub->sm_length>(MAX_SM_LEN-1))
					{
						m_pSMData->SMLength=MAX_SM_LEN-1;
					}
					else
					{
						m_pSMData->SMLength = pSmppSub->sm_length;
					}
					memcpy(m_pSMData->SMUD,pSmppSub->short_message,m_pSMData->SMLength);

					if (pSmppSub->ext_data_len > MAX_SMS_SM_LEN)
						pSmppSub->ext_data_len=0;
					
					if(pSmppSub->ext_data_len > 0)
					{
						DWORD pos = 0;
						m_cTlv = new CTLV;
						m_cTlv->FromStream((BYTE*)pSmppSub->ext_data,pSmppSub->ext_data_len,pos);
					}
					else
					{
						m_cTlv=NULL;
					}
					
					if( m_cTlv)
					{
						char* pData = nullptr;
						DWORD sz = 0;
						pData = (char*)m_cTlv->GetItem(Tag_Ex_SendOrgAddr,sz);
						if(pData)//内部消息
						{
							//原始主叫;
							strncpy(m_CallingNo,pData,min(sizeof(m_CallingNo),sz));
						}
						else
						{
							strncpy(m_CallingNo,m_pSMData->SndMSISDN,min(sizeof(m_CallingNo),strlen(m_pSMData->SndMSISDN)));
						}

						pData = (char*)m_cTlv->GetItem(Tag_Ex_SendDestAddr,sz);
						if(pData)
						{
							//原始被叫;
							strncpy(m_CalledNo,pData,min(sizeof(m_CalledNo),sz));
						}
						else
						{
							strncpy(m_CalledNo,m_pSMData->RcvMSISDN,min(sizeof(m_CalledNo),strlen(m_pSMData->RcvMSISDN)));
						}

						pData = (char*)m_cTlv->GetItem(Tag_Ex_SendServiceCode,sz);
						if(pData)
						{
							this->m_orgservicecode = std::string(pData,sz);
						}
						pData = (char*)m_cTlv->GetItem(Tag_Ex_SendCPID,sz);
						
						if(pData)
						{
							this->m_orgspcode = std::string(pData,sz);
						}

						pData = (char*)m_cTlv->GetItem(Tag_Ex_SourceIpAddr,sz);
						if(pData)
						{
							this->m_sourceipaddr = std::string(pData,sz);
						}

						pData = (char*)m_cTlv->GetItem(Tag_Ex_SourceCodeType,sz);
						if(pData)
						{
							int idata = *(int *)pData;
							this->m_sourcecodetype = ntohl(idata);
						}

						pData = (char*)m_cTlv->GetItem(Tag_Ex_ReturnFirst,sz);
						if(pData)
						{
							int idata = *(int*)pData;
							this->m_bReturnFrist = ntohl(idata);
						}
					}


					break;
				}
				
			default:
				{
					ASSERT(0);
					break;
				}
			}

			break;
		}
		
	case SMS_QUERYUSERSTATE:
		{
			tagSmsQueryUserState* pSM=(tagSmsQueryUserState*)pMsg;

			m_ulSequenceID = pSM->nSequenceID;
			m_ulCmdID = pSmsHead->nCmdID;
			m_ulRecverID = pSM->nRecverID;
			m_ulSenderID = pSM->nSenderID;

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->sQueryUserAddr, sizeof(pSM->sQueryUserAddr));
			this->SetFeeAddr(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->sSMG_No, sizeof(pSM->sSMG_No));
			this->SetSrcSMGNO(buf);

			break;
		}
		
	case SMS_PAYMNTREQUEST:
		{
			tagSmsPaymntRequest* pSM=(tagSmsPaymntRequest*)pMsg;

			m_ulSequenceID = pSM->nSequenceID;
			m_ulCmdID = pSmsHead->nCmdID;
			m_ulRecverID = pSM->nRecverID;
			m_ulSenderID = pSM->nSenderID;

			this->SetSMMsgType(pSM->MsgType);
			this->SetPriority(pSM->Priority);
			this->SetSMType(pSM->SMType);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->SMGNo, sizeof(pSM->SMGNo));
			this->SetSrcSMGNO(buf);	
			
			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FwdSMGNo, sizeof(pSM->FwdSMGNo));
			this->SetDestSMGNO(buf);
			
			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->SMCNo, sizeof(pSM->SMCNo));
			this->SetSMCNO(buf);
			
			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->AreaCode, sizeof(pSM->AreaCode));
			this->SetAreaCode(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->ChargeTermID, sizeof(pSM->ChargeTermID));
			this->SetFeeAddr(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->CPID, sizeof(pSM->CPID));
			this->SetCPID(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->DestTermID, sizeof(pSM->DestTermID));
			this->SetDestAddr(buf);

/*			CString time, tmp;
			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->DoneTime, sizeof(pSM->DoneTime));
			tmp = buf;
			time.Format("%s/%s/%s %s:%s:%s", tmp.Left(4), tmp.Mid(4,2), tmp.Mid(6,2), tmp.Mid(8,2), tmp.Mid(10,2), tmp.Right(2));
			
			COleVariant varTime(time);
			COleDateTime tmTime(varTime);
			if(tmTime.GetStatus() == COleDateTime::invalid)
			{
				tmTime = COleDateTime::GetCurrentTime();
				time = tmTime.Format("%Y/%m/%d %H:%M:%S");
			}

			this->SetDlrTime(time);
*/
			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FeeCode, sizeof(pSM->FeeCode));
			this->SetFeeCode(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FeeType, sizeof(pSM->FeeType));
			this->SetFeeType(atoi(buf));

			this->SetSMMsgType(pSM->MsgType);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FixedFee, sizeof(pSM->FixedFee));
			this->SetFixFee(buf);

			m_pSMData->SMLength = pSM->MsgLen;

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->OrigMsgID, sizeof(pSM->OrigMsgID));
			this->SetSourceID(buf);

			this->SetPriority(pSM->Priority);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->ServiceID, sizeof(pSM->ServiceID));
			this->SetServiceID(buf);

			this->SetSMType(pSM->SMType);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->SrcTermID, sizeof(pSM->SrcTermID));
			this->SetOrgAddr(buf);

/*			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->SubmitTime, sizeof(pSM->SubmitTime));
			tmp = buf;
			time.Format("%s/%s/%s %s:%s:%s", tmp.Left(4), tmp.Mid(4,2), tmp.Mid(6,2), tmp.Mid(8,2), tmp.Mid(10,2), tmp.Right(2));

			varTime = COleVariant(time);
			tmTime = COleDateTime(varTime);
			if(tmTime.GetStatus() == COleDateTime::invalid)
			{
				tmTime = COleDateTime::GetCurrentTime();
				time = tmTime.Format("%Y/%m/%d %H:%M:%S");
			}

			this->SetSubTime(time);
*/
			break;
		}

	case SMS_FEE_PAYMNTREQUEST:
		{
			tagSmsFeePaymntRequest *pSM = (tagSmsFeePaymntRequest *)pMsg;

			m_ulSequenceID = pSM->nSequenceID;
			m_ulCmdID = pSmsHead->nCmdID;
			m_ulRecverID = pSM->nRecverID;
			m_ulSenderID = pSM->nSenderID;

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->AreaCode, sizeof(pSM->AreaCode));
			this->SetAreaCode(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->ChargeTermID, sizeof(pSM->ChargeTermID));
			this->SetFeeAddr(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->CPID, sizeof(pSM->CPID));
			this->SetCPID(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->DestMsgID, sizeof(pSM->DestMsgID));			
			this->SetUnikey(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->OrigMsgID, sizeof(pSM->OrigMsgID));			
			this->SetSourceID(buf);		
			
			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->Smid, sizeof(pSM->Smid));			
			this->SetSmid(buf);			

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->DestTermID, sizeof(pSM->DestTermID));			
			this->SetDestAddr(buf);

			CString time, tmp;
			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->DoneTime, sizeof(pSM->DoneTime));			
			tmp = buf;
			time.Format("%s/%s/%s %s:%s:%s", (LPCTSTR)tmp.Left(4), (LPCTSTR)tmp.Mid(4,2), (LPCTSTR)tmp.Mid(6,2), (LPCTSTR)tmp.Mid(8,2), (LPCTSTR)tmp.Mid(10,2), (LPCTSTR)tmp.Right(2));
			
			COleVariant varTime(time);
			COleDateTime tmTime(varTime);
			if(tmTime.GetStatus() == COleDateTime::invalid)
			{
				tmTime = COleDateTime::GetCurrentTime();
				time = tmTime.Format("%Y/%m/%d %H:%M:%S");
			}

			this->SetDlrTime(time);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FeeCode, sizeof(pSM->FeeCode));			
			this->SetFeeCode(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FeeType, sizeof(pSM->FeeType));			
			this->SetFeeType(atoi(buf));

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FixedFee, sizeof(pSM->FixedFee));				
			this->SetFixFee(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->FwdSMGNo, sizeof(pSM->FwdSMGNo));				
			this->SetDestSMGNO(buf);
			
			this->SetInnerMsgType(pSM->MessageType);
			m_pSMData->SMLength = pSM->MsgLen;
			this->SetSMMsgType(pSM->MsgType);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->OrgTermID, sizeof(pSM->OrgTermID));				
			this->SetOrgAddr(buf);
			
			this->SetPriority(pSM->Priority);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->ServiceID, sizeof(pSM->ServiceID));				
			this->SetServiceID(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->SMCNo, sizeof(pSM->SMCNo));
			this->SetSMCNO(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->SMGNo, sizeof(pSM->SMGNo));
			this->SetSrcSMGNO(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->NxtNwkNd, sizeof(pSM->NxtNwkNd));
			this->SetToNo(buf);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->PreNwkNd, sizeof(pSM->PreNwkNd));
			this->SetFromNo(buf);
			
			this->SetSMType(pSM->SMtype);

			memset(buf, 0, sizeof(buf));
			memcpy(buf, pSM->SubmitTime, sizeof(pSM->SubmitTime));
			tmp = buf;
			time.Format("%s/%s/%s %s:%s:%s", (LPCTSTR)tmp.Left(4), (LPCTSTR)tmp.Mid(4,2), (LPCTSTR)tmp.Mid(6,2), (LPCTSTR)tmp.Mid(8,2), (LPCTSTR)tmp.Mid(10,2), (LPCTSTR)tmp.Right(2));

			varTime = COleVariant(time);
			tmTime = COleDateTime(varTime);
			if(tmTime.GetStatus() == COleDateTime::invalid)
			{
				tmTime = COleDateTime::GetCurrentTime();
				time = tmTime.Format("%Y/%m/%d %H:%M:%S");
			}

			this->SetSubTime(time);

			break;
		}
		
	default:
		{
			ASSERT(0);
			break;
		}
	}
	
	//BakeAddr();

#ifdef __DEBUG_SHORTMSG__
	APP_BEGIN_LOG(0)
	CString s;
	s.Format("CShortMsg(LPVOID pMsg) construct :[%x] ",this);
	APP_DEBUG_LOG((LPCTSTR)s);
	APP_END_LOG
#endif

};



CShortMsg::CShortMsg(const CShortMsg * sm)
{
	memset(m_CalledNo, 0, sizeof(m_CalledNo));
	strncpy(m_CalledNo, sm->m_CalledNo, sizeof(m_CalledNo) - 1);

	memset(m_CallingNo, 0, sizeof(m_CallingNo));
	strncpy(m_CallingNo, sm->m_CallingNo, sizeof(m_CallingNo) - 1);

	if (sm->m_cTlv) 
	{
		this->m_cTlv = new CTLV( * sm->m_cTlv);
	}
	else
	{
		m_cTlv = NULL;
	}

	m_DealStep = sm->m_DealStep;

	memset(m_FeeCallNo, 0, sizeof(m_FeeCallNo));
	strncpy(m_FeeCallNo, sm->m_FeeCallNo, sizeof(m_FeeCallNo) - 1);

	//m_IsReport = sm->m_IsReport;
	m_nReSubmitCount = sm->m_nReSubmitCount;
	m_nSubmitTime = sm->m_nSubmitTime;

	m_pSMData = new SM_STRUCT;
	memset(m_pSMData, 0, sizeof(m_pSMData));

	if (sm->m_pSMData)
	{
		memcpy(m_pSMData, sm->m_pSMData, sizeof(SM_STRUCT) );
	}

	m_SendCmdID = sm->m_SendCmdID;
	m_SendMsgFlag = sm->m_SendMsgFlag;

	m_ulCmdID = sm->m_ulCmdID;
	m_ulRecverID = sm->m_ulRecverID;
	m_ulSenderID = sm->m_ulSenderID;
	m_ulSequenceID = sm->m_ulSequenceID;	
	//m_bFromHttp = sm->m_bFromHttp;
	m_bSndToFee =sm-> m_bSndToFee;
	m_sendorgaddr = sm->m_sendorgaddr;
	m_sourceipaddr = sm->m_sourceipaddr;
	m_sourcecodetype = sm->m_sourcecodetype;
	m_bReturnFrist = sm->m_bReturnFrist;
	m_senddestaddr = sm->m_senddestaddr;
	m_sendsevicecode = sm->m_sendsevicecode;
	m_sendspcode = sm->m_sendspcode;
	m_orgservicecode = sm->m_orgservicecode;
	m_orgspcode = sm->m_orgspcode;
	m_bTransferInner = sm->m_bTransferInner;
#ifdef __DEBUG_SHORTMSG__
	APP_BEGIN_LOG(0)
	CString s;
	s.Format("CShortMsg(const CShortMsg * sm) construct :[%x] ",this);
	APP_DEBUG_LOG((LPCTSTR)s);
	APP_END_LOG
#endif
}




CShortMsg::~CShortMsg()
{
#ifdef __DEBUG_SHORTMSG__
	APP_BEGIN_LOG(0)
	CString s;
	s.Format("CShortMsg destruct :[%x] ",this);
	APP_DEBUG_LOG((LPCTSTR)s);
	APP_END_LOG
#endif
	if(m_pSMData!=NULL)
		delete m_pSMData; 
	
	if(m_cTlv!=NULL)
		delete m_cTlv;
}



//获取消息包
int CShortMsg::GetMessagePacket(tagSmsSubmitAddr* pMsg,int nMsgSize,bool bInner)
{
	if(m_pSMData == NULL)
		return FALSE;
	
	switch(m_SendCmdID)
	{
	case SMS_FEE_CHECK:
	case SMS_SUBMIT_ADDR:
	case SMS_DELIVER_ADDR:
	case SMS_FWD_ADDR:
	case SMS_SHREPORT_ADDR:
	case SMS_AUTHPRICEREQ:
	case SMS_AUTHPRICECNFMREQ:
		{
			//***SMGW43-15, 2005-09-28, jdz, modi begin***//
			char pExtData[STREAM_LEN_MAX + 1];
			memset(pExtData, 0, sizeof(pExtData));
			DWORD nExtDataSize = 0;

			//if(SMS_AUTHPRICEREQ==m_SendCmdID && m_bFromHttp && m_msgidlist.size())
			//if(SMS_AUTHPRICEREQ==m_SendCmdID && m_bFromHttp)
			if(SMS_AUTHPRICEREQ==m_SendCmdID)
			{
				if(GetSubMsgCount())
				{
					CTLV tlv(*m_cTlv);
					//tlv.AddLongIntItem(Tag_Ex_HttpMsgCount,m_msgidlist.size()+1);
					tlv.AddLongIntItem(Tag_Ex_HttpMsgCount,GetSubMsgCount());
					CCodeAndDecode code;
					//std::wstring nlmsg = code.ConvertWStringToNWString(this->m_wlongmsg);				
					std::wstring nlmsg = GetAllContentNW(); 
					VLOG(5)<<"send long content to auth ["<<code.ConvertWStringToString(code.ConvertNWStringToWString(nlmsg))<<"]"
						<<"; Tag_Ex_HttpMsgCount=["<<GetSubMsgCount()<<"]";
					tlv.AddItem(Tag_Ex_HttpLongMsg,nlmsg.size()*2,(BYTE*)&nlmsg[0]);
					tlv.ToStream((BYTE*)pExtData, nExtDataSize);
				}
			}
			//else if(m_SendCmdID == SMS_DELIVER_ADDR && this->m_bSndToFee == true)//发送话单消息
			else if(m_SendCmdID == SMS_DELIVER_ADDR && bInner == true)//发送话单消息
			{
				CTLV tlv(*m_cTlv);
				tlv.SetLongIntItem(Tag_Ex_SourceCodeType,m_sourcecodetype);
				if(m_sendorgaddr.size())
				{
					int len = m_sendorgaddr.size();
					char *temp = new char[len+1];
					strcpy(temp,m_sendorgaddr.c_str());
					tlv.SetItem(Tag_Ex_SendOrgAddr,m_sendorgaddr.size(),(BYTE*)temp);
					delete temp;
				}
				if(m_senddestaddr.size())
				{
					int len = m_senddestaddr.size();
					char *temp = new char[len+1];
					strcpy(temp,m_senddestaddr.c_str());
					tlv.SetItem(Tag_Ex_SendDestAddr,m_senddestaddr.size(),(BYTE*)temp);
					delete temp;
				}
				tlv.SetItem(Tag_Ex_SourceIpAddr,m_sourceipaddr.size(),(BYTE*)&m_sourceipaddr[0]);
				tlv.SetLongIntItem(Tag_Ex_ReturnFirst,m_bReturnFrist);
				if(m_sendspcode.size())
				{
					int len = m_sendspcode.size();
					char *temp = new char[len+1];
					strcpy(temp,m_sendspcode.c_str());
					tlv.SetItem(Tag_Ex_SendCPID,m_sendspcode.size(),(BYTE*)temp);
					delete temp;
				}
				if(m_sendsevicecode.size())
				{
					int len = m_sendsevicecode.size();
					char *temp = new char[len+1];
					strcpy(temp,m_sendsevicecode.c_str());
					tlv.SetItem(Tag_Ex_SendServiceCode,m_sendsevicecode.size(),(BYTE*)temp);
					delete temp;
				}
				tlv.ToStream((BYTE*)pExtData, nExtDataSize);
				APP_BEGIN_LOG(5);
				CString s;
				s.Format("Send To Fee GetParcket [%s][%d]  SendOrgAddr[%s] SenddestAddr[%s] SendCPID[%s] SendServiceID[%s] bReturnFirst[%d]",
					m_sourceipaddr.c_str(),
					m_sourcecodetype,
					m_sendorgaddr.c_str(),
					m_senddestaddr.c_str(),
					m_sendspcode.c_str(),
					m_sendsevicecode.c_str(),
					m_bReturnFrist
					);
				APP_DEBUG_LOG((LPCTSTR)s);				
				APP_END_LOG;
			}
			//else if(this->m_bTransferInner)
			else if(bInner)
			{
				CTLV tlv(*m_cTlv);
				
				tlv.SetLongIntItem(Tag_Ex_SourceCodeType,m_sourcecodetype);				
				{
					int len = strlen(this->m_CallingNo);
					tlv.SetItem(Tag_Ex_SendOrgAddr,len,(BYTE*)m_CallingNo);
				}				
				
				{
					int len = strlen(this->m_CalledNo);
					tlv.SetItem(Tag_Ex_SendDestAddr,len,(BYTE*)m_CalledNo);
				}

				tlv.SetItem(Tag_Ex_SourceIpAddr,m_sourceipaddr.size(),(BYTE*)&m_sourceipaddr[0]);
				tlv.SetLongIntItem(Tag_Ex_ReturnFirst,m_bReturnFrist);

				if(m_orgspcode.size())
				{
					int len = m_orgspcode.size();
					char *temp = new char[len+1];
					strcpy(temp,m_orgspcode.c_str());
					tlv.SetItem(Tag_Ex_SendCPID,m_orgspcode.size(),(BYTE*)temp);
					delete temp;
				}

				if(m_orgservicecode.size())
				{
					int len = m_orgservicecode.size();
					char *temp = new char[len+1];
					strcpy(temp,m_orgservicecode.c_str());
					tlv.SetItem(Tag_Ex_SendServiceCode,m_orgservicecode.size(),(BYTE*)temp);
					delete temp;
				}
				tlv.ToStream((BYTE*)pExtData, nExtDataSize);
				APP_BEGIN_LOG(5);
				CString s;
				s.Format("Send To Fee GetParcket [%s][%d]  SendOrgAddr[%s] SenddestAddr[%s] SendCPID[%s] SendServiceID[%s] bReturnFirst[%d]",
					m_sourceipaddr.c_str(),
					m_sourcecodetype,
					m_sendorgaddr.c_str(),
					m_senddestaddr.c_str(),
					m_sendspcode.c_str(),
					m_sendsevicecode.c_str(),
					m_bReturnFrist
					);
				APP_DEBUG_LOG((LPCTSTR)s);				
				APP_END_LOG;
			}
			else
			{
				if(m_cTlv)
				{
					m_cTlv->ToStream((BYTE*)pExtData, nExtDataSize);
				}
			}

			int nLen = (sizeof(tagSmsSubmitAddr) - MAX_SMS_SM_LEN \
				+ sizeof(smpp_Message_header)\
				+ sizeof(smpp_Submit_Sm)\
				+ nExtDataSize);

			if(nMsgSize < nLen)
			{
				return FALSE;
			}

			//init SMS HEADER
			pMsg->nLen = nLen;
			pMsg ->nCmdID = m_SendCmdID;
			pMsg ->nStatus = 0;
			pMsg ->nSequenceID = 0;
			pMsg ->nSenderID = 0;
			pMsg ->nRecverID = 0;
			pMsg ->byUDHI = UDHI_TYPE_SMPP;
			pMsg ->byDCS = m_pSMData ->DCS;
			pMsg ->byPri = 1;
			pMsg ->byPID = m_pSMData ->pid;
			pMsg ->ulUDL = + sizeof(smpp_Message_header) + sizeof(smpp_Submit_Sm) + nExtDataSize;			
			
			//init smpp header
			smpp_Message_header* pHeader = (smpp_Message_header*)pMsg->UD;
			pHeader ->Command_ID = m_SendCmdID;
			pHeader ->Command_Status = 0;
			pHeader ->Sequence_No = 0; //不在此分配序列号
			pHeader ->Command_Length = sizeof(smpp_Message_header)+ sizeof(smpp_Submit_Sm)+ nExtDataSize;
			//***SMGW43-15, 2005-09-28, jdz, modi begin***//
			
			//set smpp body
			smpp_Submit_Sm* pSmppSub = (smpp_Submit_Sm*)(pMsg->UD+sizeof(smpp_Message_header));
			memset(pSmppSub,0,sizeof(SM_STRUCT));
			
			pSmppSub->dest_addr_npi	= m_pSMData->RcvNpi;
			pMsg->byDestNPI = m_pSMData->RcvNpi;
			pSmppSub ->dest_addr_ton = m_pSMData->RcvTon;
			pMsg->byDestTON = m_pSMData->RcvTon;
			
			memcpy(pSmppSub->destination_addr, m_pSMData ->RcvMSISDN, MAX_ADDR_LEN);
			pSmppSub->destination_addr[MAX_ADDR_LEN-1] = 0;
			
			char sStr[200];
			int flag = 0;
			if(pMsg == NULL)
			{
				sprintf(sStr, "CShortMsg::GetMessagePacket函数出现异常：pMsg == NULL");
				flag = 1;
			}
			else if(pMsg->sDestAddr == NULL)
			{
				sprintf(sStr, "CShortMsg::GetMessagePacket函数出现异常：pMsg->sDestAddr == NULL");
				flag = 1;
			}
			else if(m_pSMData == NULL)
			{
				sprintf(sStr, "CShortMsg::GetMessagePacket函数出现异常：m_pSMData == NULL");
				flag = 1;
			}
			else if(m_pSMData ->RcvMSISDN == NULL)
			{
				sprintf(sStr, "CShortMsg::GetMessagePacket函数出现异常：m_pSMData ->RcvMSISDN == NULL");
				flag = 1;
			}
			if(flag == 1)
			{
				//SMGW42-104, 2005-10-26, jdz, modi begin//
				GetSmsApp()->WriteTestRecord(sStr, 0);
				//SMGW42-104, 2005-10-26, jdz, modi end//
				return FALSE;
			}
			
			memcpy(pMsg->sDestAddr, m_pSMData ->RcvMSISDN, MAX_ADDR_LEN);
			
			pSmppSub->source_addr_npi = m_pSMData->SndNpi;
			pMsg->byOrgNPI = m_pSMData->SndNpi;
			pSmppSub->source_addr_ton = m_pSMData->SndTon;
			pMsg->byOrgTON = m_pSMData->SndTon;
			
			
			strcpy(pSmppSub->source_addr,m_pSMData->SndMSISDN);
			strcpy(pMsg->sOrgAddr,m_pSMData->SndMSISDN);
			strcpy(pSmppSub->schedule_delivery_time,m_pSMData->ScheduleTime);
			strcpy(pSmppSub->validity_period,m_pSMData->ExpireTime);  
			
			strcpy(pSmppSub->service_type,m_pSMData->ServiceType);  
			
			pSmppSub->esm_class	= m_pSMData->EmsClass;
			
			pSmppSub->protocol_ID = m_pSMData->pid;
			pSmppSub->priority_flag	= m_pSMData->pri;
			pSmppSub->data_coding =	m_pSMData->DCS;
			if(G_APP_STATUS_REPORT_SUPPORT==1)
			{
				if(m_pSMData->ReportType == SMPP_MONEH_TYPE_SM)
					pSmppSub->registered_delivery_flag = m_pSMData->ReportType;
				else
					pSmppSub->registered_delivery_flag = TRUE;
			}
			else
			{
				pSmppSub->registered_delivery_flag = m_pSMData->ReportType;
			}
			
			pSmppSub->sm_length	= m_pSMData->SMLength;
			memcpy(pSmppSub->short_message,m_pSMData->SMUD,pSmppSub->sm_length);
			
			//***SMGW43-15, 2005-09-28, jdz, modi begin***//
			if(nExtDataSize > 0)
			{
				pSmppSub->ext_data_len = nExtDataSize;
				memcpy(pSmppSub->ext_data, pExtData, pSmppSub->ext_data_len);
			}
			//***SMGW43-15, 2005-09-28, jdz, modi begin***//

			return TRUE;
		}

	default:
		return FALSE;
	}
	
	return FALSE;
}


//获取预付费查询消息的消息包
int CShortMsg::GetQueryUserStatePacket(tagSmsQueryUserState* pMsg,int nMsgSize)
{
	
	memset(pMsg,0,sizeof(tagSmsQueryUserState));
	switch(m_SendCmdID)
	{
	case SMS_QUERYUSERSTATE:		
		{
			pMsg->nCmdID = SMS_QUERYUSERSTATE;
			pMsg->nLen = sizeof(tagSmsQueryUserState);
			
			char* pFeeAddr = this->GetFeeAddr();
			if(pFeeAddr)
			{
				memcpy(pMsg->sQueryUserAddr, pFeeAddr, MAX_ADDR_LEN);
			}
			else
			{
				return FALSE;
			}
			
			char* pSMGNO = this->GetSrcSMGNO();
			if(pSMGNO)
			{
				memcpy(pMsg->sSMG_No, pSMGNO, MAX_SMG_NO_LEN);
			}
			else
			{
				return FALSE;
			}
			
			return TRUE;
		}		
		
	default:
		ASSERT(0);
		return FALSE;
	}
	
	return FALSE;
}


//获取预付费扣费消息消息包
int CShortMsg::GetPaymntRequestPacket(tagSmsPaymntRequest* pMsg,int nMsgSize)
{
	memset(pMsg,0,sizeof(tagSmsPaymntRequest));

	switch(m_SendCmdID)
	{
	case SMS_PAYMNTREQUEST:		
		{
			pMsg->nCmdID = SMS_PAYMNTREQUEST;
			pMsg->nLen = sizeof(tagSmsPaymntRequest);

			char* ptmp = NULL;

			pMsg->MsgType  =  this->GetSMMsgType();
			pMsg->InnerMsgType = this->GetInnerMsgType();
			
			switch(pMsg->InnerMsgType)
			{			
			case SERVICE_MT_TYPE:
			case SERVICE_MTC_TYPE:
			case SERVICE_MC_TYPE:
			case SERVICE_MCA_TYPE:				
				{//下级帐号是短消息中心时	
					ptmp = this->GetSourceID();
					if(ptmp)
					{
						strncpy(pMsg->DestMsgID, ptmp, 20);
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}
					
					break;
				}
				
			case SERVICE_MCC_TYPE:
				{
					ptmp = this->GetSmid();
					if(ptmp)
					{
						char GwID[10];
						memset(GwID, 0, sizeof(GwID));
						GetSmsApp()->GetEnvironment()->GetLocalGWID((char*)GwID, MAX_SMG_NO_LEN);

						char SrcSeq[7];
						memset(SrcSeq, 0, sizeof(SrcSeq));
						strncpy(SrcSeq, ptmp, 6);

						char SrcTime[10];
						memset(SrcTime, 0, sizeof(SrcTime));
						strncpy(SrcTime, ptmp+6, 8);
						
						strncpy(pMsg->DestMsgID, GwID, 6);
						strncpy(pMsg->DestMsgID+6, SrcTime, 8);
						strncpy(pMsg->DestMsgID+14, SrcSeq, 6);
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}					

					break;	
				}

			default:
				{
					ptmp = this->GetUnikey();
					if(ptmp)
					{
						strncpy(pMsg->DestMsgID, ptmp, 20);
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}

					break;
				}
			}
			
			ptmp = this->GetOrgAddr();
			if(ptmp)
			{
				strncpy(pMsg->SrcTermID, ptmp, 21);
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}
			
			ptmp = this->GetFeeAddr();
			if(ptmp)
			{
				strncpy(pMsg->ChargeTermID, ptmp, 21);\
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}
			
			switch(pMsg->InnerMsgType)
			{	
				//下行消息
			case SERVICE_MTC_TYPE:
			case SERVICE_MT_TYPE:
			case SERVICE_MTF_TYPE:
			case SERVICE_MTFF_TYPE:
			case SERVICE_FWDPAYMSG_TYPE:
			case SERVICE_FWDQUERYMSG_TYPE:				
				{
					ptmp = this->GetCPID();
					if(ptmp)
					{
						strncpy(pMsg->CPID, ptmp, sizeof(pMsg->CPID));	
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}
					
					ptmp = this->GetServiceID();
					if(ptmp)
					{
						strncpy(pMsg->ServiceID, ptmp, 10);
					}
					else
					{
						ASSERT(0);
						return FALSE;
					}

					break;
				}	
				
			default:
				break;
			}
			
			ptmp = this->GetDestAddr();
			if(ptmp)
			{
				strncpy(pMsg->DestTermID, ptmp, 21);
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}			
			
			char* pFeeType = this->GetFeeType();
			if(pFeeType)
			{
				CString str;
				str.Format("%02d", pFeeType[0]);
				strncpy(pMsg->FeeType, str, 2);
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}
			
			ptmp = this->GetFeeCode();
			if(ptmp)
			{
				strncpy(pMsg->FeeCode, ptmp, 6);
			}
			else
			{
				strcpy(pMsg->FeeCode, "00");
			}
			
			ptmp = this->GetFixFee();
			if(ptmp)
			{
				strncpy(pMsg->FixedFee, ptmp, 6);
			}
			else
			{
				strcpy(pMsg->FixedFee, "00");
			}
			
			pMsg->Priority = this->GetPriority();
			pMsg->MsgLen = this->m_pSMData->SMLength;
			
			ptmp = this->GetAreaCode();
			if(ptmp)
			{
				strncpy(pMsg->AreaCode, ptmp, 4);
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}
			
			//本地网关代码
			ptmp = this->GetSrcSMGNO();
			if(ptmp)
			{
				strncpy(pMsg->SMGNo, ptmp, 6);
			}
			
			//下级网关代码
			ptmp = this->GetDestSMGNO();
			if(ptmp)
			{
				strncpy(pMsg->FwdSMGNo, ptmp, 6);
			}
			
			//短信中心代码
			ptmp = this->GetSMCNO();
			if(ptmp)
			{
				strncpy(pMsg->SMCNo, ptmp, 6);
			}
			
			//上级网元代码
			ptmp = this->GetFromNo();
			if(ptmp)
			{
				strncpy(pMsg->PreNwkNd, ptmp, 6);
			}

			//下级网元代码
			ptmp = this->GetToNo();
			if(ptmp)
			{
				strncpy(pMsg->NxtNwkNd, ptmp, 6);
			}

			CString time;
			CString tmp = this->GetDlrTime();
			if(!tmp.IsEmpty())
			{
				time.Format("%s%s%s%s%s%s", (LPCTSTR)tmp.Left(4), (LPCTSTR)tmp.Mid(5,2), (LPCTSTR)tmp.Mid(8,2), (LPCTSTR)tmp.Mid(11,2), (LPCTSTR)tmp.Mid(14,2), (LPCTSTR)tmp.Right(2));
			}
			else
			{
				COleDateTime cur = COleDateTime::GetCurrentTime();
				time = cur.Format("%Y%m%d%H%M%S");
			}
			strncpy(pMsg->DoneTime, time, 14);

			tmp = this->GetSubTime();
			if(!tmp.IsEmpty())
			{
				time.Format("%s%s%s%s%s%s", (LPCTSTR)tmp.Left(4), (LPCTSTR)tmp.Mid(5,2), (LPCTSTR)tmp.Mid(8,2), (LPCTSTR)tmp.Mid(11,2), (LPCTSTR)tmp.Mid(14,2), (LPCTSTR)tmp.Right(2));
			}
			else
			{
				COleDateTime cur = COleDateTime::GetCurrentTime();
				time = cur.Format("%Y%m%d%H%M%S");
			}
			strncpy(pMsg->SubmitTime, time, 14);

			pMsg->SMType  = this->GetSMType(); 
			
			return TRUE;
		}

	default:
		ASSERT(0);
		return FALSE;
	}
	return FALSE;
}


//获取消息中的消息类型
US  CShortMsg::GetSMMsgType() 
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		US ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_SMMsgType,dLen);
		if(temp == NULL)
			return 0;
		
		ret=*((US*)temp);
		return ret;
	}
	else
		return 0;
	
}

//设置消息中的消息类型
bool CShortMsg::SetSMMsgType(US MsgType)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_SMMsgType, 2, (BYTE*)&MsgType);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_SMMsgType, 2, (BYTE*)&MsgType);
		SetNewTLV(&tlv);
	}
	
	return true;
}

//设置是否发送给计费模块的标记
bool CShortMsg::SetSendToFeeFlag(UC SendToFeeFlag)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_SendToFee, 1, (BYTE*)&SendToFeeFlag);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_SendToFee, 1, (BYTE*)&SendToFeeFlag);
		SetNewTLV(&tlv);
	}
	
	return true;
}

//获取是否发送给计费模块的标记
UC  CShortMsg::GetSendToFeeFlag()
{
	if( m_cTlv != NULL)
	{
		DWORD dLen;
		UC ret;
		
		const BYTE * temp;
		temp = m_cTlv->GetItem(Tag_SendToFee,dLen);
		if(temp == NULL)
			return 0;
		
		ret = *((UC*)temp);
		return ret;
	}
	else
		return 0;
	
}



//获取内部定义的消息转发类型
int  CShortMsg::GetInnerMsgType()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_Inner_MsgType);
	}
	else
	{
		return 0;
	}
}


//设置内部定义的消息转发类型
bool CShortMsg::SetInnerMsgType(int MsgType)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_Inner_MsgType, MsgType);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_Inner_MsgType, MsgType);
		SetNewTLV(&tlv);
	}

	return true;
}



//获取发送给计费模块的预付费扣费结果消息的消息包
int CShortMsg::GettagSmsFeePaymntConfirmPacket(tagSmsFeePaymntConfirm *pMsg,int nlen)
{
	switch(m_SendCmdID)
	{
	case SMS_FEE_PAYMNTCONFIRM:		
		{
			pMsg->nCmdID = SMS_FEE_PAYMNTCONFIRM;
			pMsg->nLen = sizeof(tagSmsFeePaymntConfirm);
//			pMsg->nSequenceID = nSequenceID;

			char* ptmp = this->GetUnikey();
			if(ptmp)
			{
				strncpy(pMsg->DestMsgID, ptmp, sizeof(pMsg->DestMsgID) - 1);
				pMsg->ResultNotifyCode = this->GetReslutNotifyCode();
				pMsg->Status = this->GetStatus();

				return TRUE;
			}
			else
			{
				ASSERT(0);
				return FALSE;
			}
		}		

	default:
		ASSERT(0);
		return FALSE;
		
	}
	return FALSE;
}



//设置消息中的计费用户类型
bool CShortMsg::SetFeeUserType(UC FeeUserType)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Fee_UserType, 1, (BYTE*)&FeeUserType);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Fee_UserType, 1, (BYTE*)&FeeUserType);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息中的计费用户类型
UC  CShortMsg::GetFeeUserType()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		UC ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_Fee_UserType,dLen);
		if(temp == NULL)
			return 0;
		
		ret=*((UC*)temp);
		return ret;
	}
	else
		return 0;
	
}


//获取是否是下行消息标记
int  CShortMsg::GetMTFlag()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_MtFlag);
	}
	else
	{
		return 0;
	}
	
}


//设置是否是下行消息标记
bool CShortMsg::SetMTFlag(int MTFlag)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_MtFlag, MTFlag);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_MtFlag, MTFlag);
		SetNewTLV(&tlv);
	}

	return true;
}


//获取被赠送用户号码
char* CShortMsg::GetGiftAddr()
{
	if( m_cTlv != NULL)
	{
		DWORD dLen;
		char* ret;
		
		const BYTE * temp;
		temp = m_cTlv->GetItem(Tag_GiftAddr,dLen);
		if(temp == NULL)
			return NULL;
		
		ret = (char*)temp;
		return ret;
	}
	else
		return NULL;
}


//设置被赠送用户号码
bool CShortMsg::SetGiftAddr(const char *pAddr)
{
	if(NULL == pAddr)
		return false;

	int len = strlen(pAddr);
	if (len > 21) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_GiftAddr, len, (BYTE*)pAddr);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_GiftAddr, len, (BYTE*)pAddr);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取发送帐号
char * CShortMsg::GetSendAccount()
{
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_EX_SendAccount,dLen);
		if(temp == NULL)
			return NULL;
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
	
}


//设置发送帐号
BOOL CShortMsg::SetSendAccount(const char *pName)
{
	if(NULL == pName)
		return false;
	
	if( m_cTlv != NULL)
	{
		int len = strlen(pName);
		m_cTlv->SetItem(Tag_EX_SendAccount, len, (BYTE*)pName);
	}
	else
	{
		CTLV tlv;
		int len = strlen(pName);
		tlv.AddItem(Tag_EX_SendAccount, len, (BYTE*)pName);
		SetNewTLV(&tlv);
	}
	
	return true;
}



//获取接收帐号
char * CShortMsg::GetRcvAccount()
{
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_EX_RecvAccount,dLen);
		if(temp == NULL)
			return NULL;
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
	
}


//设置接收帐号
BOOL CShortMsg::SetRcvAccount(const char *pName)
{
	if(NULL == pName)
		return false;
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_EX_RecvAccount, strlen(pName), (BYTE*)pName);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_EX_RecvAccount, strlen(pName), (BYTE*)pName);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息中的计费号码
char * CShortMsg::GetFeeAddr()
{
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_Fee_termial_id,dLen);
		if(temp == NULL)
			return NULL;
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
	
}



//重新设置计费号码
BOOL CShortMsg::SetFeeAddr(char *pAddr)
{
	if(NULL == pAddr)
		return false;

	int len = strlen(pAddr);
	if (len > 21) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Fee_termial_id, len, (BYTE*)pAddr);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Fee_termial_id, len, (BYTE*)pAddr);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息中的计费类型
char * CShortMsg::GetFeeType()
{
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_FeeType,dLen);
		if(temp == NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;	
	
}


//设置消息中的计费类型
bool CShortMsg::SetFeeType(int FeeType)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_FeeType, 2, (BYTE*)&FeeType);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_FeeType, 2, (BYTE*)&FeeType);
		SetNewTLV(&tlv);
	}
	
	return true;
}



// 获取定购处理中配置的计费类型
//DEL char* CShortMsg::GetServiceFeeType()
//DEL {
//DEL 	if( m_cTlv!=NULL)
//DEL 	{
//DEL 		DWORD dLen;
//DEL 		char * ret;
//DEL 		
//DEL 		const BYTE *temp;
//DEL 		temp=m_cTlv->GetItem(Tag_ServiceFeeType, dLen);
//DEL 		if(temp==NULL)
//DEL 			return NULL;
//DEL 		
//DEL 		ret=(char*)temp;
//DEL 		return ret;
//DEL 		
//DEL 	}
//DEL 	else
//DEL 		return NULL;
//DEL }


// 设置定购处理中配置的计费类型
//DEL bool CShortMsg::SetServiceFeeType(char* ServiceFeeType)
//DEL {
//DEL 	if(NULL == ServiceFeeType)
//DEL 		return false;
//DEL 	
//DEL 	if( m_cTlv != NULL)
//DEL 	{
//DEL 		m_cTlv->SetItem(Tag_ServiceFeeType, 2, (BYTE*)ServiceFeeType);
//DEL 	}
//DEL 	else
//DEL 	{
//DEL 		CTLV tlv;
//DEL 		tlv.AddItem(Tag_ServiceFeeType, 2, (BYTE*)ServiceFeeType);
//DEL 		SetNewTLV(&tlv);
//DEL 	}
//DEL 	
//DEL 	return true;
//DEL }


// 获取业务代码
char* CShortMsg::GetServiceID()
{
	if( m_cTlv != NULL)
	{
		DWORD dLen;
		char* ret;
		
		const BYTE * temp;
		temp = m_cTlv->GetItem(Tag_ServiceID,dLen);
		if(temp == NULL)
			return 0;
		
		ret = (char*)temp;
		return ret;
	}
	else
		return 0;
}


//设置业务代码
bool CShortMsg::SetServiceID(const char* ServiceID)
{
	if(NULL == ServiceID)
		return false;

	int len = strlen(ServiceID);
	if (len > 10) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_ServiceID, len, (BYTE*)ServiceID);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_ServiceID, len, (BYTE*)ServiceID);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//设置消息提交时间
BOOL CShortMsg::SetSubTime(const char * pTime)
{
	if(NULL == pTime)
		return false;	
	
	if(strlen(pTime) >= MAX_TIMESTAMP_LEN)
	{
		return FALSE;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_EX_SubTime, strlen(pTime), (BYTE*)pTime);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_EX_SubTime, strlen(pTime), (BYTE*)pTime);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息提交时间
char * CShortMsg::GetSubTime()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_EX_SubTime, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//设置消息的处理结束时间
BOOL CShortMsg::SetDlrTime(const char * pTime)
{
	if(NULL == pTime)
		return false;

	if(strlen(pTime) >= MAX_TIMESTAMP_LEN)
	{
		return FALSE;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_EX_DlrTime, strlen(pTime), (BYTE*)pTime);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_EX_DlrTime, strlen(pTime), (BYTE*)pTime);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息的处理结束时间
char *  CShortMsg::GetDlrTime()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_EX_DlrTime, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}



//SMGW45-15, 2006-2-8, ZF modi begin//

//Tag_LocalFwdMsgid 设置下一级网关MsgID
bool CShortMsg::SetLocalFwdMsgid(char* LocalFwdMsgid)
{
	if(NULL == LocalFwdMsgid)
		return false;
	
	int len = strlen(LocalFwdMsgid);
	if (len > 64) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_LocalFwdMsgid, len, (BYTE*)LocalFwdMsgid);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_LocalFwdMsgid, len, (BYTE*)LocalFwdMsgid);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//Tag_LocalFwdMsgid 下一级网关MsgID
char* CShortMsg::GetLocalFwdMsgid()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_LocalFwdMsgid, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//add by wj
bool CShortMsg::SetSourceGwName(const char* name)
{

	if(NULL == name)
		return false;
	
	int len = strlen(name);
	if (len > 30) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Ex_SourceGWName, len+1, (BYTE*)name);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Ex_SourceGWName, len+1, (BYTE*)name);
		SetNewTLV(&tlv);
	}
	
	return true;	
}
const char* CShortMsg::GetSourceGwName()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_Ex_SourceGWName, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;

}

bool CShortMsg::SetDestGwName(const char* name)
{
	if(NULL == name)
		return false;
	
	int len = strlen(name);
	if (len > 30) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Ex_DestGWName, len+1, (BYTE*)name);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Ex_DestGWName, len+1, (BYTE*)name);
		SetNewTLV(&tlv);
	}
	return true;
}

const char* CShortMsg::GetDestGwName()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_Ex_DestGWName, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}

//设置上级网关代码
bool CShortMsg::SetFromNo(char* FromNo)
{
	if(NULL == FromNo)
		return false;
	
	int len = strlen(FromNo);
	if (len > 7) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_FromNo, len, (BYTE*)FromNo);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_FromNo, len, (BYTE*)FromNo);
		SetNewTLV(&tlv);
	}
	
	return true;	
}

//获取上级网关代码
char* CShortMsg::GetFromNo()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_FromNo, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}

//设置下级网关代码
bool CShortMsg::SetToNo(char *ToNo)
{
	if(NULL == ToNo)
		return false;
	
	int len = strlen(ToNo);
	if (len > 7) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_ToNo, len, (BYTE*)ToNo);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_ToNo, len, (BYTE*)ToNo);
		SetNewTLV(&tlv);
	}
	
	return true;
}
//获取下级网关代码
char* CShortMsg::GetToNo()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_ToNo, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}

//设置源SourceID
bool CShortMsg::SetSourceID(char *SourceID)
{
	if(NULL == SourceID)
		return false;
	
	int len = strlen(SourceID);
	if (len > 26) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_SOURCEID, len, (BYTE*)SourceID);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_SOURCEID, len, (BYTE*)SourceID);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取源SourceID
char* CShortMsg::GetSourceID()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_SOURCEID, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
	}
	else
		return NULL;
}

//设置消息的总段数
bool CShortMsg::SetPk_Total(UC Pk_Total)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Pk_Total, 1, (BYTE*)&Pk_Total);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Pk_Total, 1, (BYTE*)&Pk_Total);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息的总段数
UC  CShortMsg::GetPk_Total()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		UC ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_Pk_Total,dLen);
		if(temp == NULL)
			return 0;
		
		ret=*((UC*)temp);
		return ret;
	}
	else
		return 0;
	
}

//设置消息分段后的段号
bool CShortMsg::SetPk_Number(UC Pk_Number)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Pk_Number, 1, (BYTE*)&Pk_Number);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Pk_Number, 1, (BYTE*)&Pk_Number);
		SetNewTLV(&tlv);
	}
	
	return true;
}

//获取消息分段后的段号
UC  CShortMsg::GetPk_Number()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		UC ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_Pk_Number,dLen);
		if(temp == NULL)
			return 0;
		
		ret=*((UC*)temp);
		return ret;
	}
	else
		return 0;
}
//SMGW45-15, 2006-2-8, ZF modi end//

//Tag_SMID 设置Smid
bool CShortMsg::SetSmid(char* Smid)
{
	if(NULL == Smid)
		return false;

	int len = strlen(Smid);
	if (len > 64) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_SMID, len, (BYTE*)Smid);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_SMID, len, (BYTE*)Smid);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//Tag_SMID 获取Smid
char* CShortMsg::GetSmid()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_SMID, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//Tag_UNIKEY，获取Unikey
char * CShortMsg::GetUnikey()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_UNIKEY, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//Tag_UNIKEY，设置Unikey
bool CShortMsg::SetUnikey(char* Unikey)
{
	if(NULL == Unikey)
		return false;

	int len = strlen(Unikey);
	if (len > 64) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_UNIKEY, len, (BYTE*)Unikey);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_UNIKEY, len, (BYTE*)Unikey);
		SetNewTLV(&tlv);
	}
	
	return true;
}



//获取企业代码
char * CShortMsg::GetCPID()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_Ex_CPID, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}

//设置企业代码
bool CShortMsg::SetCPID(const char* CPID)
{
	if(NULL == CPID)
		return false;

	int len = strlen(CPID);
	if (len > 10) 
	{
		ASSERT(0);
		return false;
	}	
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Ex_CPID, len, (BYTE*)CPID);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Ex_CPID, len, (BYTE*)CPID);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//Tag_SmcNo，获取短信中心代码
char * CShortMsg::GetSMCNO()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_SmcNo, dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//Tag_SmcNo，设置短信中心代码
bool CShortMsg::SetSMCNO(char* SMCNO)
{
	if(NULL == SMCNO)
		return false;

	int len = strlen(SMCNO);
	if (len > 6) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_SmcNo, len, (BYTE*)SMCNO);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_SmcNo, len, (BYTE*)SMCNO);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息中的SMType
UC CShortMsg::GetSMType()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		UC  ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_SMType, dLen);
		if(temp==NULL)
			return 0;
		
		ret= *((UC*)temp);
		return ret;
		
	}
	else
		return 0;
}


//设置消息中的SMType
bool CShortMsg::SetSMType(UC SMType)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_SMType, 1, (BYTE*)&SMType);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_SMType, 1, (BYTE*)&SMType);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取预付费扣费应答消息中的确认标记
UC CShortMsg::GetReslutNotifyCode()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		UC  ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_ReslutNotifyCode, dLen);
		if(temp==NULL)
			return 0;
		
		ret= *((UC*)temp);
		return ret;
		
	}
	else
		return 0;
}


//设置预付费扣费应答消息中的确认标记
bool CShortMsg::SetReslutNotifyCode(UC ReslutNotifyCode)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_ReslutNotifyCode, 1, (BYTE*)&ReslutNotifyCode);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_ReslutNotifyCode, 1, (BYTE*)&ReslutNotifyCode);
		SetNewTLV(&tlv);
	}
	
	return true;
}




//获取消息的计费点标记
int  CShortMsg::GetFeeDefine()
{
	
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_Fee_Define);
	}
	else
	{
		return 0;
	}
}


//设置消息的计费点标记
bool CShortMsg::SetFeeDefine(int FeeDefine)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_Fee_Define, FeeDefine);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_Fee_Define, FeeDefine);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//设置订购关系处理中的操作类型
int  CShortMsg::GetOperatorType()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_OperatorType);
	}
	else
	{
		return 0;
	}
}


//设置定购关系处理中操作类型
bool CShortMsg::SetOperatorType(int OperatorType)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_OperatorType, OperatorType);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_OperatorType, OperatorType);
		SetNewTLV(&tlv);
	}

	return true;
}



// 获取定购处理中配置的SP的信用度
int CShortMsg::GetSPCredit()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_SPCredit);
	}
	else
	{
		return 0;
	}
}


//设置定购处理中配置的SP的信用度
bool CShortMsg::SetSPCredit(int SPCredit)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_SPCredit, SPCredit);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_SPCredit, SPCredit);
		SetNewTLV(&tlv);
	}

	return true;
}





// 获取定购处理中业务配置中能下发的最大条数
int CShortMsg::GetMaxMTCount()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_MaxMTCount);
	}
	else
	{
		return 0;
	}
}


// 设置定购处理中业务配置中能下发的最大条数
bool CShortMsg::SetMaxMTCount(int MaxMTCount)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_MaxMTCount, MaxMTCount);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_MaxMTCount, MaxMTCount);
		SetNewTLV(&tlv);
	}

	return true;
}


// 获取定购处理中业务配置的生存期
//DEL int CShortMsg::GetServiceValidTime()
//DEL {
//DEL 	if( m_cTlv != NULL)
//DEL 	{
//DEL 		return m_cTlv->GetLongIntItemValue(Tag_ServiceValidTime);
//DEL 	}
//DEL 	else
//DEL 	{
//DEL 		return 0;
//DEL 	}
//DEL }


// 设置定购处理中业务配置的生存期
//DEL bool CShortMsg::SetServiceValidTime(int ServiceValidTime)
//DEL {
//DEL 	if( m_cTlv != NULL)
//DEL 	{
//DEL 		m_cTlv->SetLongIntItem(Tag_ServiceValidTime, ServiceValidTime);
//DEL 	}
//DEL 	else
//DEL 	{
//DEL 		CTLV tlv;
//DEL 		tlv.AddLongIntItem(Tag_ServiceValidTime, ServiceValidTime);
//DEL 		SetNewTLV(&tlv);
//DEL 	}
//DEL 
//DEL 	return true;
//DEL }


// 获取定购处理中定购关系总个数
int CShortMsg::GetOrderPriceCount()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_OrderPriceCount);
	}
	else
	{
		return 0;
	}
}


// 设置定购处理中定购关系总个数
bool CShortMsg::SetOrderPriceCount(int OrderPriceCount)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_OrderPriceCount, OrderPriceCount);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_OrderPriceCount, OrderPriceCount);
		SetNewTLV(&tlv);
	}

	return true;
}



//设置消息中计费号码的区号
bool CShortMsg::SetAreaCode(char* AreaCode)
{
	if(NULL == AreaCode)
		return false;
	
	int len = strlen(AreaCode);
	if(len > 4)
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_Ex_AreaCode, len, (BYTE*)AreaCode);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_Ex_AreaCode, len, (BYTE*)AreaCode);
		SetNewTLV(&tlv);
	}
	
	return true;
}

//获取消息中计费号码的区号
char* CShortMsg::GetAreaCode()
{
	
	if( m_cTlv != NULL)
	{
		DWORD dLen;
		char* ret;
		
		const BYTE * temp;
		temp = m_cTlv->GetItem(Tag_Ex_AreaCode,dLen);
		if(temp == NULL)
			return 0;
		
		ret = (char*)temp;
		return ret;
	}
	else
		return 0;
	
	
}

// 获取定购处理中鉴权模块产生的鉴权ID
char* CShortMsg::GetAuthPriceID()
{
	if( m_cTlv != NULL)
	{
		DWORD dLen;
		char* ret;
		
		const BYTE * temp;
		temp = m_cTlv->GetItem(Tag_AuthPriceID,dLen);
		if(temp == NULL)
			return 0;
		
		ret = (char*)temp;
		return ret;
	}
	else
		return 0;
}

int CShortMsg::AddSign(int head,const char* psign ,int len)
{
	if(len==0)
	{
		VLOG(5)<<"AddSign len==0 pass";
		return 0;
	}
	if(m_wlongmsg.size())//httpmsg
	{
		CCodeAndDecode code;
		std::wstring wzSign = code.ConvertStringToWString(std::string(psign,len));
		if(head==0)
		{
			VLOG(5)<<"AddSign http msg at tail";
			m_wlongmsg+=wzSign;
		}
		else
		{
			VLOG(5)<<"AddSign http msg at head";
			m_wlongmsg = m_wlongmsg+wzSign;
		}
		return 0;
	}
	else
	{
			BYTE dcs = GetMsgFormat();
			int hlen = 0;
			if(GetSMData()->EmsClass&ESM_CLASS_UDHI)
			{
				hlen = GetMsgContent()[0]+1;//sm head len ,first char is head payload len ,so total head len is char[0] +1;
				VLOG(5)<<"AddSign  msg  head len "<< hlen;
			}
			if(hlen>140)
			{
				LOG(ERROR)<<"error sm header head len > 140  "<< hlen;
				return -1;
			}
			VLOG(8)<<"AddSign   dcs "<<std::hex<<(int)dcs;
			std::vector<char> vsign(psign,psign+len);
			if((dcs&0x0C)==8)
			{//unicode
				CCodeAndDecode code;
				std::wstring wzSign = code.ConvertStringToWString(std::string(psign,len));
				std::wstring nwzSign = code.ConvertWStringToNWString(wzSign);				
				vsign.resize(nwzSign.size()*2);
				memcpy(&vsign[0],&nwzSign[0],vsign.size());
			}
			if(hlen + vsign.size()>140)
			{
				LOG(ERROR)<<"error sm header head len+signlen  > 140  ["<< hlen<<"][" <<vsign.size()<<"]" ;
				return -2;
			}

			if(head==0)
			{
				int smlen = GetSMLength();
				VLOG(5)<<"shortmsg add tail sign start at "<<smlen << "; len ="<<min(vsign.size(),140-smlen);				
				memcpy(GetMsgContent()+smlen,&vsign[0],min(vsign.size(),140-smlen));
				m_pSMData->SMLength = min(vsign.size()+smlen,140);
			}
			else
			{
				int smlen = GetSMLength();
				VLOG(5)<<"shortmsg add head sign move start at "<<hlen<< "; to "<<hlen+vsign.size() <<"; len ="<<min(140-hlen-vsign.size(),smlen-hlen);
				memmove(GetMsgContent()+hlen+vsign.size(),GetMsgContent()+hlen,min(140-hlen-vsign.size(),smlen-hlen));
				memcpy(GetMsgContent()+hlen,&vsign[0],vsign.size());
				m_pSMData->SMLength = min(smlen+vsign.size(),140);
			}
			VLOG(5)<<"shortmsg add head sign total len="<<m_pSMData->SMLength;
			
	}
	return 0;
}


// 设置定购处理中鉴权模块产生的鉴权ID
bool CShortMsg::SetAuthPriceID(char* AuthPriceID)
{
	if(NULL == AuthPriceID)
		return false;

	int len = strlen(AuthPriceID);
	if (len > 65) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_AuthPriceID, len, (BYTE*)AuthPriceID);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_AuthPriceID, len, (BYTE*)AuthPriceID);
		SetNewTLV(&tlv);
	}
	
	return true;
}


// 获取是否转发状态报告
int CShortMsg::GetReportSendFlag()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_ReportSendFlag);
	}
	else
	{
		return 0;
	}
}


void CShortMsg::SetReportSendFlag(int ReportFlag)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_ReportSendFlag, ReportFlag);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_ReportSendFlag, ReportFlag);
		SetNewTLV(&tlv);
	}
}


// 设置定购处理中定购关系列表
bool CShortMsg::SetOrderPriceBuf(char* OrderPriceBuf)
{
	if(NULL == OrderPriceBuf)
		return false;
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_OrderPriceBuf, strlen(OrderPriceBuf), (BYTE*)OrderPriceBuf);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_OrderPriceBuf, strlen(OrderPriceBuf), (BYTE*)OrderPriceBuf);
		SetNewTLV(&tlv);
	}
	
	return true;
}


// 获取定购处理中定购关系列表
char* CShortMsg::GetOrderPriceBuf()
{
	if( m_cTlv != NULL)
	{
		DWORD dLen;
		char* ret;
		
		const BYTE * temp;
		temp = m_cTlv->GetItem(Tag_OrderPriceBuf,dLen);
		if(temp == NULL)
			return 0;
		
		ret = (char*)temp;
		return ret;
	}
	else
		return 0;
}



//获取消息中的按条资费
char *  CShortMsg::GetFeeCode()
{ 
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_FeeCode,dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//设置消息中的按条资费
bool CShortMsg::SetFeeCode(char* FeeCode)
{
	if(NULL == FeeCode)
		return false;

	int len = strlen(FeeCode);
	if (len > 6) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_FeeCode, len, (BYTE*)FeeCode);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_FeeCode, len, (BYTE*)FeeCode);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//获取消息中的包月/封顶资费
char *  CShortMsg::GetFixFee()
{ 
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_FixedFee,dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//设置消息中的包月/封顶资费
bool CShortMsg::SetFixFee(char* FixFee)
{
	if(NULL == FixFee)
		return false;

	int len = strlen(FixFee);
	if (len > 6) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_FixedFee, len, (BYTE*)FixFee);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_FixedFee, len, (BYTE*)FixFee);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//DEL char *  CShortMsg::GetGivenCode()
//DEL { 
//DEL 	if( m_cTlv!=NULL)
//DEL 	{
//DEL 		
//DEL 		DWORD dLen;
//DEL 		char * ret;
//DEL 		
//DEL 		const BYTE *temp;
//DEL 		temp=m_cTlv->GetItem(Tag_GivenCode,dLen);
//DEL 		if(temp==NULL)
//DEL 			return NULL;
//DEL 		ret=(char*)temp;
//DEL 		return ret;
//DEL 		
//DEL 	}
//DEL 	else
//DEL 		return NULL;	
//DEL }




BOOL CShortMsg::SetNewTLV(CTLV *pTLV)
{
	char extdata[ STREAM_LEN_MAX + 1];
	memset(extdata, 0, sizeof(extdata));
	DWORD esize=0;
	
	pTLV->ToStream( (BYTE*)extdata, esize );
	
	if( m_cTlv != NULL )
		delete m_cTlv;
	
	m_cTlv = new CTLV;
	
	DWORD nlen=0;
	m_cTlv->FromStream( (BYTE*)extdata, esize, nlen );

	return TRUE;	
}



//恢复号码
void CShortMsg::ResetAddr()
{
	SetOrgAddr( m_CallingNo );
	SetDestAddr( m_CalledNo );
	SetFeeAddr( m_FeeCallNo );
}

//备份号码
void CShortMsg::BakeAddr()
{
	if(m_pSMData)
	{
		strncpy(this->m_CallingNo , m_pSMData->SndMSISDN,MAX_MSISDN_LEN);
		strncpy(this->m_CalledNo , m_pSMData->RcvMSISDN,MAX_MSISDN_LEN); 
	}

	
	//备份计费号码
	char* pFeeAddr = GetFeeAddr();
	if(pFeeAddr)
		strcpy(this->m_FeeCallNo , pFeeAddr);
}


void CShortMsg::BackupServiceInfo()
{

	char* pOrgServceCode = GetServiceID();
	if(pOrgServceCode)
		m_orgservicecode = pOrgServceCode;
	char* pOrgCPID = GetCPID();

	if(pOrgCPID)
	{
		m_orgspcode = pOrgCPID;
	}
}

void CShortMsg::RestoreServiceInfo()
{
	SetServiceID(m_orgservicecode.c_str());
	SetCPID(m_orgspcode.c_str());
}


//将CShortMsg类中成员数据以二进制数据形式保存到文件中
//调用时请将参数pbyStream设置为空指针
//当返回值为TRUE时候
//		输出 用new分配的一段字串pbyStream，dwLen指明了字串的长度
//		用完后需要delete[] pbyStream
//当返回值为FALSE时候 pbyStream为NULL, dwLen为0
BOOL CShortMsg::ToStream( BYTE*& pbyStream, DWORD& dwLen)
{
	if(pbyStream)
	{
		ASSERT(0);
		return FALSE;
	}

	SHORTMSG*  pShortMsg = NULL;

	try
	{
		BYTE* pbuf = new BYTE[sizeof(SHORTMSG)];
		memset(pbuf, 0, sizeof(SHORTMSG));	

		pShortMsg = (SHORTMSG*)pbuf;		

		strncpy(pShortMsg->m_CalledNo, m_CalledNo, sizeof(pShortMsg->m_CalledNo) - 1);
		strncpy(pShortMsg->m_CallingNo, m_CallingNo, sizeof(pShortMsg->m_CallingNo) - 1);
		
		pShortMsg->m_nExtDataSize = 0;
		if(m_cTlv) 
		{
			DWORD nExtDataSize = 0;
			m_cTlv->ToStream((BYTE*)pShortMsg->m_ExtData, nExtDataSize);
			pShortMsg->m_nExtDataSize = nExtDataSize;
		}
		
		pShortMsg->m_DealStep = m_DealStep;
		strncpy(pShortMsg->m_FeeCallNo, m_FeeCallNo, sizeof(pShortMsg->m_FeeCallNo) - 1);
		//pShortMsg->m_IsReport = m_IsReport;
		pShortMsg->m_nReSubmitCount = m_nReSubmitCount;
		pShortMsg->m_nSubmitTime = m_nSubmitTime;
		
		if (m_pSMData)
		{
			memcpy(&(pShortMsg->m_SMData), m_pSMData, sizeof(SM_STRUCT) );
		}
		
		pShortMsg->m_SendCmdID = m_SendCmdID;
		pShortMsg->m_SendMsgFlag = m_SendMsgFlag;
		pShortMsg->m_ulCmdID = m_ulCmdID;
		pShortMsg->m_ulRecverID = m_ulRecverID;
		pShortMsg->m_ulSenderID = m_ulSenderID;
		pShortMsg->m_ulSequenceID = m_ulSequenceID;	
		
		dwLen = sizeof(SHORTMSG);		
		pbyStream = pbuf;

		return true;
	}
	catch(...)
	{
		CString log;
		log.Format("CShortMsg::FromStream()异常");
		GetSmsApp()->WriteTestRecord(log, 0);	
		
		dwLen = 0;		
		pbyStream = NULL;

		if(pShortMsg)
		{
			delete pShortMsg;
			pShortMsg = NULL;
		}

		return false;
	}
}


//从文件中读取数据到CShortMsg类中成员数据
BOOL CShortMsg::FromStream( const BYTE* pbyStream, const DWORD& dwLen)
{
	if(!pbyStream)
	{
		return false;
	}

	int step = 0;

	SHORTMSG* pShortMsg = (SHORTMSG*)pbyStream;

	try
	{
		memset(m_CalledNo, 0, sizeof(m_CalledNo));
		strncpy(m_CalledNo, pShortMsg->m_CalledNo, sizeof(m_CalledNo) - 1);
		step++;

		memset(m_CallingNo, 0, sizeof(m_CallingNo));
		strncpy(m_CallingNo, pShortMsg->m_CallingNo, sizeof(m_CallingNo) - 1);
		step++;

		if(m_cTlv)
		{
			delete m_cTlv;
			m_cTlv = NULL;
		}
		step++;
		if(pShortMsg->m_nExtDataSize > 0)
		{
			m_cTlv = new CTLV;
			DWORD nlen=0;
			m_cTlv->FromStream((BYTE*)(pShortMsg->m_ExtData), pShortMsg->m_nExtDataSize, nlen);
		}
		step++;
		m_DealStep = pShortMsg->m_DealStep;

		memset(m_FeeCallNo, 0, sizeof(m_FeeCallNo));
		strncpy(m_FeeCallNo, pShortMsg->m_FeeCallNo, sizeof(m_FeeCallNo) - 1);
		step++;
		//m_IsReport = pShortMsg->m_IsReport;
		m_nReSubmitCount = pShortMsg->m_nReSubmitCount;
		m_nSubmitTime = pShortMsg->m_nSubmitTime;
		step++;
		if(!m_pSMData)
		{
			m_pSMData = new SM_STRUCT;
		}
		step++;
		memset(m_pSMData, 0, sizeof(m_pSMData));
		memcpy(m_pSMData, &(pShortMsg->m_SMData), sizeof(SM_STRUCT) );
		step++;
		m_SendCmdID = pShortMsg->m_SendCmdID;
		m_SendMsgFlag = pShortMsg->m_SendMsgFlag;
		step++;
		m_ulCmdID = pShortMsg->m_ulCmdID;
		m_ulRecverID = pShortMsg->m_ulRecverID;
		m_ulSenderID = pShortMsg->m_ulSenderID;
		m_ulSequenceID = pShortMsg->m_ulSequenceID;	
		step++;
	}
	catch(...)
	{
		CString log;
		log.Format("CShortMsg::FromStream()异常 step %d ",step);
		GetSmsApp()->WriteTestRecord(log, 0);

		return false;
	}

	return true;
}



//获取鉴权错误码
long int CShortMsg::GetAuthErrCode()
{
	if (m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_AuthErrCode);
	}
	else
	{
		return 0;
	}
}

//设置鉴权错误码
bool CShortMsg::SetAuthErrCode(long int AuthErrCode)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_AuthErrCode, AuthErrCode);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_AuthErrCode, AuthErrCode);
		SetNewTLV(&tlv);
	}
	
	return true;
}


// SMGW251-29, 2007-3-26, wzy add begin //
//设置SP通知消息标记
bool CShortMsg::SetNotifyFlag(long int nNotifyFlag)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_NotifyMsg, nNotifyFlag);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_NotifyMsg, nNotifyFlag);
		SetNewTLV(&tlv);
	}
	
	return true;
}
// SMGW251-29, 2007-3-26, wzy add end //


//获取是否支持预付费
int CShortMsg::GetPayBeforeSupport()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_PayBeforeSupport);
	}
	else
	{
		return 0;
	}
}


//设置是否支持预付费
void CShortMsg::SetPayBeforeSupport(int PayBeforeSupport)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_PayBeforeSupport, PayBeforeSupport);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_PayBeforeSupport, PayBeforeSupport);
		SetNewTLV(&tlv);
	}
}


//获取后付费用户是否走预付费流程
int CShortMsg::GetAferUserPrePay()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_PayFlow);
	}
	else
	{
		return 0;
	}
}


//设置后付费用户是否走预付费流程
void CShortMsg::SetAferUserPrePay(int FeeFlow)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_PayFlow, FeeFlow);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_PayFlow, FeeFlow);
		SetNewTLV(&tlv);
	}
}



//获取计费用户的预付费类型：0：预付费 2：后付费
int CShortMsg::GetFeeUserPayType()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_FeeUserPayType);
	}
	else
	{
		return 0;
	}
}


//设置计费用户的预付费类型，0：预付费 2：后付费
void CShortMsg::SetFeeUserPayType( int FeeUserPayType )
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_FeeUserPayType, FeeUserPayType);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_FeeUserPayType, FeeUserPayType);
		SetNewTLV(&tlv);
	}
}


//设置消息处理的状态值
void CShortMsg::SetStatus(int iStatus)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetLongIntItem(Tag_Status, iStatus);
	}
	else
	{
		CTLV tlv;
		tlv.AddLongIntItem(Tag_Status, iStatus);
		SetNewTLV(&tlv);
	}
}


//获取消息处理的状态值
int CShortMsg::GetStatus()
{
	if( m_cTlv != NULL)
	{
		return m_cTlv->GetLongIntItemValue(Tag_Status);
	}
	else
	{
		return 0;
	}
}



//Tag_DestSMGNo，设置目的网关代码
bool CShortMsg::SetDestSMGNO(char* pDestSMGNo)
{
	if(NULL == pDestSMGNo)
		return false;

	int len = strlen(pDestSMGNo);
	if (len > 6) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_DestSMGNo, len, (BYTE*)pDestSMGNo);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_DestSMGNo, len, (BYTE*)pDestSMGNo);
		SetNewTLV(&tlv);
	}
	
	return true;
}

//Tag_DestSMGNo，获取目的网关代码
char *  CShortMsg::GetDestSMGNO()
{ 
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_DestSMGNo,dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


//Tag_SrcSMGNo，设置源网关代码
bool CShortMsg::SetSrcSMGNO(char* pSrcSMGNo)
{
	if(NULL == pSrcSMGNo)
		return false;

	int len = strlen(pSrcSMGNo);
	if (len > 6) 
	{
		ASSERT(0);
		return false;
	}
	
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_SrcSMGNo, len, (BYTE*)pSrcSMGNo);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_SrcSMGNo, len, (BYTE*)pSrcSMGNo);
		SetNewTLV(&tlv);
	}
	
	return true;
}


//Tag_SrcSMGNo，获取源网关代码
char *  CShortMsg::GetSrcSMGNO()
{ 
	if( m_cTlv!=NULL)
	{
		
		DWORD dLen;
		char * ret;
		
		const BYTE *temp;
		temp=m_cTlv->GetItem(Tag_SrcSMGNo,dLen);
		if(temp==NULL)
			return NULL;
		
		ret=(char*)temp;
		return ret;
		
	}
	else
		return NULL;
}


UC CShortMsg::GetCongestionState()
{
	if( m_cTlv!=NULL)
	{
		DWORD dLen;
		UC ret;
		
		const BYTE *temp;
		temp = m_cTlv->GetItem(Tag_CongestionState,dLen);
		if(temp == NULL)
			return 0;
		
		ret=*((UC*)temp);
		return ret;
	}
	else
		return 0;

}

void CShortMsg::SetCongestionState(UC CgsState)
{
	if( m_cTlv != NULL)
	{
		m_cTlv->SetItem(Tag_CongestionState, 1, (BYTE*)&CgsState);
	}
	else
	{
		CTLV tlv;
		tlv.AddItem(Tag_CongestionState, 1, (BYTE*)&CgsState);
		SetNewTLV(&tlv);
	}

}

void CShortMsg::Test()
{
	SetCongestionState(12) ;	
	SetAuthErrCode(13);	
	SetReportSendFlag(14);
	SetMaxMTCount(15);
	SetOperatorType(17);
	SetSPCredit(18);
	SetServiceID("ServiceID0");
	SetSendMsgFlag(19);
	SetAuthPriceID("AuthPriceID");
	SetSmid("Smid");
	SetAreaCode("0755");
	SetOrderPriceCount(20);
	SetOrderPriceBuf("OrderPriceBuf");
	SetReslutNotifyCode(23);
	SetSourceID("SourceID");
	SetCPID("CPID123456");
	SetSMCNO("SMCNO");
	SetSMType(24);
	SetUnikey("Unikey");
	SetPriority(25);
	SetMsgContent("MsgContent");
	SetDestAddr("DestAddr");
	SetOrgAddr("OrgAddr");
//	SetServiceID("ServiceID");
	SetFeeUserType(26);
	SetFeeUserPayType(27);
	SetStatus(28);
	SetFeeAddr("FeeAddr");
	SetGiftAddr("GiftAddr");
	SetFeeType(1);
	SetFeeCode("299999");
	SetFixFee("309999");
	SetMTFlag(31);
	SetSMMsgType(31);
	SetSendToFeeFlag(32);
	SetInnerMsgType(33);
	SetSubTime("SubTime");
	SetDlrTime("DlrTime");
	SetFeeDefine(34);
	SetSendAccount("SendAccount");	
	SetRcvAccount("RcvAccount");	
	SetSendCmdID(35);	
	SetSrcSMGNO("777777");
	SetDestSMGNO("666666");
	SetAferUserPrePay(36);
	SetPayBeforeSupport(37);

	ASSERT(GetCongestionState()==12);
	ASSERT(strcmp(GetSMCNO(),"SMCNO")==0);
	ASSERT(GetSMType() == 24);
	ASSERT(strcmp(GetUnikey(),"Unikey")==0);
	ASSERT(GetPriority()==25);
	ASSERT(strcmp(GetMsgContent(),"MsgContent")==0);	
	ASSERT(strcmp(GetDestAddr(),"DestAddr")==0);
	ASSERT(strcmp(GetOrgAddr(),"OrgAddr")==0);
//	ASSERT(strcmp(GetServiceID(),"ServiceID")==0);
	ASSERT(GetFeeUserType()==26);
	ASSERT(GetFeeUserPayType()==27);
	ASSERT( GetStatus() == 28);
	ASSERT(strcmp(GetFeeAddr(),"FeeAddr")==0);
	ASSERT(strcmp(GetGiftAddr(),"GiftAddr")==0);
	char* pFeeType = GetFeeType();
	ASSERT(pFeeType[0] ==1);
	ASSERT(strcmp(GetFeeCode(),"299999")==0);
	ASSERT( GetPayBeforeSupport() == 37);
	ASSERT( GetAferUserPrePay() ==36);
	ASSERT(strcmp(GetDestSMGNO(),"666666")==0);
	ASSERT(strcmp(GetSrcSMGNO(),"777777")==0);
	ASSERT(GetSendCmdID()==35);
	ASSERT(strcmp(GetRcvAccount(),"RcvAccount")==0);	
	ASSERT(strcmp( GetSendAccount(), "SendAccount") ==0);	
	ASSERT( GetFeeDefine() == 34);
	ASSERT(strcmp(GetDlrTime(),"DlrTime")==0);
	ASSERT(strcmp(GetSubTime(),"SubTime")==0);
	ASSERT( GetInnerMsgType()==33);
	ASSERT( GetSMMsgType() == 31);
	ASSERT( GetSendToFeeFlag()==32);
	ASSERT(GetMTFlag()==31);
	ASSERT(strcmp(GetFixFee(),"309999")==0);
	ASSERT(GetAuthErrCode() == 13);
	ASSERT(GetReportSendFlag() == 14);	
	ASSERT(GetMaxMTCount() == 15);
	ASSERT(GetOperatorType() == 17);
	ASSERT(GetSPCredit() == 18);
	ASSERT(strcmp(GetServiceID(), "ServiceID0") == 0);
	ASSERT(GetSendMsgFlag() == 19);
	ASSERT(strcmp(GetAuthPriceID(), "AuthPriceID") == 0);
	ASSERT(strcmp(GetSmid(), "Smid") == 0);
	ASSERT(strcmp(GetAreaCode(), "0755") == 0) ;
	ASSERT(GetOrderPriceCount() == 20);
	ASSERT(strcmp(GetOrderPriceBuf(), "OrderPriceBuf")==0);
	ASSERT(GetReslutNotifyCode() == 23);
	ASSERT(strcmp(GetCPID(),"CPID123456")==0);
		
}


void CShortMsg::GetLongSM_Info(std::string& key /*key identity the long SM*/,
							    int& total_count/*total items in the SM*/,
								int& item_index/*items index*/)
{
	total_count = 0;
	item_index = 0;
	if(!m_pSMData)
		return ;
	if(m_pSMData->SMUD[1]!=0)//no long msg header indicator
		return ;

	if(m_pSMData->SMUD[2]!=3)//erro long msg head len ,must be 3 
		return ;
	//m_pSMData[3];// msg ref

	total_count = m_pSMData->SMUD[4];// msg total count
	item_index = m_pSMData->SMUD[5];//msg index
	key.clear();

	key+=GetDestAddr();
	key+="@";
	key+=GetOrgAddr();
	key+="@";
	key+=std::to_string((long long)m_pSMData->SMUD[3]);
	return ;
	//m_pSMData[5];// msg index
	
}

bool CShortMsg::IsLongSM_Item()
{

	if(!m_pSMData)
		return false;
	char* pMsgSt = m_pSMData->SMUD;

	if(m_pSMData->EmsClass&ESM_CLASS_UDHI)
	{
		BYTE len = *(BYTE*)m_pSMData->SMUD+1;//get header len; "+1" include the headlen one byte
		if(len < m_pSMData->SMLength //total len shound > head len
			&& pMsgSt[1] == 0x00 //first byte "0x00" indicator longmsgtype 
			)
			return TRUE;
	}

	//it's possible not set UDHI 
	if (
		 m_pSMData->SMLength > 5 &&
		(pMsgSt[0] >= 5) &&
		(pMsgSt[1] == 0x00) &&
		(pMsgSt[2] == 3) //longmsg header len
		)
	{
		return TRUE;
	}

	return FALSE;
}
