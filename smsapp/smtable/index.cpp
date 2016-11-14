#include "stdafx.h"
#include  "index.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

void *  CElement::GetValue()
{
    return m_pValue;
}



 CElement::CElement(void * pValue)
 {   
	 m_pValue=pValue;
	 for(int i=0;i<MAX_INDEX;i++)
	 {
		 m_posIndexItem[i].Offset=0;
		 m_posIndexItem[i].Point=NULL; 
	 }
 }

void  CElement::SetRelativeIndex(CPOSITION * pos,int index)
{
    

           m_posIndexItem[index].Point =pos->Point;
		   m_posIndexItem[index].Offset=pos->Offset;
	

}
CPOSITION  * CElement::GetRelativeIndex(int index)
{
     
	       return  &m_posIndexItem[index];
		   

}

CPOSITION * CElement::GetRelativeMap()
{          
	     
	return    &m_posMapItem;
    
}
void CElement::SetRelativeMap(CPOSITION * pos)
{        
	 m_posMapItem.Offset=pos->Offset;
	 m_posMapItem.Point=pos->Point;  

}

//func for string
/*=========================================================







*==========================================================*/
CStringIndexItem::CStringIndexItem( char * pKey,CElement * pLinkElement, CStringIndexItem *pNext,CStringIndexItem *pPre)
{   
#ifdef _DEBUG
	strcpy(str, "CStringIndexItem, ABCDEFGHIJKABCDEFGHIJKABCDEFGHIJKABCDEFGHIJK");
#endif
	

    m_pKey=new char[strlen(pKey)+1];
	strcpy(m_pKey,pKey);
 //	m_pKey=pKey;
    m_pLinkElement=pLinkElement;
    m_pNext=pNext;
    m_pPre=pPre;
}  
CStringIndexItem::~CStringIndexItem()
{
	delete[] m_pKey;
}

CStringIndex::CStringIndex(int HashSize, int DefStringSize , int Unique)
{
    m_nHashSize=HashSize;
	m_nKeyLen=DefStringSize;

	m_nUnique=Unique;
	m_HashTable=new CStringIndexItem *[m_nHashSize];
	memset(m_HashTable,0,sizeof(CStringIndexItem *)*m_nHashSize);
	
	
}
CStringIndex::~CStringIndex()
{   
	
	delete[] m_HashTable;
}

/*=========================================================









*==========================================================*/

int CStringIndex::Find( char* pKey,CElement *** pResult)
{
    

    int ResultCount=0;
	int HashOffset;
    CStringIndexItem  *pNext=NULL;
	
    CStringIndexItem  *ptemp=NULL,*ptempForDel,*pNewResult=NULL,*pPre=NULL;
     
    
	
    
	if(!pKey)
	{
		return NOT_FIND;
	}
	
	
	
	HashOffset=HashFuc(pKey);

	if (m_HashTable[HashOffset]!=NULL && m_HashTable[HashOffset]->m_pNext==NULL)
	{
	
			 
	/*  old1		 
			 *pResult=new CElement *[1];
			 if (!pResult)
			 {
				 return OUT_OFF_MEMORY;
			 }
     */ 
			 if(strcmp(m_HashTable[HashOffset]->m_pKey,pKey)==0)
			 {
                
      //  change1  (move)
                *pResult=new CElement *[1];
			    if (!pResult)
				{
				    return OUT_OFF_MEMORY;
				}
	  // end 			
			    (*pResult)[0]=m_HashTable[HashOffset]->m_pLinkElement;
                ResultCount=1;  
				return  ResultCount;  
			 }
			 else
			 {
                return NOT_FIND;
			 }
			 
			 
             
		} 
	    else if (m_HashTable[HashOffset]==NULL)
		{
		     return NOT_FIND;
		}
	    else
		{   
		     pNext=m_HashTable[HashOffset];
		     while(pNext)
			 {
                  if (strcmp(pNext->m_pKey,pKey)==0)
				  {
				        
					  
					   ResultCount++;
                       
					   if (ResultCount==1)
					   {
						    pNewResult=new  CStringIndexItem(pNext->m_pKey,pNext->m_pLinkElement,NULL,pNext);
					        if (pNewResult==NULL)
							{
								return OUT_OFF_MEMORY;
							}
							
                            pPre=pNewResult;
                            
						
					   }
					   else
					   {
                             
						    pNewResult=new  CStringIndexItem(pNext->m_pKey,pNext->m_pLinkElement,pPre,pNext);
			                if (!pNewResult)
							{
								return OUT_OFF_MEMORY;
							}

							pPre=pNewResult;


					   }
				  }
			     
				  pNext=pNext->m_pNext; 
				 
                
			 }
			 
			 if (ResultCount>0)
			 {     
                  ptemp=pNewResult; 
			      *pResult=new  CElement *[ResultCount];
				 
                  int i=0;
			      if (ptemp)
				  {
	    		       do
					   {
    			            (*pResult)[i]=ptemp->m_pLinkElement;
				            ptempForDel=ptemp;
							ptemp=ptemp->m_pNext;
							delete  ptempForDel;
						
                            i++;
					   }while(ptemp!=NULL && i<ResultCount);
				

 
				  }
			      
				  
				  
				  return  ResultCount;
			 }
		     return NOT_FIND;
		}		
	 
        return 0;  

	
}
/*=========================================================







*==========================================================*/


