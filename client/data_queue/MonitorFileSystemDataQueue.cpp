/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorFileSystemDataQueue.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/15
/// @note
/// <author>        <time>          <version >          <desc> \n
///
// ****************************************************************************

#include "MonitorFileSystemDataQueue.h"
#include "../NetworkPack.h"
#include <thread>
CMonitorFileSystemDataQueue* CMonitorFileSystemDataQueue::m_file_system_queue = nullptr;

CMonitorFileSystemDataQueue* CMonitorFileSystemDataQueue::get_monitor_queue_instance()
{
	static std::once_flag flag;
	if (m_file_system_queue == nullptr)
	{
		std::call_once(flag, [&](){m_file_system_queue = new (std::nothrow)CMonitorFileSystemDataQueue();});	
	}
	return m_file_system_queue;
}

CMonitorFileSystemDataQueue::CMonitorFileSystemDataQueue()
{
	m_network_pack_queue.reset(new (std::nothrow)CMonitorFileSystemNetworkPackQueue(5));
}
	
CMonitorFileSystemDataQueue::~CMonitorFileSystemDataQueue()
{

}

bool CMonitorFileSystemDataQueue::push_network_pack(std::shared_ptr<MonitorDataPack> data_pack)
{
	return m_network_pack_queue && m_network_pack_queue->push(data_pack);
}

bool CMonitorFileSystemDataQueue::pop_network_pack(std::shared_ptr<MonitorDataPack>& data_pack)
{
	return m_network_pack_queue && m_network_pack_queue->pop(data_pack);
}



CMonitorFileSystemNetworkPackQueue::CMonitorFileSystemNetworkPackQueue(int merge_time)
		:m_merge_interval_time(merge_time)
{

}
	
CMonitorFileSystemNetworkPackQueue::~CMonitorFileSystemNetworkPackQueue()
{

}

bool CMonitorFileSystemNetworkPackQueue::push(std::shared_ptr<MonitorDataPack> data_pack)
{
	boost::unique_lock<boost::shared_mutex> write_lock(m_pre_data_queue_shared_mutex);
	return m_pre_data_queue.push_data(data_pack);
}
	
bool CMonitorFileSystemNetworkPackQueue::pop(std::shared_ptr<MonitorDataPack>& data_pack)
{	
	boost::unique_lock<boost::shared_mutex> write_lock(m_pre_data_queue_shared_mutex);
	//暂时不去重
	return m_pre_data_queue.pop_data(data_pack);
//	return m_formal_data_queue.pop_data(data_pack);
}







