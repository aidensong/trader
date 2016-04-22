#include "AutoTrader.h"
#include "common.h"

//回调类
CMdSpi* pMDUserSpi = nullptr;

CTraderSpi* pTraderUserSpi = nullptr;


///读取合约配置文件
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
		
		payoff.insert(map<string, int>::value_type(s, 0));
		
		//买报价引用
		BidORDER_REF_present.insert(map<string, string>::value_type(s, ""));
		
		//卖报价引用
		AskORDER_REF_present.insert(map<string, string>::value_type(s, ""));

		//if (s.length()>6)
		//	s.substr(8, 4);
	}
	inf.close();
}
//打印持仓
void  LogpInvestorPosition()
{
	for (CThostFtdcInvestorPositionField InvestorPosition : InvestorPositionList)
	{
		cerr << "持仓|合约<" << InvestorPosition.InstrumentID << "> | 持仓数：" << InvestorPosition.Position
			<< " | 多头冻结数：" << InvestorPosition.LongFrozen
			<< " | 空头冻结数：" << InvestorPosition.ShortFrozen
			<< endl;
		LOG(INFO) << "持仓|合约<" << InvestorPosition.InstrumentID << "> | 持仓数：" << InvestorPosition.Position
			<< " | 多头冻结数：" << InvestorPosition.LongFrozen
			<< " | 空头冻结数：" << InvestorPosition.ShortFrozen
			<< endl;
	}
}


///报单线程 双边报价策略
void QuotaStrategy()
{
	//	///TFtdcOrderStatusType是一个报单状态类型
	//	/////////////////////////////////////////////////////////////////////////
	//	///全部成交
	//#define THOST_FTDC_OST_AllTraded '0'
	//	///部分成交还在队列中
	//#define THOST_FTDC_OST_PartTradedQueueing '1'
	//	///部分成交不在队列中
	//#define THOST_FTDC_OST_PartTradedNotQueueing '2'
	//	///未成交还在队列中
	//#define THOST_FTDC_OST_NoTradeQueueing '3'
	//	///未成交不在队列中
	//#define THOST_FTDC_OST_NoTradeNotQueueing '4'
	//	///撤单
	//#define THOST_FTDC_OST_Canceled '5'
	//	///未知
	//#define THOST_FTDC_OST_Unknown 'a'
	//	///尚未触发
	//#define THOST_FTDC_OST_NotTouched 'b'
	//	///已触发
	//#define THOST_FTDC_OST_Touched 'c'
	//
	//	typedef char TThostFtdcOrderStatusType;
	//市场存在买卖报价	
	for (string InstrumentID : Instrumentlist)
	{
		TThostFtdcOrderStatusType AskOrderStatus;
		TThostFtdcOrderStatusType BidOrderStatus;
		//入场
		if (MarketDataField[InstrumentID].BidPrice1 > 0 && MarketDataField[InstrumentID].AskPrice1 > 0&
			MarketDataField[InstrumentID].BidPrice1 <10000000 && MarketDataField[InstrumentID].AskPrice1 <10000000)
		{
			//卖委托
			if (AskORDER_REF_present[InstrumentID] != "")
			{
				if (OrderMap.count(AskORDER_REF_present[InstrumentID])>0)
				{
					AskOrderStatus = OrderMap[AskORDER_REF_present[InstrumentID]].OrderStatus;
				}
				else
				{    ///委托被CTP退回
					AskOrderStatus = 'e';
				}
			}
			else///没有开始报单
				AskOrderStatus = THOST_FTDC_OST_AllTraded;

		
			//买委托
			if (BidORDER_REF_present[InstrumentID] != "")
			{		

				if (OrderMap.count(BidORDER_REF_present[InstrumentID])>0)
				{
					BidOrderStatus = OrderMap[BidORDER_REF_present[InstrumentID]].OrderStatus;
				}
				else
				{    ///委托被CTP退回
					BidOrderStatus = 'e';
				}
			}
			else///没有开始报单
				BidOrderStatus = THOST_FTDC_OST_AllTraded;			
			
			///双边都成交重新报单
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_AllTraded))
			{
				//#define THOST_FTDC_D_Buy 
				//#define THOST_FTDC_D_Sell		
				stringstream bidstr;				
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, MarketDataField[InstrumentID].BidPrice1 - spreed / 2, InstrumentID);
				bidstr << iNextOrderRef;
				bidstr >> BidORDER_REF_present[InstrumentID];
				stringstream askstr;
				pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID);
				askstr << iNextOrderRef;
				askstr >> AskORDER_REF_present[InstrumentID];
			}
			///买入报单成交而卖出报单没有成交
			if ((BidOrderStatus == THOST_FTDC_OST_AllTraded) && (AskOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
			{
				//#define THOST_FTDC_D_Buy 
				//#define THOST_FTDC_D_Sell
				//是否已经是改单
				if (Ask_refill.count(AskORDER_REF_present[InstrumentID])==0)
				{
					stringstream ss;
					//撤单
					pTraderUserSpi->ReqOrderAction(&OrderMap[AskORDER_REF_present[InstrumentID]]);
					//
					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Sell, MarketDataField[InstrumentID].AskPrice1 + spreed / 2, InstrumentID);
					ss << iNextOrderRef;
					ss >> AskORDER_REF_present[InstrumentID];
					Ask_refill[InstrumentID] = AskORDER_REF_present[InstrumentID];
				}
			}

			///卖出报单成交而买入报单没有成交
			if ((AskOrderStatus == THOST_FTDC_OST_AllTraded) && (BidOrderStatus == THOST_FTDC_OST_NoTradeQueueing))
			{
				//#define THOST_FTDC_D_Buy
				//#define THOST_FTDC_D_Sell 

				//是否已经是改单			
				
				if (Bid_refill.count(BidORDER_REF_present[InstrumentID])==0);			
				{
					stringstream ss;
					//撤单
					pTraderUserSpi->ReqOrderAction(&OrderMap[BidORDER_REF_present[InstrumentID]]);
					//
					pTraderUserSpi->ReqOrderInsert(THOST_FTDC_D_Buy, MarketDataField[InstrumentID].BidPrice1 - spreed / 2, InstrumentID);
					ss << iNextOrderRef;
					ss >> BidORDER_REF_present[InstrumentID];
					Bid_refill[InstrumentID] = BidORDER_REF_present[InstrumentID];
				}

			}

		}
	}
}

