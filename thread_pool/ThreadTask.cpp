/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ThreadTask.cpp
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
#include "ThreadTask.h"

CThreadTask::CThreadTask()
{

}

CThreadTask::~CThreadTask()
{

}

unsigned int CThreadTask::run()
{
	unsigned int run_ret = -1;
	if (m_fun_ptr)
	{
		run_ret = m_fun_ptr->run();
	}
	return run_ret;
}
