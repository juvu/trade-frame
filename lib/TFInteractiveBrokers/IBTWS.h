/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <functional>

#ifndef IB_USE_STD_STRING
#define IB_USE_STD_STRING
#endif

#include <boost/shared_ptr.hpp>
#include "boost/date_time/posix_time/posix_time.hpp"
using namespace boost::posix_time;
using namespace boost::gregorian;
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/mutex.hpp>

#include <OUCommon/FastDelegate.h>
#include <OUCommon/Delegate.h>
//#include <OUCommon/MSWindows.h>   // commented out 2015/02/22 not needed in linux, required in msw?

#include <TFTrading/TradingEnumerations.h>
#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderInterface.h>
#include <TFTrading/Order.h>

#include "IBSymbol.h"  // has settings for IBString, which affects the following TWS includes.

#ifndef TWSAPIDLLEXP
#define TWSAPIDLLEXP
#endif

#ifdef _WIN32
#include "win/EPosixClientSocket.h"
#else
#include "linux/EPosixClientSocket.h"
#endif
#include "Shared/EWrapper.h"

#include "Shared/Contract.h"
#include "Shared/Order.h"
#include "Shared/OrderState.h"
#include "Shared/Execution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class IBTWS :
  public ProviderInterface<IBTWS, IBSymbol>,
  public EWrapper
{
public:

  typedef boost::shared_ptr<IBTWS> pProvider_t;
  typedef ProviderInterface<IBTWS, IBSymbol> ProviderInterface_t;
  typedef IBSymbol::pSymbol_t pSymbol_t;
  typedef Instrument::pInstrument_t pInstrument_t;
  typedef Order::pOrder_t pOrder_t;
  typedef int reqId_t;  // request id type
  typedef ::Contract Contract;
  typedef ::ContractDetails ContractDetails;

  IBTWS( const std::string &acctCode = "", const std::string &address = "127.0.0.1", unsigned int port = 7496 );
  virtual ~IBTWS(void);

  // From ProviderInterface:
  void Connect( void );
  void Disconnect( void );

  void SetClientId( int idClient ) { m_idClient = idClient; }

  // From ProviderInterface Execution Section
  void PlaceOrder( pOrder_t order );
  void PlaceOrder( pOrder_t order, long idParent, bool bTransmit );
  void PlaceBracketOrder( pOrder_t pOrderEntry, pOrder_t pOrderProfit, pOrder_t pOrderStop );
  void CancelOrder( pOrder_t order );

  static void ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth );
  static void ContractExpiryField( Contract& contract, boost::uint16_t nYear, boost::uint16_t nMonth, boost::uint16_t nDay );

  // old, deprecated set of event based handling, these are forwarded to the new set of handlers

  typedef FastDelegate2<const ContractDetails&, pInstrument_t&> OnContractDetailsHandler_t;
  typedef FastDelegate0<void> OnContractDetailsDoneHandler_t;

  void RequestContractDetails( const std::string& sSymbolBaseName, pInstrument_t,
                                                         OnContractDetailsHandler_t fProcess, OnContractDetailsDoneHandler_t fDone );
  void RequestContractDetails( const Contract& contract, OnContractDetailsHandler_t fProcess, OnContractDetailsDoneHandler_t fDone );
  void RequestContractDetails( const Contract& contract, OnContractDetailsHandler_t fProcess, OnContractDetailsDoneHandler_t fDone, pInstrument_t );

  // new set of event based handling

  typedef std::function<void(const ContractDetails&, pInstrument_t&)> fOnContractDetail_t;
  typedef std::function<void(void)> fOnContractDetailDone_t;

  void RequestContractDetails( const std::string& sSymbolBaseName, pInstrument_t,
                                                         fOnContractDetail_t fProcess, fOnContractDetailDone_t fDone );
  void RequestContractDetails( const Contract& contract, fOnContractDetail_t fProcess, fOnContractDetailDone_t fDone );
  void RequestContractDetails( const Contract& contract, fOnContractDetail_t fProcess, fOnContractDetailDone_t fDone, pInstrument_t );

  struct PositionDetail {
    std::string sSymbol;
    std::string sLocalSymbol;
    long contract;
    double strike;
    std::string sExpiry;
    std::string sMultiplier;
    int position;
    double marketPrice;
    double marketValue;
    double averageCost;
    double unrealizedPNL;
    double realizedPNL;
    std::string sExchange;
    std::string sCurrency;
    PositionDetail(
      const std::string& sSymbol_,
      const std::string& sLocalSymbol_,
      long contract_,
      double strike_,
      const std::string& sExpiry_,
      const std::string& sMultiplier_,
      int position_,
      double marketPrice_,
      double marketValue_,
      double averageCost_,
      double unrealizedPNL_,
      double realizedPNL_,
      const std::string& sExchange_,
      const std::string& sCurrency_
      ): sSymbol( sSymbol_ ), sLocalSymbol( sLocalSymbol_ ), contract( contract_ ), strike( strike_ ),
      sExpiry( sExpiry_ ), sMultiplier( sMultiplier_ ),
      position( position_ ), marketPrice( marketPrice_ ), marketValue( marketValue_ ),
      averageCost( averageCost_ ), unrealizedPNL( unrealizedPNL_ ), realizedPNL( realizedPNL_ ),
      sExchange( sExchange_ ), sCurrency( sCurrency_ ) {}
  };
  typedef FastDelegate1<const PositionDetail&> OnPositionDetailHandler_t;
  OnPositionDetailHandler_t OnPositionDetailHandler;

  struct AccountValue {
    std::string sKey;
    std::string sVal;
    std::string sCurrency;
    std::string sAccountName;
    AccountValue(
      const std::string& sKey_, const std::string& sVal_, const std::string& sCurrency_, const std::string& sAccountName_
      ): sKey( sKey_ ), sVal( sVal_ ), sCurrency( sCurrency_ ), sAccountName( sAccountName_ ) {}
  };
  typedef FastDelegate1<const AccountValue&> OnAccountValueHandler_t;
  OnAccountValueHandler_t OnAccountValueHandler;

  pSymbol_t GetSymbol( long ContractId );  // query existance
  pSymbol_t GetSymbol( pInstrument_t instrument );  // query for and add if doesn't exist

  void BuildInstrumentFromContract( const Contract& contract, pInstrument_t& pInstrument );
  //pInstrument_t BuildInstrumentFromContract( const Contract& contract );
  //pInstrument_t BuildInstrumentFromContract( pInstrument_t pInstrument, const Contract& contract ); // unregistered pre-existing instrument

  // TWS Specific events
  // From TWS Wrapper:
  void connectionClosed();
  void tickPrice( TickerId tickerId, TickType field, double price, int canAutoExecute);
  void tickSize( TickerId tickerId, TickType field, int size);
  void tickOptionComputation( TickerId tickerId, TickType tickType, double impliedVol, double delta,
	   double optPrice, double pvDividend, double gamma, double vega, double theta, double undPrice);
  void tickGeneric(TickerId tickerId, TickType tickType, double value);
  void tickString(TickerId tickerId, TickType tickType, const std::string& value);
  void tickEFP(TickerId tickerId, TickType tickType, double basisPoints, const std::string& formattedBasisPoints,
	   double totalDividends, int holdDays, const std::string& futureExpiry, double dividendImpact, double dividendsToExpiry);
  void orderStatus( OrderId orderId, const std::string &status, int filled,
	   int remaining, double avgFillPrice, int permId, int parentId,
	   double lastFillPrice, int clientId, const std::string& whyHeld);
  void openOrder( OrderId orderId, const Contract&, const ::Order&, const OrderState&);
  void openOrderEnd() {};  // **
  void execDetails( int reqId, const Contract& contract, const ::Execution& execution );
  void execDetailsEnd( int reqId) {};  // **
  void error(const int id, const int errorCode, const std::string errorString);
  void winError( const std::string& str, int lastError);
  void updateNewsBulletin(int msgId, int msgType, const std::string& newsMessage, const std::string& originExch);
  void currentTime(long time);
  void contractDetails( int reqId, const ContractDetails& contractDetails );
  void contractDetailsEnd( int reqId );
  void bondContractDetails( int reqId, const ContractDetails& contractDetails );
  void nextValidId( OrderId orderId);
  void updatePortfolio( const Contract& contract, int position,
      double marketPrice, double marketValue, double averageCost,
      double unrealizedPNL, double realizedPNL, const std::string& accountName);
  void updateAccountValue(const std::string& key, const std::string& val,
   const std::string& currency, const std::string& accountName);
  void updateAccountTime(const std::string& timeStamp);
  void accountDownloadEnd(const std::string& accountName) {};  // **
  void updateMktDepth(TickerId id, int position, int operation, int side,
      double price, int size);
  void updateMktDepthL2(TickerId id, int position, std::string marketMaker, int operation,
      int side, double price, int size);
  void managedAccounts( const std::string& accountsList);
     virtual void receiveFA(faDataType pFaDataType, const std::string& cxml);
  void historicalData(TickerId reqId, const std::string& date, double open, double high,
	   double low, double close, int volume, int barCount, double WAP, int hasGaps);
  void scannerParameters(const std::string& xml);
  void scannerData(int reqId, int rank, const ContractDetails &contractDetails,
	   const std::string& distance, const std::string& benchmark, const std::string& projection,
	   const std::string& legsStr);
  void scannerDataEnd(int reqId);
  void realtimeBar(TickerId reqId, long time, double open, double high, double low, double close,
	   long volume, double wap, int count);
  void fundamentalData(TickerId reqId, const std::string& data) {};  // **
  void deltaNeutralValidation(int reqId, const UnderComp& underComp) {};  // **
  void tickSnapshotEnd( int reqId) {};  // **

  void marketDataType(TickerId,int);
  void commissionReport(const CommissionReport &);
  void position(const std::string &,const Contract &,int,double);
  void positionEnd(void);
  void accountSummary(int,const std::string &,const std::string &,const std::string &,const std::string &);
  void accountSummaryEnd(int);
  void verifyMessageAPI(const std::string &);
  void verifyCompleted(bool,const std::string &);
  void displayGroupList(int,const std::string &) {};
  void displayGroupUpdated(int,const std::string &) {};
  void verifyAndAuthMessageAPI(const std::string&, const std::string&) {};
  void verifyAndAuthCompleted(bool, const std::string&) {};

