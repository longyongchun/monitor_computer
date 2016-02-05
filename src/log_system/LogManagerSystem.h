/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           LogManagerSystem.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/02/05
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#ifndef LOG_MANAGER_SYSTEM_HPP__
#define LOG_MANAGER_SYSTEM_HPP__

#include <sys/time.h>
#include <cstdio>
#include "LogSystem.h"
#include <ctime>
#define WRITE_LOG(level, format, args...)\
{\
	char szbuf[2048] = {0};\
	char* tmp = szbuf;\
	/*添加日志头*/\
	/*1. 添加日期 */\
	timeval tmval;\
	if (gettimeofday(&tmval, nullptr) >= 0)\
	{\
		struct tm curr_tm;\
		if (nullptr != localtime_r((time_t*)&tmval.tv_sec, &curr_tm))\
		{\
			int len = snprintf(tmp, sizeof(szbuf) - (tmp - szbuf), \
						"[%04d-%02d-%02d %02d:%02d:%02d:%06l]", curr_tm.tm_year + 1900, curr_tm.tm_mon + 1, curr_tm.tm_yday, curr_tm.tm_hour,\
						curr_tm.tm_min, curr_tm.tm_sec, tmval.tv_usec); \
			tmp += len;\
		}\
	}\
	/*2. 添加文件名与文件行数*/\
   	int len = snprintf(tmp, sizeof(szbuf) - (tmp - szbuf), "[filename: %s][line: %d]", __FILE__, __LINE__);\
   	tmp += len;\
	/*3. 添加日志等级*/ \
	len = snprintf(tmp, sizeof(szbuf) - (tmp - szbuf), "[level: %s] ", level);\
	tmp += len;\
	/*4. 组装日志内容*/ \
	snprintf(tmp, sizeof(szbuf) - (tmp - szbuf), format, args);\
	get_log_system_instance()->write(szbuf);\
}\

#define WRITE_INFO(format, args...)\
{\
	WRITE_LOG("INFO", format, ##args);\
}\

#define WRITE_ERROR(format, args...)\
{\
	WRITE_LOG("ERROR", format, ##args);\
}\

#define WRITE_DEBUG(format, args...)\
{\
	WRITE_LOG("DEBUG", format, ##args);\
}\

#endif

