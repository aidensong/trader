#include "common.h"

//*************全局变量*************************************************************

int iRequestID = 0; // 请求编号

int spreed = 200;

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

std::mutex   g_lockqueue;//线程互斥量

queue<Msg> MsgQueue;   ///消息队列

