// ESMECode.cpp: implementation of the CESMECode class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ESMECode.h"
//#include "msgdef.h"
#include "smeimsg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CESMECode::CESMECode()
{

}

CESMECode::~CESMECode()
{

}

int CESMECode::Decode(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
{
	EI_HEAD* pHead=(EI_HEAD*)pOrgBuff;
	EI_HEAD* pDestHead=(EI_HEAD*)pDestBuff;

	if(orgsize<(int)(sizeof(EI_HEAD)))
		return DECODE_NO_ENOUGH_ORG_BUF;
	if (pHead->len <0)
    {
        ASSERT(0);
		return DECODE_FAILED;
    }
	if(pHead->len>orgsize)
		return DECODE_NO_ENOUGH_ORG_BUF;
	if(destsize<(int)(pHead->len))
		return DECODE_NO_ENOUGH_DEST_BUF;
	memcpy((char*)pDestBuff,(char*)pOrgBuff,pHead->len);
	
	orgsize=pHead->len;
	destsize=pHead->len;
	return DECODE_SUCCEED;	

}

int CESMECode::Code(LPVOID pOrgBuff,int &orgsize,LPVOID pDestBuff,int& destsize)
{
	EI_HEAD *pOrgHead=(EI_HEAD*)pOrgBuff;
	EI_HEAD *pDestHead=(EI_HEAD*)pDestBuff;

	if(orgsize<(int)(sizeof(EI_HEAD)))
		return CODE_NO_ENOUGH_ORG_BUF;
	if(pOrgHead->len <0)
		return CODE_PROTOCOL_ERROR;
	if( (int)(pOrgHead->len) > orgsize)
		return CODE_NO_ENOUGH_ORG_BUF;
	if( destsize < (int)(pOrgHead->len) )
		return CODE_NO_ENOUGH_DEST_BUF;
	memcpy((char*)pDestBuff, (char*)pOrgBuff, pOrgHead->len);

	orgsize=pOrgHead->len;
	destsize=pOrgHead->len;
	return CODE_SUCCEED;	
}