int CStringIndex::Insert( char *pKey, CElement *pElement,CPOSITION * pos)
{   
	int HashOffset;
	CStringIndexItem  *pPre, *pCur,*pNewItem=NULL;

    
    pNewItem=new CStringIndexItem(pKey,pElement,NULL,NULL);
     
	if(!pNewItem)
	{		
		return OUT_OFF_MEMORY; 
	}

    pNewItem->m_pLinkElement=pElement;   
    
   	HashOffset=HashFuc(pKey);  
	
   if (m_HashTable[HashOffset]==NULL)
	{
        
	
		m_HashTable[HashOffset]=pNewItem;
		
	
	}
	else
	{   
		
		pCur=m_HashTable[HashOffset];
		pPre=pCur;
		while(pCur)
		{   
			if( (m_nUnique==1) && (strcmp(pCur->m_pKey,pKey)==0))
			{

				delete  pNewItem;
				pNewItem = NULL;
				return STRING_IS_UNIQUE;   
				
			}
			pPre=pCur;
			pCur=pCur->m_pNext;   
		}
		pPre->m_pNext=pNewItem;
		pNewItem->m_pPre=pPre; 


	}
	pos->Offset= HashOffset;
	pos->Point=(POINTER)pNewItem; 

		

    return 0;
}
/*=========================================================







*==========================================================*/

int CStringIndex::Delete( char *pKey)
{

	CElement ** Del;
    int DelCount;
	CStringIndexItem * DelItem;
    int DelItemOffset;
	DelCount=Find(pKey,&Del);
	if (DelCount>0)
	{
		int i;
		for(i=0;i<DelCount;i++)
		{
		   
           DelItem=(CStringIndexItem *)(Del[i]->GetRelativeIndex(0)->Point);  
		   DelItemOffset=Del[i]->GetRelativeIndex(0)->Offset;   
		   if(DelItem->m_pPre==NULL)
		   {
                
			   m_HashTable[DelItemOffset]=DelItem->m_pNext;
	/*		   if(DelItem->m_pNext)
			   {
				   DelItem->m_pNext->m_pPre=NULL; 
			   }
    */

		   }
		   else
		   {
               DelItem->m_pPre->m_pNext=DelItem->m_pNext;
			   
		   }
		   if(DelItem->m_pNext)
		   {
                  DelItem->m_pNext->m_pPre=DelItem->m_pPre;
		   }
		    delete     DelItem;
           

		}
		 delete     Del[i];

	}
	else
	{
		return NOT_FIND;
	}

    return 0;
}
/*=========================================================







*==========================================================*/

int CStringIndex::Delete(CPOSITION * pos, CElement *pElement)
{   
     
           
           if(!pos)
		   {
		          return -1;
		   }
           if(!(pos->Point))
		   {
		    	  return -1;
		   }
	        
		   CStringIndexItem * DelItem;
		   DelItem=(CStringIndexItem *)(pos->Point);
		   if(DelItem->m_pLinkElement!=pElement)
		   {
			   return -1;
		   }
		   if(!(DelItem->m_pPre))
		   {
                
			   m_HashTable[pos->Offset]=DelItem->m_pNext;
		/*	   if(DelItem->m_pNext)
			   {
                    DelItem->m_pNext->m_pPre=NULL; 
			   }
          */

		   }
		   else
		   {
               DelItem->m_pPre->m_pNext=DelItem->m_pNext;
		   }
		    if(DelItem->m_pNext)
			{
                 DelItem->m_pNext->m_pPre=DelItem->m_pPre;
			}
		   delete     DelItem;
           

	 return 0;
}



