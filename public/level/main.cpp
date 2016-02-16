/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           main.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/25
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#include <iostream>
#include "DataStoreLevelDB.h"
#include <string>
#include "MonitorDataStore.h"
struct test
{
	char sz[1024];
	int a;
};
/*int main()
{
	CMonitorDataStore* pData = new CMonitorDataStore();
	pData->init("test");
	std::string value;
	test t;
	t.a = 5;
	const char* ttt = (const char*)&t;
	pData->get("hello", &t, sizeof(t));
	std::cout << t.a << std::endl;
//	pData->get("hello", value);
	std::cout << value << std::endl;
	KEY_CONTAINER vec;
	vec.push_back("hello");
	vec.push_back("longhyongcun");
	if (pData->del_data_in_range(vec) == StoreStatus::EN_STATUS_SUCCEED)
	{
		std::cout << "succeed" << std::endl;
	}
	else
	{
		std::cout << "failed" << std::endl;
	}

	pData->ergodic([](const std::string&, const std::string&){}, true);

	return 0;
}*/
