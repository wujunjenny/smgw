// AckError.cpp: implementation of the CAckError class.
//


//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "smsapp.h"
#include "AckError.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CAckError::CAckError()
{
    Init();
}

CAckError::~CAckError()
{

}
void  CAckError::Init()
{


	
	m_sInitName=".\\ackerror.ini";
	char  sTemp[100];
	char  sSession[100]="NoReSubmit";
    char  sKey[100];
	memset(sKey,0,sizeof(sKey));
    
   	GetPrivateProfileString(sSession,"Count", "0", sTemp, sizeof(sTemp), m_sInitName); 
	int nCount=atoi(sTemp);
	m_Count[0]=nCount;
    for (int i=0;i<nCount;i++)
	{
        sprintf(sKey,"code%d",i);
        GetPrivateProfileString(sSession,sKey, "", sTemp, sizeof(sTemp), m_sInitName); 
        m_CorStr[0]=m_CorStr[0]+sTemp+"|";
	}

	strcpy(sSession,"ReSubmit");
    
	GetPrivateProfileString(sSession,"Count", "0", sTemp, sizeof(sTemp), m_sInitName); 
    nCount=atoi(sTemp);
	m_Count[1]=nCount;
    for (int i=0;i<nCount;i++)
	{
        sprintf(sKey,"code%d",i);
        GetPrivateProfileString(sSession,sKey, "", sTemp, sizeof(sTemp), m_sInitName); 
        m_CorStr[1]=m_CorStr[1]+sTemp+"|";
	}
	strcpy(sSession,"Success");
    GetPrivateProfileString(sSession,"Count", "0", sTemp, sizeof(sTemp), m_sInitName); 
	nCount=atoi(sTemp);
	m_Count[2]=nCount;
    for (int i=0;i<nCount;i++)
	{
        sprintf(sKey,"code%d",i);
        GetPrivateProfileString(sSession,sKey, "", sTemp, sizeof(sTemp), m_sInitName); 
        m_CorStr[2]=m_CorStr[2]+sTemp+"|";
	}



}
CString CAckError::GetCorStr(int index)
{
	return m_CorStr[index];
}

void CAckError::Add(int ntype, CString sError,CString sTimes)
{
	
	CString sNewString;
	
    CString sKey;
	CString sCount;
	
	if(ntype<0 || ntype>2)
		return;
	
	m_Count[ntype]++;
	if ( ntype==1 )
	{
	    sNewString.Format("%s,%s",sError,sTimes);
        m_CorStr[ntype]=m_CorStr[ntype]+sNewString;
        m_CorStr[ntype]=m_CorStr[ntype]+"|";
		sKey.Format("code%d",m_Count[ntype]-1);
		sCount.Format("%d",m_Count[ntype]);
        WritePrivateProfileString("ReSubmit","count",sCount, m_sInitName); 
        WritePrivateProfileString("ReSubmit",sKey,sNewString, m_sInitName); 

	}
	
	else if (ntype==0)
	{
        sNewString.Format("%s",sError);
        m_CorStr[ntype]=m_CorStr[ntype]+sNewString;
        m_CorStr[ntype]=m_CorStr[ntype]+"|";
        sKey.Format("code%d",m_Count[ntype]-1);
		sCount.Format("%d",m_Count[ntype]);
        WritePrivateProfileString("NoReSubmit","count",sCount, m_sInitName); 
        WritePrivateProfileString("NoReSubmit",sKey,sNewString, m_sInitName); 

        
	}
	else if (ntype==2)
	{

		sNewString.Format("%s",sError);
        m_CorStr[ntype]=m_CorStr[ntype]+sNewString;
        m_CorStr[ntype]=m_CorStr[ntype]+"|";
        sKey.Format("code%d",m_Count[ntype]-1);
		sCount.Format("%d",m_Count[ntype]);
        WritePrivateProfileString("success","count",sCount, m_sInitName); 
        WritePrivateProfileString("success",sKey,sNewString, m_sInitName); 


	}
    
}

