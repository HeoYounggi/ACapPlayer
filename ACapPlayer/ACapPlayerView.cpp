
// ACapPlayerView.cpp : implementation of the CACapPlayerView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "ACapPlayer.h"
#endif

#include "ACapPlayerDoc.h"
#include "ACapPlayerView.h"

#include "ACapManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//==============================================================================
// CACapPlayerView

IMPLEMENT_DYNCREATE(CACapPlayerView, CFormView)

BEGIN_MESSAGE_MAP(CACapPlayerView, CFormView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CFormView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CFormView::OnFilePrintPreview)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_ADDFILE, &CACapPlayerView::OnBnClickedButtonMainAddfile)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_CLEAR, &CACapPlayerView::OnBnClickedButtonMainClear)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_FILEFOLDER, &CACapPlayerView::OnBnClickedButtonMainFilefolder)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_STOP, &CACapPlayerView::OnBnClickedButtonMainStop)
	ON_BN_CLICKED(IDC_BUTTON_MAIN_PLAY, &CACapPlayerView::OnBnClickedButtonMainPlay)
	ON_BN_CLICKED(IDC_CHECK_MAIN_LOOP, &CACapPlayerView::OnBnClickedCheckMainLoop)
	ON_MESSAGE(WM_SEND_FILE_NAME_MANAGER,&CACapPlayerView::OnSendFileName)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_MAIN_CLOSE, &CACapPlayerView::OnBnClickedButtonMainClose)
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CACapPlayerView construction/destruction

CACapPlayerView::CACapPlayerView() noexcept
	: CFormView(IDD_ACAPPLAYER_FORM)
{
	// TODO: add construction code here
	m_pcManager = NULL;
	m_pDeviceList = NULL;
	m_nSelectedIndex = 0;
}

CACapPlayerView::~CACapPlayerView()
{
}

void CACapPlayerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_MAIN_ADAPTER, m_AdapterList);
	DDX_Control(pDX, IDC_LIST_MAIN_FILELIST, m_FileList);
	DDX_Control(pDX, IDC_SLIDER_MAIN_SPEED, m_Speed_Slider);
	DDX_Control(pDX, IDC_SPIN_MAIN_LOOP, m_Loop_Spin);
	DDX_Control(pDX, IDC_SPIN_MAIN_DELAYLOOP, m_DelayLoop_Spin);
	DDX_Control(pDX, IDC_CHECK_MAIN_LOOP, m_Loop_CheckBox);
	DDX_Control(pDX, IDC_STATIC_MAIN_CURRENT_TEXT, m_Current_Text);
	DDX_Control(pDX, IDC_STATIC_MAIN_SENT_TEXT, m_SentText);
}

BOOL CACapPlayerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CFormView::PreCreateWindow(cs);
}

void CACapPlayerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	HICON hIcon = AfxGetApp()->LoadIcon(IDI_ICON1);
	//작업표시줄 등에서 사용되는 큰 아이콘
	GetParentFrame()->SetIcon(hIcon, TRUE);
	//왼쪽 상단에 사용되는 작은 아이콘
	GetParentFrame()->SetIcon(hIcon, FALSE);

	m_Speed_Slider.SetRange(0, 6);
	m_Speed_Slider.SetPos(3);
	m_Speed_Slider.SetLineSize(1);
	m_Speed_Slider.SetPageSize(1);
	m_Speed_Slider.SetTicFreq(1);

	m_Loop_Spin.SetRange(0, 10000);
	m_Loop_Spin.SetPos(1);

	m_DelayLoop_Spin.SetRange(0, 32767);
	m_DelayLoop_Spin.SetPos(1000);

	m_pcManager = new CACapManager(this->GetSafeHwnd());

	if (m_pcManager->GetNICList(nicList))
	{
		for (const auto& nic : nicList)
		{
			m_AdapterList.AddString(CString(nic.c_str()));
		}
	}

	else
	{
		AfxMessageBox(TEXT("네트워크 디바이스 목록을 가져오지 못했습니다. 다시 확인해 주세요."));
	}

	m_AdapterList.SetCurSel(0);
}

void CACapPlayerView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	delete m_pcManager;
	m_pcManager = NULL;

}

// CACapPlayerView printing

BOOL CACapPlayerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CACapPlayerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CACapPlayerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CACapPlayerView::OnPrint(CDC* pDC, CPrintInfo* /*pInfo*/)
{
	// TODO: add customized printing code here
}


