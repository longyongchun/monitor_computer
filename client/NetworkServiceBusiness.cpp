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
#include "network/NetworkService.h"
#include <thread>
#include "thread_pool/ThreadPoolFactory.h"
#include "thread_pool/ThreadPool.h"
#include "data_queue/MonitorFileSystemDataQueue.h"
#include "NetworkPack.h"
#include "../public/level/MonitorDataStore.h"
#include "../public/log_system/LogManagerSystem.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
#include "../public/level/MonitorDataStore.h"
CNetworkServiceBusiness::CNetworkServiceBusiness()
		:m_node_mgr_tag("Network_Business_Node_Mgr"),
		m_node_tag("Network_Business_Node")
{
	m_net_service.reset(new (std::nothrow)CNetworkService());
	m_mon_data_store.reset(new (std::nothrow)CMonitorDataStore());
}

CNetworkServiceBusiness::~CNetworkServiceBusiness()
{

}

unsigned int CNetworkServiceBusiness::init()
{
	if (m_mon_data_store)
	{
		m_mon_data_store->init("test");
	}

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
			while (this->send_data_task(conn_id) == 0)
			{
				;
			}
			//任务发送完毕 睡一秒
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};

	start_task->reset(std::bind(lambda_fun, conn_id));
	THREAD_POOL->register_thread_task(m_node_mgr_tag.c_str(), m_node_tag.c_str(), start_task, true);
	return 0;
}
	

int CNetworkServiceBusiness::send_data_task(unsigned int conn_id)
{
	int result = -1;
	std::shared_ptr<MonitorDataPack> mon_pack;
	if (MONITOR_DATA_QUEUE->pop_network_pack(mon_pack))
	{
		if (mon_pack)
		{//获取包成功
			result = 0;
			char network_data[1024] = {0};
			int network_data_len = this->build_monitor_network_data(network_data, sizeof(network_data), mon_pack);
			if (network_data_len >= sizeof(NetworkHeadPack))
			{//发送的数据最小单位为一个包头
				if (m_net_service->send_data(conn_id, network_data, network_data_len) == 0xffffffff)
				{//发送网络包失败
					WRITE_ERROR_LOG("发送网络包失败,长度为[%d]", network_data_len);
					//写leveldb
					m_mon_data_store->put(this->get_leveldb_key(mon_pack).c_str(), network_data, network_data_len);					
				}
			}
		}
	}
	return result;
}

std::string CNetworkServiceBusiness::get_leveldb_key(const std::shared_ptr<MonitorDataPack>& data_pack)const
{
	static boost::uuids::random_generator rgn;
	//生成唯一标识码
	boost::uuids::uuid u = rgn();
	//生成的键格式为 mon_data_yyyymmddhhmmss_02d_uuid
	std::string key(43, '\0');
	char op_date[15] = {0};
	memmove(op_date, data_pack->m_op_date, sizeof(data_pack->m_op_date));
	snprintf(&key[0], key.size(), "mon_data_%s_%02d_%s", op_date, 
				data_pack->m_head.m_event_id, u.data);
	return std::move(key);
}

int CNetworkServiceBusiness::build_monitor_network_data(char* buf, unsigned int buf_len, const std::shared_ptr<MonitorDataPack>& data_pack)const
{
	if (data_pack)
	{
		if (data_pack->m_head.m_total_len + sizeof(NetworkHeadPack) > buf_len)
		{
			return 0;
		}
		//总长度
		*((unsigned int*)buf) = data_pack->m_head.m_total_len + sizeof(NetworkHeadPack);
		buf += sizeof(unsigned int);
		//网络事件
		*((unsigned int*)buf) = 0;
		buf += sizeof(unsigned int);
		//序号
		*((unsigned int*)buf) = 0;
		buf += sizeof(unsigned int);
		//监控包
		memmove(buf, (void*)data_pack.get(), data_pack->m_head.m_total_len);
	}

	return data_pack->m_head.m_total_len + sizeof(NetworkHeadPack);
}








