/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           Business.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/02/03
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************



#ifndef BUSINESS_HPP__
#define BUSINESS_HPP__

#include "IBusiness.h"
#include <list>
#include <memory>

class CBusiness : public IBusiness
{
public:
	CBusiness();
	virtual ~CBusiness();


public:
	virtual unsigned int init();
	virtual unsigned int start();


private:
	std::list<std::shared_ptr<IBusiness> > m_business_list;
};


#endif