void CAckError::Del(int ntype, CString sError,CString sTimes)
{
    CString sNewString;
	
    CString sKey;
	CString sCount;
	CString sValue,soTimes;
	int i=0;
	int  nCodesPos=0,nCodeePos,nCodeLen;
	if(ntype<0 || ntype>2)
		return;
	
	m_Count[ntype]--;
	if ( ntype==1 )
	{
	    sNewString.Format("%s,%s|",sError,sTimes);
        
		// chang by jdz 2004.03.26
        //m_CorStr[ntype].Replace(sNewString,"");	存在BUG,如当m_CorStr[ntype]是"1288,3|8,3|"
		int tmpPos = m_CorStr[ntype].Find(sNewString);
		if(tmpPos == 0)
			m_CorStr[ntype].Replace(sNewString,"");		//位于开头
		else
		{
			//位于中间
			sNewString.Format("|%s,%s|",sError,sTimes);
			m_CorStr[ntype].Replace(sNewString,"|");
		}
		//end chang

		sCount.Format("%d",m_Count[ntype]);
		CString  tempStr=m_CorStr[ntype]; 
        i=0;    
    	nCodesPos=0;
	    WritePrivateProfileString("ReSubmit","count",sCount, m_sInitName); 
        while(tempStr.Find("|",nCodesPos)>0)
		{  
           nCodeePos=tempStr.Find(",",nCodesPos);
	       nCodeLen=nCodeePos-nCodesPos; 
           sValue=tempStr.Mid(nCodesPos,nCodeLen);
           nCodesPos=nCodeePos+1;
       
	       nCodeePos=tempStr.Find("|",nCodesPos);
           nCodeLen=nCodeePos-nCodesPos; 
           soTimes=tempStr.Mid(nCodesPos,nCodeLen);
           nCodesPos=nCodeePos+1;
           sKey.Format("code%d",i);
		   sValue=sValue+",";
		   sValue=sValue+soTimes;
           WritePrivateProfileString("ReSubmit",sKey, sValue, m_sInitName);  
         
          
	       i++;

		}



	}
	
	else if (ntype==0)
	{
        sNewString.Format("%s|",sError);
        m_CorStr[ntype].Replace(sNewString,"");
	
/*		
		sKey.Format("code%d",m_Count[ntype]-1);
		sCount.Format("%d",m_Count[ntype]);
        WritePrivateProfileString("NoReSubmit","count",sCount, m_sInitName); 
        WritePrivateProfileString("NoReSubmit",sKey,sNewString, m_sInitName); 
*/     	sCount.Format("%d",m_Count[ntype]);
		CString  tempStr=m_CorStr[ntype]; 
	    WritePrivateProfileString("NoReSubmit","count",sCount, m_sInitName); 
	    i=0;
		nCodesPos=0;
		while( tempStr.Find("|",nCodesPos)>0)
		{  
          nCodeePos=tempStr.Find("|",nCodesPos);
	      nCodeLen=nCodeePos-nCodesPos; 
          sValue=tempStr.Mid(nCodesPos,nCodeLen);
          sKey.Format("code%d",i);
          WritePrivateProfileString("NoReSubmit",sKey, sValue, m_sInitName);  
          nCodesPos=nCodeePos+1;
	      i++;
		}  


        
	}
	else if (ntype==2)
	{

		sNewString.Format("%s|",sError);
        m_CorStr[ntype].Replace(sNewString,"");
        

       	sCount.Format("%d",m_Count[ntype]);
		CString  tempStr=m_CorStr[ntype]; 
	    WritePrivateProfileString("success","count",sCount, m_sInitName); 
	    i=0;
		nCodesPos=0;
		while( tempStr.Find("|",nCodesPos)>0)
		{  
          nCodeePos=tempStr.Find("|",nCodesPos);
	      nCodeLen=nCodeePos-nCodesPos; 
          sValue=tempStr.Mid(nCodesPos,nCodeLen);
          sKey.Format("code%d",i);
          WritePrivateProfileString("success",sKey, sValue, m_sInitName);  
          nCodesPos=nCodeePos+1;
	      i++;
		}  



 /* 
		sKey.Format("code%d",m_Count[ntype]-1);
		sCount.Format("%d",m_Count[ntype]);
        WritePrivateProfileString("success","count",sCount, m_sInitName); 
        WritePrivateProfileString("success",sKey,sNewString, m_sInitName); 
 */   
       
	}
 

	
/*	
	nCodeePos=NoResubmit.Find("|",nCodesPos);
	nCodeLen=nCodeePos-nCodesPos;
	sValue=NoResubmit.Mid(nCodesPos,nCodeLen);
    sValue=sValue+"    不重发";  
     m_cErrList.AddString(sValue);
    nCodesPos=nCodeePos+1;
*/ 

	 
	

}