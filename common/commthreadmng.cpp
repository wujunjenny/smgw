///////////////////////////////////////////////////////////////////////////////////
//	此文件实现一个面向短消息协议结构的任务管理员
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

void CCommThreadMng::SendMessageToQueue(PTRMSG pMsg) // 消息进入函数
{
	SendMessage(pMsg, ((MI_ONLYHEADER*)pMsg)->nLen+sizeof(MI_ONLYHEADER));
}

