/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           FileSystem.h
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

#ifndef FILE_SYSTEM_HPP__
#define FILE_SYSTEM_HPP__

class CFileSystem
{
public:
	CFileSystem();
	~CFileSystem();

public:
	/**
	 * @brief 文件或目录是否存在 
	 *
	 * @param full_path
	 *
	 * @return 
	 */
	bool file_exists(const char* full_path)const;

public:
	static CFileSystem* get_instance_pointer();
	static CFileSystem& get_instance_object();

private:
	static CFileSystem m_file_system;
};

#define FILE_SYSTEM_POINTER  CFileSystem::get_instance_pointer()
#define FILE_SYSTEM_OBJECT 	 CFileSystem::get_instance_object()



#endif










