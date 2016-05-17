#include "MdSpi.h"
#include "common.h"


void CMdSpi::OnRspError(CThostFtdcRspInfoField *pRspInfo,
		int nRequestID, bool bIsLast)
{
	cerr << "--->>> "<< "OnRspError" << endl;
	LOG(INFO) << "--->>> " << "OnRspError" << endl;
	IsErrorRspInfo(pRspInfo);
}

void CMdSpi::OnFrontDisconnected(int nReason)
{
	cerr << "--->>> " << "OnFrontDisconnected" << endl;
	cerr << "--->>> Reason = " << nReason << endl;
	LOG(INFO) << "--->>> " << "OnFrontDisconnected" << endl;
	LOG(INFO) << "--->>> Reason = " << nReason << endl;
}
		
void CMdSpi::OnHeartBeatWarning(int nTimeLapse)
{
	cerr << "--->>> " << "OnHeartBeatWarning" << endl;
	cerr << "--->>> nTimerLapse = " << nTimeLapse << endl;
	LOG(INFO) << "--->>> " << "OnHeartBeatWarning" << endl;
	LOG(INFO) << "--->>> nTimerLapse = " << nTimeLapse << endl;
}

void CMdSpi::OnFrontConnected()
{
	cerr << "--->>> " << "OnFrontConnected" << endl;
	LOG(INFO) << "--->>> " << "OnFrontConnected" << endl;
	///用户登录请求
	ReqUserLogin();
}

void CMdSpi::ReqUserLogin()
{
	//CThostFtdcReqUserLoginField req;
	//memset(&req, 0, sizeof(req));
	////strcpy(req.BrokerID, BROKER_ID);
	////strcpy(req.UserID, INVESTOR_ID);
	////strcpy(req.Password, PASSWORD);

	int iResult = pMDUserApi->ReqUserLogin(&reqLoginField, ++iRequestID);
	cerr << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
	LOG(INFO) << "--->>> 发送用户登录请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
		CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "--->>> " << "OnRspUserLogin" << endl;
	LOG(INFO) << "--->>> " << "OnRspUserLogin" << endl;
	if (bIsLast && !IsErrorRspInfo(pRspInfo))
	{
		///获取当前交易日
		cerr << "--->>> 获取当前交易日 = " << pMDUserApi->GetTradingDay() << endl;

		LOG(INFO) << "--->>> 获取当前交易日 = " << pMDUserApi->GetTradingDay() << endl;		
		// 请求订阅行情
		SubscribeMarketData();	
		// 请求订阅询价,只能订阅郑商所的询价，其他交易所通过traderapi相应接口返回
		SubscribeForQuoteRsp();	
	}
}

void CMdSpi::SubscribeMarketData()
{
	
	////读取配置文件代码
	////std::vector <string> list;

	//ifstream inf;

	//inf.open("instruments.txt");

	//string s;

	//while (getline(inf, s))
	//{
	//	//std::vector <string> vect;

	//	//split(s, ",", vect);

	//	//instrumap.insert(map<string, string>::value_type(vect[0], vect[1]));

	//	//string sss = vect[0];

	//	list.push_back(s);

	//}

	//inf.close();

	unsigned int len = Instrumentlist.size();
	//unsigned int len = Instrumap.count;
	char** pInstId = new char*[len];

	for (unsigned int i = 0; i < len; i++)
		pInstId[i] = (char*)Instrumentlist[i].data();

	//pInstId[i] = (char*)Instrumap[i].data();
	int iResult = pMDUserApi->SubscribeMarketData(pInstId, len);
	
	cerr << "--->>> 发送行情订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
	
}

void CMdSpi::SubscribeForQuoteRsp()
{
	//int iResult = pMDUserApi->SubscribeForQuoteRsp(ppInstrumentID, iInstrumentID);
	//cerr << "--->>> 发送询价订阅请求: " << ((iResult == 0) ? "成功" : "失败") << endl;
}

void CMdSpi::OnRspSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	
  cerr << "OnRspSubMarketData | 订阅：<" << pSpecificInstrument ->InstrumentID<< ">" << endl;
}

void CMdSpi::OnRspSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspSubForQuoteRsp" << endl;
}

void CMdSpi::OnRspUnSubMarketData(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubMarketData" << endl;
}

void CMdSpi::OnRspUnSubForQuoteRsp(CThostFtdcSpecificInstrumentField *pSpecificInstrument, CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast)
{
	cerr << "OnRspUnSubForQuoteRsp" << endl;
}

