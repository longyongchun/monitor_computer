/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolManager.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/06
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************


#ifndef THREAD_POOL_MANAGER_HPP__
#define THREAD_POOL_MANAGER_HPP__

#include "IThreadPoolManager.h"
#include <unordered_map>
#include <string>
#include <memory>
class IThreadPoolNodeManager;

class CThreadPoolManager : public IThreadPoolManager
{
public:
	CThreadPoolManager();
	virtual ~CThreadPoolManager();

public:
	virtual unsigned int init();
	virtual unsigned int start();
	virtual unsigned int create_thread_node_mgr(const char* node_mgr_flag);
	virtual unsigned int create_thread_node(const char* node_mgr_flag, const char* node_flag, unsigned int thread_count);
	virtual unsigned int register_thread_task(const char* node_mgr_flag, const char* node_flag, IThreadTask* task, bool auto_realse);

public:
	virtual unsigned int destroy_thread_node_mgr(const char* node_mgr_flag);
	virtual unsigned int destroy_thread_node(const char* node_mgr_flag, const char* node_flag);

private:
	typedef std::unordered_map<std::string, std::shared_ptr<IThreadPoolNodeManager> > NODE_MGR_MAP;
	NODE_MGR_MAP m_node_mgr_map;
};


#endif

