/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           Business.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/02/03
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************
#include "Business.h"
#include <thread>
#include <chrono>
#include "MonitorFileSystemBusiness.h"
#include "network/NetworkServiceBusiness.h"
#include "config/GolabConfig.h"
CBusiness::CBusiness()
{

}

CBusiness::~CBusiness()
{

}

unsigned int CBusiness::init()
{
	//初始化全局配置
	GOLAB_CONFIG->init();
	

	IBusiness* business = nullptr;
	//本地监控业务
	business = new (std::nothrow)CMonitorFileSystemBusiness();
	if (business != nullptr)
	{
		business->init();
		m_business_list.push_back(std::shared_ptr<IBusiness>(business));
	}
	else
	{//

	}

	//网络业务
	business = new (std::nothrow)CNetworkServiceBusiness();
	if (business != nullptr)
	{
		business->init();
		m_business_list.push_back(std::shared_ptr<IBusiness>(business));
	}
	else
	{//

	}
	
	return 0;

}

unsigned int CBusiness::start()
{
	//启动所有业务
	for (auto& business : m_business_list)
	{
		if (business->start() == 0xffffffff)
		{
			return -1;
		}
	}
	
	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(100000));
	}
	return 0;
}








