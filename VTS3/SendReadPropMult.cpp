// SendReadPropMult.cpp : implementation file
//

#include "stdafx.h"
#include "VTS.h"

#include "Send.h"
#include "SendReadPropMult.h"

#include "VTSObjectIdentifierDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace NetworkSniffer {
	extern char *BACnetPropertyIdentifier[];
}

void EncoderToHex( const BACnetAPDUEncoder &enc, CString &str );

BACnetAPDUEncoder CSendReadPropMult::pageContents;

/////////////////////////////////////////////////////////////////////////////
// CSendReadPropMult dialog

IMPLEMENT_DYNCREATE( CSendReadPropMult, CPropertyPage )

#pragma warning( disable : 4355 )
CSendReadPropMult::CSendReadPropMult( void )
	: CSendPage( CSendReadPropMult::IDD )
	, m_PropListList( this )
{
	//{{AFX_DATA_INIT(CSendReadPropMult)
	//}}AFX_DATA_INIT
}
#pragma warning( default : 4355 )

void CSendReadPropMult::DoDataExchange(CDataExchange* pDX)
{
	ReadPropListPtr	rplp = m_PropListList.rpllCurElem
	;

	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendReadPropMult)
	DDX_Control(pDX, IDC_OBJLIST, m_ObjList);
	DDX_Control(pDX, IDC_PROPLIST, m_PropList);
	//}}AFX_DATA_MAP

	// if there is a selected object, allow the ObjID to update
	if (rplp)
		rplp->rplObjID.UpdateData( pDX->m_bSaveAndValidate );

	// if there is a selected property, allow the controls to update
	if (rplp && rplp->rplCurElem) {
		rplp->rplCurElem->rpePropCombo.UpdateData( pDX->m_bSaveAndValidate );
		rplp->rplCurElem->rpeArrayIndex.UpdateData( pDX->m_bSaveAndValidate );
	}
}

BEGIN_MESSAGE_MAP(CSendReadPropMult, CPropertyPage)
	//{{AFX_MSG_MAP(CSendReadPropMult)
	ON_BN_CLICKED(IDC_ADDOBJ, OnAddObj)
	ON_BN_CLICKED(IDC_REMOVEOBJ, OnRemoveObj)
	ON_EN_CHANGE(IDC_OBJID, OnChangeObjID)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_OBJLIST, OnItemchangingObjList)
	ON_BN_CLICKED(IDC_ADDPROP, OnAddProp)
	ON_BN_CLICKED(IDC_REMOVEPROP, OnRemoveProp)
	ON_NOTIFY(LVN_ITEMCHANGING, IDC_PROPLIST, OnItemchangingPropList)
	ON_CBN_SELCHANGE(IDC_PROPCOMBO, OnSelchangePropCombo)
	ON_EN_CHANGE(IDC_ARRAYINDEX, OnChangeArrayIndex)
	ON_BN_CLICKED(IDC_OBJECTIDBTN, OnObjectIDButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//
//	CSendReadPropMult::InitPage
//

void CSendReadPropMult::InitPage( void )
{
	TRACE0( "CSendReadPropMult::InitPage()\n" );
}

//
//	CSendReadPropMult::EncodePage
//

void CSendReadPropMult::EncodePage( CByteArray* contents )
{
	CByteArray			header
	;
	BACnetAPDUEncoder	enc
	;

	// encode the service choice
	header.Add( 14 );

	// encode the contents
	m_PropListList.Encode( enc );

	// copy the encoding into the byte array
	for (int i = 0; i < enc.pktLength; i++)
		header.Add( enc.pktBuffer[i] );

	// stuff the header on the front
	contents->InsertAt( 0, &header );
}

//
//	CSendReadPropMult::SavePage
//

void CSendReadPropMult::SavePage( void )
{
	TRACE0( "CSendReadPropMult::SavePage\n" );

	pageContents.Flush();

	// ### save the list
}

//
//	CSendReadPropMult::RestorePage
//

void CSendReadPropMult::RestorePage( void )
{
	BACnetAPDUDecoder	dec( pageContents )
	;

	TRACE0( "CSendReadPropMult::RestorePage\n" );

	if (dec.pktLength == 0)
		return;

	// ### restore the list
}

//
//	CSendReadPropMult::OnInitDialog
//

BOOL CSendReadPropMult::OnInitDialog() 
{
	TRACE0( "CSendReadPropMult::OnInitDialog()\n" );

	CDialog::OnInitDialog();
	
	// only allow one selection at a time, no sorting
	m_ObjList.m_nFlags |= LVS_SINGLESEL;
	m_ObjList.m_nFlags &= ~LBS_SORT;

	// initialize the object list
	m_ObjList.InsertColumn( 0, "Object ID", LVCFMT_LEFT, 128 );

	// only allow one selection at a time, no sorting
	m_PropList.m_nFlags |= LVS_SINGLESEL;
	m_PropList.m_nFlags &= ~LBS_SORT;

	// set up the property list columns
	m_PropList.InsertColumn( 0, "Property", LVCFMT_LEFT, 96 );
	m_PropList.InsertColumn( 1, "Index", LVCFMT_RIGHT, 48 );

	// disable the controls, they'll be enabled when an object is selected
	GetDlgItem( IDC_OBJID )->EnableWindow( false );
	GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );
	GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
	GetDlgItem( IDC_PROPCOMBO )->EnableWindow( false );
	GetDlgItem( IDC_ARRAYINDEX )->EnableWindow( false );
	
	// load the enumeration table
	CComboBox	*cbp = (CComboBox *)GetDlgItem( IDC_PROPCOMBO );
	for (int i = 0; i < MAX_PROP_ID; i++)
		cbp->AddString( NetworkSniffer::BACnetPropertyIdentifier[i] );

	return TRUE;
}

