/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IniFileParse.cpp
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

#include "IniFileParse.h"
#include <regex>
#include <fstream>
#include <iostream>
CIniFileParse::CIniFileParse()
{

}
	
CIniFileParse::~CIniFileParse()
{

}

bool CIniFileParse::load_file(const char* file_path)
{
	std::ifstream ifs(file_path);
	if (!ifs)
	{
		return false;
	}

	//解析文件的每一行
	std::string line_buf;
	std::string section;
	while (std::getline(ifs, line_buf))
	{
		//1. 首先匹配是否是注释行 如果是直接丢弃
		if (this->is_comment(line_buf))
		{
			continue;
		}
		//2. 判断是否是section
		if (this->get_section(line_buf, section))
		{
			this->insert_section(section);	
			continue;
		}
		//3. 获取键值对
		std::string key;
		std::string value;
		if (this->get_key_value(line_buf, key, value))
		{
			this->insert_key_value(section, std::move(key), std::move(value));		
		}
	}
	ifs.close();
	return true;
}
	
bool CIniFileParse::insert_section(const std::string& section)
{
	if (m_ini_file_map.find(section) != m_ini_file_map.end())
	{//区域已经存在 
		return true;
	}

	if (!m_ini_file_map.insert(SECTION_KEY_VALUE_MAP::value_type(section, KEY_VALUE_MAP())).second)
	{//插入失败
		return false;
	}

	return true;
}


bool CIniFileParse::insert_key_value(const std::string& section, std::string&& key, std::string&& value)
{
	SECTION_KEY_VALUE_MAP::iterator it_section_map = m_ini_file_map.find(section);
	if (it_section_map == m_ini_file_map.end())
	{//区域不存在
		return false;
	}
	
	KEY_VALUE_MAP& key_value_map = it_section_map->second;
	if (key_value_map.find(key) != key_value_map.end())
	{//键存在
		return false;
	}

	if (!key_value_map.insert(KEY_VALUE_MAP::value_type(key, value)).second)
	{//插入失败
		return false;
	}

	return true;
}

bool CIniFileParse::is_comment(const std::string& line_data)const
{
	static const std::regex pattern("^;.*$");
	return std::regex_match(line_data, pattern);
}

bool CIniFileParse::get_section(const std::string& line_data, std::string& section)const
{
	std::cout << "ehllo" << std::endl;
	static const std::regex pattern("^\\[([a-z0-9A-Z ]*)\\]$", std::regex_constants::egrep);
	std::cout << "good" << std::endl;
	std::cmatch result;
	bool search_result = std::regex_search(line_data.c_str(), result, pattern);
	if (search_result)
	{//匹配成功
		section = std::string(result[1].first, result[1].second);
		std::cout << section << std::endl;
	}
	return search_result;
}

bool CIniFileParse::get_key_value(const std::string& line_data, std::string& key, std::string& value)const
{
	static const std::regex pattern("^(\\w*)\\s*=\\s*(\\w*)$");
	std::cmatch result;
	bool search_result = std::regex_search(line_data.c_str(), result, pattern);
	if (search_result)
	{//匹配成功
		key = std::string(result[1].first, result[1].second);
		value = std::string(result[2].first, result[2].second);
	}
	return search_result;

}

bool CIniFileParse::get_item_value(const std::string& section, const std::string& key, std::string& value)const
{
	SECTION_KEY_VALUE_MAP::const_iterator it_section_map = m_ini_file_map.find(section);
	if (it_section_map == m_ini_file_map.end())
	{//没有对应的section
		return false;
	}
	const KEY_VALUE_MAP& key_value_map = it_section_map->second;
	KEY_VALUE_MAP::const_iterator it_key_value = key_value_map.find(key);
	if (it_key_value == key_value_map.end())
	{//对应的键不存在
		return false;
	}
	value = it_key_value->second;
	return true;
}
	
bool CIniFileParse::print_file(FILE* fp)
{
	if (fp == nullptr)
	{
		return false;
	}
	char buf[4096] = {0};
	for (auto& section_pair : m_ini_file_map)
	{
		int real_len = snprintf(buf, sizeof(buf), "[%s]\n", section_pair.first.c_str());
		fwrite(buf, real_len, 1, fp);
		for (auto& key_value_pair : section_pair.second)
		{
			real_len = snprintf(buf, sizeof(buf), "%s = %s\n", key_value_pair.first.c_str(), key_value_pair.second.c_str());
			fwrite(buf, real_len, 1, fp);
		}
	}
	return true;
}


/*int main()
{
	std::string line_data = "[good morning]";
	std::string section;
	std::regex pattern("^\\\\[([a-z0-9A-Z ]*)\\\\]$");
	std::cout << "good" << std::endl;
	std::cmatch result;
	bool search_result = std::regex_search(line_data.c_str(), result, pattern);
	if (search_result)
	{//匹配成功
		section = std::string(result[1].first, result[1].second);
		std::cout << section << std::endl;
	}
	return search_result;
	CIniFileParse ini;
	ini.load_file("test.txt");
	FILE* fp = fdopen(2, "w");
	ini.print_file(fp);
	return 0;
}*/



