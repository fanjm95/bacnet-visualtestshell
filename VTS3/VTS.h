// VTS.h : main header file for the VTS application
//

#if !defined(AFX_VTS_H__BDE65080_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
#define AFX_VTS_H__BDE65080_B82F_11D3_BE52_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

//
//	VTSApp
//

class VTSApp : public CWinApp {
	public:
		VTSApp();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(VTSApp)
		public:
		virtual BOOL InitInstance();
		virtual int ExitInstance();
		virtual BOOL PreTranslateMessage(MSG* pMsg);
		//}}AFX_VIRTUAL

	// Implementation
		//{{AFX_MSG(VTSApp)
		afx_msg void OnAppAbout();
			// NOTE - the ClassWizard will add and remove member functions here.
			//    DO NOT EDIT what you see in these blocks of generated code !
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

typedef VTSApp *VTSAppPtr;

//
//	Application specific windows messages
//

#define WM_VTS_RCOUNT		(WM_APP+1)			// new record count
#define WM_VTS_PORTSTATUS	(WM_APP+2)			// new port status
#define WM_VTS_EXECMSG		(WM_APP+3)			// new executor message

//
//	VTSPreferences
//

class VTSPreferences {
	public:
		int		summaryFields;					// bit mask of display options
				//	0		packet number
				//	1		time
				//	2		source
				//	3		destination
		int		summaryNameWidth;				// summary view name width
		int		summaryTimeFormat;				// 0=local time, 1=UTC

		void Load( void );
		void Save( void );
	};

typedef VTSPreferences *VTSPreferencesPtr;

extern VTSPreferences	gVTSPreferences;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTS_H__BDE65080_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)