/*=========================================================








*==========================================================*/

int CStringIndex::HashFuc(const char * String)
{   
	
	int   HashOffset;  
	int   StringLen;
	int   Convert[7];
	char  ToInt[2];
	int   AddTimes;
	const int Internal=7;
	const char * Temp=String;
	memset(Convert,0,sizeof(Convert));
	StringLen=strlen(String);
	AddTimes=StringLen / Internal;
	
	if (StringLen % Internal==0)
	{
       AddTimes=AddTimes-1;
	}
	for(int i=0;i<Internal;i++)
	{
		for(int  j=0;j<=AddTimes;j++)
		{
			if ((i+j*Internal)<StringLen)
			{
			    ToInt[0]=Temp[i+j*Internal];
				ToInt[1]='\0';
				Convert[i]=Convert[i]+atoi(ToInt);
			}
		}
        Convert[i]=Convert[i] % 10; 
	}
	

    HashOffset=Convert[0]*1+Convert[1]*10+Convert[2]*100+Convert[3]*1000+Convert[4]*10000+Convert[5]*100000+Convert[6]*1000000;
    HashOffset=HashOffset % m_nHashSize;

	return HashOffset;

    
}
void CStringIndex::print()
{

      CStringIndexItem  *Next,*tail;
     
      UL i;
	 
      for(i=0;i<m_nHashSize;i++)
	  {
	       if (m_HashTable[i]!=NULL) 
		   {   
		       std::cout<<"在HASH中的位置"<<i<<"/ Key:";
                   
		      std::cout<<m_HashTable[i]->m_pKey<<"->";
		   	   Next=m_HashTable[i]->m_pNext;
		       while(Next!=NULL)
			   {
                     std::cout<<Next->m_pKey<<"->";
			         tail=Next;
					 Next=Next->m_pNext; 

			   }
			   while(tail)
			   {
				   std::cout<<tail->m_pKey<<"<-";
				   tail=tail->m_pPre; 
			   }
		       std::cout<<"\n";
		   }
    
	  }  

}

// func for timeindex

