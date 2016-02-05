/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IBusiness.h
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

#ifndef IBUSINESS_HPP__
#define IBUSINESS_HPP__

class IBusiness
{
public:
	virtual ~IBusiness(){}

public:
	virtual unsigned int init() = 0;
	virtual unsigned int start() = 0;
};

#endif




