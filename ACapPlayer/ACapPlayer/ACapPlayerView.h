
// ACapPlayerView.h : interface of the CACapPlayerView class
//

#pragma once

class CACapManager;

class CACapPlayerView : public CFormView
{
protected: // create from serialization only
	CACapPlayerView() noexcept;
	DECLARE_DYNCREATE(CACapPlayerView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_ACAPPLAYER_FORM };
#endif

// Attributes
public:
	CACapPlayerDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CACapPlayerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CACapManager* m_pcManager;
	std::vector<std::string> nicList;
	pcap_if_t* m_pDeviceList;
	int m_nSelectedIndex;
// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

public:
	CComboBox m_AdapterList;
	CListBox m_FileList;
	CSliderCtrl m_Speed_Slider;
	CSpinButtonCtrl m_Loop_Spin;
	CSpinButtonCtrl m_DelayLoop_Spin;
	CStatic m_Current_Text;
	CButton m_Loop_CheckBox;
	CStatic m_SentText;
	CString m_strTotalCounut;

	afx_msg void OnBnClickedButtonMainAddfile();
	afx_msg void OnBnClickedButtonMainClear();
	afx_msg void OnBnClickedButtonMainFilefolder();
	afx_msg void OnBnClickedButtonMainStop();
	afx_msg void OnBnClickedButtonMainPlay();
	afx_msg void OnBnClickedCheckMainLoop();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonMainClose();
	afx_msg LRESULT OnSendFileName(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT TotalPacket(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#ifndef _DEBUG  // debug version in ACapPlayerView.cpp
inline CACapPlayerDoc* CACapPlayerView::GetDocument() const
   { return reinterpret_cast<CACapPlayerDoc*>(m_pDocument); }
#endif

