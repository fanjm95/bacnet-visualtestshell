// VTSDeviceStatusDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vts.h"
#include "VTSDeviceStatusDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
namespace NetworkSniffer {
	extern char *BACnetDeviceStatus[];
}
/////////////////////////////////////////////////////////////////////////////
// VTSDeviceStatusDlg dialog


VTSDeviceStatusDlg::VTSDeviceStatusDlg(CWnd* pParent /*=NULL*/)
	: CDialog(VTSDeviceStatusDlg::IDD, pParent)
	, m_enumcombo( this, IDC_ENUMRATECOMBO, NetworkSniffer::BACnetDeviceStatus, 6, true )
{
	//{{AFX_DATA_INIT(VTSDeviceStatusDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void VTSDeviceStatusDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(VTSDeviceStatusDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(VTSDeviceStatusDlg, CDialog)
	//{{AFX_MSG_MAP(VTSDeviceStatusDlg)
	ON_CBN_SELCHANGE(IDC_ENUMRATECOMBO, OnSelchangeEnumratecombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// VTSDeviceStatusDlg message handlers

void VTSDeviceStatusDlg::OnSelchangeEnumratecombo() 
{	
	m_enumcombo.ctrlNull = false;
	m_enumcombo.CtrlToObj();
		
}
void VTSDeviceStatusDlg::Encode(BACnetAPDUEncoder& enc, int context)
{
	m_enumcombo.Encode(enc, context);
}
void VTSDeviceStatusDlg::Decode(BACnetAPDUDecoder& dec )
{	
	if(dec.pktLength!=0)
	m_enumcombo.Decode(dec);
}
BOOL VTSDeviceStatusDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_enumcombo.LoadCombo();
	m_enumcombo.ObjToCtrl();
	
	return TRUE;  
}