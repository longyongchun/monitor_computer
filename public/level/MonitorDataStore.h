/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorDataStore.h
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

#ifndef MONITOR_DATA_STORE_HPP__
#define MONITOR_DATA_STORE_HPP__

#include "IDataStore.h"
#include <memory>

class CMonitorDataStore : public IDataStore
{
public:
	CMonitorDataStore();
	virtual ~CMonitorDataStore();

public:
	virtual StoreStatus put(const std::string& key, const std::string& value);
	virtual StoreStatus put(const char* key, void* value, int value_len);
	virtual StoreStatus get(const std::string& key, std::string& value);
	virtual StoreStatus get(const char* key, void* value, int value_len);
	virtual StoreStatus del(const std::string& key);
	virtual StoreStatus ergodic(IErgodicFunction* fun, bool move_flag);
	virtual StoreStatus del_data_in_range(const KEY_CONTAINER& key_container);
public:
	template <typename FUN>
	StoreStatus ergodic(FUN fun, bool move_flag);
public:
	bool init(const std::string& store_path);

private:
	std::shared_ptr<IDataStore> m_data_store;
};

template <typename FUN>
class CErgodicFunction : public IErgodicFunction
{
public:
	CErgodicFunction(FUN fun)
		:m_fun(fun)
	{

	}
	virtual ~CErgodicFunction(){}

public:
	virtual bool run(const std::string& key, const std::string& value)
	{
		m_fun(key, value);
		return true;
	}

private:
	FUN m_fun;
};

#include <iostream>
template <typename FUN>
StoreStatus CMonitorDataStore::ergodic(FUN fun, bool move_flag)
{
	std::shared_ptr<IErgodicFunction> ergodic_fun(new(std::nothrow)CErgodicFunction<FUN>(fun));
	if (ergodic_fun)
	{
		std::cout << "CMonitorDataStore::ergodic" << std::endl;
		return this->ergodic(ergodic_fun.get(), move_flag);
	}
	return StoreStatus::EN_STATUS_FAILED;
}


#endif

