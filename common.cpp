#include "common.h"

//*************ȫ�ֱ���*************************************************************

CThostFtdcMdApi* pMDUserApi;

CThostFtdcTraderApi* pTraderUserApi;

int iRequestID = 0; // ������

int spreed = 2000000;

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

std::mutex   g_lockqueue;


TThostFtdcOrderRefType	ORDER_REF;	//��������
TThostFtdcOrderRefType	EXECORDER_REF;	//ִ����������
TThostFtdcOrderRefType	FORQUOTE_REF;	//ѯ������
TThostFtdcOrderRefType	QUOTE_REF;	//��������

TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
TThostFtdcSessionIDType	SESSION_ID;	//�Ự���

char MD_FRONT_ADDR[] = "tcp://172.16.100.225:41213"; // MD���ò���

//"tcp://210.5.151.247:41213"; // 		// ǰ�õ�ַ
// Trader���ò���

char  TRADER_FRONT_ADDR[] = "tcp://172.16.100.225:41205";	// ǰ�õ�ַ

TThostFtdcBrokerIDType	BROKER_ID = "7080";				// ���͹�˾����
TThostFtdcInvestorIDType INVESTOR_ID = "20104965";			// Ͷ���ߴ���
TThostFtdcPasswordType  PASSWORD = "112288";			// �û�����
//***************************************************************

queue<Msg> MsgQueue;   ///��Ϣ����

