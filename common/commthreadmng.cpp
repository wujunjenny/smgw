///////////////////////////////////////////////////////////////////////////////////
//	���ļ�ʵ��һ���������ϢЭ��ṹ���������Ա
#include "stdafx.h"
#include "commthreadmng.h"
#include "msginner.h"

//////////////////////////////////////////////////////////////////////////////
//	class CCommThreadMng 
CCommThreadMng::CCommThreadMng(UINT nTimerCount)
	: CThreadAndTimer(nTimerCount)
{
}

CCommThreadMng::~CCommThreadMng()
{
}

void CCommThreadMng::SendMessageToQueue(PTRMSG pMsg) // ��Ϣ���뺯��
{
	SendMessage(pMsg, ((MI_ONLYHEADER*)pMsg)->nLen+sizeof(MI_ONLYHEADER));
}

