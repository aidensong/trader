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

