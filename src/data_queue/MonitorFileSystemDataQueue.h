/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorFileSystemDataQueue.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/15
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************


#ifndef MONITOR_FILE_SYSTEM_DATA_QUEUE_HPP__
#define MONITOR_FILE_SYSTEM_DATA_QUEUE_HPP__

#include <memory>
#include <boost/thread/thread.hpp>
#include "DataQueue.hpp"
#include <unordered_set>
class CMonitorFileSystemNetworkPackQueue;
struct MonitorDataPack;

class CMonitorFileSystemDataQueue
{
private:
	CMonitorFileSystemDataQueue();
	CMonitorFileSystemDataQueue(const CMonitorFileSystemDataQueue& qu) = delete;
	CMonitorFileSystemDataQueue& operator=(const CMonitorFileSystemDataQueue& qu) = delete;
public:
	~CMonitorFileSystemDataQueue();

public:
	static CMonitorFileSystemDataQueue* get_monitor_queue_instance();

public:
	bool push_network_pack(std::shared_ptr<MonitorDataPack> data_pack);
	bool pop_network_pack(std::shared_ptr<MonitorDataPack>& data_pack);

private:
	static CMonitorFileSystemDataQueue* m_file_system_queue;

private:
	std::shared_ptr<CMonitorFileSystemNetworkPackQueue> m_network_pack_queue;
};


class CMonitorFileSystemNetworkPackQueue
{
public:
	CMonitorFileSystemNetworkPackQueue(int merge_time);
	~CMonitorFileSystemNetworkPackQueue();

public:
	bool push(std::shared_ptr<MonitorDataPack> data_pack);
	bool pop(std::shared_ptr<MonitorDataPack>& data_pack);

private:
	const int m_merge_interval_time;                  //合并数据间隔时间 单位秒
	boost::shared_mutex							  m_pre_data_queue_shared_mutex;
	CDataQueue<std::shared_ptr<MonitorDataPack> > m_pre_data_queue;     //预处理数据 每m_merge_interval_time合并成正式数据

	boost::shared_mutex							  m_formal_data_queue_shared_mutex;
	CDataQueue<std::shared_ptr<MonitorDataPack> > m_formal_data_queue;  //正式数据队列

	std::unordered_set<std::shared_ptr<MonitorDataPack> > m_tmp_data_queue;
};


#define MONITOR_DATA_QUEUE CMonitorFileSystemDataQueue::get_monitor_queue_instance()

#endif




