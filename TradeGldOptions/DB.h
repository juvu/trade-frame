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
#pragma once

#include <OUSQL/Session.h>
#include <OUSqlite/ISqlite3.h>

class CDB
{
public:

  CDB(void);
  ~CDB(void);

  void Open( const std::string& sDbName );
  void Close( void );

protected:
private:

  ou::db::CSession<ou::db::ISqlite3> m_session;

  void Populate( void );

};

