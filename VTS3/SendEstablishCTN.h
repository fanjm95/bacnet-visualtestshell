#if !defined(AFX_SENDESTABLISHCTN_H__7DC66987_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)
#define AFX_SENDESTABLISHCTN_H__7DC66987_5224_11D4_BEBC_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendEstablishCTN.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendEstablishCTN dialog

class CSendEstablishCTN : public CSendPage
{
	DECLARE_DYNCREATE(CSendEstablishCTN)

// Construction
public:
	CSendEstablishCTN( void );   // non-standard constructor

	VTSIntegerCtrl	m_DNET;						// DNET
	VTSIntegerCtrl	m_TermTime;					// termination time

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( void );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendEstablishCTN)
	enum { IDD = IDD_SENDESTABLISHCTN };
		// NOTE - ClassWizard will add data members here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendEstablishCTN)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendEstablishCTN)
	afx_msg void OnChangeDNET();
	afx_msg void OnChangeTermTime();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDESTABLISHCTN_H__7DC66987_5224_11D4_BEBC_00A0C95A9812__INCLUDED_)