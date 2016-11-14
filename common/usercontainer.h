// UserContainer.h: interface for the CUserContainer class.
// ע�⣺    ��1���������еĽ������ǽ�����ϵ��
//           ��2���������е��ͷŻ�ɾ�������ͷ���ϵ��
//////////////////////////////////////////////////////////////////////
#if !defined(_USERCONTAINER_H_INCLUDED_)
#define _USERCONTAINER_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define USERS 20000
class CUserObj ;
//class CStockAlert ;
class CUserContainer  
{
public:
	CUserContainer();
	virtual ~CUserContainer();
private:
	CUserObj * m_pUserArray ;                                  //�û���������
	CMap <CString, LPCSTR, CUserObj*, CUserObj*> m_ActiveUserMap ; //�û��������
	CPtrList m_IdleUserList ; ///�����û��������
    int m_iTotalUser;

protected:
	CUserObj* RemoveHeadFromList () ;	              //�ӿ��ж�����ȡ��һ���û�����
    void AddTailOfList (CUserObj* pUser) ;            //����һ���û����󵽿��ж�����
	CUserObj* FindOneUserInList (char* MSISDN) ;      //�ڿ��ж����в���һ���û�����

    CUserObj* RemoveElementFromMap (char* MSISDN) ;   //����Ч������ȡ��һ���û�����
	void AddToMap (CUserObj * pUser) ;                //����һ���û�������Ч������
	CUserObj* FindOneUserInMap (LPCSTR MSISDN) ;       //����Ч�����в���һ���û�����
//Function
public:
	BOOL InitUserArray() ;                                   //��ʼ���û��������
	void FreeUserArray() ;                                   //�ͷ��û�������� 
public:
	int GetAllActiveUserCount();
	void ClearAllNoRefreshUser();
	void ResetAllRefreshFlag();
	void ResetUserArray();
    //CUserObj* AllocOneUser() ;                               //����һ���û�����
    //����һ���û�����
    CUserObj* AllocOneUser(UC byTON, UC byNPI, US MSType, LPCSTR sMSISDN);
    //CUserObj* AllocOneUser(tagStSetNotify* pMsg) ;                               //����һ���û�����
	void FreeOneUser(char * MSISDN) ;                        //�ͷ�һ���û�����
	CUserObj* FindOneUser(LPCSTR MSISDN) ;                   //����һ���û�����
    CUserObj* UpdateOneUserObj (CUserObj* pUser) ;	         //����һ���û�����
//�����ú���
    void DisplayUser();
};
#endif // !defined(_USERCONTAINER_H_INCLUDED_)
