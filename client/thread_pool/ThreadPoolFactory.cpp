/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolFactory.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/14
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************
#include "ThreadPoolFactory.h"
#include "ThreadPoolManager.h"

CThreadPoolFactory CThreadPoolFactory::m_thread_factory;

CThreadPoolFactory::CThreadPoolFactory()
{
	m_thread_pool.reset(new(std::nothrow)CThreadPoolManager());
}
	
CThreadPoolFactory::~CThreadPoolFactory()
{

}

IThreadPoolManager* CThreadPoolFactory::get_thread_pool(unsigned int tag)
{
	return m_thread_factory.m_thread_pool.get();
}


