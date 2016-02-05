/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolNodeManager.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/07
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#ifndef THREAD_POOL_NODE_MANAGER_HPP__
#define THREAD_POOL_NODE_MANAGER_HPP__
#include "IThreadPoolNodeManager.h"
#include <unordered_map>
#include <string>
#include <memory>
class CThreadPoolNodeManager : public IThreadPoolNodeManager
{
public:
	CThreadPoolNodeManager();
	virtual ~CThreadPoolNodeManager();

public:
	virtual unsigned int init();
	virtual unsigned int start();
	virtual unsigned int stop();
	virtual unsigned int pause();

	virtual unsigned int add_node(const char* thread_tag, unsigned int thread_count);
	virtual unsigned int register_task(const char* thread_tag, IThreadTask* task, bool auto_release);


private:
	typedef std::unordered_map<std::string, std::shared_ptr<IThreadPoolNode> > NODE_MAP;
	NODE_MAP m_node_map;

};


#endif


