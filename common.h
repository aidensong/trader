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

//*************ȫ�ֱ���*************************************************************

using namespace std;

extern CThostFtdcMdApi* pMDUserApi;

extern CThostFtdcTraderApi* pTraderUserApi;

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

extern TThostFtdcOrderRefType	ORDER_REF;	//��������
extern TThostFtdcOrderRefType	EXECORDER_REF;	//ִ����������
extern TThostFtdcOrderRefType	FORQUOTE_REF;	//ѯ������
extern TThostFtdcOrderRefType	QUOTE_REF;	//��������

extern TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
extern TThostFtdcSessionIDType	SESSION_ID;	//�Ự���

// MD���ò���
extern char MD_FRONT_ADDR[];// = "tcp://172.16.100.225:41213";
//"tcp://210.5.151.247:41213"; // 		// ǰ�õ�ַ


// Trader���ò���
extern char  TRADER_FRONT_ADDR[];		     // ǰ�õ�ַ
extern TThostFtdcBrokerIDType	BROKER_ID;				// ���͹�˾����extern 
extern TThostFtdcInvestorIDType INVESTOR_ID;			// Ͷ���ߴ���
extern TThostFtdcPasswordType  PASSWORD;			// �û�����
//***************************************************************

 enum MsgType
	{
		RtnOrder = 0,		// ί�лر�

		RtnTrade = 1,       // �ɽ��ر�
		
		InputOrder=2,       //����¼��

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

 extern queue<Msg> MsgQueue;   ///��Ϣ����

#endif