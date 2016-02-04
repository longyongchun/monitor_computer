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
#include <unordered_map>

class CIniFileParse
{
public:
	CIniFileParse();
	~CIniFileParse();

public:
	bool load_file(const char* file_path);

	bool get_item_value(const std::string& section, const std::string& key, std::string& value)const;

private:
	bool is_comment(const std::string& line_data)const;
	bool get_section(const std::string& line_data, std::string& section)const;
	bool get_key_value(const std::string& line_data, std::string& key, std::string& value)const;

private:
	typedef std::unordered_map<std::string, std::string>   KEY_VALUE_MAP;
	typedef std::unordered_map<std::string, KEY_VALUE_MAP> SECTION_KEY_VALUE_MAP;

	SECTION_KEY_VALUE_MAP m_ini_file_map;
};


#endif




