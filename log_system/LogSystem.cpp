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

ILogSystem* get_log_system_instance()
{
	return CLogSystem::get_instance();
}

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
	m_log_pack.m_log_path = "/home/code/src";
	m_log_pack.m_log_file_prex = "monitor";
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
	if (m_log_pack.m_log_fp == nullptr && !this->open_file())
	{//打开文件失败 直接返回
		return -1;
	}
	
	if (!this->write_file(log_context))
	{//写日志记录失败 关闭文件 下次重新打开
		this->close_file();
		return -1;
	}

	return 0;
}


bool CLogSystem::close_file()
{
	boost::unique_lock<boost::shared_mutex> write_lock(m_log_pack_shared_mutex);
	if (m_log_pack.m_log_fp == nullptr)
	{
		return true;
	}
	bool result = ::fclose(m_log_pack.m_log_fp) != EOF;
	m_log_pack.m_log_fp = nullptr;

	return result;
}

//写文件
bool CLogSystem::write_file(const char* log_context)
{
	boost::shared_lock<boost::shared_mutex> read_lock(m_log_pack_shared_mutex);
	if (::fwrite(log_context, strlen(log_context), 1, m_log_pack.m_log_fp) != 1)
	{
		return false;
	}
	return true;
}

bool CLogSystem::open_file()
{
	boost::unique_lock<boost::shared_mutex>  write_lock(m_log_pack_shared_mutex);
	if(m_log_pack.m_log_fp != nullptr)
	{
		return true;
	}
	boost::filesystem::path log_path(m_log_pack.m_log_path);
	std::string log_file_name = m_log_pack.m_log_file_prex;
	//获取当前日期
	time_t curr_time = time(NULL);
	struct tm curr_tm = {0};
	if (::localtime_r(&curr_time, &curr_tm) == nullptr)
	{//格式化时间出错
		return false;
	}
	char buf[32] = {0};
	snprintf(buf, sizeof(buf), "%04d%02d%02d%02d%02d%02d.txt", curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_mday,
				curr_tm.tm_hour, curr_tm.tm_min, curr_tm.tm_sec);
	log_file_name += buf;
	log_path /= log_file_name;

	m_log_pack.m_log_fp = fopen(log_path.string().c_str(), "a+");
	
	return m_log_pack.m_log_fp != nullptr;
}

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