CNumberIndex::CNumberIndex(int HashSize,int Unique)
{   
	m_nHashSize=HashSize;
    m_nUnique=Unique;
	m_HashTable=new CNumberIndexItem *[m_nHashSize];
	memset(m_HashTable,0,sizeof(CNumberIndexItem *)*m_nHashSize);
}
CNumberIndex::~CNumberIndex()
{   

    delete[] m_HashTable;
}
int  CNumberIndex::Find(UL nkey,CElement *** pResult)
{    
	int ResultCount=0;
	int HashOffset;
    CNumberIndexItem  *pNext=NULL;
	
    CNumberIndexItem  *ptemp=NULL,*ptempForDel=NULL,*pNewResult=NULL,*pPre=NULL;
     
    
	*pResult=NULL;
    
	
	
	
	HashOffset=HashFuc(nkey);

	if (m_HashTable[HashOffset] && !(m_HashTable[HashOffset]->m_pNext))
	{
	
			 
	/*		 
			 *pResult=new CElement *[1];
			 if (!(*pResult))
			 {
				 return OUT_OFF_MEMORY;
			 }
    */ 
			 if(m_HashTable[HashOffset]->m_nKey==nkey)
			 {
               
                 *pResult=new CElement *[1];
        		 if (!(*pResult))
				 {
				    return OUT_OFF_MEMORY;
				 }
    

			    (*pResult)[0]=m_HashTable[HashOffset]->m_pLinkElement;
                ResultCount=1;  
				return  ResultCount;  
			 }
			 else
			 {
                return NOT_FIND;
			 }
			 
			 
             
		} 
	    else if (!m_HashTable[HashOffset])
		{
		     return NOT_FIND;
		}
	    else
		{   
		     pNext=m_HashTable[HashOffset];
		     while(pNext!=NULL)
			 {
                  if (pNext->m_nKey==nkey)
				  {
				        
					  
					   ResultCount++;
                       
					   if (ResultCount==1)
					   {
						    pNewResult=new  CNumberIndexItem(pNext->m_nKey,pNext->m_pLinkElement,NULL,pNext);
					        if (!pNewResult)
							{
								return OUT_OFF_MEMORY;
							}
							
                            pPre=pNewResult;
                            
						
					   }
					   else
					   {
                             
						    pNewResult=new  CNumberIndexItem(pNext->m_nKey,pNext->m_pLinkElement,pPre,pNext);
			                if (!pNewResult)
							{
								return OUT_OFF_MEMORY;
							}

							pPre=pNewResult;


					   }
				  }
			     
				  pNext=pNext->m_pNext; 
				 
                
			 }
			 
			 if (ResultCount>0)
			 {     
                  ptemp=pNewResult; 
			      *pResult=new  CElement *[ResultCount];
				  if(!(*pResult))
				  {
					  return OUT_OFF_MEMORY;
				  }
                  int i=0;
			      if (ptemp!=NULL)
				  {
	    		       do
					   {
    			            (*pResult)[i]=ptemp->m_pLinkElement;
				            ptempForDel=ptemp;      
							ptemp=ptemp->m_pNext;
							delete  ptempForDel;
						
                            i++;
					   }while(ptemp!=NULL && i<ResultCount);
				

 
				  }
			      
				  
				  
				  return  ResultCount;
			 }
		     return NOT_FIND;
		}
		return 0;
    
}
int CNumberIndex::Insert(UL nkey,CElement *pElement,CPOSITION * pos)
{
	
	int HashOffset;
	CNumberIndexItem  *pPre, *pCur,*pNewItem=NULL;
	 
	pNewItem=new CNumberIndexItem(nkey,pElement,NULL,NULL);
	if (!pNewItem)
	{
		
        return OUT_OFF_MEMORY; 
	}
    
    pNewItem->m_pLinkElement=pElement;   
    
   	HashOffset=HashFuc(nkey);  
	
   if (m_HashTable[HashOffset]==NULL)
	{
        
		m_HashTable[HashOffset]=pNewItem;
		
	}
	else
	{   
		
		pCur=m_HashTable[HashOffset];
		pPre=pCur;
		while(pCur)
		{   
			if(m_nUnique==1 && pCur->m_nKey==nkey )
			{
               
				return TIME_IS_UNIQUE;   
				
			}
			pPre=pCur;
			pCur=pCur->m_pNext;   
		}
		pPre->m_pNext=pNewItem;
		pNewItem->m_pPre=pPre; 


	}
	pos->Point =(POINTER)pNewItem;
	pos->Offset=HashOffset;


    

    return 0;


    
}
int CNumberIndex::Delete(UL nkey)
{    

     CElement ** Del;
    int DelCount;
	CNumberIndexItem * DelItem;
    int DelItemOffset;
	DelCount=Find(nkey,&Del);

	if (DelCount>0)
	{
		int i;
		for(i=0;i<DelCount;i++)
		{   
		   DelItem=(CNumberIndexItem *)(Del[i]->GetRelativeIndex(1)->Point);  
		   DelItemOffset=Del[i]->GetRelativeIndex(1)->Offset;   
		   if(!(DelItem->m_pPre))
		   {
                
		 	   m_HashTable[DelItemOffset]=DelItem->m_pNext;
           /*    
			   if(DelItem->m_pNext)
			   {
				   DelItem->m_pNext->m_pPre=NULL; 
			   }
             */
		   }
		   else
		   {
               DelItem->m_pPre->m_pNext=DelItem->m_pNext;
		   }
		   if(DelItem->m_pNext)
		   {
		
			   DelItem->m_pNext->m_pPre=DelItem->m_pPre; 
		   }
		    delete     DelItem;
            

		}
		delete     Del[i];

	}
	else
	{
		return NOT_FIND;
	}

    return 0;
	 
    
}

int CNumberIndex::Delete(CPOSITION * pos,  CElement *pElement)
{    
	 

  
	      if(!pos)
		  {
			  return -1;
		  }
          if(!(pos->Point))
		  {
			  return -1;
		  }
		  
		   CNumberIndexItem * DelItem;
		   DelItem=(CNumberIndexItem *)(pos->Point); 
		   if(DelItem->m_pLinkElement!=pElement)
		   {
			   return -1;
		   }

		   if(DelItem->m_pPre==NULL)
		   {
                
			   m_HashTable[pos->Offset]=DelItem->m_pNext;

			  /* 
			   if(DelItem->m_pNext)
			   {
                   DelItem->m_pNext->m_pPre=NULL; 
			   }
			   */

		   }
		   else
		   {
               DelItem->m_pPre->m_pNext=DelItem->m_pNext;
			  
		   }
		   if(DelItem->m_pNext)
		   {
               DelItem->m_pNext->m_pPre=DelItem->m_pPre;
		   }
		   
		   delete     DelItem;
           
	

	
	 return 0;
      
     
}

