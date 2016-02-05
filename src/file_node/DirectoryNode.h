/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           CDirectoryNode.h
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

#ifndef DIRECTORY_NODE_HPP__
#define DIRECTORY_NODE_HPP__

#include "IFileNode.h"
#include <string>
class CDirectoryNode : public IFileNode
{
public:
	CDirectoryNode(const std::string& name);
	virtual ~CDirectoryNode();

public:
	virtual std::string get_name()const;

public:
	std::string set_name(const std::string& name);

private:
	std::string m_name;
};


#endif



