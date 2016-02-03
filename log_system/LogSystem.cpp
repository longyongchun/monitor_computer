/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           CLogSystem.cpp
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

#include "LogSystem.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <mutex>
#include <boost/filesystem.hpp>

ILogSystem* CLogSystem::m_log_system = nullptr;

ILogSystem* CLogSystem::get_instance()
{
	static std::once_flag flag;
	auto lambda_fun = [&]()
	{
		CLogSystem* log_system = new (std::nothrow)CLogSystem();
		if (log_system != nullptr)
		{
			log_system->init();
		}
		m_log_system = log_system;
	};
	std::call_once(flag, lambda_fun);

	return m_log_system;
}

CLogSystem::CLogSystem()
{
	m_vec_buffer.resize(4096);
	m_log_pack.m_log_fp = nullptr;
}

CLogSystem::~CLogSystem()
{

}

unsigned int CLogSystem::init()
{
	return 0;
}

unsigned int CLogSystem::write(const char* log_context)
{
	if (log_context == nullptr || strlen(log_context) == 0)
	{
		return -1;
	}
	if (m_log_pack.m_log_fp == nullptr)
	{//打开文件
		
	}
	return 0;
}

bool CLogSystem::open_file()
{
	boost::unique_lock<boost::shared_mutex>  write_lock(m_log_pack_shared_mutex);
	if(m_log_pack.m_log_fp != nullptr)
	{
		return true;
	}
	boost::filesystem::path log_path(m_log_pack.m_log_path);
	std::string log_file_name = 	

const char* CLogSystem::build_log_record(const char* format, va_list& argptr)
{
	int cnt = vsnprintf(&m_vec_buffer[0], m_vec_buffer.size(), format, argptr);
	va_end(argptr);
	if (cnt < 0)
	{//格式化出错
		return nullptr;
	}
	return &m_vec_buffer[0];	
}




