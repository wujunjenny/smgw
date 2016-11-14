// WordAnalyse.cpp: implementation of the CWordAnalyse class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "WordAnalyse.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void CWordAnalyse::SetWord( char * const str, int pos )
{
	if ( pos >= 0 && pos < MAX_WORD_NUMBER )
	{
		char tbuf[ MAX_WORD_DEST_LEN ];
		char * p = str;
		int i = 0;
		for ( ; *p; p++ )
		{
			if ( *p >= 'a' && *p <= 'z' )
				tbuf[ i++ ] = *p - ('a'-'A');
			else
				tbuf[ i++ ] = *p;
		}
		tbuf[ i ] = '\0';
		m_WordTable.ReplaceItem( tbuf, (HANDLE)(pos+1) );
	}
}

void CWordAnalyse::Analyse(char *buf, int len, CWordDest& cwd)
{
	char aybuf[ MAX_WORD_LENGTH ];

	cwd.Clean();

	if ( len > MAX_WORD_LENGTH - 1 )
		len = MAX_WORD_LENGTH - 1;

	if ( len <= 0 )
		return;

	memset( aybuf, 0, sizeof( aybuf ) );
	memcpy( aybuf, buf, len );
	for ( int i = 0; i < len; i++ )
	{
		char c = aybuf[ i ];
		/*
		for ( int j = 0; j < m_DelimiterNum; j ++ )
		{
			if ( c == m_Delimiter[ j ] )
			{
				aybuf[ i ] = ' ';
				continue;
			}
		}
		*/

		if ( c >= 'a' && c <= 'z' )
		{
			aybuf[ i ] = c - ('a'-'A');
			continue;
		}
	}

	// SMGW431-06, 2005-10-27, wzy modify begin //
	//char *p1, *p2, *p;
	char *p1 = 0, *p2 = 0;
	int GetPos = 0, GetPos2 = 0;
	
	//int l, l2;
	int l = 0;
	// SMGW431-06, 2005-10-27, wzy modify end //
	

	p1 = aybuf;

	for ( ; p1 - aybuf < len; p1++ )
	{
		//if ( *p1 == ' ' ) continue;
		l = 0;
		GetPos = ( int ) m_WordTable.GetItem( p1, l );
		if ( GetPos )
		{
			p1 += l;

nextbegin:
			for ( p2 = p1; p2 - aybuf < len; p2++ )
			{
				l = 0;
				GetPos2 = ( int ) m_WordTable.GetItem( p2, l );
				if ( GetPos2 )
				{
					if ( !cwd.m_Flag[ GetPos - 1 ] )
					{
						cwd.m_Flag[ GetPos - 1 ] = 1;
						cwd.m_WordLen[ GetPos - 1 ] = p2 - p1;
						memcpy( cwd.m_Word[ GetPos - 1 ], buf + (p1 - aybuf), p2 - p1 );
						WORDTRIM( cwd.m_Word[ GetPos - 1 ], cwd.m_WordLen[ GetPos - 1 ] );
					}

					GetPos = GetPos2;
					p1 = p2 + l;
					goto nextbegin;
				}
			}

			// 到了最后一个
			if ( !cwd.m_Flag[ GetPos - 1 ] )
			{
				cwd.m_Flag[ GetPos - 1 ] = 1;
				cwd.m_WordLen[ GetPos - 1 ] = len - (p1 - aybuf);
				memcpy( cwd.m_Word[ GetPos - 1 ], buf + (p1 - aybuf), len - (p1 - aybuf) );
				WORDTRIM( cwd.m_Word[ GetPos - 1 ], cwd.m_WordLen[ GetPos - 1 ] );
			}

			return;
		}
	}

}

