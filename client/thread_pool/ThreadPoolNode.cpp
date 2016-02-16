/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolNode.cpp
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
#include "ThreadPoolNode.h"
#include <unistd.h>
#include <iostream>
#include "IThreadPoolManager.h"

CThreadPoolNode::CThreadPoolNode()
{
	m_task_id = 0;
}

CThreadPoolNode::~CThreadPoolNode()
{
	//释放线程 防止线程节点退出时 发生异常而退出
	std::for_each(m_thread_container.begin(), m_thread_container.end(), 
				[](std::shared_ptr<std::thread>& thread_ptr){thread_ptr->detach();});
}

unsigned int CThreadPoolNode::init(unsigned int thread_count)
{
	{
		boost::unique_lock<boost::shared_mutex> write_lock(m_thread_shared_mutex);
		if (m_thread_container.empty())
		{
			m_thread_container.reserve(thread_count);
			for (int i = 0; i < thread_count; ++i)
			{
				std::thread* thread_cell = new (std::nothrow)std::thread(std::bind(&CThreadPoolNode::thread_function, this));
				if (thread_cell == nullptr)
				{
					continue;
				}
				m_thread_container.push_back(std::shared_ptr<std::thread>(thread_cell));
			}
		}
	}

	return 0;
}

unsigned int CThreadPoolNode::register_task(IThreadTask* task, bool auto_release)
{
	if (task == nullptr)
	{
		return -1;
	}
	
	std::shared_ptr<TaskPack> task_pack = this->create_task_pack(task, auto_release);
	if (task_pack)
	{
		boost::unique_lock<boost::shared_mutex> write_lock(m_wait_shared_mutex);
		m_wait_task_queue.push(task_pack);
		return task_pack->m_task_id;
	}

	return -1;
}

unsigned int CThreadPoolNode::start()
{
	return 0;
}

unsigned int CThreadPoolNode::stop()
{
	return 0;
}
	
unsigned int CThreadPoolNode::pause()
{
	return 0;
}


unsigned int CThreadPoolNode::thread_function()
{
	while (true)
	{
		//1. 从wait队列中获取需要执行的任务
		std::shared_ptr<TaskPack> task = this->pop_task_from_wait_queue();
		if (!task || !task->m_thread_task)
		{//没有需要执行的任务 睡眠等待
			sleep(2);
			continue;
		}
		//2. 将任务加入到正在执行的队列中
		this->insert_task_to_executing_map(task);
		task->m_thread_task->run();
		//3. 弹出正在执行的任务
		this->pop_task_from_executing_map(task->m_task_id);	
	}
	return 0;
}


std::shared_ptr<CThreadPoolNode::TaskPack> CThreadPoolNode::pop_task_from_wait_queue()
{
	std::shared_ptr<TaskPack> task;
	{
		boost::unique_lock<boost::shared_mutex> write_lock(m_wait_shared_mutex);
		if (!m_wait_task_queue.empty())
		{
			//1. 获取队首的任务
			task = m_wait_task_queue.front();
			//2. 弹出队首的元素
			m_wait_task_queue.pop();
		}
	}
	return task;
}


unsigned int CThreadPoolNode::insert_task_to_executing_map(std::shared_ptr<TaskPack> task)
{
	if (task)
	{
		boost::unique_lock<boost::shared_mutex> write_lock(m_executing_shared_mutex);
		if (m_executing_task_map.insert(std::unordered_map<int, std::shared_ptr<TaskPack> >::value_type(task->m_task_id, task)).second)
		{
			return 0;
		}
	}
	return -1;
}

std::shared_ptr<CThreadPoolNode::TaskPack> CThreadPoolNode::pop_task_from_executing_map(unsigned int task_id)
{
	std::shared_ptr<TaskPack> task;

	{
		boost::unique_lock<boost::shared_mutex> write_lock(m_executing_shared_mutex);
		if (m_executing_task_map.find(task_id) != m_executing_task_map.end())
		{
			task = m_executing_task_map[task_id];
			m_executing_task_map.erase(task_id);
		}
	}
	
	return task;
}


std::shared_ptr<CThreadPoolNode::TaskPack> CThreadPoolNode::create_task_pack(std::shared_ptr<IThreadTask> task)
{
	std::shared_ptr<TaskPack> task_ptr;
	if (task)
	{
		TaskPack* task_pack = new (std::nothrow)TaskPack;
		if (task_pack == nullptr)
		{
			return task_ptr;
		}
		task_pack->m_thread_task = task;
		task_pack->m_task_id = ++m_task_id;
		task_ptr.reset(task_pack);
	}
	return task_ptr;
}

std::shared_ptr<CThreadPoolNode::TaskPack> CThreadPoolNode::create_task_pack(IThreadTask* task, bool auto_release)
{
	std::shared_ptr<IThreadTask> task_ptr;
	do
	{
		if (task == nullptr)
		{
			break;
		}
		if (auto_release)
		{
			task_ptr.reset(task);
		}
		else
		{
			task_ptr.reset(task, [](IThreadTask*){});
		}
	}while (false);

	return this->create_task_pack(task_ptr);
}

