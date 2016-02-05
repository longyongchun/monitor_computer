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

unsigned int CMonitorFileSystemBusiness::start()
{
	if (m_file_system)
	{
		m_file_system->add_monitor_node("/home/code/src/lyc/", true);
		return 0;
	}
	return -1;
}



