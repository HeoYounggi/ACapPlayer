
// ACapPlayer.h : main header file for the ACapPlayer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CACapPlayerApp:
// See ACapPlayer.cpp for the implementation of this class
//

class CACapPlayerApp : public CWinApp
{
public:
	CACapPlayerApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CACapPlayerApp theApp;
