// GTScalp.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols

#include "GTWindowsConstants.h"  // based upon WM_USER

#include "..\..\Genesis\v8\GenSrc\API\INC\GTAPI.h"

#include "ConsoleCoutMessages.h"
#include "TradingLogic.h"
#include "IQFeed.h" 

#include "GeneratePeriodicRefresh.h"  // need the above constants

// CGTScalpApp:
// See GTScalp.cpp for the implementation of this class
//

class CGTScalpApp : public CWinApp {
public:
	CGTScalpApp();

// Overrides
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

  CGTSessionX m_session1;
  CGTSessionX m_session2;
  CConsoleCoutMessages* pConsoleMessages;
  //CTradingLogic* pTradingLogic;

  CIQFeed *m_pIQFeed;

  CGeneratePeriodicRefresh *m_pRefresh;

  bool m_bLive;
  unsigned int m_nDaysAgo;
  bool m_bAllowTrades;


	DECLARE_MESSAGE_MAP()
};

extern CGTScalpApp theApp;