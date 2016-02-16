/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           DataStoreLevelDB.cpp
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

#include "DataStoreLevelDB.h"
#include <leveldb/write_batch.h>
#include "../log_system/LogManagerSystem.h"
#include <mutex>

CDataStoreLevelDB::CDataStoreLevelDB(const std::string& leveldb_path)
		:m_leveldb_path(leveldb_path),
		m_leveldb(nullptr)
{
	
}

CDataStoreLevelDB::~CDataStoreLevelDB()
{

}

StoreStatus CDataStoreLevelDB::put(const std::string& key, const std::string& value)
{
	boost::unique_lock<boost::shared_mutex> write_lock(m_leveldb_shared_mutex);
	leveldb::Status status = m_leveldb->Put(leveldb::WriteOptions(), key, value);
	if (status.ok())
	{
		return StoreStatus::EN_STATUS_SUCCEED;
	}
	else
	{
		WRITE_ERROR_LOG("插入键[%s]值失败, 错误信息[%s]", key.c_str(), status.ToString().c_str());
	}
	return StoreStatus::EN_STATUS_FAILED;
}

StoreStatus CDataStoreLevelDB::put(const char* key, void* value, int value_len)
{
	boost::unique_lock<boost::shared_mutex> write_lock(m_leveldb_shared_mutex);
	leveldb::Slice  s_key(key, strlen(key));
	leveldb::Slice  s_value((const char*)value, value_len);
	leveldb::Status status = m_leveldb->Put(leveldb::WriteOptions(), s_key, s_value);
	if (status.ok())
	{
		return StoreStatus::EN_STATUS_SUCCEED;
	}
	else
	{	
		WRITE_ERROR_LOG("插入键[%s]值失败, 错误信息[%s]", key, status.ToString().c_str());
	}
	return StoreStatus::EN_STATUS_FAILED;
}

StoreStatus CDataStoreLevelDB::get(const std::string& key, std::string& value)
{
	boost::shared_lock<boost::shared_mutex> read_lock(m_leveldb_shared_mutex);
	leveldb::Status status = m_leveldb->Get(leveldb::ReadOptions(), key, &value);
	if (status.ok())
	{
		return StoreStatus::EN_STATUS_SUCCEED;
	}
	else
	{
		WRITE_ERROR_LOG("获取键[%s]值失败, 错误信息[%s]", key.c_str(), status.ToString().c_str());
	}
	return StoreStatus::EN_STATUS_FAILED;
}

StoreStatus CDataStoreLevelDB::get(const char* key, void* value, int value_len)
{
	std::string s_key(key);
	std::string s_value;
	boost::shared_lock<boost::shared_mutex> read_lock(m_leveldb_shared_mutex);
	leveldb::Status status = m_leveldb->Get(leveldb::ReadOptions(), s_key, &s_value);
	if (status.ok())
	{
		memmove(value, s_value.c_str(), std::min<size_t>(value_len, s_value.size()));
		return StoreStatus::EN_STATUS_SUCCEED;
	}
	else
	{
		WRITE_ERROR_LOG("获取键[%s]值失败, 错误信息[%s]", key, status.ToString().c_str());
	}
	return StoreStatus::EN_STATUS_FAILED;
}

StoreStatus CDataStoreLevelDB::del(const std::string& key)
{
	boost::unique_lock<boost::shared_mutex> write_lock(m_leveldb_shared_mutex);
	leveldb::Status status = m_leveldb->Delete(leveldb::WriteOptions(), key);
	if (status.ok())
	{
		return StoreStatus::EN_STATUS_SUCCEED;
	}
	else
	{
		WRITE_ERROR_LOG("删除键[%s]值失败, 错误信息[%s]", key.c_str(), status.ToString().c_str());
	}
	return StoreStatus::EN_STATUS_FAILED;
}

#include <iostream>
StoreStatus CDataStoreLevelDB::ergodic(IErgodicFunction* fun, bool move_flag)
{
	std::cout << "CDataStoreLevelDB::ergodic" << std::endl;
	do
	{
		if (fun == nullptr)
		{
			break;
		}
		leveldb::WriteBatch batch;
		//获取快照
		leveldb::ReadOptions options;
		{
			boost::shared_lock<boost::shared_mutex> read_lock(m_leveldb_shared_mutex);
			options.snapshot = m_leveldb->GetSnapshot();
		}

		leveldb::Iterator* it = m_leveldb->NewIterator(options);
		for (it->SeekToFirst(); it->Valid(); it->Next())
		{
			if (it->status().ok())
			{
				fun->run(it->key().ToString(), it->value().ToString());
				if (move_flag)
				{
					batch.Delete(it->key());
				}
			}
			else
			{
				;
			}
		}
		delete it;
		{//删除已经遍历的键
			boost::unique_lock<boost::shared_mutex> write_lock(m_leveldb_shared_mutex);
			if (m_leveldb->Write(leveldb::WriteOptions(), &batch).ok())
			{//删除成功

			}
		}
		
		return StoreStatus::EN_STATUS_SUCCEED;
	}while(false);
	return StoreStatus::EN_STATUS_FAILED;
}

StoreStatus CDataStoreLevelDB::del_data_in_range(const KEY_CONTAINER& key_container)
{
	leveldb::WriteBatch batch;
	for (auto& key : key_container)
	{
		batch.Delete(key);
	}
	boost::unique_lock<boost::shared_mutex> write_lock(m_leveldb_shared_mutex);
	{
		if (m_leveldb->Write(leveldb::WriteOptions(), &batch).ok())
		{//删除成功
			return StoreStatus::EN_STATUS_SUCCEED;
		}
	}	
	return StoreStatus::EN_STATUS_FAILED;
}

bool CDataStoreLevelDB::init()
{
	//只能被初始化一次
	static std::once_flag flag;
	auto lambda_init = [&]()
	{
		leveldb::Options options;
		//如果leveldb存在 打开leveldb数据库出错
		options.create_if_missing = true;
		//设置32MB的缓存大小
		leveldb::Status status = leveldb::DB::Open(options, m_leveldb_path, &m_leveldb);
		if (status.ok())
		{

		}
		else
		{	

		}
	};
	std::call_once(flag, lambda_init);

	return m_leveldb != nullptr;
}