void CSendReadPropMult::OnAddObj() 
{
	m_PropListList.AddButtonClick();
}

void CSendReadPropMult::OnRemoveObj() 
{
	m_PropListList.RemoveButtonClick();
}

void CSendReadPropMult::OnChangeObjID() 
{
	m_PropListList.OnChangeObjID();
}

void CSendReadPropMult::OnObjectIDButton() 
{
	m_PropListList.OnObjectIDButton();
}

void CSendReadPropMult::OnItemchangingObjList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	m_PropListList.OnItemChanging( pNMHDR, pResult );
}

void CSendReadPropMult::OnAddProp() 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->AddButtonClick();
}

void CSendReadPropMult::OnRemoveProp() 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->RemoveButtonClick();
}

void CSendReadPropMult::OnItemchangingPropList(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->OnItemChanging( pNMHDR, pResult );
}

void CSendReadPropMult::OnSelchangePropCombo()
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->OnSelchangePropCombo();
}

void CSendReadPropMult::OnChangeArrayIndex()
{
	if (m_PropListList.rpllCurElem)
		m_PropListList.rpllCurElem->OnChangeArrayIndex();
}

//
//	ReadPropElem::ReadPropElem
//

ReadPropElem::ReadPropElem( CSendPagePtr wp )
	: rpePropCombo( wp, IDC_PROPCOMBO, NetworkSniffer::BACnetPropertyIdentifier, MAX_PROP_ID, true )
	, rpeArrayIndex( wp, IDC_ARRAYINDEX )
{
	// controls start out disabled
	rpePropCombo.ctrlEnabled = false;
	rpeArrayIndex.ctrlEnabled = false;
}

//
//	ReadPropElem::Bind
//

void ReadPropElem::Bind( void )
{
	// set the control value to this element values
	rpePropCombo.ObjToCtrl();
	rpePropCombo.Enable();
	rpeArrayIndex.ObjToCtrl();
	rpeArrayIndex.Enable();
}

//
//	ReadPropElem::Unbind
//

void ReadPropElem::Unbind( void )
{
	// clear out the contents of the controls
	rpePropCombo.ctrlWindow->GetDlgItem( IDC_PROPCOMBO )->SetWindowText( "" );
	rpePropCombo.Disable();
	rpeArrayIndex.ctrlWindow->GetDlgItem( IDC_ARRAYINDEX )->SetWindowText( "" );
	rpeArrayIndex.Disable();
}

//
//	ReadPropElem::Encode
//

void ReadPropElem::Encode( BACnetAPDUEncoder& enc )
{
	// encode the property
	if (rpePropCombo.ctrlNull)
		throw "Property ID required";
	rpePropCombo.Encode( enc, 0 );

	// encode the (optional) array index
	if (!rpeArrayIndex.ctrlNull)
		rpeArrayIndex.Encode( enc, 1 );
}

//
//	ReadPropList::ReadPropList
//

ReadPropList::ReadPropList( CSendReadPropMultPtr pp )
	: rplPagePtr( pp )
	, rplCurElem(0), rplCurElemIndx(0)
	, rplObjID( pp, IDC_OBJID )
{
	// give the object ID a default value
	rplObjID.ctrlEnabled = false;
	rplObjID.ctrlNull = false;
	rplObjID.objID = 0;
}

//
//	ReadPropList::~ReadPropList
//
//	If there have been any property value objects added to the list they need to 
//	be removed here.
//

ReadPropList::~ReadPropList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	ReadPropList::Bind
//

