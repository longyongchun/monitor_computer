/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           DirectoryNode.cpp
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

#include "DirectoryNode.h"
#include <utility>
CDirectoryNode::CDirectoryNode(const std::string& name)
		:m_name(name)
{

}

CDirectoryNode::~CDirectoryNode()
{

}

std::string CDirectoryNode::get_name()const
{
	return m_name;	
}

std::string CDirectoryNode::set_name(const std::string& name)
{
	std::string tmp_name(std::move(m_name));
	m_name = name;
	return std::move(tmp_name);
}


