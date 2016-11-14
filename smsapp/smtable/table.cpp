#include  "StdAfx.h"
#include "Table.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

CTable::CTable(UL strIndexSize1,UL IsUnique1,UL strIndexSize2,UL IsUnique2,UL numIndexSize1,UL IsUnique3,UL numIndexSize2,UL IsUnique4)
{   
    m_ulMapIndexSize=numIndexSize1;
	m_pMapIndex=new CMapIndex(m_ulMapIndexSize,1);
	m_pStringIndex[0]=new CStringIndex(strIndexSize1,40,IsUnique1);
    m_pStringIndex[1]=new CStringIndex(strIndexSize2,60,IsUnique2);
    m_pNumberIndex[0]=new  CNumberIndex(numIndexSize1,IsUnique3);   
	m_pNumberIndex[1]=new  CNumberIndex(numIndexSize2,IsUnique4);   
	

};
CTable::~CTable()
{
	Clear();
	int i;
	for(i=0;i<MAX_STRING_INDEX;i++)
	      delete m_pStringIndex[i];
	for(i=0;i<MAX_UL_INDEX;i++)
	      delete  m_pNumberIndex[i];
	delete m_pMapIndex;
}


int  CTable::Insert( char* pKey,void * pValue,int nOrder)
{    
	CElement * pNewElement;
    int nRet;
	CElement ** pFind;

	CPOSITION pos,mappos;

    int  count= m_pMapIndex->Find((UL)pValue,&pFind);
	if(count>0)                  //若value已在表中
	{     
          nRet=m_pStringIndex[nOrder]->Insert(pKey,pFind[0],&pos); 
		  if(!nRet)
		  {
		      pFind[0]->SetRelativeIndex(&pos,nOrder);
			  delete[] pFind;
		  }
		  else
		  {   
			   delete[] pFind;
			   return nRet;
		  }
		 
	}
	else
	{

		 pNewElement=new CElement(pValue); 

         if(pNewElement==NULL)
		 {
		       return  OUT_OFF_MEMORY;
		 }
		 nRet=m_pStringIndex[nOrder]->Insert(pKey,pNewElement,&pos);
  	     if (!nRet)
		 {   
			  nRet=m_pMapIndex->Insert((UL)pValue,pNewElement,&mappos); 
			  if(!nRet)
			  {
   	              pNewElement->SetRelativeIndex(&pos,nOrder);
			      pNewElement->SetRelativeMap(&mappos);
			  }
			  else
			  { 
				  m_pStringIndex[nOrder]->Delete(&pos,pNewElement);
				  delete  pNewElement;
				  return  nRet;

			  }
		 }
	     else
		 {
		      delete  pNewElement;
              return  nRet;

		 }

	}
	return 0;

}


int  CTable::Insert(UL  Key,void * pValue,int nOrder)
{    
	CElement * pNewElement;
    int nRet;
	CElement ** pFind;
	CPOSITION pos,mappos;
    int  count= m_pMapIndex->Find((UL)pValue,&pFind);
	if(count>0)
	{     
         nRet=m_pNumberIndex[nOrder]->Insert(Key,pFind[0],&pos);
		 if(!nRet)
		 {   
			  
		      pFind[0]->SetRelativeIndex(&pos,nOrder+MAX_STRING_INDEX);
			  delete[] pFind;
		 }
		 else
		 {   
			  delete[] pFind;
			  return nRet;
		 }
		
	}
	else
	{

		 pNewElement=new CElement(pValue); 

         if(pNewElement==NULL)
		 {
		       return  OUT_OFF_MEMORY;
		 }
		 nRet=m_pNumberIndex[nOrder]->Insert(Key,pNewElement,&pos);
  	     if (!nRet)                            //插入成功
		 {    
			 nRet=m_pMapIndex->Insert((UL)pValue,pNewElement,&mappos);  
			 if(!nRet)                          //插入成功

             {   
   	            pNewElement->SetRelativeIndex(&pos,nOrder+MAX_STRING_INDEX);
			    pNewElement->SetRelativeMap(&mappos);
			 }
			 else
			 {
                 m_pNumberIndex[nOrder]->Delete(&pos,pNewElement);
				 delete  pNewElement;
				 return nRet;

				 
			 }
		 }
	     else
		 {
		      delete  pNewElement;
              return  nRet;

		 }

	}
	return 0;

}






int  CTable::Find( char * pKey,void *** pResult,int nOrder)
{
     
	 CElement ** Result;
	 int Count;
	
	 Count=m_pStringIndex[nOrder]->Find(pKey,&Result);
     if (Count >0)
	 {
         *pResult=new void *[Count];
		 for(int i=0;i<Count;i++)
		 {
	       (*pResult)[i]=Result[i]->GetValue(); 
		  
		 }
		 delete[] Result; 
	     return Count; 
		
	 }
   
	 return 0;

};
int  CTable::Find(UL nKey,void *** pResult,int nOrder)
{
     
	 CElement ** Result;
	 int Count;
	
	 Count=m_pNumberIndex[nOrder]->Find(nKey,&Result);
     if (Count >0)
	 {
         *pResult=new void *[Count];
		 for(int i=0;i<Count;i++)
		 {
	       (*pResult)[i]=Result[i]->GetValue(); 
		  
		 }
		 delete[] Result;
	     return Count; 
	 }
   
	 return 0;

};


