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
			std::cout << line_buf << std::endl;
			continue;
		}
		//2. 判断是否是section
		if (this->get_section(line_buf, section))
		{
			std::cout << section << std::endl;
			continue;
		}
		//3. 获取键值对
		std::string key;
		std::string value;
		if (this->get_key_value(line_buf, key, value))
		{
			std::cout << key << std::endl;
			std::cout << value << std::endl;
			continue;
		}
	}
	ifs.close();
	return true;
}
	
bool CIniFileParse::is_comment(const std::string& line_data)const
{
	static const std::regex pattern("^;.*$");
	return std::regex_match(line_data, pattern);
}

bool CIniFileParse::get_section(const std::string& line_data, std::string& section)const
{
	static const std::regex pattern("^\\[([\\w\\s]*)\\]$");
	std::cmatch result;
	bool search_result = std::regex_search(line_data.c_str(), result, pattern);
	if (search_result)
	{//匹配成功
		section = std::string(result[1].first, result[1].second);
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
	return false;
}

/*#include <iostream>  
#include <regex>  

using namespace std;  

int main()  
{  
	char* reg_esp = "^\\[([\\w\\s]*)\\]$";
	regex rgx(reg_esp);  
	cmatch match;  
	const char* target = "[good morningddddd]";  
	if(regex_search(target,match,rgx))  
	{
		for (size_t a = 1; a < match.size(); ++a)
		{
			std::cout << std::string(match[a].first, match[a].second) << std::endl;
		}
	}
	else
	{
		std::cout << "No Match Case !" << std::endl;
	}
}*/

int main()
{
	CIniFileParse ini;
	ini.load_file("test.txt");
	return 0;
}



