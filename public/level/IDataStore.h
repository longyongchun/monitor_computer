/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           IDataStore.h
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

#ifndef IDATA_STORE_HPP__
#define IDATA_STORE_HPP__
#include <string>
#include <vector>

typedef std::vector<std::string> KEY_CONTAINER;

enum class StoreStatus
{
	EN_STATUS_FAILED,
	EN_STATUS_SUCCEED
};

class IErgodicFunction
{
public:
	virtual ~IErgodicFunction(){}

public:
	virtual bool run(const std::string& key, const std::string& value) = 0;
};

 


class IDataStore
{
public:
	virtual ~IDataStore(){}

public:
	virtual StoreStatus put(const std::string& key, const std::string& value) = 0;
	virtual StoreStatus put(const char* key, void* value, int value_len) = 0;
	virtual StoreStatus get(const std::string& key, std::string& value) = 0;
	virtual StoreStatus get(const char* key, void* value, int value_len) = 0;	
	virtual StoreStatus del(const std::string& key) = 0;
	virtual StoreStatus ergodic(IErgodicFunction* fun, bool move_flag) = 0;
	virtual StoreStatus del_data_in_range(const KEY_CONTAINER& key_container) = 0;
};

#endif