int  CTable::Find(UL nKey,void *** pResult)
{
     
	 CElement ** Result;
	 int Count;
	
	 Count=m_pMapIndex->Find(nKey,&Result);
     if (Count >0)
	 {
         *pResult=new void *[Count];
		 for(int i=0;i<Count;i++)
		 {
	       (*pResult)[i]=Result[i]->GetValue(); 
		  
		 }
	     delete[] Result;	 
	     return Count; 
	 }
     
	 return 0;

};
void CTable::Delete( char *pKey,int nOrder)
{
    
  
    CElement ** Del;
    int DelCount;
    int k,i;
	DelCount=m_pStringIndex[nOrder]->Find(pKey,&Del);
	if (DelCount>0)
	{
		for(i=0;i<DelCount;i++)
		{  
           for(k=0;k<MAX_STRING_INDEX;k++)
		   {
		     m_pStringIndex[k]->Delete(Del[i]->GetRelativeIndex(k),Del[i]);
		   }
		   for(k=0;k<MAX_UL_INDEX;k++)
		   {
              m_pNumberIndex[k]->Delete(Del[i]->GetRelativeIndex(k+MAX_STRING_INDEX),Del[i]);
		   }
		   m_pMapIndex->Delete(Del[i]->GetRelativeMap(),Del[i]);  
           delete     Del[i];
 		
		}
		 delete[]   Del;
          

	}
	
    return;


}
void CTable::Delete(UL nBeginNumber,UL nEndNumber,int nOrder)
{

      
    CElement ** Del;
    int DelCount;


	for(UL j=nBeginNumber;j<=nEndNumber;j++)
	{
	     DelCount=m_pNumberIndex[nOrder]->Find(j,&Del);
	     if (DelCount>0)
		 {    

		      for(int i=0;i<DelCount;i++)
			  {
					 int k;
				     for(k=0;k<MAX_STRING_INDEX;k++)
				         m_pStringIndex[k]->Delete(Del[i]->GetRelativeIndex(k),Del[i]);
					
					 for(k=0;k<MAX_UL_INDEX;k++)
					     m_pNumberIndex[k]->Delete(Del[i]->GetRelativeIndex(k+MAX_STRING_INDEX),Del[i]);
                     
					  m_pMapIndex->Delete(Del[i]->GetRelativeMap(),Del[i]); 
					  
					 delete     Del[i];

            

			  }
			   delete[]   Del;
              

		 }
	}
	return;


}

void CTable::Delete(UL ulvalue)
{

      
    CElement ** Del;
    int DelCount;



	
	     DelCount=m_pMapIndex->Find(ulvalue,&Del);
	     if (DelCount>0)
		 {    

		      for(int i=0;i<DelCount;i++)
			  {      
					 int k;
				     for(k=0;k<MAX_STRING_INDEX;k++)
				         m_pStringIndex[k]->Delete(Del[i]->GetRelativeIndex(k),Del[i]);
					
					 for(k=0;k<MAX_UL_INDEX;k++)
					     m_pNumberIndex[k]->Delete(Del[i]->GetRelativeIndex(k+MAX_STRING_INDEX),Del[i]);
                     
					  m_pMapIndex->Delete(Del[i]->GetRelativeMap(),Del[i]); 
				
		//			 print(); 
					 delete     Del[i];

            

			  }
			   delete[]   Del;
              

		 }
	
	return;


}

void CTable::Clear()
{
   
	
    CElement ** Del;
    int DelCount;


	for(UL j=0;j<m_ulMapIndexSize;j++)
	{
	     
		 DelCount=m_pMapIndex->FindForClear(j,&Del);
		 
    
	     if (DelCount>0)
		 {    
      
		      for(int i=0;i<DelCount;i++)
			  {      
					 int k;
				     for(k=0;k<MAX_STRING_INDEX;k++)
				         m_pStringIndex[k]->Delete(Del[i]->GetRelativeIndex(k),Del[i]);
					
					 for(k=0;k<MAX_UL_INDEX;k++)
					     m_pNumberIndex[k]->Delete(Del[i]->GetRelativeIndex(k+MAX_STRING_INDEX),Del[i]);
                     
					  m_pMapIndex->Delete(Del[i]->GetRelativeMap(),Del[i]); 
					  
					 delete     Del[i];
                    
	
            

			  }
			   delete[]   Del;
              

		 }
	}
	return;

 

}
void CTable::print()
{
  
   int	i;
   for(i=0;i<MAX_STRING_INDEX;i++)
   {  
      std::cout<<"字符串索引"<<i<<"\n";
      m_pStringIndex[i]->print();
   }

   for(i=0;i<MAX_UL_INDEX;i++)
   {  
      std::cout<<"数字索引"<<i<<"\n";
     m_pNumberIndex[i]->print();
   }
  
	
    std::cout<<"索引\n";
      m_pMapIndex->print();
//      m_pMapIndex->printHash();    

}

