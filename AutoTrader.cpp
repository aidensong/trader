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


//注意：当字符串为空时，也会返回一个空字符串  
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

///检查可平仓数
int CheckEnClose(string InstrumentID, TThostFtdcDirectionType Direction)
{
	if (Direction == THOST_FTDC_D_Buy)
		return	ShortEnClose[InstrumentID];
	else
		return	LongEnClose[InstrumentID];
  //
//		
//THOST_FTDC_D_Buy 
//		///卖
//THOST_FTDC_D_Sell '1'
}
////报单
//void OrderInsert(TThostFtdcPriceType Price, string InstrumentID, TThostFtdcDirectionType Direction)
//{		
//				//cerr << MarketDataField["TF1606"].LastPrice << endl;
//
//				CThostFtdcInputOrderField req;				
//
//				memset(&req, 0, sizeof(req));
//				///经纪公司代码
//				strcpy(req.BrokerID, BROKER_ID);
//				///投资者代码
//				strcpy(req.InvestorID, INVESTOR_ID);
//				///合约代码
//				strcpy(req.InstrumentID, (char*)InstrumentID.data());
//				
//				iNextOrderRef++;
//				
//				sprintf(ORDER_REF, "%d", iNextOrderRef);
//				
//				///报单引用
//				strcpy(req.OrderRef, ORDER_REF);				
//				
//				///用户代码
//				//	TThostFtdcUserIDType	UserID;
//				///报单价格条件: 限价
//				req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
//				///买卖方向: 
//				req.Direction = Direction;			
//				
//				if (CheckEnClose(InstrumentID, Direction) > 0)
//				{///组合开平标志: 开仓
//					req.CombOffsetFlag[0] = THOST_FTDC_OF_Close;//THOST_FTDC_OF_Open;
//				}else
//					///组合开平标志: 平仓
//					req.CombOffsetFlag[0] = THOST_FTDC_OF_Open;
//				
//				
//				///组合投机套保标志
//				req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
//				
//				///价格
//				req.LimitPrice = 4000;//Price;
//				
//				///数量: 1
//				req.VolumeTotalOriginal = 1;
//				///有效期类型: 当日有效
//				req.TimeCondition = THOST_FTDC_TC_GFD;
//				///GTD日期
//				//	TThostFtdcDateType	GTDDate;
//				///成交量类型: 任何数量
//				req.VolumeCondition = THOST_FTDC_VC_AV;
//				///最小成交量: 1
//				req.MinVolume = 1;
//				///触发条件: 立即
//				req.ContingentCondition = THOST_FTDC_CC_Immediately;
//				///止损价
//				//	TThostFtdcPriceType	StopPrice;
//				///强平原因: 非强平
//				req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
//				///自动挂起标志: 否
//				req.IsAutoSuspend = 0;
//				///业务单元
//				//	TThostFtdcBusinessUnitType	BusinessUnit;
//				///请求编号
//				//	TThostFtdcRequestIDType	RequestID;
//				///用户强评标志: 否
//				req.UserForceClose = 0;//				
//				
//				int iResult = pTraderUserApi->ReqOrderInsert(&req, ++iRequestID);
//				
//				
//			/*	if (Direction == THOST_FTDC_D_Buy)
//				{
//					cerr << "--->>> 买入<" << InstrumentID << ">报单|价格：" << Price << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//					LOG(INFO) << "--->>> 买入<" << InstrumentID << ">报单|价格：" << Price << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//				}
//				else
//				{
//					cerr << "--->>> 卖出<" << InstrumentID << ">报单|价格：" << Price << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//					LOG(INFO)<< "--->>> 卖出<" << InstrumentID << ">报单|价格：" << Price << "录入请求: " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//				
//				}*/
//				
//}
//
//
/////撤单
//void ReqOrderCancel(string OrderRef, string InstrumentID, TThostFtdcDirectionType Direction, TThostFtdcPriceType Price)
//{
//		//static bool ORDER_ACTION_SENT = false;		//是否发送了报单
//	
//		//if (ORDER_ACTION_SENT)
//		//	return;
//	
//		CThostFtdcInputOrderActionField req;
//
//		memset(&req, 0, sizeof(req));
//		///经纪公司代码
//		strcpy(req.BrokerID, BROKER_ID);
//		///投资者代码
//		strcpy(req.InvestorID, INVESTOR_ID);
//		///报单操作引用
//	//	TThostFtdcOrderActionRefType	OrderActionRef;
//		///报单引用
//	    strcpy(req.OrderRef, (char*)OrderRef.data());
//		///请求编号
//	//	TThostFtdcRequestIDType	RequestID;
//		///前置编号
//		req.FrontID = FRONT_ID;
//		///会话编号
//		req.SessionID = SESSION_ID;	
//		
//		///交易所代码
//	//	TThostFtdcExchangeIDType	ExchangeID;
//		///报单编号
//	//	TThostFtdcOrderSysIDType	OrderSysID;
//	
//		///操作标志
//		req.ActionFlag = THOST_FTDC_AF_Delete;
//		
//		///价格
//	
//		//	TThostFtdcPriceType	LimitPrice;
//		///数量变化
//	//	TThostFtdcVolumeType	VolumeChange;
//		///用户代码
//	//	TThostFtdcUserIDType	UserID;
//		///合约代码
//	
//		strcpy(req.InstrumentID, (char*)InstrumentID.data());
//		
//		//int iResult = pTraderUserApi->ReqOrderAction(&req, ++iRequestID);
//
//	 
//		//if (Direction == THOST_FTDC_D_Buy)
//		//{
//		//	cerr << "--->>> 买入<" << InstrumentID << ">报单| 价格："<<Price<<" | 报入请求: 取消" << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//		//	LOG(INFO) << "--->>> 买入<" << InstrumentID << ">报单 |价格 ：" << Price << " | |报入请求: 取消" << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//		//}
//		//else
//		//{
//		//	cerr << "--->>> 卖出<" << InstrumentID << ">报单| 价格：" << Price << " | 报入请求:取消 " << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//		//	LOG(INFO) << "--->>> 卖出<" << InstrumentID << ">报单| 价格：" << Price << " | 报入请求: 取消" << iResult << ((iResult == 0) ? ", 成功" : ", 失败") << endl;
//
//		//}///ORDER_ACTION_SENT = true;
//}


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

				//OrderInsert(MarketDataField[InstrumentID].BidPrice1 - spreed / 2, InstrumentID, THOST_FTDC_D_Buy);
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
	char MD_FRONT_ADDR[] = "tcp://172.16.100.225:41213"; // MD配置参数
	//"tcp://210.5.151.247:41213"; // 		// 前置地址
	// Trader配置参数
	char  TRADER_FRONT_ADDR[] = "tcp://172.16.100.225:41205";	// 前置地址
	TThostFtdcBrokerIDType	BROKER_ID = "7080";			// 经纪公司代码
	TThostFtdcInvestorIDType INVESTOR_ID = "20104965";		// 投资者代码
	TThostFtdcPasswordType  PASSWORD = "112288";		// 用户密码	
	
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




