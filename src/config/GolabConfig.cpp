/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           GolabConfig.cpp
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
#include "GolabConfig.h"
#include <cstdio>
#include <thread>
#include <mutex>
CGolabConfig* CGolabConfig::m_golab_config = nullptr;

CGolabConfig* CGolabConfig::get_instance()
{
	static std::once_flag flag;
	auto lambda_fun = [&]()
	{
		if (m_golab_config == nullptr)
		{
			m_golab_config = new(std::nothrow)CGolabConfig();
		}
	};

	std::call_once(flag, lambda_fun);
	return m_golab_config;
}

CGolabConfig::CGolabConfig()
{

}

CGolabConfig::~CGolabConfig()
{

}

unsigned int CGolabConfig::init()
{
	if (m_monitor_ini_file.load_file("config/monitor.ini"))
	{
		
		//获取日志配置信息
		m_monitor_ini_file.get_item_value("LOG_CONFIG", "log_path", log_info_cfg.m_log_path); 
		m_monitor_ini_file.get_item_value("LOG_CONFIG", "log_pre", log_info_cfg.m_log_pre); 
	
		//获取监控路径
		std::string path_value;
		int idx = 0;
		char key[32] = {0};
		do
		{
			++idx;
			snprintf(key, sizeof(key), "mon_path%d", idx);
			if (m_monitor_ini_file.get_item_value("MONITOR_PATH", key, path_value))
			{
				m_mon_path_list.push_front(std::move(path_value));
				continue;
			}
			break;
		}while(true);

		FILE* fp_screen = fdopen(2, "w");
		m_monitor_ini_file.print_file(fp_screen);
		if (fp_screen != nullptr)
		{
			fclose(fp_screen);
		}
	}
	return 0;
}