// CACapPlayerView diagnostics

#ifdef _DEBUG
void CACapPlayerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CACapPlayerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CACapPlayerDoc* CACapPlayerView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CACapPlayerDoc)));
	return (CACapPlayerDoc*)m_pDocument;
}
#endif //_DEBUG


// CACapPlayerView message handlers
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainPlay()
{
	// TODO: Add your control notification handler code here
	UINT ControlsToAble[] =
	{
		IDC_BUTTON_MAIN_STOP,
		IDC_BUTTON_MAIN_PAUSE
	};

	for (UINT id : ControlsToAble)
	{
		GetDlgItem(id)->EnableWindow(TRUE);
	}

	UINT ControlsToDisable[] =
	{
		IDC_BUTTON_MAIN_PLAY,
		IDC_COMBO_MAIN_ADAPTER,
		IDC_BUTTON_MAIN_ADDFILE,
		IDC_BUTTON_MAIN_FILEFOLDER,
		IDC_BUTTON_MAIN_CLEAR,
		IDC_SLIDER_MAIN_SPEED,
		IDC_CHECK_MAIN_LOOP,
		IDC_CHECK_MAIN_IGNORE,
		IDC_LIST_MAIN_FILELIST,
		IDC_SPIN_MAIN_LOOP,
		IDC_SPIN_MAIN_DELAYLOOP,
		IDC_STATIC_MAIN_DELAYLOOP,
		IDC_STATIC_MAIN_LOOPS,
		IDC_STATIC_MAIN_MILLISECONDS,
		IDC_BUTTON_MAIN_ADAPTERINFO,
		IDC_STATIC_MAIN_PLAYSPEED,
		IDC_STATIC_MAIN_18x,
		IDC_STATIC_MAIN_14x,
		IDC_STATIC_MAIN_12x,
		IDC_STATIC_MAIN_1x,
		IDC_STATIC_MAIN_2x,
		IDC_STATIC_MAIN_4x,
		IDC_STATIC_MAIN_BURST,
		IDC_STATIC_MAIN_ADAPTER,
		IDC_STATIC_MAIN_PACKETFILE
	};

	for (UINT id : ControlsToDisable)
	{
		GetDlgItem(id)->EnableWindow(FALSE);
	}

	m_nSelectedIndex = m_AdapterList.GetCurSel();

	m_pcManager->SetNIC(m_nSelectedIndex);

	CString m_FilePath;
	m_FileList.GetText(0, m_FilePath);
	m_pcManager->SetFilePath(m_FilePath.GetBuffer());

	int nPos = m_Speed_Slider.GetPos();
	m_pcManager->GetPlaySpeed(nPos);

	SetTimer(1, 200, NULL);

	m_pcManager->Start();
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainStop()
{
	// TODO: Add your control notification handler code here
	UINT ControlsToAble[] =
	{
		IDC_BUTTON_MAIN_PLAY,
		IDC_COMBO_MAIN_ADAPTER,
		IDC_BUTTON_MAIN_ADDFILE,
		IDC_BUTTON_MAIN_FILEFOLDER,
		IDC_BUTTON_MAIN_CLEAR,
		IDC_SLIDER_MAIN_SPEED,
		IDC_CHECK_MAIN_LOOP,
		IDC_CHECK_MAIN_IGNORE,
		IDC_LIST_MAIN_FILELIST,
		IDC_SPIN_MAIN_LOOP,
		IDC_SPIN_MAIN_DELAYLOOP,
		IDC_STATIC_MAIN_DELAYLOOP,
		IDC_STATIC_MAIN_LOOPS,
		IDC_STATIC_MAIN_MILLISECONDS,
		IDC_BUTTON_MAIN_ADAPTERINFO,
		IDC_STATIC_MAIN_PLAYSPEED,
		IDC_STATIC_MAIN_18x,
		IDC_STATIC_MAIN_14x,
		IDC_STATIC_MAIN_12x,
		IDC_STATIC_MAIN_1x,
		IDC_STATIC_MAIN_2x,
		IDC_STATIC_MAIN_4x,
		IDC_STATIC_MAIN_BURST,
		IDC_STATIC_MAIN_ADAPTER,
		IDC_STATIC_MAIN_PACKETFILE
		
	};

	for (UINT id : ControlsToAble)
	{
		GetDlgItem(id)->EnableWindow(TRUE);
	}

	UINT ControlsToDisable[] =
	{
		IDC_BUTTON_MAIN_STOP,
		IDC_BUTTON_MAIN_PAUSE
	};

	for (UINT id : ControlsToDisable)
	{
		GetDlgItem(id)->EnableWindow(FALSE);
	}

	KillTimer(1);

	m_pcManager->Stop();
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainClose()
{
	// TODO: Add your control notification handler code here
	AfxGetMainWnd()->PostMessageW(WM_CLOSE);
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainClear()
{
	m_FileList.ResetContent();
	ASSERT(m_FileList.GetCount() == 0);
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainAddfile()
{
	// TODO: Add your control notification handler code here
	CString sFileType = TEXT("pcap fules(*.pcap)|*.pcap|");
	CFileDialog fdDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ALLOWMULTISELECT, sFileType, this);

	if (fdDlg.DoModal() == IDOK)
	{
		POSITION poPos = fdDlg.GetStartPosition();
		while (poPos)
		{
			CString sPathName = fdDlg.GetNextPathName(poPos);
			int Index = m_FileList.AddString(sPathName);
			m_FileList.SetCurSel(Index);
		}
	}
}
//==============================================================================

void CACapPlayerView::OnBnClickedButtonMainFilefolder()
{
	// TODO: Add your control notification handler code here
	BROWSEINFO m_BrInfo; 
	TCHAR m_szFolderBuffer[512];

	::ZeroMemory(&m_BrInfo, sizeof(BROWSEINFO));
	::ZeroMemory(m_szFolderBuffer, 512);

	m_BrInfo.hwndOwner = GetSafeHwnd();
	m_BrInfo.lpszTitle = TEXT("Choose File Folder");
	m_BrInfo.ulFlags = BIF_NEWDIALOGSTYLE | BIF_EDITBOX | BIF_RETURNFSANCESTORS;
	LPITEMIDLIST m_pFolderIDList = ::SHBrowseForFolder(&m_BrInfo);
	::SHGetPathFromIDList(m_pFolderIDList, m_szFolderBuffer);

	CString m_strFolderPath(m_szFolderBuffer);
	CString m_strSearchPath = m_strFolderPath + TEXT("\\*.pcap");
	WIN32_FIND_DATA m_FindFileData;
	HANDLE m_hFind = FindFirstFile(m_strSearchPath, &m_FindFileData);

	if (m_hFind != INVALID_HANDLE_VALUE)
	{
		do
		{
			CString m_strFileName = m_FindFileData.cFileName;
			CString m_strFilePath = m_strFolderPath + TEXT("\\") + m_strFileName;
			int m_FileIndex = m_FileList.AddString(m_strFilePath);
			m_FileList.SetCurSel(m_FileIndex);
		} while (FindNextFile(m_hFind, &m_FindFileData) != 0);
		FindClose(m_hFind);
	}
}
//==============================================================================

void CACapPlayerView::OnBnClickedCheckMainLoop()
{
	// TODO: Add your control notification handler code here
	if (m_Loop_CheckBox.GetCheck() == BST_CHECKED)
	{
		GetDlgItem(IDC_EDIT_MAIN_LOOP)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MAIN_DELAY)->EnableWindow(TRUE);
	}

	else if (m_Loop_CheckBox.GetCheck() == BST_UNCHECKED)
	{
		GetDlgItem(IDC_EDIT_MAIN_LOOP)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MAIN_DELAY)->EnableWindow(FALSE);
	}
}
//==============================================================================

afx_msg LRESULT CACapPlayerView::OnSendFileName(WPARAM wParam,LPARAM lParam)
{
	LPCTSTR pszFileName = (LPCTSTR)lParam;
	CString strSendingFileName = pszFileName;

	m_Current_Text.SetWindowTextW(strSendingFileName);

	return 0;
}
//==============================================================================

void CACapPlayerView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 1:
		unsigned int m_unSendCount= m_pcManager->GetPacketSendCount();
		CString m_strValue;
		m_strValue.Format(TEXT("%d"), m_unSendCount);
		m_SentText.SetWindowTextW(m_strValue);
		//m_SentText.SetWindowTextW(TEXT("HI"));
	}
	CFormView::OnTimer(nIDEvent);
}
//==============================================================================