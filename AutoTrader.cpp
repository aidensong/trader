#include "AutoTrader.h"
#include "common.h"

////*************ȫ�ֱ���*************************************************************
//
//CThostFtdcMdApi* pMDUserApi;
//
//CThostFtdcTraderApi* pTraderUserApi;
//
//int iRequestID = 0; // ������
//
//map<string, CThostFtdcDepthMarketDataField> MarketDataField;//��Լ����
//
//map<string, string> InstrumentStatus;//��Լ״̬
//
//map<string, double> payoff; //��Լ����
//
//map<string, double> Bid;//�򱨼�
//
//map<string, double> Ask;//������
//
//map<string, int> LongPosition; //��ֲ�
//
//map<string, int> ShortPosition;//���ֲ�
//
//map<string, int> LongEnClose; //��ֿ�ƽ����
//
//map<string, int> ShortEnClose;//���ֿ�ƽ����
//
//map<string, string> BidORDER_REF;//�򱨼�����
//
//map<string, string> AskORDER_REF;//����������
//
//vector<string> Instrumentlist; //���ĺ�Լlist
//
//vector<double> StrikePrices;  //��Ȩ��list
//
//std::mutex   g_lockqueue;
//
//int spreed=2;
//
//int iNextOrderRef=0;  ///����Ӧ�ñ��
//
//TThostFtdcOrderRefType	ORDER_REF;	//��������
//TThostFtdcOrderRefType	EXECORDER_REF;	//ִ����������
//TThostFtdcOrderRefType	FORQUOTE_REF;	//ѯ������
//TThostFtdcOrderRefType	QUOTE_REF;	//��������
//
//TThostFtdcFrontIDType	FRONT_ID;	//ǰ�ñ��
//TThostFtdcSessionIDType	SESSION_ID;	//�Ự���
//
//// MD���ò���
//char MD_FRONT_ADDR[] = "tcp://172.16.100.225:41213";
////"tcp://210.5.151.247:41213"; // 		// ǰ�õ�ַ
//// Trader���ò���
//char  TRADER_FRONT_ADDR[] = "tcp://172.16.100.225:41205";		// ǰ�õ�ַ
//
//TThostFtdcBrokerIDType	BROKER_ID = "7080";				// ���͹�˾����
//TThostFtdcInvestorIDType INVESTOR_ID = "20104965";			// Ͷ���ߴ���
//TThostFtdcPasswordType  PASSWORD = "112288";			// �û�����
////***************************************************************
//
////queue<>
//


//�ص���
CMdSpi* pMDUserSpi = nullptr;

CTraderSpi* pTraderUserSpi = nullptr;


   ////��ȡ��Լ�����ļ�
void  SysInit()
{
	//��ȡ�����ļ�����
	//std::vector <string> list;

	ifstream inf;

	inf.open("instruments.txt");

	string s;

	while (getline(inf, s))
	{
	
		Instrumentlist.push_back(s);

		CThostFtdcDepthMarketDataField DepthMarketData;
		
		DepthMarketData.LastPrice = -1;
		DepthMarketData.AskPrice1 = -1;
		DepthMarketData.BidPrice1 = -1;

		//������Ӧ��map key
		MarketDataField.insert(map<string, CThostFtdcDepthMarketDataField>::value_type(s, DepthMarketData));

		InstrumentStatus.insert(map<string, string>::value_type(s, "NONE"));	
		
		payoff.insert(map<string, int>::value_type(s, 0));
		
		//�򱨼�
		Bid.insert(map<string, int>::value_type(s, 0));
		//������
		Ask.insert(map<string, int>::value_type(s, 0));		
		
		//�򱨼�����
		BidORDER_REF.insert(map<string, string>::value_type(s, ""));
		
		//����������
		AskORDER_REF.insert(map<string, string>::value_type(s, ""));
		
		//��ֲֿ�ƽ����
		LongEnClose.insert(map<string, int>::value_type(s, 0));

		//���ֲֿ�ƽ����
		ShortEnClose.insert(map<string, int>::value_type(s, 0));

		//��ֲֿ�ƽ����
		LongPosition.insert(map<string, int>::value_type(s, 0));

		//���ֲֿ�ƽ����
		ShortPosition.insert(map<string, int>::value_type(s, 0));

		//if (s.length()>6)
		//	s.substr(8, 4);

	}
	inf.close();
}

