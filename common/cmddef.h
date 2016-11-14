#ifndef _CMDDEF_H_INCLUDED_
#define _CMDDEF_H_INCLUDED_ 

//�ʺŷ�������
enum varServiceType
{
    SERVICE_NO = 0,
    SERVICE_SMS = 8,						//ҵ������ģ��
    SERVICE_LocalService = 34,				//�ڲ�ҵ������ģ��
    SERVICE_MT = 121,						//ά��ģ��
    SERVICE_FEE =124,					    //�Ʒ�ģ��
    SERVICE_SMG =125,					    //���ؽӿ�
    SERVICE_SMC =128,					    //����Ϣ����ģ��
	SERVICE_PAYMNT =129,					//Ԥ����ƽ̨ģ��
    SERVICE_OTHER_YIDONG_SMG = 130,			//��������ģ��_�ƶ�
    SERVICE_OTHER_LIANTONG_SMG = 131,		//��������ģ��_��ͨ
	SERVICE_CP_SMG = 132,					//CP����ƽ̨
	SERVICE_SPMSGW = 133,					//SPMS��Ȩ����ģ��
	SERVICE_LOGICMNG = 134,					//SPMS�߼�����ģ��
	SERVICE_AUC = 135,						//SPMS��Ȩģ��
	SERVICE_QUEUE_PUB_CPGW = 150,				//QUEUE����ģ�鵽����CPGW
	SERVICE_QUEUE_ROUTE = 151,					//QUEUE��·�ɷ���
	SERVICE_CP_SMG_BALANCE = 152,				//���ؾ���CPGW
	SERVICE_CP_SMG_QUEUE =153,				//CP�ڲ���Ϣ����

    SERVICE_NO_BLANK = 9999,
};

//�ʺ���������
enum AccountServiceType
{
	AccountSerType_NoDefined = 0,
	AccountSerType_SMC = 1,//��������
	AccountSerType_SMGW,//����
	AccountSerType_OtherSMGW,//��������
	AccountSerType_SP,//SP
	AccountSerType_CP_SMG,	//CP��������
	AccountSerType_LOCALFEE,//�Ʒ�ģ��
	AccountSerType_LOGICMNG,//�߼�����ģ��
};

//������������
#define NumberType_Unknow			0
#define NumberType_CP				1
#define NumberType_PHS				2
#define NumberType_OtherTerminal	3


//��Ϣת������
#define SERVICE_MT_TYPE				1
#define SERVICE_MO_TYPE				2
#define SERVICE_MC_TYPE				3
#define SERVICE_MTF_TYPE			4
#define SERVICE_MOF_TYPE			5
#define SERVICE_MCF_TYPE			6
#define SERVICE_MTC_TYPE			7
#define SERVICE_MOC_TYPE			8
#define SERVICE_MCFF_TYPE			9
#define SERVICE_MTFF_TYPE			10
#define SERVICE_MOFF_TYPE			11
#define SERVICE_MCC_TYPE			12
#define SERVICE_MCAF_TYPE			13
#define SERVICE_MCA_TYPE			14
#define SERVICE_MCAFF_TYPE			15
#define SERVICE_MACFF_TYPE			16
#define SERVICE_FWDPAYMSG_TYPE		18
#define SERVICE_NO_TYPE				17
#define SERVICE_FWDQUERYMSG_TYPE	19


//��Ϣ������
#define SMTYPE_MO		 0
#define SMTYPE_MCAF      1
#define SMTYPE_MT        2
#define SMTYPE_MCA       3
#define SMTYPE_MCC       4
#define SMTYPE_MCFF      5  
#define SMTYPE_NO_TYPE   9 


//��ͨ��Ϣ
#define IS_COMMON_MSG(nCommand) (nCommand<100)&&(nCommand>=0)
#define IS_COMMAND_ACK(nCommand) (nCommand & 0x8000)
#define IS_SMPP_ACK(nCommand) (nCommand & 0x80000000)



//����ִ��״̬
#define CMD_STATUS_SUCCESS	0
#define CMD_STATUS_FAIL		1

//BIND ACK��Ϊ�˿�UP��֪ͨ
#define SMS_IF_ACTIVE           64      
//ͨ�ô�����֪ͨ��Ϣ
#define SMS_ERR_MSG				99	

//�����������
#define ERR_TYPE_IF			0
//��ͨ�����붨��
#define ERR_COMMON_BASE		0
#define ERR_NO_ERR			(ERR_COMMON_BASE+0)		//û�д���
#define ERR_BIND_REPEAT		(ERR_COMMON_BASE+1)		//�ظ���¼
#define ERR_IF_NOT_EXIST	(ERR_COMMON_BASE+2)		//�ӿڲ�����
#define ERR_IF_TCP			(ERR_COMMON_BASE+3)		//�����
#define ERR_COMMON_MAX		19
//smc�ӿڴ������
#define ERR_SMCIF_BASE		20
#define ERR_SUBMITSM		(ERR_SMCIF_BASE+0)		//�ύ����Ϣʧ��
#define ERR_PARSER			(ERR_SMCIF_BASE+1)		//������ʧ��
#define ERR_SMCIF_MAX		39


