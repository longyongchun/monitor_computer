/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorDataStore.cpp
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
#include "MonitorDataStore.h"
#include "DataStoreLevelDB.h"
#include <mutex>
CMonitorDataStore::CMonitorDataStore()
{

}

CMonitorDataStore::~CMonitorDataStore()
{

}

StoreStatus CMonitorDataStore::put(const std::string& key, const std::string& value)
{
	return m_data_store->put(key, value);
}

StoreStatus CMonitorDataStore::put(const char* key, void* value, int value_len)
{
	return m_data_store->put(key, value, value_len);
}

StoreStatus CMonitorDataStore::get(const std::string& key, std::string& value)
{
	return m_data_store->get(key, value);
}

StoreStatus CMonitorDataStore::get(const char* key, void* value, int value_len)
{
	return m_data_store->get(key, value, value_len);
}

StoreStatus CMonitorDataStore::del(const std::string& key)
{
	return m_data_store->del(key);
}

StoreStatus CMonitorDataStore::ergodic(IErgodicFunction* fun, bool move_flag)
{
	return m_data_store->ergodic(fun, move_flag);
}

StoreStatus CMonitorDataStore::del_data_in_range(const KEY_CONTAINER& key_container)
{
	return m_data_store->del_data_in_range(key_container);
}

bool CMonitorDataStore::init(const std::string& store_path)
{
	static std::once_flag flag;
	auto lambda_init = [&]()
	{
		CDataStoreLevelDB* data_store = new (std::nothrow)CDataStoreLevelDB(store_path);
		if (data_store)
		{
			data_store->init();
			m_data_store.reset(data_store);
		}
	};
	std::call_once(flag, lambda_init);

	return m_data_store.operator bool();
}