protected:

  static const char* TickTypeStrings[];

  std::string m_sAccountCode;
  std::string m_sIPAddress;
  unsigned int m_nPort;
  TickerId m_curTickerId;

  double m_dblBuyingPower;
  double m_dblAvailableFunds;

  static const char *szSecurityType[];
  static const char *szOrderType[];

  pSymbol_t NewCSymbol( pInstrument_t pInstrument );

  // overridden from ProviderInterface
  void StartQuoteWatch( pSymbol_t pSymbol );
  void  StopQuoteWatch( pSymbol_t pSymbol );

  void StartTradeWatch( pSymbol_t pSymbol );
  void  StopTradeWatch( pSymbol_t pSymbol );

  void StartQuoteTradeWatch( pSymbol_t pSymbol );
  void  StopQuoteTradeWatch( pSymbol_t pSymbol );

  void StartDepthWatch( pSymbol_t pSymbol );
  void  StopDepthWatch( pSymbol_t pSymbol );

  void StartGreekWatch( pSymbol_t pSymbol );
  void  StopGreekWatch( pSymbol_t pSymbol );

private:
  EPosixClientSocket *pTWS;
  long m_time;
  int m_idClient; // for session uniqueness when multiple applications are connected to TWS

  std::stringstream m_ss;  // for OutputDebugStrings in background thread

  OnContractDetailsHandler_t OnContractDetails;
  OnContractDetailsDoneHandler_t OnContractDetailsDone;

  // stuff comes back from IB with ticker id so use this to look up symbol,
  //    which is stored in the map of the class from which we inherited
  std::vector<pSymbol_t> m_vTickerToSymbol;

  // given a contract id, see if we have a symbol assigned
  typedef std::map<long, pSymbol_t> mapContractToSymbol_t;
  typedef std::pair<long, pSymbol_t> pair_mapContractToSymbol_t;
  mapContractToSymbol_t m_mapContractToSymbol;

  // do we actually need this anymore given that we have the above vector?
  // given a ticker id, see if we have a symbol assigned
