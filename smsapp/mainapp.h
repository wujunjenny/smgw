#ifndef _MAINAPP_H_INCLUDED_
#define _MAINAPP_H_INCLUDED_

class CSmsAppApp;

CSmsAppApp* GetMainApp()
{
	return (CSmsAppApp*)AfxGetApp();
};
#include "SmsApp.h"

#endif	//_MAINAPP_H_INCLUDED_