#if !defined(_FEP_H_INCLUDED_)
#define _FEP_H_INCLUDED_

 //���е��÷��� 0 ���ɹ� ������ �� ʧ��

int __stdcall TcpInit(unsigned long MyID);
//�ú����ṩ�������̺�ΪMyID�ĳ�ʼ��������
void __stdcall TcpExit();
//�ú����ṩ�������̺ŵ��˳�������
//һ�����̿��Զ�ε���TcpInit(unsigned long MyID)�������Լ��ж����ͬ���̺š�
//ÿ����TcpInitһ�� ���˳�ʱ��Ӧ����һ��TcpExit();

int __stdcall TcpSend(unsigned long TargetIP, unsigned long TargetID, char * data, unsigned long datalen, unsigned long timeout);
//�ú����ṩ��ָ��������TargetIP�ͽ��̺�TargetID������Ϣ���Ե�һ�ε���TcpInitʱ��MyIDΪ�Լ��Ľ��̺š�

int __stdcall TcpRecv( unsigned long * SourceIP, unsigned long * SourceprocID, char * data, unsigned long * datalen);
//�ú���������Ϣ  �����óɹ�ʱ ��Ϣ��*SourceIP ��*SourceprocID�����Ե�һ�ε���TcpInitʱ��MyIDΪ�Լ��Ľ��̺š�
//�ú����������á�

int __stdcall TcpSendExt( unsigned long MyID , unsigned long TargetIP, unsigned long TargetprocID, char * data, unsigned long datalen, unsigned long timeout);
//�ú����ṩ��ָ��������TargetIP�ͽ��̺�TargetprocID������Ϣ��MyIDΪ�Լ��Ľ��̺š�
int __stdcall TcpRecvExt( unsigned long * SourceIP, unsigned long * SourceprocID, unsigned long MyID , char * data, unsigned long * datalen);
//�ú���������Ϣ  �����óɹ�ʱ ��Ϣ��*SourceIP ��*SourceprocID��MyIDΪ�Լ��Ľ��̺š�
//�ú����������á�

#endif // !defined(_FEP_H_INCLUDED_)