//  typedef std::map<TickerId, pSymbol_t> mapTickerIdToSymbol_t;
//  typedef std::pair<TickerId, pSymbol_t> pair_mapContractToSymbol_t;
//  pair_mapContractToSymbol_t m_mapContractToSymbol;

  boost::thread m_thrdIBMessages;

  void ProcessMessages( void );

  void DecodeMarketHours( const std::string&, ptime& dtOpen, ptime& dtClose );

  struct structRequest_t {
    reqId_t id;
    pInstrument_t pInstrument;  // add info to existing pInstrument, future use with BuildInstrumentFromContract
    //OnContractDetailsHandler_t fProcess;
    //OnContractDetailsDoneHandler_t fDone;
    fOnContractDetail_t fOnContractDetail;
    fOnContractDetailDone_t fOnContractDetailDone;
    //structRequest_t( reqId_t id_, OnContractDetailsHandler_t fProcess_, OnContractDetailsDoneHandler_t fDone_ )
    //  : id( id_ ), fProcess( fProcess_ ), fDone( fDone_ ) {};
    //structRequest_t( reqId_t id_, OnContractDetailsHandler_t fProcess_, OnContractDetailsDoneHandler_t fDone_, pInstrument_t pInstrument_ )
    //  : id( id_ ), fProcess( fProcess_ ), fDone( fDone_ ), pInstrument( pInstrument_ ) {};
    structRequest_t( reqId_t id_, fOnContractDetail_t fProcess_, fOnContractDetailDone_t fDone_, pInstrument_t pInstrument_ )
      : id( id_ ), fOnContractDetail( fProcess_ ), fOnContractDetailDone( fDone_ ), pInstrument( pInstrument_ ) {};
  };

  reqId_t m_nxtReqId;
  std::vector<structRequest_t*> m_vInActiveRequestId;  // can this be re-written with lockless structure?
//  typedef std::pair<reqId_t, structRequest_t*> mapActiveRequestId_pair_t;
  typedef std::map<reqId_t, structRequest_t*> mapActiveRequestId_t;
  mapActiveRequestId_t m_mapActiveRequestId;
  boost::mutex m_mutexContractRequest;

  void DisconnectCommon( bool bSignalEnd );

};

} // namespace tf
} // namespace ou