int  CMapIndex::FindForClear(UL nOffset,CElement *** pResult)
{

	int ResultCount=0;
	int HashOffset;
    CNumberIndexItem  *pNext=NULL;
	
    CNumberIndexItem  *ptemp=NULL,*ptempForDel=NULL,*pNewResult=NULL,*pPre=NULL;
        
	HashOffset=nOffset;     // HashFuc(nkey);

	if (m_HashTable[HashOffset] && !(m_HashTable[HashOffset]->m_pNext))
	{
	
			 
			 
			 *pResult=new CElement *[1];
			 if (!(*pResult))
			 {
				 return OUT_OFF_MEMORY;
			 }
			 
			 

			 (*pResult)[0]=m_HashTable[HashOffset]->m_pLinkElement;
             ResultCount=1;  
			 return  ResultCount;  
			 
			 
             
		} 
	    else if (!m_HashTable[HashOffset])
		{
		     return NOT_FIND;
		}
	    else
		{   
		     pNext=m_HashTable[HashOffset];
		     while(pNext!=NULL)
			 {
                  
				  
				        
					  
					   ResultCount++;
                       
					   if (ResultCount==1)
					   {
						    pNewResult=new  CNumberIndexItem(pNext->m_nKey,pNext->m_pLinkElement,NULL,pNext);
					        if (!pNewResult)
							{
								return OUT_OFF_MEMORY;
							}
							
                            pPre=pNewResult;
                            
						
					   }
					   else
					   {
                             
						    pNewResult=new  CNumberIndexItem(pNext->m_nKey,pNext->m_pLinkElement,pPre,pNext);
			                if (!pNewResult)
							{
								return OUT_OFF_MEMORY;
							}

							pPre=pNewResult;


					   }
					   pNext=pNext->m_pNext; 
			 
			 }
			 
				  
				 
                
			 
			 
			 if (ResultCount>0)
			 {     
                  ptemp=pNewResult; 
			      *pResult=new  CElement *[ResultCount];
				  if(!(*pResult))
				  {
					  return OUT_OFF_MEMORY;
				  }
                  int i=0;
			      if (ptemp!=NULL)
				  {
	    		       do
					   {
    			            (*pResult)[i]=ptemp->m_pLinkElement;
				            ptempForDel=ptemp;      
							ptemp=ptemp->m_pNext;
							delete  ptempForDel;
						
                            i++;
					   }while(ptemp!=NULL && i<ResultCount);
				

 
				  }
			      
				  
				  
				  return  ResultCount;
			 }
		     return NOT_FIND;
		}		
	
   
}

int CNumberIndex::HashFuc(UL nkey)
{   

	return nkey  % m_nHashSize;
}

int CMapIndex::HashFuc(UL nkey)
{   
	return (nkey / 96  )  %  m_nHashSize;
}



void CNumberIndex::print()
{
      CNumberIndexItem  *Next, *tail;
       UL i;
	 
      for(i=0;i<m_nHashSize;i++)
	  {
	     
		  if (m_HashTable[i]!=NULL) 
		   {   
		       std::cout<<"在HASH中的位置"<<i<<"/ Key:";
                   
		       std::cout<<m_HashTable[i]->m_nKey<<"->";
		   	   Next=m_HashTable[i]->m_pNext;
		       while(Next!=NULL)
			   {
                     std::cout<<Next->m_nKey<<"->";
					 tail=Next;

			         Next=Next->m_pNext; 
			   }
			    while(tail)
			   {
				   std::cout<<tail->m_nKey<<"<-";
				   tail=tail->m_pPre; 
			   }
		       std::cout<<"\n";
		   }
		 



    
	  }  

}

void CNumberIndex::printHash()
{ 
	
	for(int i=0; i < (int)m_nHashSize; i++)
	{
			  
	 
		     std::cout<<"在HASH中的位置"<<i<<" value is:"<<m_HashTable[i]<<"\n";
	}
    	  
		 



}