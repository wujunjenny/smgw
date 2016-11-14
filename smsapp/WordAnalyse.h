// WordAnalyse.h: interface for the CWordAnalyse class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORDANALYSE_H__936C968E_AF93_4098_8AD1_86B36B2D4F0C__INCLUDED_)
#define AFX_WORDANALYSE_H__936C968E_AF93_4098_8AD1_86B36B2D4F0C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "routetable.h"

#define MAX_WORD_LENGTH		256
#define MAX_WORD_NUMBER		8
#define MAX_WORD_DEST_LEN	MAX_WORD_LENGTH
#define MAX_DELIMITER_NUM	8

class CWordDest
{
public:
	char m_Word[ MAX_WORD_NUMBER ][ MAX_WORD_DEST_LEN ];
	int	m_WordLen[ MAX_WORD_NUMBER ];
	char m_Flag[ MAX_WORD_NUMBER ];

	CWordDest()
	{
		Clean();
	}

	virtual ~CWordDest(){}

	void Clean()
	{
		memset( this, 0, sizeof( CWordDest ) );
	}
};

class CWordAnalyse  
{
public:
	void SetWord( char * const str, int pos );
	void Analyse( char * buf, int len, CWordDest& cwd );

	void AddDelimiter( char  c )
	{
		if ( m_DelimiterNum < MAX_DELIMITER_NUM )
		{
			m_Delimiter[ m_DelimiterNum++ ] = c;
		}
	}

	void Clean()
	{
		m_WordTable.DeleteAllItem();
		m_DelimiterNum = 0;
	}

	CWordAnalyse()
	{
		Clean();
	}

	virtual ~CWordAnalyse()
	{
		Clean();
	}

private:
	char m_Delimiter[ MAX_DELIMITER_NUM ];
	int m_DelimiterNum;

	CRouteTable m_WordTable;


	inline void WORDTRIM( char *w, int& wl )
	{
		if ( m_DelimiterNum < 1 || wl < 1 )
			return;

		int f = 0;
		for ( int i = wl-1; i >= 0; i -- )
		{
			f = 0;
			for ( int j = 0; j < m_DelimiterNum; j ++ )
			{
				if ( w[ i ] == m_Delimiter[ j ] )
				{
					w[ i ] = '\0';
					wl --;
					f = 1;
					break;
				}
			}
			if ( !f )
				break;
		}
	}

};

#endif // !defined(AFX_WORDANALYSE_H__936C968E_AF93_4098_8AD1_86B36B2D4F0C__INCLUDED_)
