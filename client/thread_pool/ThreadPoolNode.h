/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolNode.h
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


#ifndef THREAD_POOL_NODE_HPP__
#define THREAD_POOL_NODE_HPP__
//此句必须定义在thread.hpp前面
#include <boost/thread.hpp>
#include "IThreadPoolNode.h"
#include <memory>
#include <queue>
#include <vector>
#include <thread>
#include <atomic>
#include <unordered_map>

class IThreadTask;

class CThreadPoolNode : public IThreadPoolNode
{
public:
	CThreadPoolNode();
	virtual ~CThreadPoolNode();

public:
	virtual unsigned int init(unsigned int thread_count);
	virtual unsigned int register_task(IThreadTask* task, bool auto_release);
	virtual unsigned int start();
	virtual unsigned int stop();
	virtual unsigned int pause();

protected:
	struct TaskPack
	{
		unsigned int                 m_task_id;      //任务ID
		std::shared_ptr<IThreadTask> m_thread_task;  //任务
	};

private:
	unsigned int thread_function();
	std::shared_ptr<TaskPack> create_task_pack(std::shared_ptr<IThreadTask> task);
	std::shared_ptr<TaskPack> create_task_pack(IThreadTask* task, bool auto_release);

	std::shared_ptr<TaskPack> pop_task_from_wait_queue();
	unsigned int insert_task_to_executing_map(std::shared_ptr<TaskPack> task);
	std::shared_ptr<TaskPack> pop_task_from_executing_map(unsigned int task_id);

	
private:
	boost::shared_mutex						  	m_wait_shared_mutex;     //等待执行的任务操作的读写锁
	std::queue<std::shared_ptr<TaskPack> > 		m_wait_task_queue;       //等待执行的任务队列 

	boost::shared_mutex                       					m_executing_shared_mutex; //正在执行的任务操作读写锁	
	std::unordered_map<int, std::shared_ptr<TaskPack> >         m_executing_task_map;   //正在执行的任务队列

	boost::shared_mutex                        m_thread_shared_mutex;   //操作线程容器读写锁
	std::vector<std::shared_ptr<std::thread> > m_thread_container;

	std::atomic<unsigned int>                  m_task_id;
};


#endif


