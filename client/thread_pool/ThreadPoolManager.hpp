/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolManager.hpp
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

#ifndef THREAD_POOL_MANAGER_HPP_
#define THREAD_POOL_MANAGER_HPP_

#include "IThreadPoolNodeManager.h"
#include <memory>
class CThreadPoolManager
{
public:
	CThreadPoolManager();
	~CThreadPoolManager();

public:
	unsigned int init();

private:
	std::shared_ptr<IThreadPoolNodeManager> m_thread_pool_node_mgr_ptr;		

};

#endif



