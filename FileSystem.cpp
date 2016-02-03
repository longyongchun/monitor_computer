/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           FileSystem.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/12
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#include "FileSystem.h"
#include <unistd.h>

CFileSystem CFileSystem::m_file_system;

CFileSystem::CFileSystem()
{

}
	
CFileSystem::~CFileSystem()
{

}

CFileSystem* CFileSystem::get_instance_pointer()
{
	return &m_file_system;
}

CFileSystem& CFileSystem::get_instance_object()
{
	return m_file_system;
}


bool CFileSystem::file_exists(const char* full_path)const
{
	if (access(full_path, F_OK) == 0)
	{
		return true;
	}
	return false;
}









