// 作者：YinBiaozheng,2002-12-10
// 修改: zhangliju   ,2004-07-14,增加256全字符编码
#ifndef __CTreeTable256_H__
#define __CTreeTable256_H__

//#define MAX_ADDRESS_STRING256		21
#define MAX_ADDRESS_STRING256		300
//#define CHARKINDSUM256				(18 + 27)
#define CHARKINDSUM256				(256)
#define ANS_NULLHANDLE				0

#define  GetIndex256(A)   ((unsigned char )(A))
#define  GetChar256(A)    ((char)(A))			
typedef struct tag_node256
{
	HANDLE Handle;
	tag_node256* SonNode[CHARKINDSUM256];
	tag_node256* Parent;
}ADDRESSNODE256;



class CTreeTable256
{
public:

protected:
	ADDRESSNODE256 * Head;
	int m_iCount;

	ADDRESSNODE256* NewNode();
	BOOL DeleteTree(ADDRESSNODE256* pNode);
	ADDRESSNODE256* GetNextPos(ADDRESSNODE256 *node);
	BOOL GetItemKey(POSITION& pos,LPTSTR pRevBuff,int iBuffSize);
	//int  GetIndex(char ch);
	//char GetChar(int index); 

public:
	CTreeTable256();
	~CTreeTable256();

	virtual HANDLE GetItem(LPCTSTR pAddress);              //根据关键字得到对应对应码,最贴近前项匹配,存在则返回HANDLE,否则为NULL
	virtual HANDLE GetFullItem(LPCTSTR pAddress);          //根据关键字得到对应对应码,全匹配,存在则返回HANDLE,否则为NULL
	HANDLE ReplaceItem(LPCTSTR pAddress, HANDLE NewItem);  //用新码值替换原来关键字对应码值,如果原来关键字不存在则新建该关键字和码对应关系
	BOOL DeleteItem(LPCTSTR pAddress);                     //删除此 关键字项.成功删除返回TRUE 不存在返回 false
	BOOL InsertItem(LPCTSTR pAddress, HANDLE NewItem);     //插入 关键字和码对应关系项,如果原来关键字对应项已经存在则返回false
	POSITION GetFirstItem();                               //循环 树 得到首关键字
	HANDLE GetNextItem(POSITION& pos,LPTSTR pRevBuff=NULL,int iBuffSize=0);  //循环树 得到下一个关键字
	int GetCount();											//循环树 得到关键字项个数
	BOOL DeleteAllItem();									//删除所有项

	///　根据关键字得到对应对应码,最贴近前项匹配,存在则返回HANDLE,否则为NULL,并且得到匹配关键字的查询
    bool GetItem(LPCTSTR pAddress, HANDLE &ResultHandle,char* pKeys, int size);

};

#endif