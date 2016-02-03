/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolManager.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/13
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#include "ThreadPoolManager.h"
#include "ThreadPoolNodeManager.h"

CThreadPoolManager::CThreadPoolManager()
{

}

CThreadPoolManager::~CThreadPoolManager()
{

}

unsigned int CThreadPoolManager::init()
{
	return 0;
}

unsigned int CThreadPoolManager::start()
{
	return 0;
}

unsigned int CThreadPoolManager::create_thread_node_mgr(const char* node_mgr_flag)
{
	std::string mgr_flag(node_mgr_flag);
	if (m_node_mgr_map.find(mgr_flag) != m_node_mgr_map.end())
	{//存在相应的节点 返回成功
		return 0;
	}
	IThreadPoolNodeManager* thread_pool_node_mgr = new(std::nothrow)CThreadPoolNodeManager();
	if (thread_pool_node_mgr == nullptr)
	{
		return -1;
	}
	if (m_node_mgr_map.insert(NODE_MGR_MAP::value_type(mgr_flag, std::shared_ptr<IThreadPoolNodeManager>(thread_pool_node_mgr))).second)
	{
		return 0;
	}
	return -1;
}

unsigned int CThreadPoolManager::create_thread_node(const char* node_mgr_flag, const char* node_flag, unsigned int thread_count)
{
	NODE_MGR_MAP::iterator node_mgr_it = m_node_mgr_map.find(std::string(node_mgr_flag));
	if (node_mgr_it == m_node_mgr_map.end())
	{//不存在相应的线程池节点
		return -1;
	}
	if (!node_mgr_it->second)
	{
		return -1;
	}

	return node_mgr_it->second->add_node(node_flag, thread_count);
}

unsigned int CThreadPoolManager::register_thread_task(const char* node_mgr_flag, const char* node_flag, IThreadTask* task, bool auto_realse)
{
	NODE_MGR_MAP::iterator node_mgr_it = m_node_mgr_map.find(std::string(node_mgr_flag));
	if (node_mgr_it == m_node_mgr_map.end())
	{
		return -1;
	}

	if (!node_mgr_it->second)
	{
		return -1;
	}

	return node_mgr_it->second->register_task(node_flag, task, auto_realse);
}

unsigned int CThreadPoolManager::destroy_thread_node_mgr(const char* node_mgr_flag)
{
	return 0;
}

unsigned int CThreadPoolManager::destroy_thread_node(const char* node_mgr_flag, const char* node_flag)
{
	return 0;
}