///消息处理主线程
void mProcess()
{
	while (true)
	{
		if (!MsgQueue.empty())
		{
			Msg msg;
			g_lockqueue.lock();

			msg = MsgQueue.front();
			cerr << "消息类型：" << msg.Msg_Type << endl;
			LOG(INFO) << "消息类型：" << msg.Msg_Type << endl;
			cerr <<"消息队列消息数："<<MsgQueue.size()<< endl;
			LOG(INFO)<< "消息队列消息数：" << MsgQueue.size() << endl;		
			//LOG(INFO) << "--->>> 买入<" << InstrumentID << ">报单|价格：" << Price << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
			MsgQueue.pop();
			g_lockqueue.unlock();
			//pTraderUserSpi->IsErrorRspInfo(&msg.RspInfo);
			//消息队列处理//

			switch (msg.Msg_Type)
			{
				// 委托回报
			case RtnOrder:
			{
				g_lockqueue.lock();				
			
			
				
				//根据委托回报的状态计算**********************************************************************
				
				TThostFtdcOrderStatusType OrderStatus = 'n';//原委托状态，无委托初始值为n
				
				
				if (OrderMap.count(msg.RtnOrder.OrderRef)>0)
				OrderStatus = OrderMap[msg.RtnOrder.OrderRef].OrderStatus;		
				
				
				//委托状态改变
				if (OrderStatus != msg.RtnOrder.OrderStatus)
				{
					///委托录入 冻结相应的持仓		
					if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_NoTradeQueueing)
					
						PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], msg.RtnOrder.VolumeTotalOriginal);
					//委托撤单 回复相应的持仓	
					
					if (msg.RtnOrder.OrderStatus == THOST_FTDC_OST_Canceled)
					    PositionFrozen(msg.RtnOrder.InstrumentID, msg.RtnOrder.Direction, msg.RtnOrder.CombOffsetFlag[0], -msg.RtnOrder.VolumeTotalOriginal);
				
				}
				
				
				//更新委托列表中的状态
				OrderMap[msg.RtnOrder.OrderRef] = msg.RtnOrder;
				
				//*******************************************************************************************************
				cerr << "委托|编号:" << msg.RtnOrder.OrderSysID << " | 合约 <" << msg.RtnOrder.InstrumentID
					<< "> 委托| 方向(Buy'0' Sell '1'):" << msg.RtnOrder.Direction << "手数：" << msg.RtnOrder.VolumeTotalOriginal
					<< " | 价格：" << msg.RtnOrder.LimitPrice
					<< " | 报单状态：" << msg.RtnOrder.OrderStatus
					<< endl;
				LOG(INFO) << "委托|编号:" << msg.RtnOrder.OrderSysID << " | 合约 <" << msg.RtnOrder.InstrumentID
					<< "> 委托| 方向(Buy'0' Sell '1'):" << msg.RtnOrder.Direction << "手数：" << msg.RtnOrder.VolumeTotalOriginal
					<< " | 价格：" << msg.RtnOrder.LimitPrice
					<< " | 报单状态：" << msg.RtnOrder.OrderStatus
					<< endl;
				LogpInvestorPosition();
				
				
				g_lockqueue.unlock();
				
				
				break;



			};
			// 成交回报
			case RtnTrade:
			{
				g_lockqueue.lock();
				TradeList.push_back(msg.RtnTrade);
				
				PositionChange(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);
				
				PositionFrozen(msg.RtnTrade.InstrumentID, msg.RtnTrade.Direction, msg.RtnTrade.OffsetFlag, msg.RtnTrade.Volume);
				
				LogpInvestorPosition();
				
				g_lockqueue.unlock();

				break;
			};
			//报单录入
			case InputOrder:
			{
				break;

			};
			// 报单操作录入
			case InputOrderAction:
			{
				break;
			}

			}

			//执行策略
			QuotaStrategy();
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
	TThostFtdcBrokerIDType	BROKER_ID ="7080" ;			// 经纪公司代码
	TThostFtdcInvestorIDType INVESTOR_ID = "20104965";//"20105161";//"		// 投资者代码
		TThostFtdcPasswordType  PASSWORD = "112288";//"123456";//;		// 用户密码	

    //LOG
	google::InitGoogleLogging("AutoTrader");  //参数为自己的可执行文件名 		

	google::SetLogDestination(google::GLOG_INFO, "./Log/Auto_Info");

	FLAGS_max_log_size = 50;  //最大日志大小为 50MB
	
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
	
	//std::unique_lock <std::mutex> lck(g_lockqueue);

	//while (!InitFinished) // 如果标志位不为 true, 则等待...
	//
	//	cv.wait(lck); // 当前线程被阻塞, 当全局标志位变为 true 之后 线程继续执行

	//等待系统初始化完成
	while (!InitFinished)
	  Sleep(1000);
	
	//执行策略
	QuotaStrategy();
		
	//自动报单线程	
	std::thread th(mProcess);
	
	th.join();


}




