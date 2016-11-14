// UserContainer.h: interface for the CUserContainer class.
// 注意：    （1）这里所有的建立都是建立联系，
//           （2）这里所有的释放或删除都是释放联系，
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
	CUserObj * m_pUserArray ;                                  //用户对象数组
	CMap <CString, LPCSTR, CUserObj*, CUserObj*> m_ActiveUserMap ; //用户对象队列
	CPtrList m_IdleUserList ; ///空闲用户对象队列
    int m_iTotalUser;

protected:
	CUserObj* RemoveHeadFromList () ;	              //从空闲队列中取出一个用户对象
    void AddTailOfList (CUserObj* pUser) ;            //加入一个用户对象到空闲队列中
	CUserObj* FindOneUserInList (char* MSISDN) ;      //在空闲队列中查找一个用户对象

    CUserObj* RemoveElementFromMap (char* MSISDN) ;   //从有效队列中取出一个用户对象
	void AddToMap (CUserObj * pUser) ;                //加入一个用户对象到有效队列中
	CUserObj* FindOneUserInMap (LPCSTR MSISDN) ;       //在有效队列中查找一个用户对象
//Function
public:
	BOOL InitUserArray() ;                                   //初始化用户对象队列
	void FreeUserArray() ;                                   //释放用户对象队列 
public:
	int GetAllActiveUserCount();
	void ClearAllNoRefreshUser();
	void ResetAllRefreshFlag();
	void ResetUserArray();
    //CUserObj* AllocOneUser() ;                               //创建一个用户对象
    //创建一个用户对象
    CUserObj* AllocOneUser(UC byTON, UC byNPI, US MSType, LPCSTR sMSISDN);
    //CUserObj* AllocOneUser(tagStSetNotify* pMsg) ;                               //创建一个用户对象
	void FreeOneUser(char * MSISDN) ;                        //释放一个用户对象
	CUserObj* FindOneUser(LPCSTR MSISDN) ;                   //查找一个用户对象
    CUserObj* UpdateOneUserObj (CUserObj* pUser) ;	         //更新一个用户对象
//调试用函数
    void DisplayUser();
};
#endif // !defined(_USERCONTAINER_H_INCLUDED_)