void ReadPropList::Bind( void )
{
	int			i
	;
	CString		someText
	;

	// set the control value to this object id
	rplObjID.ObjToCtrl();
	rplObjID.Enable();
	rplPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( true );

	// fill out the table with the current list of elements
	i = 0;
	for (POSITION pos = GetHeadPosition(); pos != NULL; i++ ) {
		ReadPropElemPtr	rpep = GetNext( pos )
		;

		rplPagePtr->m_PropList.InsertItem( i
			, NetworkSniffer::BACnetPropertyIdentifier[ rpep->rpePropCombo.enumValue ]
			);
		if (rpep->rpeArrayIndex.ctrlNull)
			rplPagePtr->m_PropList.SetItemText( i, 1, "" );
		else {
			someText.Format( "%d", rpep->rpeArrayIndex.uintValue );
			rplPagePtr->m_PropList.SetItemText( i, 1, someText );
		}
	}

	// enable the other controls
	rplPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( true );
	rplPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( true );
}

//
//	ReadPropList::Unbind
//

void ReadPropList::Unbind( void )
{
	// clear out the contents of the object id control
	rplObjID.ctrlWindow->GetDlgItem( IDC_OBJID )->SetWindowText( "" );
	rplObjID.Disable();
	rplPagePtr->GetDlgItem( IDC_OBJECTIDBTN )->EnableWindow( false );

	// wipe out the contents of the table
	rplPagePtr->m_PropList.DeleteAllItems();

	// disable the other controls
	rplPagePtr->GetDlgItem( IDC_ADDPROP )->EnableWindow( false );
	rplPagePtr->GetDlgItem( IDC_REMOVEPROP )->EnableWindow( false );
}

//
//	ReadPropList::AddButtonClick
//

void ReadPropList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = rplPagePtr->m_PropList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = rplPagePtr->m_PropList.GetNextSelectedItem( selPos );
		rplPagePtr->m_PropList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	rplAddInProgress = true;
	rplPagePtr->m_PropList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	rplCurElem = new ReadPropElem( rplPagePtr );
	rplCurElemIndx = listLen;
	AddTail( rplCurElem );

	// bind the element to the controls
	rplCurElem->Bind();

	// update the encoding
	rplAddInProgress = false;
	rplPagePtr->UpdateEncoded();
}

//
//	ReadPropList::RemoveButtonClick
//

void ReadPropList::RemoveButtonClick( void )
{
	int					curRow = rplCurElemIndx
	;
	ReadPropElemPtr	curElem = rplCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	rplPagePtr->m_PropList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	rplPagePtr->m_PropList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	delete GetAt( pos );
	RemoveAt( pos );

	// update the encoding
	rplPagePtr->UpdateEncoded();
}

//
//	ReadPropList::OnSelchangePropCombo
//

void ReadPropList::OnSelchangePropCombo( void )
{
	if (rplCurElem) {
		rplCurElem->rpePropCombo.UpdateData();
		rplPagePtr->UpdateEncoded();

		rplPagePtr->m_PropList.SetItemText( rplCurElemIndx, 0
			, NetworkSniffer::BACnetPropertyIdentifier[ rplCurElem->rpePropCombo.enumValue ]
			);
	}
}

//
//	ReadPropList::OnChangeArrayIndex
//

void ReadPropList::OnChangeArrayIndex( void )
{
	if (rplCurElem) {
		CString		someText
		;

		rplCurElem->rpeArrayIndex.UpdateData();
		rplPagePtr->UpdateEncoded();

		if (rplCurElem->rpeArrayIndex.ctrlNull)
			rplPagePtr->m_PropList.SetItemText( rplCurElemIndx, 1, "" );
		else {
			someText.Format( "%d", rplCurElem->rpeArrayIndex.uintValue );
			rplPagePtr->m_PropList.SetItemText( rplCurElemIndx, 1, someText );
		}
	}
}

//
//	ReadPropList::OnItemChanging
//

void ReadPropList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = rplCurElemIndx
	;
	ReadPropElemPtr	curElem = rplCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (rplAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		rplCurElemIndx = pNMListView->iItem;
		rplCurElem = GetAt( FindIndex( rplCurElemIndx ) );

		// bind the new current element
		rplCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == rplCurElemIndx) {
			// set nothing selected
			rplCurElem = 0;
			rplCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	ReadPropList::Encode
//

void ReadPropList::Encode( BACnetAPDUEncoder& enc )
{
	// encode the object ID
	if (rplObjID.ctrlNull)
		throw "Object ID required";
	rplObjID.Encode( enc, 0 );

	// encode each of the bound properties
	BACnetOpeningTag().Encode( enc, 1 );
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
	BACnetClosingTag().Encode( enc, 1 );
}

//
//	ReadPropListList::ReadPropListList
//

ReadPropListList::ReadPropListList( CSendReadPropMultPtr pp )
	: rpllPagePtr( pp )
	, rpllCurElem(0), rpllCurElemIndx(-1)
{
}

//
//	ReadPropListList::~ReadPropListList
//
//	If there are any objects that have been added to the list they need to be 
//	deleted here.  The CList class will delete the stuff that it knows how to 
//	handle, but that doesn't include the ReadPropList objects.
//

ReadPropListList::~ReadPropListList( void )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		delete GetNext( pos );
}

