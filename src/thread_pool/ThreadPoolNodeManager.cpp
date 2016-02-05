/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolNodeManager.cpp
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

#include "ThreadPoolNodeManager.h"
#include "ThreadPoolNode.h"

CThreadPoolNodeManager::CThreadPoolNodeManager()
{

}

CThreadPoolNodeManager::~CThreadPoolNodeManager()
{

}

unsigned int CThreadPoolNodeManager::init()
{
	return 0;
}

unsigned int CThreadPoolNodeManager::start()
{
	return 0;
}
	
unsigned int CThreadPoolNodeManager::stop()
{
	return 0;
}

unsigned int CThreadPoolNodeManager::pause()
{
	return 0;
}

unsigned int CThreadPoolNodeManager::add_node(const char* thread_tag, unsigned int thread_count)
{
	std::string thread_flag(thread_tag);
	NODE_MAP::iterator node_it = m_node_map.find(thread_flag);
	if (node_it != m_node_map.end())
	{//已经存在
		return 0;
	}
	
	IThreadPoolNode* thread_node = new(std::nothrow)CThreadPoolNode();
	if (thread_node == nullptr)
	{
		return -1;
	}
	if (m_node_map.insert(NODE_MAP::value_type(thread_flag, std::shared_ptr<IThreadPoolNode>(thread_node))).second)
	{
		thread_node->init(thread_count);
		return 0;
	}
	return -1;
}



unsigned int CThreadPoolNodeManager::register_task(const char* thread_tag, IThreadTask* task, bool auto_release)
{
	NODE_MAP::iterator node_map_it = m_node_map.find(std::string(thread_tag));
	if (node_map_it == m_node_map.end())
	{
		return -1;
	}
	
	if (!node_map_it->second)
	{
		return -1;
	}

	return node_map_it->second->register_task(task, auto_release);
}





