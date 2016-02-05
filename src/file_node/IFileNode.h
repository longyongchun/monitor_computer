/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IFileNode.h
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


#ifndef IFILE_NODE_HPP__
#define IFILE_NODE_HPP__

#include <string>

class IFileNode
{
public:
	virtual ~IFileNode(){}

public:
	virtual std::string get_name()const = 0;

};


#endif