/******************************************************************************************/
/*							ͨ������Ϣ													  */	
/******************************************************************************************/

//�ͻ��˵�ע�ἰ������Ϣ
#define SMS_CLIENT_BIND					1       //��������
#define SMS_CLIENT_BIND_ACK				(SMS_CLIENT_BIND|0x8000)

#define SMS_CLIENT_BIND_REC				0       //��������
#define SMS_CLIENT_BIND_REC_ACK			(SMS_CLIENT_BIND_REC|0x8000)

#define SMS_CLIENT_UNBIND				2       //��������
#define SMS_CLIENT_UNBIND_ACK			(SMS_CLIENT_UNBIND|0x8000)

#define SMS_SHAKEHAND					3       //������Ϣ
#define SMS_SHAKEHAND_ACK				(SMS_SHAKEHAND|0x8000)


//********�й�·���ʺ��Լ����õ������Ϣ******************
#define SMS_QUERY_ONE_CPSERVICE			28      //��ѯһCPҵ��
#define SMS_QUERY_ONE_CPSERVICE_ACK		(SMS_QUERY_ONE_CPSERVICE|0x8000)

#define SMS_QUERY_ALL_CP_ACCOUNTS		29      //��ѯ����CP�ʺŵ�һ������
#define SMS_QUERY_ALL_CP_ACCOUNTS_ACK   (SMS_QUERY_ALL_CP_ACCOUNTS|0x8000)

#define SMS_ADD_ACCOUNT					30      //�����ʺ�
#define SMS_ADD_ACCOUNT_ACK				(SMS_ADD_ACCOUNT|0x8000)

#define SMS_QUERY_ACCOUNT				31      //��ѯ�ʺ�
#define SMS_QUERY_ACCOUNT_ACK			(SMS_QUERY_ACCOUNT|0x8000)

#define SMS_MODI_ACCOUNT				32      //�޸��ʺ�
#define SMS_MODI_ACCOUNT_ACK			(SMS_MODI_ACCOUNT|0x8000)

#define SMS_QUERY_ALL_ACCOUNT			33      //��ѯ�����ʺ���
#define SMS_QUERY_ALL_ACCOUNT_ACK		(SMS_QUERY_ALL_ACCOUNT|0x8000)

#define SMS_DEL_ACCOUNT					34      //ɾ���ʺ�
#define SMS_DEL_ACCOUNT_ACK				(SMS_DEL_ACCOUNT|0x8000)

#define SMS_QUERY_INTERFACE				35      //��ѯ�ӿ�
#define SMS_QUERY_INTERFACE_ACK			(SMS_QUERY_INTERFACE|0x8000)

#define SMS_DEL_INTERFACE				36      //ɾ���ӿ�
#define SMS_DEL_INTERFACE_ACK			(SMS_DEL_INTERFACE|0x8000)

#define SMS_QUERY_ALL_INTERFACE			37      //��ѯ���нӿڵ�ID
#define SMS_QUERY_ALL_INTERFACE_ACK		(SMS_QUERY_ALL_INTERFACE|0x8000)

#define SMS_ADD_INTERFACE				38      //���ӿͻ��˽ӿ�
#define SMS_ADD_INTERFACE_ACK			(SMS_ADD_INTERFACE|0x8000)

#define SMS_ADD_DISTABLE				39      //�Ӽ�·��
#define SMS_ADD_DISTABLE_ACK			(SMS_ADD_DISTABLE|0x8000)

#define SMS_DEL_DISTABLE				40      //ɾ��·��
#define SMS_DEL_DISTABLE_ACK			(SMS_DEL_DISTABLE|0x8000)

#define SMS_QUERY_DISTABLE				41      //��ѯ·��
#define SMS_QUERY_DISTABLE_ACK			(SMS_QUERY_DISTABLE|0x8000)

#define SMS_SET_PARAM					42      //���ò���
#define SMS_SET_PARAM_ACK				(SMS_SET_PARAM|0x8000)

#define SMS_GET_PARAM					43      //���ò���
#define SMS_GET_PARAM_ACK				(SMS_GET_PARAM|0x8000)

#define SMS_LOAD_DISTABLE				44      //��������·��
#define SMS_LOAD_DISTABLE_ACK			(SMS_LOAD_DISTABLE|0x8000)

#define SMS_QUERY_ACCOUNT_EX			45
#define SMS_QUERY_ACCOUNT_EX_ACK		(SMS_QUERY_ACCOUNT_EX | 0x8000)

