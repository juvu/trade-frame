/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#include "StdAfx.h"

#include "ThreadMain.h"

ThreadMain::ThreadMain(enumMode eMode)
  : m_eMode(eMode) {

  std::string sDbName;

  switch ( m_eMode ) {
    case EModeSimulation:
      sDbName = ":memory:";
//      m_pExecutionProvider = m_sim;
//      m_pDataProvider = m_sim;
      break;
    case EModeLive:
      sDbName = "Hedge.db";
//      m_pExecutionProvider = m_tws;
//      m_pDataProvider = m_tws;
      break;
  }

  m_db.Open( sDbName );
  m_pProcess = new Process( eMode, m_db );

}

ThreadMain::~ThreadMain(void) {
  delete m_pProcess;
  m_db.Close();
}
