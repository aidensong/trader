#include "common.h"

//*************全局变量*************************************************************

int iRequestID = 0; // 请求编号

int spreed = 2;

int iNextOrderRef = 0;  ///报单应用编号

map<string, CThostFtdcDepthMarketDataField> MarketDataField;//合约行情

map<string, double> payoff; //合约收益

map<string, string> BidORDER_REF_present;//买报价引用

map<string, string> AskORDER_REF_present;//卖报价引用

vector<string> Instrumentlist; //订阅合约list

std::mutex   g_lockqueue;//线程互斥量

queue<Msg> MsgQueue;   ///消息队列

map<string,CThostFtdcOrderField> OrderMap;//委托列表

vector<CThostFtdcInputOrderField> InputOrderList;//委托录入

vector<CThostFtdcTradeField> TradeList; //成交列表;

vector<CThostFtdcInputOrderActionField> InputOrderActionList;//委托操作列表

vector<CThostFtdcInvestorPositionField> InvestorPositionList;//持仓列表

///检查可平仓数
int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction)
{
	bool isLong;
	TThostFtdcPosiDirectionType PosiDirectionType;

	if (Direction == THOST_FTDC_D_Buy)
	{
		PosiDirectionType = THOST_FTDC_PD_Long;
		isLong = true;
	}
	else
	{
		PosiDirectionType = THOST_FTDC_PD_Short;
		isLong = false;

	}
	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	{
		if ((InvestorPosition.PosiDirection == PosiDirectionType)
			&& (strcmp(InvestorPosition.InstrumentID,InstrumentID.c_str())==0))
		{
			return (isLong ? InvestorPosition.LongFrozen : InvestorPosition.ShortFrozen);
		}
	}
	return 0;
}

//持仓更改
void PositionChange(string InstrumentID, TThostFtdcDirectionType Direction, TThostFtdcOffsetFlagType OffsetFlag, int Volume)
{

	TThostFtdcPosiDirectionType PosiDirectionType;
	//开仓
	if (OffsetFlag == THOST_FTDC_OF_Open)
	{
		if (Direction == THOST_FTDC_D_Buy)
		{
			PosiDirectionType = THOST_FTDC_PD_Long;
		}
		else
		{
			PosiDirectionType = THOST_FTDC_PD_Short;

		}
		for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
		{
			if ((InvestorPosition.PosiDirection == PosiDirectionType)
				&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
			{
				InvestorPosition.Position += Volume;
			}
		}
	} //平仓
	else
	{
		if (Direction == THOST_FTDC_D_Buy)
		{
			PosiDirectionType = THOST_FTDC_PD_Short;
		}
		else
		{
			PosiDirectionType = THOST_FTDC_PD_Long;

		}
		for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
		{
			if ((InvestorPosition.PosiDirection == PosiDirectionType)
				&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
			{
				InvestorPosition.Position -= Volume;
			}
		}
	}
}

//持仓冻结
void PositionFrozen(string InstrumentID, TThostFtdcDirectionType Direction, TThostFtdcOffsetFlagType OffsetFlag, int Volume)
{
	
	
	bool isLong;
	TThostFtdcPosiDirectionType PosiDirectionType;
	
	if (OffsetFlag == THOST_FTDC_OF_Open)
	{
		if (Direction == THOST_FTDC_D_Buy)
		{
			PosiDirectionType = THOST_FTDC_PD_Long;
			isLong = true;
		}
		else
		{
			PosiDirectionType = THOST_FTDC_PD_Short;
			isLong = false;

		}
		for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
		{
			if ((InvestorPosition.PosiDirection == PosiDirectionType)
				&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
			{
				if (isLong)
					InvestorPosition.LongFrozen += Volume;
				else
					InvestorPosition.ShortFrozen += Volume;
			}
		}
	}
	else //平仓
	{
		if (Direction == THOST_FTDC_D_Buy)
		{
			PosiDirectionType = THOST_FTDC_PD_Short;
			isLong = false;
		}
		else
		{
			PosiDirectionType = THOST_FTDC_PD_Long ;
			isLong = true;

		}
		for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
		{
			if ((InvestorPosition.PosiDirection == PosiDirectionType)
				&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
			{
				if (isLong)
					InvestorPosition.LongFrozen -= Volume;
				else
					InvestorPosition.ShortFrozen -= Volume;
			}
		}
	}
}


//委托状态查询
void OrderCheck(string InstrumentID, TThostFtdcDirectionType Direction, int Volume)
{
	bool isLong;
	TThostFtdcPosiDirectionType PosiDirectionType;
	if (Direction == THOST_FTDC_D_Buy)
	{
		PosiDirectionType = THOST_FTDC_PD_Long;
		isLong = true;
	}
	else
	{
		PosiDirectionType = THOST_FTDC_PD_Short;
		isLong = false;

	}
	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	{
		if ((InvestorPosition.PosiDirection == PosiDirectionType)
			&& (strcmp(InvestorPosition.InstrumentID, InstrumentID.c_str()) == 0))
		{
			if (isLong)
				InvestorPosition.LongFrozen += Volume;
			else
				InvestorPosition.ShortFrozen += Volume;
		}
	}
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