//
//	ReadPropListList::AddButtonClick
//
//	This procedure is called when the user wants to add an additional object to
//	the list.
//

void ReadPropListList::AddButtonClick( void )
{
	int		listLen = GetCount()
	;

	// deselect if something was selected
	POSITION selPos = rpllPagePtr->m_ObjList.GetFirstSelectedItemPosition();
	if (selPos != NULL) {
		int nItem = rpllPagePtr->m_ObjList.GetNextSelectedItem( selPos );
		rpllPagePtr->m_ObjList.SetItemState( nItem, 0, LVIS_SELECTED );
	}

	// create a new list item
	rpllAddInProgress = true;
	rpllPagePtr->m_ObjList.InsertItem( listLen, "" );

	// create a new item, add to the end of the list
	rpllCurElem = new ReadPropList( rpllPagePtr );
	rpllCurElemIndx = listLen;
	AddTail( rpllCurElem );

	// bind the element to the controls
	rpllCurElem->Bind();

	// update the encoding
	rpllAddInProgress = false;
	rpllPagePtr->UpdateEncoded();
}

//
//	ReadPropListList::RemoveButtonClick
//

void ReadPropListList::RemoveButtonClick( void )
{
	int					curRow = rpllCurElemIndx
	;
	ReadPropListPtr	curElem = rpllCurElem
	;

	// must have a selected row
	if (curRow < 0)
		return;

	// deselect the row, this will cause an unbind
	rpllPagePtr->m_ObjList.SetItemState( curRow, 0, LVIS_SELECTED );

	// delete the row from the list
	rpllPagePtr->m_ObjList.DeleteItem( curRow );

	// delete the element
	POSITION pos = FindIndex( curRow );
	delete GetAt( pos );
	RemoveAt( pos );

	// update the encoding
	rpllPagePtr->UpdateEncoded();
}

//
//	ReadPropListList::OnChangeObjID
//

void ReadPropListList::OnChangeObjID( void )
{
	CString			rStr
	;

	// make sure we have a current object
	if (!rpllCurElem)
		return;

	// pass the message to the current selected object
	rpllCurElem->rplObjID.UpdateData();

	// get the control contents, update the list
	rpllPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( rStr );
	rpllPagePtr->m_ObjList.SetItemText( rpllCurElemIndx, 0, rStr );

	// update the encoding
	rpllPagePtr->UpdateEncoded();
}

//
//	ReadPropListList::OnObjectIDButton
//

void ReadPropListList::OnObjectIDButton( void )
{
	if (!rpllCurElem)
		return;

	VTSObjectIdentifierDialog	dlg
	;

	dlg.objID = rpllCurElem->rplObjID.objID;
	if (dlg.DoModal() && dlg.validObjID) {
		rpllCurElem->rplObjID.ctrlNull = false;
		rpllCurElem->rplObjID.objID = dlg.objID;
		rpllCurElem->rplObjID.ObjToCtrl();

		// copy the text from the control to the list
		CString text;
		rpllPagePtr->GetDlgItem( IDC_OBJID )->GetWindowText( text );
		rpllPagePtr->m_ObjList.SetItemText( rpllCurElemIndx, 0, text );

		rpllPagePtr->UpdateEncoded();
	}
}

//
//	ReadPropListList::OnItemChanging
//

void ReadPropListList::OnItemChanging( NMHDR *pNMHDR, LRESULT *pResult )
{
	int					curRow = rpllCurElemIndx
	;
	ReadPropListPtr	curElem = rpllCurElem
	;
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*)pNMHDR
	;

	// forget messages that don't change the selection state
	if (pNMListView->uOldState == pNMListView->uNewState)
		return;

	// skip messages during new item creation
	if (rpllAddInProgress)
		return;

	if ((pNMListView->uNewState * LVIS_SELECTED) != 0) {
		// item becoming selected
		rpllCurElemIndx = pNMListView->iItem;
		rpllCurElem = GetAt( FindIndex( rpllCurElemIndx ) );

		// bind the new current element
		rpllCurElem->Bind();
	} else {
		// item no longer selected
		if (pNMListView->iItem == rpllCurElemIndx) {
			// set nothing selected
			rpllCurElem = 0;
			rpllCurElemIndx = -1;

			// unbind from the controls
			curElem->Unbind();
		}
	}
}

//
//	ReadPropListList::Encode
//
//	Each ReadPropList element understands how to encode itself, so this procedure 
//	just calls Encode() for each of the objects in its list.
//

void ReadPropListList::Encode( BACnetAPDUEncoder& enc )
{
	for (POSITION pos = GetHeadPosition(); pos != NULL; )
		GetNext( pos )->Encode( enc );
}
