/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           NetworkServiceBusiness.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/02/02
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#include "NetworkServiceBusiness.h"
#include "NetworkService.h"
#include <thread>
#include "../thread_pool/ThreadPoolFactory.h"
#include "../thread_pool/ThreadPool.h"

CNetworkServiceBusiness::CNetworkServiceBusiness()
		:m_node_mgr_tag("Network_Business_Node_Mgr"),
		m_node_tag("Network_Business_Node")
{
	m_net_service.reset(new (std::nothrow)CNetworkService());
}

CNetworkServiceBusiness::~CNetworkServiceBusiness()
{

}

unsigned int CNetworkServiceBusiness::init()
{
	if (m_net_service)
	{
		unsigned int result = m_net_service->init();
		if (result != 0xffffffff)
		{
			if (0xffffffff != (result = THREAD_POOL->create_thread_node_mgr(m_node_mgr_tag.c_str())))
			{
				result = THREAD_POOL->create_thread_node(m_node_mgr_tag.c_str(), m_node_tag.c_str(), 1);
			}
		}
		return result;
	}
	return -1;
}

unsigned int CNetworkServiceBusiness::start()
{
	if (m_net_service)
	{
		unsigned int result = m_net_service->start();
		if (result != 0xffffffff)
		{
			unsigned int conn_id = m_net_service->connect("127.0.0.1", 13140);
			if (conn_id != 0xffffffff)
			{
				this->register_send_data_task(conn_id);
			}
		}	
		return result;
	}
	return -1;
}


int CNetworkServiceBusiness::register_send_data_task(unsigned int conn_id)
{
	CThreadTask* start_task = new (std::nothrow)CThreadTask();
	if (start_task == nullptr)
	{
		return -1;
	}
	auto lambda_fun = [&](unsigned int conn_id)
	{
		while (true)
		{
			std::this_thread::sleep_for(std::chrono::seconds(1));
			std::cout << "hello " << std::endl;
			m_net_service->send_data(conn_id, "hello", strlen("hello"));		
		}
	};

	start_task->reset(std::bind(lambda_fun, conn_id));
	THREAD_POOL->register_thread_task(m_node_mgr_tag.c_str(), m_node_tag.c_str(), start_task, true);
	return 0;
}
	

