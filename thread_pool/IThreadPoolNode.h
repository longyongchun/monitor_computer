/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IThreadPoolNode.h
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

#ifndef ITHREAD_POOL_NODE_HPP__
#define ITHREAD_POOL_NODE_HPP__

class IThreadTask;

class IThreadPoolNode
{
public:
	virtual ~IThreadPoolNode(){}

public:
	virtual unsigned int init(unsigned int thread_count) = 0;
	virtual unsigned int register_task(IThreadTask* task, bool auto_release) = 0;
	
	virtual unsigned int start() = 0;

	virtual unsigned int stop() = 0;

	virtual unsigned int pause() = 0;

};


#endif






