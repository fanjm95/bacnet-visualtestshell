// SendWriteProp.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendWriteProp.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace NetworkSniffer {
	extern char *BACnetPropertyIdentifier[];
}

BACnetAPDUEncoder CSendWriteProp::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendWriteProp dialog

IMPLEMENT_DYNCREATE( CSendWriteProp, CPropertyPage )

CSendWriteProp::CSendWriteProp( void )
	: CSendPage( CSendWriteProp::IDD )
	, m_ObjectID( this, IDC_OBJECTID )
	, m_PropCombo( this, IDC_PROPCOMBO, NetworkSniffer::BACnetPropertyIdentifier, 124, true )
	, m_ArrayIndex( this, IDC_ARRAYINDEX )
	, m_Priority( this, IDC_PRIORITYX )
{
	//{{AFX_DATA_INIT(CSendWriteProp)
	//}}AFX_DATA_INIT
}

void CSendWriteProp::DoDataExchange(CDataExchange* pDX)
{
	TRACE1( "CSendWriteProp::DoDataExchange(%d)\n", pDX->m_bSaveAndValidate );

	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendWriteProp)
	//}}AFX_DATA_MAP

	m_ObjectID.UpdateData( pDX->m_bSaveAndValidate );
	m_PropCombo.UpdateData( pDX->m_bSaveAndValidate );
	m_ArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	m_Priority.UpdateData( pDX->m_bSaveAndValidate );
}

BEGIN_MESSAGE_MAP(CSendWriteProp, CPropertyPage)
	//{{AFX_MSG_MAP(CSendWriteProp)
	ON_EN_CHANGE(IDC_OBJECTID, OnChangeObjectID)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_VALUE, OnValue)
	ON_EN_CHANGE(IDC_PRIORITYX, OnChangePriority)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendWriteProp::OnInitDialog
//

BOOL CSendWriteProp::OnInitDialog() 
{
	CPropertyPage::OnInitDialog();
	
	// load the enumeration table
	m_PropCombo.LoadCombo();
	
	return TRUE;
}

//
//	CSendWriteProp::InitPage
//

void CSendWriteProp::InitPage( void )
{
	// flush the data
	m_ObjectID.ctrlNull = true;
	m_PropCombo.ctrlNull = true;
	m_ArrayIndex.ctrlNull = true;
}

//
//	CSendWriteProp::EncodePage
//

void CSendWriteProp::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 15 );

	// encode the object ID
	if (m_ObjectID.ctrlNull)
		throw "Object ID required";
	m_ObjectID.Encode( enc, 0 );

	// encode the property
	if (m_PropCombo.ctrlNull)
		throw "Property ID required";
	m_PropCombo.Encode( enc, 1 );

	// encode the (optional) array index
	if (!m_ArrayIndex.ctrlNull)
		m_ArrayIndex.Encode( enc, 2 );

	// do the value
	if (m_Value.m_anyList.Length() < 1)
		throw "Value required";
	m_Value.Encode( enc, 3 );

	// encode the (optional) priority
	if (!m_Priority.ctrlNull) {
		if ((m_Priority.uintValue < 1) || (m_Priority.uintValue > 16))
			throw "Priority out of range 1..16";
		m_Priority.Encode( enc, 4 );
	}

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendWriteProp::SavePage
//

void CSendWriteProp::SavePage( void )
{
	TRACE0( "CSendWriteProp::SavePage\n" );

	pageContents.Flush();

	m_ObjectID.SaveCtrl( pageContents );
	m_PropCombo.SaveCtrl( pageContents );
	m_ArrayIndex.SaveCtrl( pageContents );
	m_Value.SaveCtrl( pageContents );
	m_Priority.SaveCtrl( pageContents );
}

//
//	CSendWriteProp::RestorePage
//

void CSendWriteProp::RestorePage( void )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendWriteProp::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	m_ObjectID.RestoreCtrl( dec );
	m_PropCombo.RestoreCtrl( dec );
	m_ArrayIndex.RestoreCtrl( dec );
	m_Value.RestoreCtrl( dec );
	m_Priority.RestoreCtrl( dec );
}

//
//	CSendWriteProp::OnChangeX
//
//	The following set of functions are called when one of the interface elements
//	has changed.
//

void CSendWriteProp::OnChangeObjectID()
{
	m_ObjectID.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnObjectIDButton() 
{
	VTSObjectIdentifierDialog	dlg
	;

	dlg.objID = m_ObjectID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		m_ObjectID.ctrlNull = false;
		m_ObjectID.objID = dlg.objID;
		m_ObjectID.ObjToCtrl();

		SavePage();
		UpdateEncoded();
	}
}

void CSendWriteProp::OnSelchangePropCombo()
{
	m_PropCombo.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnChangeArrayIndex()
{
	m_ArrayIndex.UpdateData();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnValue() 
{
	m_Value.DoModal();
	SavePage();
	UpdateEncoded();
}

void CSendWriteProp::OnChangePriority()
{
	m_Priority.UpdateData();
	SavePage();
	UpdateEncoded();
}