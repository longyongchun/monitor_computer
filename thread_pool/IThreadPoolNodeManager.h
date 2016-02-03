/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IThreadPoolNodeManager.h
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


#ifndef ITHREAD_POOL_NODE_MANAGER_HPP_
#define ITHREAD_POOL_NODE_MANAGER_HPP_

#include "IThreadPoolNode.h"

class IThreadPoolNodeManager
{
public:
	virtual ~IThreadPoolNodeManager(){}

public:
	virtual unsigned int init() = 0;
	virtual unsigned int start() = 0;
	virtual unsigned int stop() = 0;
	virtual unsigned int pause() = 0;

	virtual unsigned int add_node(const char* thread_tag, unsigned int thread_count) = 0;
	virtual unsigned int register_task(const char* thread_tag, IThreadTask* task, bool auto_release) = 0;

};


#endif



