#ifndef COMMON_H_
#define COMMON_H_

#include "ThostTraderApi\ThostFtdcMdApi.h"
#include "ThostTraderApi\ThostFtdcTraderApi.h"
#include <map> 
#include <queue> 
#include <mutex>
#include <fstream>
#include "stdafx.h"
#include  <map> 
#include  <queue> 
#include <mutex>

//*************全局变量*************************************************************

using namespace std;

extern CThostFtdcMdApi* pMDUserApi;

extern CThostFtdcTraderApi* pTraderUserApi;

extern int iRequestID; // 请求编号

extern map<string, CThostFtdcDepthMarketDataField> MarketDataField;//合约行情

extern map<string, string> InstrumentStatus;//合约状态

extern map<string, double> payoff; //合约收益

extern map<string, double> Bid;//买报价

extern map<string, double> Ask;//卖报价

extern map<string, int> LongPosition; //买持仓

extern map<string, int> ShortPosition;//卖持仓

extern map<string, int> LongEnClose; //买持可平仓量

extern map<string, int> ShortEnClose;//卖持可平仓量extern extern 

extern map<string, string> BidORDER_REF;//买报价引用

extern map<string, string> AskORDER_REF;//卖报价引用

extern vector<string> Instrumentlist; //订阅合约list

extern vector<double> StrikePrices;  //行权价list

extern std::mutex   g_lockqueue;

extern int spreed ;//= 2;

extern int iNextOrderRef;  ///报单应用编号

extern TThostFtdcOrderRefType	ORDER_REF;	//报单引用
extern TThostFtdcOrderRefType	EXECORDER_REF;	//执行宣告引用
extern TThostFtdcOrderRefType	FORQUOTE_REF;	//询价引用
extern TThostFtdcOrderRefType	QUOTE_REF;	//报价引用

extern TThostFtdcFrontIDType	FRONT_ID;	//前置编号
extern TThostFtdcSessionIDType	SESSION_ID;	//会话编号

// MD配置参数
extern char MD_FRONT_ADDR[];// = "tcp://172.16.100.225:41213";
//"tcp://210.5.151.247:41213"; // 		// 前置地址


// Trader配置参数
extern char  TRADER_FRONT_ADDR[];		     // 前置地址
extern TThostFtdcBrokerIDType	BROKER_ID;				// 经纪公司代码extern 
extern TThostFtdcInvestorIDType INVESTOR_ID;			// 投资者代码
extern TThostFtdcPasswordType  PASSWORD;			// 用户密码
//***************************************************************

 enum MsgType
	{
		RtnOrder = 0,		// 委托回报

		RtnTrade = 1,       // 成交回报
		
		InputOrder=2,       //报单录入

		InputOrderAction = 3,   // 报单操作录入

		//CThostFtdcInputOrderField
	};

 struct Msg
 {
	 MsgType Msg_Type;

	 CThostFtdcOrderField  RtnOrder; //委托回报

	 CThostFtdcTradeField  RtnTrade; //成交回报

	 CThostFtdcInputOrderField InputOrder;//报单录入

	 CThostFtdcInputOrderActionField InputOrderAction;//报单操作录入

	 CThostFtdcRspInfoField RspInfo;

 };

 extern queue<Msg> MsgQueue;   ///消息队列

#endif