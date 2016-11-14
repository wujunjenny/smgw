// AckError.h: interface for the CAckError class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_ACKERROR_H_INCLUDED_)
#define _ACKERROR_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
typedef struct
{
	CString  sCode;
    CString  sTimes;
}  ERR_STRUCT;


class CAckError  
{
public:
	CAckError();
	virtual ~CAckError();
	CString GetCorStr(int index);
	void  Add(int ntype, CString sError,CString sTimes);
	void  Del(int ntype, CString sError,CString sTimes);

protected:
	void Init();
	CString m_CorStr[3];
	int     m_Count[3];
	CString m_sInitName;
   
	int WriteToFile();

};

#endif // !defined(_ACKERROR_H_INCLUDED_)
