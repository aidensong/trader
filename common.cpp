#include "common.h"

//*************全局变量*************************************************************

CThostFtdcMdApi* pMDUserApi;

CThostFtdcTraderApi* pTraderUserApi;

int iRequestID = 0; // 请求编号

int spreed = 2000000;

int iNextOrderRef = 0;  ///报单应用编号

map<string, CThostFtdcDepthMarketDataField> MarketDataField;//合约行情

map<string, string> InstrumentStatus;//合约状态

map<string, double> payoff; //合约收益

map<string, double> Bid;//买报价

map<string, double> Ask;//卖报价

map<string, int> LongPosition; //买持仓

map<string, int> ShortPosition;//卖持仓

map<string, int> LongEnClose; //买持可平仓量

map<string, int> ShortEnClose;//卖持可平仓量

map<string, string> BidORDER_REF;//买报价引用

map<string, string> AskORDER_REF;//卖报价引用

vector<string> Instrumentlist; //订阅合约list

vector<double> StrikePrices;  //行权价list

std::mutex   g_lockqueue;


TThostFtdcOrderRefType	ORDER_REF;	//报单引用
TThostFtdcOrderRefType	EXECORDER_REF;	//执行宣告引用
TThostFtdcOrderRefType	FORQUOTE_REF;	//询价引用
TThostFtdcOrderRefType	QUOTE_REF;	//报价引用

TThostFtdcFrontIDType	FRONT_ID;	//前置编号
TThostFtdcSessionIDType	SESSION_ID;	//会话编号

char MD_FRONT_ADDR[] = "tcp://172.16.100.225:41213"; // MD配置参数

//"tcp://210.5.151.247:41213"; // 		// 前置地址
// Trader配置参数

char  TRADER_FRONT_ADDR[] = "tcp://172.16.100.225:41205";	// 前置地址

TThostFtdcBrokerIDType	BROKER_ID = "7080";				// 经纪公司代码
TThostFtdcInvestorIDType INVESTOR_ID = "20104965";			// 投资者代码
TThostFtdcPasswordType  PASSWORD = "112288";			// 用户密码
//***************************************************************

queue<Msg> MsgQueue;   ///消息队列

