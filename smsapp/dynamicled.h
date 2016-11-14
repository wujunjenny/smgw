#if !defined(_DYNAMICLED_H_INCLUDED_)
#define _DYNAMICLED_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DynamicLED.h : header file
//
#define ID_LED_RED 2001
#define ID_LED_GREEN 2002
#define ID_LED_BLUE 2003

/////////////////////////////////////////////////////////////////////////////
// CDynamicLED window

class CDynamicLED : public CStatic
{
// Construction
public:
	CDynamicLED();

	// The constants needed to define the colours of the LED
	#define ID_LED_RED		2001
	#define ID_LED_GREEN	2002
	#define ID_LED_BLUE		2003
	#define ID_LED_YELLOW	2004

	// The constants needed to define the shape of the LED
	#define ID_SHAPE_ROUND	3001
	#define ID_SHAPE_SQUARE	3002
	
// Attributes
protected:
	
	
	// This variable is used to store the shape and color
	// set by the user for resetting the led later
	UINT m_nID;
	UINT m_nShape;

	BOOL m_bBright;

	CWnd *m_pWnd;

	int m_nTimerInterval,m_nPrevTimerInterval;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDynamicLED)
	//}}AFX_VIRTUAL

// Implementation
public:
	void SwitchOn();
	void SwitchOff();

	void SetLED(CWnd *pWnd,UINT nIDColor, UINT nIDShape, int nTimerInterval);
	
	virtual ~CDynamicLED();

private:
	// This function cannot be called directly from your dialog application.
	// Its used internally by this class to reset the LED again.

	void ResetLED(UINT nIDColor, UINT nIDShape, int nTimerInterval);

	// Generated message map functions
protected:
	//{{AFX_MSG(CDynamicLED)
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(_DYNAMICLED_H_INCLUDED_)
