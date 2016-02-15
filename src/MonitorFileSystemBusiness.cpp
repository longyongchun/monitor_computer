/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorFileSystemBusiness.cpp
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

#include "MonitorFileSystemBusiness.h"
#include "MonitorFileSystem.h"
#include "config/GolabConfig.h"

CMonitorFileSystemBusiness::CMonitorFileSystemBusiness()
{

}

CMonitorFileSystemBusiness::~CMonitorFileSystemBusiness()
{

}

unsigned int CMonitorFileSystemBusiness::init()
{
	if (!m_file_system)
	{
		IMonitorFileSystem* file_system = new(std::nothrow)CMonitorFileSystem();
		if (file_system == nullptr)
		{
			return -1;
		}
		m_file_system.reset(file_system);
		m_file_system->init();
		return 0;
	}
	return -1;
}
#include <iostream>
unsigned int CMonitorFileSystemBusiness::start()
{
	if (m_file_system)
	{
		std::list<std::string>& path_list = GOLAB_CONFIG->m_mon_path_list;
		for (auto& mon_path : path_list)
		{
 			m_file_system->add_monitor_node(mon_path.c_str(), true);
			std::cout << mon_path << std::endl;
		}	
		return 0;
	}
	return -1;
}



