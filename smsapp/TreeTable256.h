// ���ߣ�YinBiaozheng,2002-12-10
// �޸�: zhangliju   ,2004-07-14,����256ȫ�ַ�����
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

	virtual HANDLE GetItem(LPCTSTR pAddress);              //���ݹؼ��ֵõ���Ӧ��Ӧ��,������ǰ��ƥ��,�����򷵻�HANDLE,����ΪNULL
	virtual HANDLE GetFullItem(LPCTSTR pAddress);          //���ݹؼ��ֵõ���Ӧ��Ӧ��,ȫƥ��,�����򷵻�HANDLE,����ΪNULL
	HANDLE ReplaceItem(LPCTSTR pAddress, HANDLE NewItem);  //������ֵ�滻ԭ���ؼ��ֶ�Ӧ��ֵ,���ԭ���ؼ��ֲ��������½��ùؼ��ֺ����Ӧ��ϵ
	BOOL DeleteItem(LPCTSTR pAddress);                     //ɾ���� �ؼ�����.�ɹ�ɾ������TRUE �����ڷ��� false
	BOOL InsertItem(LPCTSTR pAddress, HANDLE NewItem);     //���� �ؼ��ֺ����Ӧ��ϵ��,���ԭ���ؼ��ֶ�Ӧ���Ѿ������򷵻�false
	POSITION GetFirstItem();                               //ѭ�� �� �õ��׹ؼ���
	HANDLE GetNextItem(POSITION& pos,LPTSTR pRevBuff=NULL,int iBuffSize=0);  //ѭ���� �õ���һ���ؼ���
	int GetCount();											//ѭ���� �õ��ؼ��������
	BOOL DeleteAllItem();									//ɾ��������

	///�����ݹؼ��ֵõ���Ӧ��Ӧ��,������ǰ��ƥ��,�����򷵻�HANDLE,����ΪNULL,���ҵõ�ƥ��ؼ��ֵĲ�ѯ
    bool GetItem(LPCTSTR pAddress, HANDLE &ResultHandle,char* pKeys, int size);

};

#endif