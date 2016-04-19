#include "AutoTrader.h"
#include "common.h"

////*************全局变量*************************************************************
//
//CThostFtdcMdApi* pMDUserApi;
//
//CThostFtdcTraderApi* pTraderUserApi;
//
//int iRequestID = 0; // 请求编号
//
//map<string, CThostFtdcDepthMarketDataField> MarketDataField;//合约行情
//
//map<string, string> InstrumentStatus;//合约状态
//
//map<string, double> payoff; //合约收益
//
//map<string, double> Bid;//买报价
//
//map<string, double> Ask;//卖报价
//
//map<string, int> LongPosition; //买持仓
//
//map<string, int> ShortPosition;//卖持仓
//
//map<string, int> LongEnClose; //买持可平仓量
//
//map<string, int> ShortEnClose;//卖持可平仓量
//
//map<string, string> BidORDER_REF;//买报价引用
//
//map<string, string> AskORDER_REF;//卖报价引用
//
//vector<string> Instrumentlist; //订阅合约list
//
//vector<double> StrikePrices;  //行权价list
//
//std::mutex   g_lockqueue;
//
//int spreed=2;
//
//int iNextOrderRef=0;  ///报单应用编号
//
//TThostFtdcOrderRefType	ORDER_REF;	//报单引用
//TThostFtdcOrderRefType	EXECORDER_REF;	//执行宣告引用
//TThostFtdcOrderRefType	FORQUOTE_REF;	//询价引用
//TThostFtdcOrderRefType	QUOTE_REF;	//报价引用
//
//TThostFtdcFrontIDType	FRONT_ID;	//前置编号
//TThostFtdcSessionIDType	SESSION_ID;	//会话编号
//
//// MD配置参数
//char MD_FRONT_ADDR[] = "tcp://172.16.100.225:41213";
////"tcp://210.5.151.247:41213"; // 		// 前置地址
//// Trader配置参数
//char  TRADER_FRONT_ADDR[] = "tcp://172.16.100.225:41205";		// 前置地址
//
//TThostFtdcBrokerIDType	BROKER_ID = "7080";				// 经纪公司代码
//TThostFtdcInvestorIDType INVESTOR_ID = "20104965";			// 投资者代码
//TThostFtdcPasswordType  PASSWORD = "112288";			// 用户密码
////***************************************************************
//
////queue<>
//


//回调类
CMdSpi* pMDUserSpi = nullptr;

CTraderSpi* pTraderUserSpi = nullptr;


   ////读取合约配置文件
void  SysInit()
{
	//读取配置文件代码
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

		//创建对应的map key
		MarketDataField.insert(map<string, CThostFtdcDepthMarketDataField>::value_type(s, DepthMarketData));

		InstrumentStatus.insert(map<string, string>::value_type(s, "NONE"));	
		
		payoff.insert(map<string, int>::value_type(s, 0));
		
		//买报价
		Bid.insert(map<string, int>::value_type(s, 0));
		//卖报价
		Ask.insert(map<string, int>::value_type(s, 0));		
		
		//买报价引用
		BidORDER_REF.insert(map<string, string>::value_type(s, ""));
		
		//卖报价引用
		AskORDER_REF.insert(map<string, string>::value_type(s, ""));
		
		//买持仓可平仓量
		LongEnClose.insert(map<string, int>::value_type(s, 0));

		//卖持仓可平仓量
		ShortEnClose.insert(map<string, int>::value_type(s, 0));

		//买持仓可平仓量
		LongPosition.insert(map<string, int>::value_type(s, 0));

		//卖持仓可平仓量
		ShortPosition.insert(map<string, int>::value_type(s, 0));

		//if (s.length()>6)
		//	s.substr(8, 4);

	}
	inf.close();
}

