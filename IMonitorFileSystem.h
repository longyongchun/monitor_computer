/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IMonitorFileSystem.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/07
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#ifndef IMONITOR_FILE_SYSTEM_HPP__
#define IMONITOR_FILE_SYSTEM_HPP__

class IFileNode;

enum class MonitorStatus
{
	EN_MONITOR_ERROR,
	EN_MONITOR_SUCCESS
};


class IMonitorFileSystem
{
public:
	virtual ~IMonitorFileSystem(){}

public:

	/**
	 * @brief  初始化
	 *
	 * @return 
	 */
	virtual unsigned int init() = 0;

	/**
	 * @brief  增加监控节点
	 *
	 * @param path       
	 * @param is_recursive  是否递归监视目录 true: 递归监视 fasle: 不递归监视
	 *
	 * @return 
	 */
	virtual MonitorStatus add_monitor_node(const char* path, bool is_recursive) = 0;

	/**
	 * @brief  开始执行文件系统监控
	 *
	 * @return 
	 */
	virtual MonitorStatus start() = 0;
};

#endif

