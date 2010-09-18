/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

// 2010/09/12
// At some point, make order manager responsible for constructing COrder

#include <map>

#include <LibCommon/Delegate.h>

#include <LibBerkeleyDb/AutoIncKeys.h>

#include "TradingEnumerations.h"
#include "ManagerBase.h"
#include "Order.h"
#include "Execution.h"

//
// COrderManager
//

class CProviderInterfaceBase;

// this is a singleton so use the Instance() call from all users
class COrderManager: public ManagerBase<COrderManager, COrder::orderid_t, COrder> {
public:

  typedef COrder::pOrder_t pOrder_t;
  typedef COrder::orderid_t orderid_t;

  COrderManager(void);
  ~COrderManager(void);
  void PlaceOrder( CProviderInterfaceBase* pProvider, COrder::pOrder_t pOrder );
  void CancelOrder( COrder::orderid_t nOrderId );
  void ReportExecution( COrder::orderid_t orderId, const CExecution& exec );  // feedback from provider
  void ReportCommission( COrder::orderid_t nOrderId, double dblCommission );
  void ReportErrors( COrder::orderid_t nOrderId, OrderErrors::enumOrderErrors eError );
  Delegate<const COrder &> OnOrderCompleted;

  orderid_t CheckOrderId( orderid_t );  // used by ibtws to sync order ids

protected:

  typedef std::pair<CProviderInterfaceBase*,pOrder_t> pairProviderOrder_t;
  typedef std::pair<orderid_t, pairProviderOrder_t> pairIdOrder_t;
  typedef std::map<orderid_t, pairProviderOrder_t> mapOrders_t;

private:

  CAutoIncKey m_orderIds;  // may need to worry about multi-threading at some point in time

  mapOrders_t m_mapActiveOrders;  // two lists in order to minimize lookup times on active orders
  mapOrders_t m_mapCompletedOrders;
  mapOrders_t m_mapAllOrders; // all orders for when checking for consistency
  mapOrders_t::iterator LocateOrder( orderid_t nOrderId );
  void MoveActiveOrderToCompleted( orderid_t nOrderId );

};
