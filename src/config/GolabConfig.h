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
class CGolabConfig
{
public:
	CGolabConfig();
	~CGolabConfig();

public:
	unsigned int init();

public:
	static CGolabConfig* get_instance();
private:
	static CGolabConfig* m_golab_config; 

private:
	CIniFileParse m_monitor_ini_file;
};

#define GOLAB_CONFIG CGolabConfig::get_instance()



#endif