void CMdSpi::OnRtnDepthMarketData(CThostFtdcDepthMarketDataField *pDepthMarketData)
{	
  
	//行情接收	
	   //g_lockqueue.lock();
	 
	
	CThostFtdcDepthMarketDataField ThostFtdcDepthMarketDataField;
	 
	 ThostFtdcDepthMarketDataField = *pDepthMarketData;
	
	 //MarketDataField.insert(map<string, CThostFtdcDepthMarketDataField>::value_type(pDepthMarketData->InstrumentID, ThostFtdcDepthMarketDataField));
	 
	 MarketDataField[pDepthMarketData->InstrumentID]=ThostFtdcDepthMarketDataField;

	 //if(MarketDataField.second)
	 //{
		// Cout << ”Insert Successfully” << endl;
	 //}
	 //Else
	 //{
		// Cout << ”Insert Failure” << endl;
	 //}
	 //int size = m_dataList.size();
	   //g_lockqueue.unlock();
		
		//g_lockqueue.lock();
		//Redis_dataList.push(*pDepthMarketData);
		//int Redis_size = Redis_dataList.size();
		//g_lockqueue.unlock();	
	 
	 ////买报价
	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "ALLNOT")
		// && (Ask[pDepthMarketData->InstrumentID] <= pDepthMarketData->BidPrice1))
	 //{		

		// InstrumentStatus[pDepthMarketData->InstrumentID]="ASK";

	 //}

	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "BID") && (InstrumentStatus[pDepthMarketData->InstrumentID] == "ASKChange")
		// && (Ask[pDepthMarketData->InstrumentID] <= pDepthMarketData->BidPrice1))
	 //{
		// payoff[pDepthMarketData->InstrumentID] += Ask[pDepthMarketData->InstrumentID] - Bid[pDepthMarketData->InstrumentID];
	

		// InstrumentStatus[pDepthMarketData->InstrumentID] = "NONE";

		// LOG(INFO) << "Payoff<" << pDepthMarketData->InstrumentID << "> | "<<payoff[pDepthMarketData->InstrumentID] << endl;

	 //}
	 ////买报价
	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "ALLNOT")
		// && (Bid[pDepthMarketData->InstrumentID] >= pDepthMarketData->AskPrice1))
	 //{

		// InstrumentStatus[pDepthMarketData->InstrumentID] = "BID";

	 //}
	 //if ((InstrumentStatus[pDepthMarketData->InstrumentID] == "ASK") && (InstrumentStatus[pDepthMarketData->InstrumentID] == "BIDChange")
		// && (Bid[pDepthMarketData->InstrumentID] >= pDepthMarketData->AskPrice1))
	 //{
		// payoff[pDepthMarketData->InstrumentID] += Ask[pDepthMarketData->InstrumentID] - Bid[pDepthMarketData->InstrumentID];

		// LOG(INFO) << "Payoff<" << pDepthMarketData->InstrumentID << "> | " << payoff[pDepthMarketData->InstrumentID] << endl;
		// 
		// InstrumentStatus[pDepthMarketData->InstrumentID] = "NONE";
	 //}	 
	// cerr << "Inst:" << pDepthMarketData->InstrumentID<<" | TIME:" << pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec
	//<< " | LastPrice:" << pDepthMarketData->LastPrice << " | Bid:" << pDepthMarketData->BidPrice1
	//	<< " | Volume:" << pDepthMarketData->BidVolume1 << "" << " | Ask:" << pDepthMarketData->AskPrice1
	//	<< " | Volume:" << pDepthMarketData->AskVolume1 << endl;

	//
	//LOG(INFO) << "Inst:" << pDepthMarketData->InstrumentID <<  " | TIME:" << pDepthMarketData->UpdateTime << "." << pDepthMarketData->UpdateMillisec << endl;
	//LOG(INFO) << " | LastPrice:" << pDepthMarketData->LastPrice << " | Bid:" << pDepthMarketData->BidPrice1
	//	<< " | Volume:" << pDepthMarketData->BidVolume1 << "" << " | Ask:" << pDepthMarketData->AskPrice1
	//	<< " | Volume:" << pDepthMarketData->AskVolume1 << endl; 

}

void CMdSpi::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp)
{
	cerr << "OnRtnForQuoteRsp" << endl;
}

bool CMdSpi::IsErrorRspInfo(CThostFtdcRspInfoField *pRspInfo)
{
	// 如果ErrorID != 0, 说明收到了错误的响应
	bool bResult = ((pRspInfo) && (pRspInfo->ErrorID != 0));
	if (bResult)
		cerr << "--->>> ErrorID=" << pRspInfo->ErrorID << ", ErrorMsg=" << pRspInfo->ErrorMsg << endl;
	return bResult;
}