/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           CGolabConfig.h
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

#ifndef GOLAB_CONFIG_HPP__
#define GOLAB_CONFIG_HPP__
#include "IniFileParse.h"
#include <list>
#include <string>
class CGolabConfig
{
private:
	CGolabConfig();
public:
	~CGolabConfig();

public:
	unsigned int init();

public:
	//日志配置信息
	struct LogInfoConfig
	{
		std::string m_log_pre;
		std::string m_log_path;
	}log_info_cfg;
	
	//监控路径
	std::list<std::string> m_mon_path_list;

public:
	static CGolabConfig* get_instance();
private:
	static CGolabConfig* m_golab_config; 

private:
	CIniFileParse m_monitor_ini_file;
};

#define GOLAB_CONFIG CGolabConfig::get_instance()



#endif






