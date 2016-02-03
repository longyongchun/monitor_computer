/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadTask.h
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

#ifndef THREAD_TASK_HPP__
#define THREAD_TASK_HPP__

#include "IThreadPoolManager.h"
#include <memory>

class IThreadFunction
{
public:
	virtual ~IThreadFunction(){};
	
public:
	virtual unsigned int run() = 0;
};

template <typename FUN>
class CThreadFunction : public IThreadFunction
{
public:
	CThreadFunction(FUN fun)
		:m_fun(fun)
	{
		
	}

public:
	virtual unsigned int run()
	{
		m_fun();
		return 0;
	}

private:
	FUN m_fun;
};


class CThreadTask : public IThreadTask
{
public:
	CThreadTask();
	virtual ~CThreadTask();

public:
	virtual unsigned int run();

public:
	template <typename FUN>
	unsigned int reset(FUN fun)
	{
		IThreadFunction* fun_ptr = new (std::nothrow)CThreadFunction<FUN>(fun);
		if (fun_ptr == nullptr)
		{
			return -1; 
		}
		m_fun_ptr.reset(fun_ptr);
		return 0;
	}

private:
	std::shared_ptr<IThreadFunction> m_fun_ptr;
};


#endif

