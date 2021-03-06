#if !defined(AFX_SENDVTCLOSEERROR_H__90AF9F83_56E3_11D4_BEC0_00A0C95A9812__INCLUDED_)
#define AFX_SENDVTCLOSEERROR_H__90AF9F83_56E3_11D4_BEC0_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SendVTCloseError.h : header file
//

#include "SendPage.h"
#include "VTSCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSendVTCloseError dialog

class CSendVTCloseError : public CSendPage
{
	DECLARE_DYNCREATE(CSendVTCloseError)

// Construction
public:
	CSendVTCloseError( void );   // non-standard constructor

	VTSIntegerCtrl			m_InvokeID;
	VTSEnumeratedCtrl		m_ServiceCombo;
	VTSEnumeratedCtrl		m_ErrorClassCombo;
	VTSEnumeratedCtrl		m_ErrorCodeCombo;

	VTSListCtrl		m_IDCtrl;					// list of session ID's

	void InitPage( void );						// give it a chance to init
	void EncodePage( CByteArray* contents );	// encode the page

	static BACnetAPDUEncoder	pageContents;

	void SavePage( void );						// save contents
	void RestorePage( int index = 0 );					// restore contents to last saved values

// Dialog Data
	//{{AFX_DATA(CSendVTCloseError)
	enum { IDD = IDD_SENDVTCLOSEERROR };
	CListCtrl	m_IDList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSendVTCloseError)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSendVTCloseError)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeInvokeID();
	afx_msg void OnSelchangeServiceCombo();
	afx_msg void OnSelchangeErrorClassCombo();
	afx_msg void OnSelchangeErrorCodeCombo();
	afx_msg void OnAddID();
	afx_msg void OnRemoveID();
	afx_msg void OnChangeID();
	afx_msg void OnItemchangingIDList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SENDVTCLOSEERROR_H__90AF9F83_56E3_11D4_BEC0_00A0C95A9812__INCLUDED_)
