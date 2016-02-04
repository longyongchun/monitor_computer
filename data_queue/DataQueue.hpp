/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           DataQueue.hpp
/// @brief          数据缓存队列类           
/// @attention
/// @todo           DataContainer需要实现 push pop front empty() 方法
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/15
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************



#ifndef DATA_QUEUE_HPP__
#define DATA_QUEUE_HPP__

#include <queue>
#include <mutex>
#include <utility>
#include <memory>
#include <iterator>

template <typename DataType, typename DataContainer = std::queue<DataType>, typename LOCK = std::mutex>
class CDataQueue
{
public:
	CDataQueue();
	~CDataQueue();
	
	CDataQueue(CDataQueue&& data_que);

	CDataQueue(const CDataQueue&) = delete;



public:
	bool pop_data(DataType& out_data);
	bool get_data(DataType& out_data);
	bool push_data(const DataType& in_data);
	bool push_data(DataType&& in_data);

	template <typename DataQueue>
	bool move_data_to_backward(DataQueue& data_queue);

	template <typename DataQueue>
	bool insert_data_to_backward(DataQueue& data_queue);
private:
	DataContainer                 m_data_container;
	std::shared_ptr<LOCK>		  m_data_lock;
};

template <typename DataType, typename DataContainer, typename LOCK>
CDataQueue<DataType, DataContainer, LOCK>::CDataQueue(CDataQueue<DataType, DataContainer, LOCK>&& data_que)
		:m_data_container(std::move(data_que.m_data_container)),
		m_data_lock(std::move(m_data_lock))
{

}

template <typename DataType, typename DataContainer, typename LOCK>
CDataQueue<DataType, DataContainer, LOCK>::CDataQueue()
{
	m_data_lock.reset(new(std::nothrow)LOCK());
}

template <typename DataType, typename DataContainer, typename LOCK>
CDataQueue<DataType, DataContainer, LOCK>::~CDataQueue()
{

}

template <typename DataType, typename DataContainer, typename LOCK>
bool CDataQueue<DataType, DataContainer, LOCK>::pop_data(DataType& out_data)
{
	std::unique_lock<LOCK> write_lock(*m_data_lock);
	if (!m_data_container.empty())
	{
		out_data = std::move(m_data_container.front());
		m_data_container.pop();
		return true;
	}
	return false;
}

template <typename DataType, typename DataContainer, typename LOCK>
bool CDataQueue<DataType, DataContainer, LOCK>::get_data(DataType& out_data)
{
	std::unique_lock<LOCK> write_lock(*m_data_lock);
	if (!m_data_container.empty())
	{
		out_data = m_data_container.front();
		return true;
	}
	return false;
}
	
template <typename DataType, typename DataContainer, typename LOCK>
bool CDataQueue<DataType, DataContainer, LOCK>::push_data(const DataType& in_data)
{
	std::unique_lock<LOCK> write_lock(*m_data_lock);
	m_data_container.push(in_data);
	return true;
}

template <typename DataType, typename DataContainer, typename LOCK>
bool CDataQueue<DataType, DataContainer, LOCK>::push_data(DataType&& in_data)
{
	std::unique_lock<LOCK> write_lock(*m_data_lock);
	m_data_container.push(std::move(in_data));
	return true;
}

template <typename DataType, typename DataContainer, typename LOCK>
template <typename DataQueue>
bool CDataQueue<DataType, DataContainer, LOCK>::move_data_to_backward(DataQueue& data_queue)
{

	std::unique_lock<LOCK> write_lock(*m_data_lock);
	data_queue.insert(data_queue.end(), std::make_move_iterator(m_data_container.begin()),
				std::make_move_iterator(m_data_container.end()));
	m_data_container.clear();
	return true;
}

template <typename DataType, typename DataContainer, typename LOCK>
template <typename DataQueue>
bool CDataQueue<DataType, DataContainer, LOCK>::insert_data_to_backward(DataQueue& data_queue)
{
	std::unique_lock<LOCK> write_lock(*m_data_lock);
	m_data_container.insert(m_data_container.begin(), data_queue.begin(), data_queue.end());
	return true;
}

#endif


