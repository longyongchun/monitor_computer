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
/// @date           2016/01/08
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************
#include "Business.h"
#include "../public/log_system/LogManagerSystem.h"
int main()
{
	WRITE_ERROR_LOG("this is a %s", "test");
	CBusiness business;
	if (business.init() != 0xffffffff)
	{
		business.start();
	}
	return 0;
}



