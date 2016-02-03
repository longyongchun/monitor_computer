/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           CMonitorFileSystemBusiness.h
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

#ifndef MONTTOR_FILE_SYSTEM_BUSINESS_HPP__
#define MONTTOR_FILE_SYSTEM_BUSINESS_HPP__

#include "IBusiness.h"
#include <memory>

class IMonitorFileSystem;
class CMonitorFileSystemBusiness : public IBusiness
{
public:
	CMonitorFileSystemBusiness();
	virtual ~CMonitorFileSystemBusiness();

public:
	virtual unsigned int init();
	virtual unsigned int start();

private:
 	std::shared_ptr<IMonitorFileSystem> m_file_system;
};


#endif

