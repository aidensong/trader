#include "AutoTrader.h"
#include "common.h"
#include "Config.h"

//�ص���
CMdSpi* pMDUserSpi = nullptr;

CTraderSpi* pTraderUserSpi = nullptr;

string MDFRONT_ADDR;       //�����ַ
string TRADERFRONT_ADDR;   //���׵�ַ
string INVESTOR_ID;        //�û�
string PASSWORD;           //����
string StrSpreed;         //�۲�


///��ȡ��Լ�����ļ�
void  SysInit()
{
	const char ConfigFile[] = "config.txt";
	Config configSettings(ConfigFile);

	MDFRONT_ADDR = configSettings.Read("MDAddress", MDFRONT_ADDR);

	TRADERFRONT_ADDR = configSettings.Read("TraderAddress", TRADERFRONT_ADDR);

	INVESTOR_ID = configSettings.Read("user", INVESTOR_ID);

	PASSWORD = configSettings.Read("pwd", PASSWORD);

	StrSpreed = configSettings.Read("spreed", StrSpreed);

    stringstream convert;
	convert << StrSpreed;
	convert >> spreed;	
	
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
//��ӡ�ֲ�
void  LogpInvestorPosition()
{
	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	{
		cerr << "<�ֲ�>||<" << InvestorPosition.InstrumentID << ">|����(2.��3.��):" << InvestorPosition.PosiDirection << "|�ֲ�:" << InvestorPosition.Position
			<< "|��ͷ����:" << InvestorPosition.LongFrozen<< "|��ͷ����:" << InvestorPosition.ShortFrozen<< endl;
		LOG(INFO) << "<�ֲ�>||<" << InvestorPosition.InstrumentID <<">|����(2.��3.��):" << InvestorPosition.PosiDirection<< "|�ֲ�:" << InvestorPosition.Position
			<< " | ��ͷ���᣺" << InvestorPosition.LongFrozen<< " | ��ͷ���᣺" << InvestorPosition.ShortFrozen<< endl;
	}
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
		double AskPrice=0;
		double BidPrice=0;
		
		//�볡
		if ((MarketDataField[InstrumentID].BidPrice1 > 0 )&&( MarketDataField[InstrumentID].AskPrice1 > 0)&&
			(MarketDataField[InstrumentID].BidPrice1 <10000000) && (MarketDataField[InstrumentID].AskPrice1 <10000000))
		{
			//��ί��
			if (AskORDER_REF_present[InstrumentID] != "")
			{
				if (OrderMap.count(AskORDER_REF_present[InstrumentID])>0)
				{
					AskOrderStatus = OrderMap[AskORDER_REF_present[InstrumentID]].OrderStatus;
					AskPrice = OrderMap[AskORDER_REF_present[InstrumentID]].LimitPrice;
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

				if (OrderMap.count(BidORDER_REF_present[InstrumentID])>0)
				{
					BidOrderStatus = OrderMap[BidORDER_REF_present[InstrumentID]].OrderStatus;
					BidPrice = OrderMap[BidORDER_REF_present[InstrumentID]].LimitPrice;
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
				stringstream bidstr;				
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, MarketDataField[InstrumentID].BidPrice1 - spreed/2 , InstrumentID);
				bidstr << iNextOrderRef;
				bidstr >> BidORDER_REF_present[InstrumentID];
				
				
				
				stringstream askstr;
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed/2 , InstrumentID);
				askstr << iNextOrderRef;
				askstr >> AskORDER_REF_present[InstrumentID];
			}



			
			///���뱨���ɽ�����������û�гɽ�
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
			{
				//#define THOST_FTDC_D_Buy 
				//#define THOST_FTDC_D_Sell
				
				
				//�Ƿ��Ѿ��Ǹĵ�
				
			if (Ask_refill[InstrumentID].compare(AskORDER_REF_present[InstrumentID])!= 0)				
				{		
					//
			    //pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID);	
				//����
			    pTraderUserSpi->ReqOrderAction(&OrderMap[AskORDER_REF_present[InstrumentID]]);
				
				}
			}
			///���뱨���ɽ������������ѳ�
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_Canceled))
			{
				
				//#define THOST_FTDC_D_Buy 
				//#define THOST_FTDC_D_Sell
				//�Ƿ��Ѿ��Ǹĵ�
				if (Ask_refill[InstrumentID].compare(AskORDER_REF_present[InstrumentID]) != 0)
				{
					stringstream ss;				
					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, BidPrice + spreed, InstrumentID);
					ss << iNextOrderRef;
					ss >> AskORDER_REF_present[InstrumentID];
					Ask_refill[InstrumentID] = AskORDER_REF_present[InstrumentID];
				}
			}
		
			///���������ɽ������뱨��û�гɽ�
			if ((AskOrderStatus == THOST_FTDC_OST_AllTraded) && (BidOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
			{
				//#define THOST_FTDC_D_Buy
				//#define THOST_FTDC_D_Sell 

				//�Ƿ��Ѿ��Ǹĵ�				
				if (Bid_refill[InstrumentID].compare(BidORDER_REF_present[InstrumentID])!=0)
				{
					//����
				pTraderUserSpi->ReqOrderAction(&OrderMap[BidORDER_REF_present[InstrumentID]]);
				
			     }

			}

			///���������ɽ������뱨���ѳ�
			if ((AskOrderStatus == THOST_FTDC_OST_AllTraded) && (BidOrderStatus == THOST_FTDC_OST_Canceled))
			{
				//#define THOST_FTDC_D_Buy
				//#define THOST_FTDC_D_Sell 

				//�Ƿ��Ѿ��Ǹĵ�			

				if (Bid_refill[InstrumentID].compare(BidORDER_REF_present[InstrumentID]) != 0)
				{
					stringstream ss;				
					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, AskPrice - spreed, InstrumentID);
					ss << iNextOrderRef;
					ss >> BidORDER_REF_present[InstrumentID];
					Bid_refill[InstrumentID] = BidORDER_REF_present[InstrumentID];
				}

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

			cerr <<      "****************************��ϢBEGIN*************************************" << endl;
			LOG(INFO) << "****************************��ϢBEGIN*************************************" << endl;
			cerr <<      "<��Ϣ>||����(0.ί��1.�ɽ�2.ί��¼��3.����):" << msg.Msg_Type << "|��Ϣ��:" << MsgQueue.size() << endl;
			LOG(INFO) << "<��Ϣ>||����(0.ί��1.�ɽ�2.ί��¼��3.����):" << msg.Msg_Type << "|��Ϣ��:" << MsgQueue.size() << endl;			
			MsgQueue.pop();			
			g_lockqueue.unlock();			
			//pTraderUserSpi->IsErrorRspInfo(&msg.RspInfo);
			//��Ϣ���д���//
			switch (msg.Msg_Type)
			{
					// ί�лر�
				case RtnOrder:
				{
					//����ί�лر���״̬����**********************************************************************				
					TThostFtdcOrderStatusType OrderStatus = 'n';//ԭί��״̬����ί�г�ʼֵΪn				
					
					//����ί���б����Ƿ���ڴ�ί�У�ȷ����ί���Ƿ�Ϊ��ί��
				
					if (OrderMap.count(msg.RtnOrder.OrderRef)>0)
				
						OrderStatus = OrderMap[msg.RtnOrder.OrderRef].OrderStatus;		
				
					//ί��״̬�ı�
					if (OrderStatus != msg.RtnOrder.OrderStatus)
					{
						///ί��¼�� ������Ӧ�ĳֲ�		
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_NoTradeQueueing)					
							PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], msg.RtnOrder.VolumeTotalOriginal);
				
						//ί�г��� �ָ�����ĳֲ�	
					
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_Canceled)
							PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], -msg.RtnOrder.VolumeTotalOriginal);
				   
						// ί�� �ɽ� �ָ�����ĳֲ�	 ����ֲ�
						if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_AllTraded)
						{
							PositionChange(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], msg.RtnOrder.VolumeTotalOriginal);

							PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], -msg.RtnOrder.VolumeTotalOriginal);
						}									
						
						cerr <<     "<ί��>||���:" << msg.RtnOrder.OrderSysID << "|<" << msg.RtnOrder.InstrumentID
							<< ">����(0.��1.��):" << msg.RtnOrder.Direction << "����:" << msg.RtnOrder.VolumeTotalOriginal
							<< "|��ƽ(0.��1.ƽ):" << msg.RtnOrder.CombOffsetFlag[0]
							<< "|�۸�:" << msg.RtnOrder.LimitPrice
							<< "|״̬:" << msg.RtnOrder.OrderStatus<< endl;						
						
						LOG(INFO) << "<ί��>||���:" << msg.RtnOrder.OrderSysID << "|<" << msg.RtnOrder.InstrumentID
							<< ">����(0.��1.��):" << msg.RtnOrder.Direction << "|����:" << msg.RtnOrder.VolumeTotalOriginal
							<< "|��ƽ(0.��1.ƽ):" << msg.RtnOrder.CombOffsetFlag[0]
							<< "|�۸�:" << msg.RtnOrder.LimitPrice
							<< "|״̬:" << msg.RtnOrder.OrderStatus << endl;			
					
						LogpInvestorPosition();					
						//����ί���б��е�״̬
						OrderMap[msg.RtnOrder.OrderRef] = msg.RtnOrder;
						
						//ִ�в���
						QuotaStrategy();
					}
					
					cerr <<      "****************************��ϢEND*************************************" << endl;
					LOG(INFO) << "****************************��ϢEND*************************************" << endl;
					break;
				};		
				// �ɽ��ر�
				case RtnTrade:
				{			
					//g_lockqueue.lock();

					//TradeList.push_back(msg.RtnTrade);
				
					//PositionChange(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);
				
					//PositionFrozen(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, -msg.RtnTrade.Volume);
				
					//LogpInvestorPosition();
				
					//g_lockqueue.unlock();

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

		
		}
		 
	}
}
void main(void)
{	
	
	//LOG
	google::InitGoogleLogging("AutoTrader");  //����Ϊ�Լ��Ŀ�ִ���ļ��� 		

	google::SetLogDestination(google::GLOG_INFO, "./Log/Auto_Info");

	FLAGS_max_log_size = 50;  //�����־��СΪ 50MB

	SysInit();//��ʼ��ȫ�ֱ���



	//CTP��Ϣ
	//char MD_FRONT_ADDR[] = "tcp://218.56.58.221:5996?exchangeId=111"; // MD���ò���
	//"tcp://210.5.151.247:41213"; // 		// ǰ�õ�ַ
	// Trader���ò���

	///char  TRADER_FRONT_ADDR[] = "tcp://218.56.58.221:5996?exchangeId=111";	// ǰ�õ�ַ

	//TThostFtdcInvestorIDType INVESTOR_ID = "demo";//"20105161";//"		// Ͷ���ߴ���
	//TThostFtdcPasswordType  PASSWORD = "112288";//"123456";//;		// �û�����	



	TThostFtdcCommandTypeType MdFront;

	TThostFtdcCommandTypeType TraderFront;

	CThostFtdcReqUserLoginField req;

	TThostFtdcBrokerIDType	BROKER_ID = "7080";			// ���͹�˾����

	memset(&req, 0, sizeof(CThostFtdcReqUserLoginField));
	memcpy(req.BrokerID, BROKER_ID, sizeof(TThostFtdcBrokerIDType));
	memcpy(req.UserID, INVESTOR_ID.c_str(), sizeof(TThostFtdcUserIDType));
	memcpy(req.Password, PASSWORD.c_str(), sizeof(TThostFtdcPasswordType));

	memset(&MdFront, 0, sizeof(TThostFtdcCommandTypeType));
	memcpy(MdFront, MDFRONT_ADDR.c_str(), sizeof(TThostFtdcCommandTypeType));

	memset(&TraderFront, 0, sizeof(TThostFtdcCommandTypeType));
	memcpy(TraderFront, TRADERFRONT_ADDR.c_str(), sizeof(TThostFtdcCommandTypeType));

	//��ʼ������UserApi
	CThostFtdcTraderApi *pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// ����UserApi

	pTraderUserSpi = new CTraderSpi(pTraderUserApi, req);

	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// ע���¼���

	pTraderUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// ע�ṫ����
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// ע��˽����

	pTraderUserApi->RegisterFront(MdFront);							// connect
	pTraderUserApi->Init();


	// ��ʼ������UserApi
	CThostFtdcMdApi* pMDUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// ����UserApi

	pMDUserSpi = new CMdSpi(pMDUserApi, req);

	pMDUserApi->RegisterSpi(pMDUserSpi);						// ע���¼���

	pMDUserApi->RegisterFront(TraderFront);					// connect

	pMDUserApi->Init();



	//�ȴ�ϵͳ��ʼ�����
	while (!InitFinished)
	  Sleep(1000);
	
	//ִ�в���
	QuotaStrategy();
		
	//�Զ������߳�	
	std::thread th(mProcess);
	
	th.join();


}




