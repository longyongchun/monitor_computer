/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           DataStoreLevelDB.h
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


#ifndef DATA_STORE_LEVEL_DB_HPP__
#define DATA_STORE_LEVEL_DB_HPP__

#include "IDataStore.h"
#include <leveldb/db.h>
#include <boost/thread/thread.hpp>
class CDataStoreLevelDB : public IDataStore
{
public:
	CDataStoreLevelDB(const std::string& leveldb_path);
	virtual ~CDataStoreLevelDB();

public:
	virtual StoreStatus put(const std::string& key, const std::string& value);
	virtual StoreStatus put(const char* key, void* value, int value_len);
	virtual StoreStatus get(const std::string& key, std::string& value);
	virtual StoreStatus get(const char* key, void* value, int value_len);
	virtual StoreStatus del(const std::string& key);
	virtual StoreStatus ergodic(IErgodicFunction* fun, bool move_flag);
	virtual StoreStatus del_data_in_range(const KEY_CONTAINER& key_container);

public:
	bool init();

private:
	std::string m_leveldb_path;

	boost::shared_mutex     m_leveldb_shared_mutex;
	leveldb::DB* 			m_leveldb;
};


#endif




