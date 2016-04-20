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
	RtnOrder = 0,		// ί�лر�

	RtnTrade = 1,       // �ɽ��ر�

	InputOrder = 2,       //����¼��

	InputOrderAction = 3,   // ��������¼��

	//CThostFtdcInputOrderField
};

struct Msg
{
	MsgType Msg_Type;

	CThostFtdcOrderField  RtnOrder; //ί�лر�

	CThostFtdcTradeField  RtnTrade; //�ɽ��ر�

	CThostFtdcInputOrderField InputOrder;//����¼��

	CThostFtdcInputOrderActionField InputOrderAction;//��������¼��

	CThostFtdcRspInfoField RspInfo;

};


//*************ȫ�ֱ���*************************************************************

using namespace std;


extern int iRequestID; // ������

extern map<string, CThostFtdcDepthMarketDataField> MarketDataField;//��Լ����

extern map<string, string> InstrumentStatus;//��Լ״̬

extern map<string, double> payoff; //��Լ����

extern map<string, double> Bid;//�򱨼�

extern map<string, double> Ask;//������

extern map<string, int> LongPosition; //��ֲ�

extern map<string, int> ShortPosition;//���ֲ�

extern map<string, int> LongEnClose; //��ֿ�ƽ����

extern map<string, int> ShortEnClose;//���ֿ�ƽ����extern extern 

extern map<string, string> BidORDER_REF;//�򱨼�����

extern map<string, string> AskORDER_REF;//����������

extern vector<string> Instrumentlist; //���ĺ�Լlist

extern vector<double> StrikePrices;  //��Ȩ��list

extern std::mutex   g_lockqueue;

extern int spreed ;//= 2;

extern int iNextOrderRef;  ///����Ӧ�ñ�� 

extern queue<Msg> MsgQueue;   ///��Ϣ����

extern vector<CThostFtdcOrderField> OrderList;//ί���б�

extern vector<CThostFtdcInputOrderField> InputOrderList;//ί��¼��

extern vector<CThostFtdcInputOrderActionField> InputOrderActionList;//ί�в����б�

extern vector<CThostFtdcInvestorPositionField> InvestorPositionList;//�ֲ��б�

extern int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction);
#endif