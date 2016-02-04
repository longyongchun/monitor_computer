/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           CLogSystem.h
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

#ifndef LOG_SYSTEM_HPP__
#define LOG_SYSTEM_HPP__
#include "ILogSystem.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <stdarg.h>
#include <boost/thread/thread.hpp>
class CLogSystem : public ILogSystem
{
public:
	CLogSystem();
	virtual ~CLogSystem();

public:
	virtual unsigned int init();
	virtual unsigned int write(const char* log_context);

public:
	static ILogSystem* get_instance();

private:
	const char* build_log_record(const char* format, va_list& argptr);

	bool open_file();
	bool write_file(const char* log_context);
	bool close_file();

private:
	boost::shared_mutex    m_log_pack_shared_mutex;
	struct LogPack
	{
		std::string m_log_path;       //日志存放的目录
		std::string m_log_file_prex;  //日志文件前缀
		FILE*       m_log_fp;
	}m_log_pack;

	std::vector<char> m_vec_buffer;
	
	static ILogSystem* m_log_system;
};

#endif