///�����߳� ˫�߱��۲���
void CheckOrdersStatus_test()
{
	while (true)
	{
		for (string InstrumentID : Instrumentlist)
		{
		
			//�볡
		
			if ((InstrumentStatus[InstrumentID] == "NONE") && (MarketDataField[InstrumentID].BidPrice1 > 0)
				&& (MarketDataField[InstrumentID].AskPrice1 > 0) 
				&& (MarketDataField[InstrumentID].AskPrice1 > MarketDataField[InstrumentID].BidPrice1))
			{
	           
			   //#define THOST_FTDC_D_Buy 
			
	           //#define THOST_FTDC_D_Sell 

				 pTraderUserSpi->ReqOrderInsert(MarketDataField[InstrumentID].BidPrice1 - spreed / 2, THOST_FTDC_D_Buy, InstrumentID);
			 //   
				//OrderInsert(MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID, THOST_FTDC_D_Sell);
				
				//�򱨼�
				Bid[InstrumentID] = MarketDataField[InstrumentID].BidPrice1 - spreed/2;
				//������				
				Ask[InstrumentID] = MarketDataField[InstrumentID].AskPrice1 + spreed/2;

				InstrumentStatus[InstrumentID]="ALLNOT";	

				cerr << "OnRtnTrade | ��Լ: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
				LOG(INFO) << "OnRtnTrade | ��Լ: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
			   
			}
			if ((InstrumentStatus[InstrumentID] == "ASK") && (MarketDataField[InstrumentID].BidPrice1 > 0)
				&& (MarketDataField[InstrumentID].AskPrice1 > 0) && (MarketDataField[InstrumentID].AskPrice1 > MarketDataField[InstrumentID].BidPrice1))
			{	
				
			/*	ReqOrderCancel(BidORDER_REF[InstrumentID], InstrumentID, THOST_FTDC_D_Buy, Bid[InstrumentID]);	*/			
				//�򱨼�
				Bid[InstrumentID] = Bid[InstrumentID] + spreed / 2;					
				
			/*	OrderInsert(Bid[InstrumentID], InstrumentID, THOST_FTDC_D_Buy);		*/	
				
				InstrumentStatus[InstrumentID] = "BID_Change";
				
				cerr << "Thread| ��Լ: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
				
				LOG(INFO) << "Thread | ��Լ: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;			

			}
		

			if ((InstrumentStatus[InstrumentID] == "BID") && (MarketDataField[InstrumentID].BidPrice1 > 0)
				&& (MarketDataField[InstrumentID].AskPrice1 > 0) && (MarketDataField[InstrumentID].AskPrice1 > MarketDataField[InstrumentID].BidPrice1))
			{		
				
				
		/*		ReqOrderCancel(AskORDER_REF[InstrumentID], InstrumentID, THOST_FTDC_D_Sell,Ask[InstrumentID]);*/
								 
				 //�򱨼�
			     
				Ask[InstrumentID] = Ask[InstrumentID] - spreed / 2;

				InstrumentStatus[InstrumentID] = "ASK_Change";
				
		/*		OrderInsert(Ask[InstrumentID], InstrumentID, THOST_FTDC_D_Sell);*/

				cerr << "Thread| ��Լ: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
				LOG(INFO) << "Thread | ��Լ: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;

			}
		
		}
	}
}

void main(void)
{	
	//CTP��Ϣ
	char MD_FRONT_ADDR[] = "tcp://172.16.100.225:41213"; // MD���ò���
	//"tcp://210.5.151.247:41213"; // 		// ǰ�õ�ַ
	// Trader���ò���
	char  TRADER_FRONT_ADDR[] = "tcp://172.16.100.225:41205";	// ǰ�õ�ַ
	TThostFtdcBrokerIDType	BROKER_ID = "7080";			// ���͹�˾����
	TThostFtdcInvestorIDType INVESTOR_ID = "20104965";		// Ͷ���ߴ���
	TThostFtdcPasswordType  PASSWORD = "112288";		// �û�����	
	

    //LOG
	google::InitGoogleLogging("AutoTrader");  //����Ϊ�Լ��Ŀ�ִ���ļ��� 		

	google::SetLogDestination(google::GLOG_INFO, "./Log/Auto_Info");

	FLAGS_max_log_size = 50;  //�����־��СΪ 100MB

	
	SysInit();//��ʼ��ȫ�ֱ���


	
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(CThostFtdcReqUserLoginField));
	memcpy(req.BrokerID, BROKER_ID, sizeof(TThostFtdcBrokerIDType));
	memcpy(req.UserID, INVESTOR_ID, sizeof(TThostFtdcUserIDType));
	memcpy(req.Password, PASSWORD, sizeof(TThostFtdcPasswordType));
	
	//��ʼ������UserApi
	CThostFtdcTraderApi *pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi
	
	pTraderUserSpi = new CTraderSpi(pTraderUserApi, req);
	
	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// ע���¼���
	
	pTraderUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ����
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// ע��˽����
	pTraderUserApi->RegisterFront(TRADER_FRONT_ADDR);							// connect
	pTraderUserApi->Init();

	
	// ��ʼ������UserApi
	CThostFtdcMdApi* pMDUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi
	
	pMDUserSpi = new CMdSpi(pMDUserApi, req);
	
	pMDUserApi->RegisterSpi(pMDUserSpi);						// ע���¼���
	
	pMDUserApi->RegisterFront(MD_FRONT_ADDR);					// connect
	
	pMDUserApi->Init();
	
	
		
	//�Զ������߳�	
	std::thread th(CheckOrdersStatus_test);
	
	th.join();


}




