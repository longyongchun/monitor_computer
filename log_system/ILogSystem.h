/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           ILogSystem.h
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


#ifndef ILOG_SYSTEM_HPP__
#define ILOG_SYSTEM_HPP__

class ILogSystem
{
public:
	virtual ~ILogSystem(){};

public:
	virtual unsigned int init() = 0;
	virtual unsigned int write(const char* log_context) = 0;
};


#endif




