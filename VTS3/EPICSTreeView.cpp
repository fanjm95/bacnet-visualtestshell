// EPICSTreeView.cpp : implementation file
//

#include "stdafx.h"
#include <afxrich.h>
#include <process.h>
#include "vts.h"


#include "DockingEPICSViewBar.h"
#include "EPICSTreeView.h"
#include "EPICSViewInfoPanel.h"
#include "EPICSViewNode.h"



///////////////////////////////
namespace PICS {
#include "db.h" 
#include "service.h"
#include "vtsapi.h"
#include "props.h"
#include "bacprim.h"
#include "dudapi.h"
#include "propid.h"
}

extern PICS::PICSdb * gPICSdb;



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEPICSTreeView

IMPLEMENT_DYNCREATE(CEPICSTreeView, CFormView)

CEPICSTreeView::CEPICSTreeView() : CFormView(CEPICSTreeView::IDD)
{
	m_pwndSplit = NULL;
	m_pnodeview = NULL;

	//{{AFX_DATA_INIT(CEPICSTreeView)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}

CEPICSTreeView::~CEPICSTreeView()
{
}

void CEPICSTreeView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEPICSTreeView)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEPICSTreeView, CFormView)
	//{{AFX_MSG_MAP(CEPICSTreeView)
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_EPICSV_RESET, OnEpicsReset)
	ON_BN_CLICKED(IDC_EPICSV_LOAD, OnEpicsLoad)
	ON_BN_CLICKED(IDC_EPICSV_INFO, OnEpicsInfoPanel)
	ON_NOTIFY(TVN_SELCHANGED, IDC_EPICSV_TREE, OnSelchangedEpicsTree)
	ON_BN_CLICKED(IDC_EPICSV_EDIT, OnEpicsEdit)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEPICSTreeView diagnostics

#ifdef _DEBUG
void CEPICSTreeView::AssertValid() const
{
	CFormView::AssertValid();
}

void CEPICSTreeView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

