#include "AutoTrader.h"
#include "common.h"

//�ص���
CMdSpi* pMDUserSpi = nullptr;

CTraderSpi* pTraderUserSpi = nullptr;


///��ȡ��Լ�����ļ�
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
		
		payoff.insert(map<string, int>::value_type(s, 0));
		
		//�򱨼�����
		BidORDER_REF_present.insert(map<string, string>::value_type(s, ""));
		
		//����������
		AskORDER_REF_present.insert(map<string, string>::value_type(s, ""));
	

		//if (s.length()>6)
		//	s.substr(8, 4);

	}
	inf.close();
}

///�����߳� ˫�߱��۲���
void QuotaStrategy()
{

	//	///TFtdcOrderStatusType��һ������״̬����
	//	/////////////////////////////////////////////////////////////////////////
	//	///ȫ���ɽ�
	//#define THOST_FTDC_OST_AllTraded '0'
	//	///���ֳɽ����ڶ�����
	//#define THOST_FTDC_OST_PartTradedQueueing '1'
	//	///���ֳɽ����ڶ�����
	//#define THOST_FTDC_OST_PartTradedNotQueueing '2'
	//	///δ�ɽ����ڶ�����
	//#define THOST_FTDC_OST_NoTradeQueueing '3'
	//	///δ�ɽ����ڶ�����
	//#define THOST_FTDC_OST_NoTradeNotQueueing '4'
	//	///����
	//#define THOST_FTDC_OST_Canceled '5'
	//	///δ֪
	//#define THOST_FTDC_OST_Unknown 'a'
	//	///��δ����
	//#define THOST_FTDC_OST_NotTouched 'b'
	//	///�Ѵ���
	//#define THOST_FTDC_OST_Touched 'c'
	//
	//	typedef char TThostFtdcOrderStatusType;


	//�г�������������	
	for (string InstrumentID : Instrumentlist)
	{
		TThostFtdcOrderStatusType AskOrderStatus;
		TThostFtdcOrderStatusType BidOrderStatus;
		//�볡
		if (MarketDataField[InstrumentID].BidPrice1 > 0 && MarketDataField[InstrumentID].AskPrice1 > 0)
		{
			//��ί��
			if (AskORDER_REF_present[InstrumentID] != "")
			{
				map<string, CThostFtdcOrderField>::iterator it = OrderMap.find(AskORDER_REF_present[InstrumentID]);

				if (it != OrderMap.end())
				{
					AskOrderStatus = it->second.OrderStatus;
				}
				else
				{    ///ί�б�CTP�˻�
					AskOrderStatus = 'e';
				}
			}
			else///û�п�ʼ����
				AskOrderStatus = THOST_FTDC_OST_AllTraded;

			//��ί��
			if (BidORDER_REF_present[InstrumentID] != "")
			{
				map<string, CThostFtdcOrderField>::iterator it = OrderMap.find(BidORDER_REF_present[InstrumentID]);

				if (it != OrderMap.end())
				{
					BidOrderStatus = it->second.OrderStatus;
				}
				else
				{    ///ί�б�CTP�˻�
					BidOrderStatus = 'e';
				}
			}
			else///û�п�ʼ����
				BidOrderStatus = THOST_FTDC_OST_AllTraded;
			
			
			///˫�߶��ɽ����±���
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_AllTraded))
			{
				//#define THOST_FTDC_D_Buy 

				//#define THOST_FTDC_D_Sell 		
			
				stringstream ss;
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, MarketDataField[InstrumentID].BidPrice1 - spreed / 2, InstrumentID);
				ss << iNextOrderRef;
				ss >> BidORDER_REF_present[InstrumentID];

				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID);
				ss << iNextOrderRef;
				ss >> AskORDER_REF_present[InstrumentID];

			}
			///���뱨���ɽ�����������û�гɽ�
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus!= THOST_FTDC_OST_AllTraded))
			{
				//#define THOST_FTDC_D_Buy 

				//#define THOST_FTDC_D_Sell 		

				stringstream ss;
				
				//����
				pTraderUserSpi->ReqOrderAction(&OrderMap[AskORDER_REF_present[InstrumentID]]);

				//
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID);
				ss << iNextOrderRef;
				ss >> AskORDER_REF_present[InstrumentID];

			}
			///���������ɽ������뱨��û�гɽ�
			if ((AskOrderStatus   == THOST_FTDC_OST_AllTraded) && (BidOrderStatus != THOST_FTDC_OST_AllTraded))
			{
				//#define THOST_FTDC_D_Buy 

				//#define THOST_FTDC_D_Sell 		

				stringstream ss;
				
				//����
				pTraderUserSpi->ReqOrderAction(&OrderMap[BidORDER_REF_present[InstrumentID]]);
				//
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, MarketDataField[InstrumentID].BidPrice1 - spreed / 2, InstrumentID);
				ss << iNextOrderRef;
				ss >> AskORDER_REF_present[InstrumentID];

			}

		}
	}
}

///��Ϣ�������߳�
void mProcess()
{
	while (true)
	{
		if (!MsgQueue.empty())
		{
			Msg msg;
			g_lockqueue.lock();

			msg = MsgQueue.front();
			cerr << MsgQueue.size()<< endl;
			//LOG(INFO) << "--->>> ����<" << InstrumentID << ">����|�۸�" << Price << "¼������: " << iResult << ((iResult == 0) ? ", �ɹ�" : ", ʧ��") << endl;
			MsgQueue.pop();



			g_lockqueue.unlock();

			//pTraderUserSpi->IsErrorRspInfo(&msg.RspInfo);

			//��Ϣ���д���//

			switch (msg.Msg_Type)
			{
				// ί�лر�
			case RtnOrder:
			{
				g_lockqueue.lock();
				OrderMap[msg.RtnOrder.OrderRef] = msg.RtnOrder;
				
				g_lockqueue.unlock();
				break;

			};
			// �ɽ��ر�
			case RtnTrade:
			{
				g_lockqueue.lock();
				TradeList.push_back(msg.RtnTrade);
				PositionChange(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);
				PositionFrozen(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);
				g_lockqueue.unlock();

				break;
			};
			//����¼��
			case InputOrder:
			{
				break;

			};
			// ��������¼��
			case InputOrderAction:
			{
				break;
			}

			}

			//ִ�в���
			QuotaStrategy();
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
	TThostFtdcBrokerIDType	BROKER_ID ="7080" ;			// ���͹�˾����
	TThostFtdcInvestorIDType INVESTOR_ID = "20104965";//"20105161";//"		// Ͷ���ߴ���
		TThostFtdcPasswordType  PASSWORD = "112288";//"123456";//;		// �û�����	

    //LOG
	google::InitGoogleLogging("AutoTrader");  //����Ϊ�Լ��Ŀ�ִ���ļ��� 		

	google::SetLogDestination(google::GLOG_INFO, "./Log/Auto_Info");

	FLAGS_max_log_size = 50;  //�����־��СΪ 50MB

	
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
	
	//std::unique_lock <std::mutex> lck(g_lockqueue);

	//while (!InitFinished) // �����־λ��Ϊ true, ��ȴ�...
	//
	//	cv.wait(lck); // ��ǰ�̱߳�����, ��ȫ�ֱ�־λ��Ϊ true ֮�� �̼߳���ִ��

	while (!InitFinished)
	  Sleep(1000);
	
	//ִ�в���
	QuotaStrategy();
		
	//�Զ������߳�	
	std::thread th(mProcess);
	
	th.join();


}




