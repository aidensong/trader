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

enum MsgType
{
	RtnOrder = 0,		// 委托回报

	RtnTrade = 1,       // 成交回报

	InputOrder = 2,       //报单录入

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


//*************全局变量*************************************************************

using namespace std;


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

extern queue<Msg> MsgQueue;   ///消息队列

extern vector<CThostFtdcOrderField> OrderList;//委托列表

extern vector<CThostFtdcInputOrderField> InputOrderList;//委托录入

extern vector<CThostFtdcInputOrderActionField> InputOrderActionList;//委托操作列表

extern vector<CThostFtdcInvestorPositionField> InvestorPositionList;//持仓列表

extern int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction);
#endif