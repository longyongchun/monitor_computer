/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadPoolFactory.h
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


#ifndef THREAD_POOL_FACTORY_HPP__
#define THREAD_POOL_FACTORY_HPP__

#include <memory>
class IThreadPoolManager;

class CThreadPoolFactory
{
protected:
	CThreadPoolFactory();

public:
	~CThreadPoolFactory();
	CThreadPoolFactory(const CThreadPoolFactory&) = delete;
	CThreadPoolFactory& operator=(const CThreadPoolFactory&) = delete;

public:
	static IThreadPoolManager* get_thread_pool(unsigned int tag);

private:
	std::shared_ptr<IThreadPoolManager> m_thread_pool;

	static CThreadPoolFactory m_thread_factory;	
};

#define THREAD_POOL CThreadPoolFactory::get_thread_pool(0) 


#endif