#define SMS_MODIFY_ACCOUNT_EX			46
#define SMS_MODIFY_ACCOUNT_EX_ACK		(SMS_MODIFY_ACCOUNT_EX | 0X8000)

//CP����
#define SMS_ADD_CPSERVER_EX				47		//���CPҵ��
#define SMS_ADD_CPSERVER_EX_ACK			(SMS_ADD_CPSERVER_EX | 0x8000)
#define SMS_DEL_CPSERVER_EX				48		//ɾ��CPҵ��
#define SMS_DEL_CPSERVER_EX_ACK			(SMS_DEL_CPSERVER_EX | 0X8000)
#define SMS_QUERY_CPSERVER_EX			49		//��ѯCPҵ��
#define SMS_QUERY_CPSERVER_EX_ACK		(SMS_QUERY_CPSERVER_EX | 0X8000)
#define SMS_MODIFY_CPSERVER_EX			50 
#define SMS_MODIFY_CPSERVER_EX_ACK		(SMS_MODIFY_CPSERVER_EX | 0X8000) 

#define SMS_QUERY_ACCOUNT_NUMCHECK		52      //��ѯ�ʺźŶα�
#define SMS_QUERY_ACCOUNT_NUMCHECK_ACK  (SMS_QUERY_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_ADD_ACCOUNT_NUMCHECK		53      //����ʺźŶα�
#define SMS_ADD_ACCOUNT_NUMCHECK_ACK    (SMS_ADD_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_DEL_ACCOUNT_NUMCHECK		54      //ɾ���ʺźŶα�
#define SMS_DEL_ACCOUNT_NUMCHECK_ACK    (SMS_DEL_ACCOUNT_NUMCHECK | 0X8000)

#define SMS_QUERY_ACCOUNT_FLOW			55      //ɾ���ʺźŶα�
#define SMS_QUERY_ACCOUNT_FLOW_ACK		(SMS_QUERY_ACCOUNT_FLOW  | 0X8000)
#define SMS_QUERY_ACCOUNT_SERVICE		56      //ɾ���ʺźŶα�
#define SMS_QUERY_ACCOUNT_SERVICE_ACK   (SMS_QUERY_ACCOUNT_SERVICE  | 0X8000)

#define SMS_QUERY_ACCOUNT_FLOW_ZERO		57      //ɾ���ʺźŶα�
#define SMS_QUERY_ACCOUNT_FLOW_ZERO_ACK    (SMS_QUERY_ACCOUNT_FLOW_ZERO  | 0X8000)
#define SMS_QUERY_ACCOUNT_SERVICE_ZERO  58      //ɾ���ʺźŶα�
#define SMS_QUERY_ACCOUNT_SERVICE_ZERO_ACK    (SMS_QUERY_ACCOUNT_SERVICE_ZERO  | 0X8000)

#define SMS_LOAD_CPSERVER				59                   
#define SMS_LOAD_CPSERVER_ACK			(SMS_LOAD_CPSERVER | 0X8000) 

//*****************����Ϣ***************************
//��Ҫ���ڶ���Ϣ���ķ��ͻ��ѯ
#define SMS_SUBMIT_ADDR					60      
#define SMS_SUBMIT_ADDR_ACK				(SMS_SUBMIT_ADDR|0x8000)       

//��Ҫ�����˹�̨�ķ��ͻ��ѯ       
#define SMS_SUBMIT_SERVICE				61      
#define SMS_SUBMIT_SERVICE_ACK			(SMS_SUBMIT_SERVICE|0x8000)    

//��Ҫ���ڶ���Ϣ�����·���Ϣ          
#define SMS_DELIVER_ADDR				62      
#define SMS_DELIVER_ADDR_ACK			(SMS_DELIVER_ADDR|0x8000)        
      
#define SMS_STATUS_REPORT				63

//��Ҫ���ڶ���Ϣ���ķ��ͻ��ѯ
#define SMS_FWD_ADDR					65      
#define SMS_FWD_ADDR_ACK				(SMS_FWD_ADDR|0x8000)     

//��Ҫ���ڶ���Ϣ���ķ��ͻ��ѯ
#define SMS_SHREPORT_ADDR				66      
#define SMS_SHREPORT_ADDR_ACK			(SMS_SHREPORT_ADDR|0x8000)     

#define SMS_ADD_RED_BLACK				67
#define SMS_ADD_RED_BLACK_ACK			(SMS_ADD_RED_BLACK | 0X8000) 
#define SMS_DEL_RED_BLACK				68
#define SMS_DEL_RED_BLACK_ACK			(SMS_DEL_RED_BLACK | 0X8000) 
#define SMS_QUERY_RED_BLACK				69
#define SMS_QUERY_RED_BLACK_ACK			(SMS_QUERY_RED_BLACK | 0X8000) 
#define SMS_AUTHPRICEREQ				70
#define SMS_AUTHPRICEREQ_RESP			( SMS_AUTHPRICEREQ | 0X8000 )
#define SMS_AUTHPRICECNFMREQ			71
#define SMS_AUTHPRICECNFMREQ_RESP		( SMS_AUTHPRICECNFMREQ | 0X8000 )