VTSDoc* CEPICSTreeView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(VTSDoc)));
	return (VTSDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CEPICSTreeView message handlers



/////////////////////////////////////////////////////////////////////////////
// CEPICSView drawing



void CEPICSTreeView::OnSize(UINT nType, int cx, int cy) 
{
	// Calculate minimum sizes
	cx = cx < 120 ? 120 : cx;
	cy = cy < 130 ? 130 : cy;

	SetScrollSizes(MM_TEXT, CSize(cx,cy));

	// Resize the tree control
	ResizeControl(GetDlgItem(IDC_EPICSV_TREE), cx, cy);
	ResizeControl(GetDlgItem(IDC_EPICSV_FILENAME), cx, -1);

	CFormView::OnSize(nType, cx, cy);
	
}


void CEPICSTreeView::ResizeControl(CWnd * pwnd, int cx, int cy)
{
	RECT rc;

	if ( pwnd != NULL )
	{
		pwnd->GetWindowRect(&rc);
		ScreenToClient(&rc);
		pwnd->SetWindowPos(NULL, 0, 0, cx - 3 - rc.left, cy == -1 ? rc.bottom - rc.top : cy - 3 - rc.top, SWP_NOMOVE | SWP_NOZORDER);
	}
}


void CEPICSTreeView::OnEpicsReset() 
{
	CString str;
	GetDlgItem(IDC_EPICSV_FILENAME)->GetWindowText(str);

	if ( str.GetLength() != 0 )
		EPICSLoad(str);
}


void CEPICSTreeView::OnEpicsLoad() 
{
	CFileDialog	fd( TRUE, "tpi", NULL, OFN_FILEMUSTEXIST, "EPICS (*.tpi)|*.tpi||" );
	
	if (fd.DoModal() == IDOK)
		EPICSLoad(fd.GetPathName());
}


void CEPICSTreeView::OnEpicsEdit() 
{
	CString str;
	GetDlgItem(IDC_EPICSV_FILENAME)->GetWindowText(str);

	if ( str.GetLength() != 0 )
	{
		CString str2;
		str2.Format("notepad.exe \"%s\"", str);
		_spawnlp(_P_NOWAIT, "notepad.exe", str2, NULL);
	}
}


void CEPICSTreeView::OnEpicsInfoPanel() 
{
}


void CEPICSTreeView::SetSplitter( CDockingEPICSViewBar * pbar, CSplitterWnd * pwndSplit, CCreateContext * pContext )
{
	m_pwndSplit = pwndSplit;
	m_pContext = pContext;
	m_pbar = pbar;
	CreateInfoView(RUNTIME_CLASS(CEPICSViewInfoPanel), FALSE);
	SetTreeImageList();
	Refresh();
}



CView * CEPICSTreeView::CreateInfoView( CRuntimeClass * pviewclass, BOOL fDeleteOldView )
{
	if ( m_pwndSplit == NULL )
		return NULL;

	int nRow = 1;
	int nCol = 0;

	if ( m_pwndSplit->GetColumnCount() == 2 )
	{
		nRow = 0;
		nCol = 1;
	}

	if ( fDeleteOldView )
		m_pwndSplit->DeleteView(nRow, nCol);

	if ( !m_pwndSplit->CreateView(nRow, nCol, pviewclass, CSize(0, 0), m_pContext) )
	{
		TRACE0("Failed to create edit pane on EPICS view\n");
		return NULL;
	}

	m_pnodeview = (CView *) m_pwndSplit->GetPane(nRow, nCol);
	return m_pnodeview;
}


void CEPICSTreeView::DeleteDB()
{
	if ( gPICSdb )
	{
		PICS::MyDeletePICSObject( gPICSdb->Database );
		delete gPICSdb;
		gPICSdb = NULL;
		m_pbar->m_nSyntaxE = 0;
		m_pbar->m_nConE = 0;
	}
}



// Return TRUE if load was successful

BOOL CEPICSTreeView::EPICSLoad( LPCSTR lpszFileName )
{
	GetDlgItem(IDC_EPICSV_FILENAME)->SetWindowText( lpszFileName == NULL ? "" : lpszFileName );

	if ( lpszFileName == NULL || lstrlen(lpszFileName) == 0 )
	{
		Refresh();
		return TRUE;
	}

	DeleteDB();

	// make a new database
	gPICSdb = new PICS::PICSdb;

	// read in the EPICS
	// madanner 6/03: ReadTextPICS now returns false if canceled by user

	if ( PICS::ReadTextPICS( (char *) (LPCSTR) lpszFileName, gPICSdb, &m_pbar->m_nSyntaxE, &m_pbar->m_nConE ) )
	{
		if ( m_pbar->m_nSyntaxE == 0 )
			gVTSPreferences.Setting_SetLastEPICS(lpszFileName);
		else
			DeleteDB();
	}
	else
	{
		// Cancelled by user... clear DB, reset last load, etc.
		gVTSPreferences.Setting_SetLastEPICS("");
		DeleteDB();
		GetDlgItem(IDC_EPICSV_FILENAME)->SetWindowText("");
	}

	Refresh();
	return gPICSdb == 0 ? FALSE : TRUE;
}


// Reload all of the controls and such for new EPICS database

void CEPICSTreeView::Refresh()
{
	CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);
	WipeOut(ptree, ptree->GetRootItem());
	ptree->DeleteAllItems();

	// Build tree nodes...  
	HTREEITEM htreeitemLast = NULL;

	// Main node title... EPICS name
	LPCSTR lpsz = "No EPICS Data";
	if ( gPICSdb == NULL )
	{
		CString str;
		GetDlgItem(IDC_EPICSV_FILENAME)->GetWindowText(str);
		if ( str.GetLength() != 0 )
			lpsz = "EPICS Error";
	}
	else
	{
		lpsz = gPICSdb->ProductName;
		GetDlgItem(IDC_EPICSV_FILENAME)->SetWindowText(gVTSPreferences.Setting_GetLastEPICS());
	}

	HTREEITEM htreeitemRoot = ptree->InsertItem(TVIF_TEXT | TVIF_PARAM | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpsz, 0, 0, 0, 0, (LPARAM) new CEPICSViewNodeRoot(this, m_pbar->m_nSyntaxE, m_pbar->m_nConE), NULL, htreeitemLast );

	// Create BIBB node
	htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("BIBBs"), 1, 1, 0, 0, (LPARAM) NULL, htreeitemRoot, NULL );

	// Create Application Services
	htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Application Services"), 2, 2, 0, 0, (LPARAM) NULL, htreeitemRoot, htreeitemLast );

	// Create Object Types
	htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Object Types"), 3, 3, 0, 0, (LPARAM) NULL, htreeitemRoot, htreeitemLast );

	// Create Data Link Options
	htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Data Link Options"), 4, 4, 0, 0, (LPARAM) NULL, htreeitemRoot, htreeitemLast );

	// Create Character Sets
	htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Character Sets"), 5, 5, 0, 0, (LPARAM) NULL, htreeitemRoot, htreeitemLast );

	// Create Special Functionality
	htreeitemLast = ptree->InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_PARAM | TVIF_SELECTEDIMAGE, _T("Special Functionality"), 6, 6, 0, 0, (LPARAM) NULL, htreeitemRoot, htreeitemLast );

	// select the first item in the tree
	ptree->SelectItem(ptree->GetFirstVisibleItem());
}


void CEPICSTreeView::WipeOut( CTreeCtrl * ptree, HTREEITEM htreeitem )
{
	while (htreeitem != NULL )
	{
		if ( ptree->GetItemData(htreeitem) != NULL )
			delete (CEPICSViewNode *) ptree->GetItemData(htreeitem);
		WipeOut(ptree, ptree->GetNextItem(htreeitem, TVGN_CHILD));
		htreeitem = ptree->GetNextItem(htreeitem, TVGN_NEXT);
	}
}



CView * CEPICSTreeView::GetInfoPanel()
{
	return m_pnodeview;
}


void CEPICSTreeView::SetTreeImageList()
{
    m_imagelist.Create( IDB_EPICSTREE, 18, 2, RGB(255,0,255) );

	CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);
	ptree->SetImageList( &m_imagelist, TVSIL_NORMAL );
}


void CEPICSTreeView::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
}



void CEPICSTreeView::OnSelchangedEpicsTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

	CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);

	HTREEITEM htreeitem = pNMTreeView == NULL ? NULL : ptree->GetSelectedItem();
	CEPICSViewNode * pnode = htreeitem != NULL ? (CEPICSViewNode *) ptree->GetItemData(htreeitem) : NULL;

	if ( pnode != NULL )
		pnode->LoadInfoPanel();

	*pResult = 0;
}



void CEPICSTreeView::OnDestroy() 
{
	CTreeCtrl * ptree = (CTreeCtrl *) GetDlgItem(IDC_EPICSV_TREE);
	WipeOut(ptree, ptree->GetRootItem());
	ptree->DeleteAllItems();

	CFormView::OnDestroy();
}