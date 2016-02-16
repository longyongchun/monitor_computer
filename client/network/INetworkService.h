/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           INetworkService.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/14
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#ifndef INETWORK_SERVICE_HPP__
#define INETWORK_SERVICE_HPP__

enum class EventFlag
{
	EN_EVENT_READ,
	EN_EVENT_WRITE,
	EN_EVENT_EXCEPTION
};

class IEventHandle;

class INetworkService
{
public:
	virtual ~INetworkService(){}

public:
	virtual unsigned int init() = 0;
	virtual unsigned int start() = 0;
		/**
	 * @brief 注册指定的网络事件 
	 * 
	 * @param conn_id 调用connect返回连接标识
	 * @param event_flag
	 * @param event_handle
	 * @param auto_release
	 *
	 * @return 
	 */
	virtual unsigned int register_event_handle(unsigned int conn_id, EventFlag event_flag, IEventHandle* event_handle, bool auto_release) = 0;

	/**
	 * @brief 发送数据 
	 *
	 * @param conn_id
	 * @param data_buf
	 * @param data_len
	 *
	 * @return 
	 */
	virtual unsigned int send_data(unsigned int conn_id, const char* data_buf, unsigned int data_len) = 0;	

	virtual unsigned int connect(const char* ip_addr, short port) = 0;

};

class IEventHandle
{
public:
	virtual ~IEventHandle(){}

public:
	virtual unsigned int process_event(const char* net_data, EventFlag event_flag) = 0;
};




#endif





