#include "common.h"

//*************ȫ�ֱ���*************************************************************

int iRequestID = 0; // ������

int spreed = 2;

int iNextOrderRef = 0;  ///����Ӧ�ñ��

map<string, CThostFtdcDepthMarketDataField> MarketDataField;//��Լ����

map<string, double> payoff; //��Լ����

map<string, string> BidORDER_REF_present;//�򱨼�����

map<string, string> AskORDER_REF_present;//����������

vector<string> Instrumentlist; //���ĺ�Լlist

std::mutex   g_lockqueue;//�̻߳�����

queue<Msg> MsgQueue;   ///��Ϣ����

map<string,CThostFtdcOrderField> OrderMap;//ί���б�

vector<CThostFtdcInputOrderField> InputOrderList;//ί��¼��

vector<CThostFtdcTradeField> TradeList; //�ɽ��б�;

vector<CThostFtdcInputOrderActionField> InputOrderActionList;//ί�в����б�

vector<CThostFtdcInvestorPositionField> InvestorPositionList;//�ֲ��б�

///����ƽ����
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

//�ֲָ���
void PositionChange(string InstrumentID, TThostFtdcDirectionType Direction, TThostFtdcOffsetFlagType OffsetFlag, int Volume)
{

	TThostFtdcPosiDirectionType PosiDirectionType;
	//����
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
	} //ƽ��
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

//�ֲֶ���
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
	else //ƽ��
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


//ί��״̬��ѯ
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




//ע�⣺���ַ���Ϊ��ʱ��Ҳ�᷵��һ�����ַ���  
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