///报单线程 双边报价策略
void CheckOrdersStatus_test()
{
	while (true)
	{
		for (string InstrumentID : Instrumentlist)
		{
		
			//入场
		
			if ((InstrumentStatus[InstrumentID] == "NONE") && (MarketDataField[InstrumentID].BidPrice1 > 0)
				&& (MarketDataField[InstrumentID].AskPrice1 > 0) 
				&& (MarketDataField[InstrumentID].AskPrice1 > MarketDataField[InstrumentID].BidPrice1))
			{
	           
			   //#define THOST_FTDC_D_Buy 
			
	           //#define THOST_FTDC_D_Sell 

				 pTraderUserSpi->ReqOrderInsert(MarketDataField[InstrumentID].BidPrice1 - spreed / 2, THOST_FTDC_D_Buy, InstrumentID);
			 //   
				//OrderInsert(MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID, THOST_FTDC_D_Sell);
				
				//买报价
				Bid[InstrumentID] = MarketDataField[InstrumentID].BidPrice1 - spreed/2;
				//卖报价				
				Ask[InstrumentID] = MarketDataField[InstrumentID].AskPrice1 + spreed/2;

				InstrumentStatus[InstrumentID]="ALLNOT";	

				cerr << "OnRtnTrade | 合约: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
				LOG(INFO) << "OnRtnTrade | 合约: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
			   
			}
			if ((InstrumentStatus[InstrumentID] == "ASK") && (MarketDataField[InstrumentID].BidPrice1 > 0)
				&& (MarketDataField[InstrumentID].AskPrice1 > 0) && (MarketDataField[InstrumentID].AskPrice1 > MarketDataField[InstrumentID].BidPrice1))
			{	
				
			/*	ReqOrderCancel(BidORDER_REF[InstrumentID], InstrumentID, THOST_FTDC_D_Buy, Bid[InstrumentID]);	*/			
				//买报价
				Bid[InstrumentID] = Bid[InstrumentID] + spreed / 2;					
				
			/*	OrderInsert(Bid[InstrumentID], InstrumentID, THOST_FTDC_D_Buy);		*/	
				
				InstrumentStatus[InstrumentID] = "BID_Change";
				
				cerr << "Thread| 合约: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
				
				LOG(INFO) << "Thread | 合约: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;			

			}
		

			if ((InstrumentStatus[InstrumentID] == "BID") && (MarketDataField[InstrumentID].BidPrice1 > 0)
				&& (MarketDataField[InstrumentID].AskPrice1 > 0) && (MarketDataField[InstrumentID].AskPrice1 > MarketDataField[InstrumentID].BidPrice1))
			{		
				
				
		/*		ReqOrderCancel(AskORDER_REF[InstrumentID], InstrumentID, THOST_FTDC_D_Sell,Ask[InstrumentID]);*/
								 
				 //买报价
			     
				Ask[InstrumentID] = Ask[InstrumentID] - spreed / 2;

				InstrumentStatus[InstrumentID] = "ASK_Change";
				
		/*		OrderInsert(Ask[InstrumentID], InstrumentID, THOST_FTDC_D_Sell);*/

				cerr << "Thread| 合约: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;
				LOG(INFO) << "Thread | 合约: <" << InstrumentID << "> | Status : " << InstrumentStatus[InstrumentID] << endl;

			}
		
		}
	}
}

void main(void)
{	
	//CTP信息
	char MD_FRONT_ADDR[] = "tcp://172.16.100.225:41213"; // MD配置参数
	//"tcp://210.5.151.247:41213"; // 		// 前置地址
	// Trader配置参数
	char  TRADER_FRONT_ADDR[] = "tcp://172.16.100.225:41205";	// 前置地址
	TThostFtdcBrokerIDType	BROKER_ID = "7080";			// 经纪公司代码
	TThostFtdcInvestorIDType INVESTOR_ID = "20104965";		// 投资者代码
	TThostFtdcPasswordType  PASSWORD = "112288";		// 用户密码	
	

    //LOG
	google::InitGoogleLogging("AutoTrader");  //参数为自己的可执行文件名 		

	google::SetLogDestination(google::GLOG_INFO, "./Log/Auto_Info");

	FLAGS_max_log_size = 50;  //最大日志大小为 100MB

	
	SysInit();//初始化全局变量


	
	CThostFtdcReqUserLoginField req;
	memset(&req, 0, sizeof(CThostFtdcReqUserLoginField));
	memcpy(req.BrokerID, BROKER_ID, sizeof(TThostFtdcBrokerIDType));
	memcpy(req.UserID, INVESTOR_ID, sizeof(TThostFtdcUserIDType));
	memcpy(req.Password, PASSWORD, sizeof(TThostFtdcPasswordType));
	
	//初始化交易UserApi
	CThostFtdcTraderApi *pTraderUserApi = CThostFtdcTraderApi::CreateFtdcTraderApi();			// 创建UserApi
	
	pTraderUserSpi = new CTraderSpi(pTraderUserApi, req);
	
	pTraderUserApi->RegisterSpi((CThostFtdcTraderSpi*)pTraderUserSpi);			// 注册事件类
	
	pTraderUserApi->SubscribePublicTopic(THOST_TERT_QUICK);				// 注册公有流
	pTraderUserApi->SubscribePrivateTopic(THOST_TERT_QUICK);				// 注册私有流
	pTraderUserApi->RegisterFront(TRADER_FRONT_ADDR);							// connect
	pTraderUserApi->Init();

	
	// 初始化行情UserApi
	CThostFtdcMdApi* pMDUserApi = CThostFtdcMdApi::CreateFtdcMdApi();			// 创建UserApi
	
	pMDUserSpi = new CMdSpi(pMDUserApi, req);
	
	pMDUserApi->RegisterSpi(pMDUserSpi);						// 注册事件类
	
	pMDUserApi->RegisterFront(MD_FRONT_ADDR);					// connect
	
	pMDUserApi->Init();
	
	
		
	//自动报单线程	
	std::thread th(CheckOrdersStatus_test);
	
	th.join();


}