//WEB�����/ɾ���ʺ�
#define SMS_WEB_ADDMODI_ACOUNT			72
#define SMS_WEB_ADDMODI_ACOUNT_RESP		(SMS_WEB_ADDMODI_ACOUNT | 0X8000)


//Ԥ����
#define SMS_QUERYUSERSTATE			610							//��ѯ�û�״̬
#define SMS_QUERYUSERSTATE_ACK		(SMS_QUERYUSERSTATE | 0X8000)	
#define SMS_PAYMNTREQUEST			611							//�۷�����
#define SMS_PAYMNTREQUEST_ACK		(SMS_PAYMNTREQUEST | 0X8000)	
#define SMS_FEE_PAYMNTREQUEST		612							//�Ʒ�ģ��۷�����
#define SMS_FEE_PAYMNTREQUEST_ACK	(SMS_FEE_PAYMNTREQUEST | 0X8000)	
#define SMS_FEE_PAYMNTCONFIRM		613							//�Ʒ�ģ��۷�ȷ��
#define SMS_FEE_PAYMNTCONFIRM_ACK	(SMS_FEE_PAYMNTCONFIRM | 0X8000)
#define SMS_FEE_CHECK				626							//�Ʒ�ģ���Check��Ϣ
#define SMS_FEE_CHECK_ACK			(SMS_FEE_CHECK | 0X8000)
#define SMS_FEE_CONFIRM				627							//�Ʒ�ģ���Confirm��Ϣ
#define SMS_FEE_CONFIRM_ACK			(SMS_FEE_CONFIRM | 0X8000)
	  



#define SMS_OMC_BASE                400
#define SMS_ADD_TRACE               (SMS_OMC_BASE + 0)
#define SMS_ADD_TRACE_ACK	        (SMS_ADD_TRACE|0x8000)
#define SMS_TRACE_INFO	            (SMS_OMC_BASE + 1)
#define SMS_CANCEL_TRACE	        (SMS_OMC_BASE + 2)
#define SMS_CANCEL_TRACE_ACK        (SMS_CANCEL_TRACE|0x8000)
#define SMS_ADD_STAT                (SMS_OMC_BASE + 4)
#define SMS_ADD_STAT_ACK            (SMS_ADD_STAT|0x8000)
#define SMS_DEL_STAT                (SMS_OMC_BASE + 5)
#define SMS_DEL_STAT_ACK            (SMS_DEL_STAT|0x8000)
#define SMS_QUERY_STAT              (SMS_OMC_BASE + 6)
#define SMS_QUERY_STAT_ACK          (SMS_QUERY_STAT|0x8000)
#define SMS_GET_STAT_RECORD         (SMS_OMC_BASE + 7)
#define SMS_GET_STAT_RECORD_ACK     (SMS_GET_STAT_RECORD|0x8000)
#define SMS_QUERY_STATS             (SMS_OMC_BASE + 8)
#define SMS_QUERY_STATS_ACK         (SMS_QUERY_STATS|0x8000)
#define SMS_QUERY_SYSTEMTIME        (SMS_OMC_BASE + 9)
#define SMS_QUERY_SYSTEMTIME_ACK    (SMS_QUERY_SYSTEMTIME|0x8000)
#define SMS_MT_GETTASK_RESNUM       (SMS_OMC_BASE + 10)
#define SMS_MT_GETTASK_RESNUM_ACK   (SMS_MT_GETTASK_RESNUM|0x8000)
#define SMS_CLEAR_TASKRES           (SMS_OMC_BASE + 11)
#define SMS_CLEAR_TASKRES_ACK       (SMS_CLEAR_TASKRES|0x8000)
#define SMS_TASK_FAIL               (SMS_OMC_BASE + 12)
#define SMS_TASK_FAIL_ACK           (SMS_TASK_FAIL|0x8000)
#define SMS_FILTER                  (SMS_OMC_BASE + 13)
#define SMS_OMC_TIMER               (SMS_OMC_BASE + 15)
#define SMS_OMC_IF_DELETE           (SMS_OMC_BASE + 16)
#define SMS_OMC_ACCOUNT_DELETE      (SMS_OMC_BASE + 17)
#define SMS_LOAD_LICENSE            (SMS_OMC_BASE + 30)
#define SMS_LOAD_LICENSE_ACK        (SMS_LOAD_LICENSE | 0x8000)


#endif	//_CMDDEF_H_INCLUDED_
