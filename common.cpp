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


vector<CThostFtdcOrderField> OrderList;//委托列表

vector<CThostFtdcInputOrderField> InputOrderList;//委托录入

vector<CThostFtdcInputOrderActionField> InputOrderActionList;//委托操作列表

vector<CThostFtdcInvestorPositionField> InvestorPositionList;//持仓列表


//CThostFtdcTradeField  RtnTrade; //成交回报

///检查可平仓数
int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction)
{
	if (Direction == THOST_FTDC_D_Buy)
		return	ShortEnClose[InstrumentID];
	else
		return	LongEnClose[InstrumentID];
	//THOST_FTDC_D_Buy 
	//		///卖
	//THOST_FTDC_D_Sell '1'
}

//注意：当字符串为空时，也会返回一个空字符串  
void split(std::string& s, std::string& delim, std::vector< std::string >* ret)
{
	size_t last = 0;
	size_t index = s.find_first_of(delim, last);
	while (index != std::string::npos)
	{
		ret->push_back(s.substr(last, index - last));
		last = index + 1;
		index = s.find_first_of(delim, last);
	}
	if (index - last>0)
	{
		ret->push_back(s.substr(last, index - last));
	}
}