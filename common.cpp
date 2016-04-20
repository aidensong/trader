#include "common.h"

//*************ȫ�ֱ���*************************************************************

int iRequestID = 0; // ������

int spreed = 200;

int iNextOrderRef = 0;  ///����Ӧ�ñ��

map<string, CThostFtdcDepthMarketDataField> MarketDataField;//��Լ����

map<string, string> InstrumentStatus;//��Լ״̬

map<string, double> payoff; //��Լ����

map<string, double> Bid;//�򱨼�

map<string, double> Ask;//������

map<string, int> LongPosition; //��ֲ�

map<string, int> ShortPosition;//���ֲ�

map<string, int> LongEnClose; //��ֿ�ƽ����

map<string, int> ShortEnClose;//���ֿ�ƽ����

map<string, string> BidORDER_REF;//�򱨼�����

map<string, string> AskORDER_REF;//����������

vector<string> Instrumentlist; //���ĺ�Լlist

vector<double> StrikePrices;  //��Ȩ��list

std::mutex   g_lockqueue;//�̻߳�����

queue<Msg> MsgQueue;   ///��Ϣ����


vector<CThostFtdcOrderField> OrderList;//ί���б�

vector<CThostFtdcInputOrderField> InputOrderList;//ί��¼��

vector<CThostFtdcInputOrderActionField> InputOrderActionList;//ί�в����б�

vector<CThostFtdcInvestorPositionField> InvestorPositionList;//�ֲ��б�


//CThostFtdcTradeField  RtnTrade; //�ɽ��ر�

///����ƽ����
int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction)
{
	if (Direction == THOST_FTDC_D_Buy)
		return	ShortEnClose[InstrumentID];
	else
		return	LongEnClose[InstrumentID];
	//THOST_FTDC_D_Buy 
	//		///��
	//THOST_FTDC_D_Sell '1'
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