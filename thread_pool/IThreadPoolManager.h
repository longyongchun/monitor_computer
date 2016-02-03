/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IThreadPoolManager.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2015/12/29
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************


#ifndef ITHREAD_POOL_MANAGER_HPP__
#define ITHREAD_POOL_MANAGER_HPP__

class IThreadTask
{
public:
	virtual ~IThreadTask(){}

public:
	virtual unsigned int run() = 0;
};



class IThreadPoolManager
{
public:
	virtual ~IThreadPoolManager(){}

public:
	virtual unsigned int init() = 0;
	virtual unsigned int start() = 0;
	virtual unsigned int create_thread_node_mgr(const char* node_mgr_flag) = 0;
	virtual unsigned int create_thread_node(const char* node_mgr_flag, const char* node_flag, unsigned int thread_count) = 0;
	virtual unsigned int register_thread_task(const char* node_mgr_flag, const char* node_flag, IThreadTask* task, bool auto_realse) = 0;

public:
	virtual unsigned int destroy_thread_node_mgr(const char* node_mgr_flag) = 0;
	virtual unsigned int destroy_thread_node(const char* node_mgr_flag, const char* node_flag) = 0;
};





#endif

