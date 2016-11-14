
// ���¶�������չ����Ϣʵ��͵�������Э��Ĵ���ֵ
//

enum ESME_Error_Code
{
    SMERR_SUCCESS               = 0X00000000,       // �ɹ�
    SMERR_OTHERERR              = 0X00000001,       // ��������
    
    // 0X00000002 - 0X0000000F ����

    SMERR_MSGLENERR             = 0X00000010,       // ��Ϣ���ȴ���
    SMERR_CMDLENERR             = 0X00000011,       // ����ȴ���
    SMERR_INVLDCMDID            = 0X00000012,       // ��ϢID��Ч
    SMERR_NORIGHT               = 0X00000013,       // û��ִ�д������Ȩ��

    // 0X00000014 - 0X0000001F ����

    SMERR_INVLDSYSTEMID         = 0X00000020,       // ��Ч��SYSTEMID
    SMERR_INVLDPASSWORD         = 0X00000021,       // ��Ч������
    SMERR_INVLDSYSTEMTYPE       = 0X00000022,       // ��Ч��SYSTEMTYPE
    SMERR_INVLDIFVER            = 0X00000023,       // ��Ч��INTERFACE_VERSION

    // 0X00000024 - 0X0000003F ����

    SMERR_ADDRERR               = 0X00000040,       // ��ַ����
    SMERR_MOEXCEED              = 0X00000041,       // ��������ύ��
    SMERR_MTEXCEED              = 0X00000042,       // ��������·���
    SMERR_INVLDUSER             = 0X00000043,       // ��Ч���û�
    SMERR_INVLDDATAFMT          = 0X00000044,       // ��Ч�����ݸ�ʽ
    SMERR_CREATEMSGFAILURE      = 0X00000045,       // ������Ϣʧ��
    SMERR_INVLDMSGID            = 0X00000046,       // ��Ч����ϢID
    SMERR_DATABASEFAILURE       = 0X00000047,       // ���ݿ�ʧ��
    SMERR_CANCELMSGFAILURE      = 0X00000048,       // ȡ����Ϣʧ��
    SMERR_MSGSTATEERR           = 0X00000049,       // ����Ϣ״̬����
    SMERR_REPLACEMSGFAILURE     = 0X0000004A,       // �滻��Ϣʧ��
    SMERR_INVLDRPLADDR          = 0X0000004B,       // �滻��ϢԴ��ַ����

    // 0X0000004C - 0X0000005F ����

    SMERR_QUERYMSGSTIMEERR      = 0X0000004C,       // ��ѯ��Ϣʱ���ʽ����

    SMERR_INVLDORGTON           = 0X00000060,       // ��Ч��Դ��ַTON
    SMERR_INVLDORGNPI           = 0X00000061,       // ��Ч��Դ��ַNPI
    SMERR_ORGADDRERR            = 0X00000062,       // Դ��ַ����
    SMERR_INVLDDESTTON          = 0X00000063,       // ��Ч��Ŀ�ĵ�ַTON
    SMERR_INVLDDESTNPI          = 0X00000064,       // ��Ч��Ŀ�ĵ�ַNPI
    SMERR_DESTADDRERR           = 0X00000065,       // Ŀ�ĵ�ַ����
    SMERR_INVLDSCHEDULE         = 0X00000066,       // ��Ч�Ķ�ʱʱ��
    SMERR_INVLDEXPIRE           = 0X00000067,       // ��Ч�ĳ�ʱʱ��
    SMERR_INVLDESM              = 0X00000068,       // ��Ч��ESM_CALSS
    SMERR_INVLDUDLEN            = 0X00000069,       // ��Ч��UDLEN
    SMERR_INVLDPRI              = 0X0000006A,       // ��Ч��PRI
    SMERR_INVLDRDF              = 0X0000006B,       // ��Ч��Registered_delivery_flag
    SMERR_INVLDRPF              = 0X0000006C,       // ��Ч��Replace_if_present_flag

    // 0X0000006D - 0X0000007F ����
    SMERR_INVLDPREDFNSM         = 0X0000006D,       // ��Ч��DEFAULTSMID
    SMERR_INVLDDCS              = 0X0000006E,       // ��Ч��DCS

    SMERR_USERALREADYEXIST      = 0X00000080,       // ָ���û��Ѿ�����
    SMERR_CREATEUSERERR         = 0X00000081,       // �����û�ʧ��
    SMERR_USERIDERR             = 0X00000082,       // �û�ID����
    SMERR_USERNOTEXIST          = 0X00000083,       // ָ���û�������

    SMERR_SMMNGSTATEERR         = 0X00000084,       // �������Ĵ��ڼ���״̬
    SMERR_OCSERR                = 0X00000085,       // TCOS����OCOS����


};

// ���¶����˶���Ϣ��״̬
//

enum SM_STATUS
{
    SMS_WORK = 0,	                        // ����̬
	SMS_DELIVER_SUCCESS,                    // �·��ɹ�
	SMS_DELIVER_FAIL,                       // �·�ʧ��
	SMS_WAIT_DELIVER_FOR_ERROR,		        // ���·�ʧ�ܶ��ȴ�
	SMS_WAIT_DELIVER_FOR_NORMAL,	        // ��ʱ��Ϣ���ȴ�
	SMS_DELETED,						    // ����Ϣ��ɾ��
	SMS_TIMEOUT,						    // ����Ϣ��ʱɾ��
    SMS_INVALID,                            // ��Ч״̬
    SMS_WAIT_ALERT                          // �ȴ�ALERT״̬
};


// ���¶����˲���GSM�Ĵ���ֵ������ɲμ�
// GSM 09.02��GSM 04.11��GSM 03.40 9.2.3.22��Э���е�������
/*
enum GSM_ERROR_CODE
{
    RP_SUCCESS                          = 0,        // �ɹ�
    RP_UNKNOWN_SUBSCRIBER               = 1,        // δ֪�û�			
    RP_UNIDENTIFIED_SUBSCRIBER          = 2,        // δ�����û�		
    RP_ILLEGAL_MS                       = 3,        // �Ƿ��û�			
    RP_TELESERVICE_NOT_PROVISIONED      = 4,        // ����ҵ��֧��	
    RP_CALL_BARRED                      = 5,        // ���б���ֹ		
    RP_CUG_REJECTED                     = 6,        // �պ��û�Ⱥ�ܾ�	
    RP_FACILITY_NOT_SUPPORTED           = 7,        // �豸��֧��		
    RP_ABSENT_SUBSCRIBER                = 8,        // �û����ڷ�����	
    RP_ABSENT_SUBSCRIBER_MWDSET         = 9,        // �û����ڷ�����MWDSET	
    RP_SM_DELIVERY_FAILURE              = 10,       // SM����ʧ��		
    RP_MESSAGE_WAITING_LIST_FULL        = 11,       // ��Ϣ�ȴ�������	
    RP_SYSTEM_FAILURE                   = 12,       // ϵͳʧ��			
    RP_DATA_MISSING                     = 13,       // ���ݶ�ʧ			
    RP_UNEXPECTED_DATA_VALUE            = 14,       // ��������			
    RP_ERROR_IN_MS                      = 15,       // MS�˴���			
    RP_MS_NOT_EQUIPED                   = 16,       // MSδװ��			
    RP_MEMORY_CAPACITY_EXCEEDED         = 17,       // �ڴ���			
    RP_SC_CONGESTION                    = 18,       // SCӵ��			
    RP_MS_NOT_SC_SUBSCRIBER             = 19,       // MS��SC�û�		
    RP_INVALID_SME_ADDRESS              = 20,       // ����ȷSME��ַ		
    RP_UNKNOWN_SERVICE_CENTRE           = 21,       // δ֪SC			
    RP_ILLEGAL_EQUIPMENT                = 22,       // �Ƿ��豸			
    RP_SUBSCRIBER_BUSY_FOR_MT_SMS       = 23,       // �û�æ			

    RP_MSC_NOACK                        = 48,       // MSC��Ӧ��			
    RP_HLR_NOACK                        = 49,       // HLR��Ӧ��			
    RP_GIW_NOACK                        = 50,       // GIWģ����Ӧ��		
    RP_MSC_REFUSE                       = 51,       // MSC�ܾ�			
    RP_HLR_REFUSE                       = 52,       // HLR�ܾ�			
    RP_GIW_REFUSE                       = 53,       // GIWģ��ܾ�		

    RP_GATEWAY_NORIGHT                  = 64,       // �������·�Ȩ�� 
    RP_GATEWAY_NOACK                    = 65,       // ��������Ӧ
    RP_GATEWAY_TEMPERR                  = 66,       // ������ʱ����
    RP_GATEWAY_INVALID                  = 67,       // ������Ч

};*/