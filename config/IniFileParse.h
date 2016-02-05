/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IniFileParse.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/02/04
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#ifndef INI_FILE_PARSE_HPP__
#define INI_FILE_PARSE_HPP__

#include <string>
#include <cstdio>
#include <unordered_map>

class CIniFileParse
{
public:
	CIniFileParse();
	~CIniFileParse();

public:
	bool load_file(const char* file_path);

	bool get_item_value(const std::string& section, const std::string& key, std::string& value)const;

	/**
	 * @brief 打印ini文件到相应的文件中 
	 *
	 * @param fp 打印信息到该文件中
	 *
	 * @return 
	 */
	bool print_file(FILE* fp);


private:
	bool is_comment(const std::string& line_data)const;
	bool get_section(const std::string& line_data, std::string& section)const;
	bool get_key_value(const std::string& line_data, std::string& key, std::string& value)const;
	bool insert_section(const std::string& section);
	bool insert_key_value(const std::string& section, std::string&& key, std::string&& value);
private:
	typedef std::unordered_map<std::string, std::string>   KEY_VALUE_MAP;
	typedef std::unordered_map<std::string, KEY_VALUE_MAP> SECTION_KEY_VALUE_MAP;

	SECTION_KEY_VALUE_MAP m_ini_file_map;
};


#endif




