// VTSDoc.h : interface of the VTSDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_VTSDOC_H__BDE65088_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)
#define AFX_VTSDOC_H__BDE65088_B82F_11D3_BE52_00A0C95A9812__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>
#include <afxtempl.h>

#include "BACnet.hpp"
#include "WinIP.hpp"
#include "WinPacket32.hpp"

#include "JConfig.hpp"
#include "JDB.hpp"

#include "VTSDB.h"

// forward declarations

class VTSDoc;
typedef VTSDoc *VTSDocPtr;

class CFrameContext;
typedef CFrameContext *CFrameContextPtr;

class VTSPort;
typedef VTSPort *VTSPortPtr;

class VTSWinPacket32Port;
typedef VTSWinPacket32Port *VTSWinPacket32PortPtr;

class VTSWinIPPort;
typedef VTSWinIPPort *VTSWinIPPortPtr;

class VTSPortDlg;
typedef VTSPortDlg *VTSPortDlgPtr;

struct CSendGroup;
typedef CSendGroup *CSendGroupPtr;
typedef CSendGroupPtr *CSendGroupList;

class ScriptNetFilter;
typedef ScriptNetFilter *ScriptNetFilterPtr;

//
//	VTSPort
//
//	The VTSPort object sits between the VTSDoc and a derived class of BACnetPort (one 
//	of the VTSxPort objects).  The port that it connects to depends on the contents of 
//	the descriptor.
//

class VTSPort {
	protected:
		void AddToMasterList( void );
		void RemoveFromMasterList( void );

	public:
		int					portStatus;					// non-zero iff error
		char				*portStatusDesc;			// status description

		objId				portDescID;					// ID of descriptor
		VTSPortDesc			portDesc;					// port configuration info

		VTSDocPtr			portDoc;					// doc for packets
		CSendGroupList		portSendGroup;				// send group to form packets
		BACnetPortPtr		portEndpoint;				// endpoint to get them
		ScriptNetFilterPtr	portFilter;					// way to process them in scripts

		VTSPort( VTSDocPtr dp, objId id );
		~VTSPort( void );

		void ReadDesc( void );							// read descriptor from database
		void WriteDesc( void );							// save changes to descriptor
		void Configure( CString *cp );					// request a configuration dialog

		void Refresh( void );							// reconnect to port

		void SendData( BACnetOctet *data, int len );	// pass data to endpoint
	};

typedef VTSPort *VTSPortPtr;

//
//	VTSPortList
//

class VTSPortList : public CList<VTSPortPtr,VTSPortPtr> {
		friend class VTSPort;

	protected:
		VTSDocPtr	m_pDoc;

	public:
		VTSPortList( void );
		~VTSPortList( void );

		void Load( VTSDocPtr docp );					// bind to database and load
		void Unload( void );							// toss the ports away

		void Add( void );								// add a new port
		void Remove( int i );							// remove a port
		VTSPortPtr FindPort( const char *name );		// find a port with a given name

		int Length( void );								// number of defined ports
		VTSPortPtr operator []( int i );				// index into port list
	};

typedef VTSPortList *VTSPortListPtr;

extern VTSPortList	gMasterPortList;					// global list of all ports

//
//	VTSNameList
//

class VTSNameList {
	protected:
		VTSDocPtr	m_pDoc;
		
		static VTSNameDesc searchName;
		static int TDSearch( const VTSNameDescPtr, const VTSNameDescPtr );

	public:
		VTSNameList( void );
		~VTSNameList( void );

		void Load( VTSDocPtr docp );					// bind to database

		void Add( void );								// add a new name
		void Remove( int i );							// remove a name

		int Length( void );								// number of defined names
		const char* AddrToName( const BACnetAddress &addr, objId portID );	// translate to a name

		void ReadName( int i, VTSNameDescPtr ndp );
		void WriteName( int i, VTSNameDescPtr ndp );

		void DefineTD( objId port, const BACnetOctet *addr, int len );
		const BACnetAddress *FindTD( objId port );		// what is the TD address for a port
	};

typedef VTSNameList *VTSNameListPtr;

namespace NetworkSniffer {

extern JDBListPtr	gNameListSearchList;
extern VTSNameDesc	gNameListSearchName;

void SetLookupContext( objId port, JDBListPtr list );
const char* LookupName( int net, const BACnetOctet *addr, int len );
int NameSearch( const VTSNameDescPtr, const VTSNameDescPtr );

}

//
//	VTSDoc
//

class VTSDoc : public CDocument
{
protected: // create from serialization only
	VTSDoc();
	DECLARE_DYNCREATE(VTSDoc)

// Attributes
public:
	enum Signal
		{ eInitialUpdate = 0
		, eNewFrameCount = 1
		};

	VTSDBPtr				m_pDB;
	int						m_PacketCount;		// packets in document

	VTSPortList				m_Ports;
	VTSNameList				m_Names;

	bool					m_postMessages;		// OK to post messages about new packets

// Operations
public:

	void BindFrameContext( CFrameContext *pfc );
	void UnbindFrameContext( CFrameContext *pfc );

	void SetPacketCount( int count );
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(VTSDoc)
	public:
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
	//}}AFX_VIRTUAL

// Implementation
public:
	VTSPortDlgPtr	m_pPortDlg;

	void DeletePackets( void );
	void DoPortsDialog( void );
	void PortStatusChange( void );
	void DoNamesDialog( void );
	void DoPreferencesDialog( void );

	void DoSendWindow( int iGroup, int iItem );

	void NewPacketCount(void);

	virtual ~VTSDoc();

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CCriticalSection		m_FrameContextsCS;
	CFrameContextPtr		m_FrameContexts;

// Generated message map functions
protected:
	//{{AFX_MSG(VTSDoc)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

typedef VTSDoc *VTSDocPtr;

//
//	VTSDocList
//

class VTSDocList : public CList<VTSDocPtr,VTSDocPtr> {
	public:
		VTSDocList( void );
		~VTSDocList( void );
	
		// list operations
		void Append( VTSDocPtr vdp );				// add a child at the end
		void Remove( VTSDocPtr vdp );				// remove a child

		int Length( void );							// number of children
		VTSDocPtr Child( int indx );				// child by index
	};

typedef VTSDocList *VTSDocListPtr;
const int kVTSDocListSize = sizeof( VTSDocList );

extern VTSDocList gDocList;							// list of all documents

//
//	VTSWinPacket32Port
//

class VTSWinPacket32Port : public WinPacket32 {
	protected:
		VTSPortPtr			m_pPort;

	public:
		VTSWinPacket32Port( VTSPortPtr pp );
		virtual ~VTSWinPacket32Port( void );

		void FilterData( BACnetOctet *, int len, BACnetPortDirection dir );

		void PortStatusChange( void );
	};

//
//	VTSWinIPPort
//

class VTSWinIPPort : public WinIP {
	protected:
		VTSPortPtr			m_pPort;

	public:
		VTSWinIPPort( VTSPortPtr pp );
		virtual ~VTSWinIPPort( void );

		void FilterData( BACnetOctet *, int len, BACnetPortDirection dir );

		void PortStatusChange( void );
	};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VTSDOC_H__BDE65088_B82F_11D3_BE52_00A0C95A9812__INCLUDED_)