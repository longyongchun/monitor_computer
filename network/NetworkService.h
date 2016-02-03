/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           NetworkService.h
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

#ifndef NETWORK_SERVICE_HPP__
#define NETWORK_SERVICE_HPP__
#include "INetworkService.h"
#include <list>
#include <string>
#include <boost/bimap.hpp>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <sys/epoll.h>
#include <boost/thread.hpp>
#include "../data_queue/DataQueue.hpp"
class CNetworkService : public INetworkService
{
public:
	CNetworkService();
	virtual ~CNetworkService();

public:
	virtual unsigned int init();
	virtual unsigned int start();
	/**
	 * @brief  连接服务器
	 *
	 * @param ip_addr iP地址
	 * @param port    端口
	 *
	 * @return        唯一标识连结 用于后续的注册任务
	 */
	virtual unsigned int connect(const char* ip_addr, short port);
	
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
	virtual unsigned int register_event_handle(unsigned int conn_id, EventFlag event_flag, IEventHandle* event_handle, bool auto_release);

	/**
	 * @brief 发送数据 
	 *
	 * @param conn_id
	 * @param data_buf
	 * @param data_len
	 *
	 * @return 
	 */
	virtual unsigned int send_data(unsigned int conn_id, const char* data_buf, unsigned int data_len);

private:
	int client_socket();
	int client_connect(int sock_fd, const char* server_ip, short port);
	
private:
	int start_network_service();
	int process_network_fd(int epfd);
	int process_network_event(const epoll_event* ep_evt, unsigned int evt_size);
	int process_network_write_event(int fd);
	int process_network_read_event(int fd);
	int process_network_error_event(int fd);
	int process_network_close_event(int fd);

	int send_network_data(unsigned int queue_id);
	std::shared_ptr<IEventHandle> get_process_network_event_handle(int fd, EventFlag event_flag);
	int wait_and_process_network_event(int epfd);
	/**
	 * @brief 处理监控网络包 
	 *
	 * @return 
	 */
	int process_monitor_network_pack();


	unsigned int generate_connect_id()const;

private:
	std::string m_node_mgr_tag;
	std::string m_node_tag;
	std::string m_send_data_tag;

	typedef struct NetworkProcessPack
	{
		NetworkProcessPack(unsigned int conn_id)
			:m_con_id(conn_id)
		{
		}
		bool operator<(const NetworkProcessPack& pack)const
		{
			return this->m_con_id < pack.m_con_id;
		}

	private:
		struct SHash
		{
			size_t operator()(const EventFlag& evt_flag)const
			{
				return m_hash((unsigned int)evt_flag);
			}

			private:
				std::hash<unsigned int> m_hash;
		};
	public:
		boost::shared_mutex                                           		         m_event_handle_map_shared_mutex;  
		typedef std::unordered_map<EventFlag, std::shared_ptr<IEventHandle>, SHash>  EVENT_HANDLE_MAP;
		EVENT_HANDLE_MAP 															 m_event_handle_map;
		const unsigned int 				   							  		         m_con_id;
	}NetworkProcessPack;
	
	typedef boost::bimap<int, unsigned int> FD_CONNECT_MAP;
	typedef std::unordered_map<unsigned int, std::shared_ptr<NetworkProcessPack> > CONN_NETWORK_MAP;
	//fd 与 连接id
	boost::shared_mutex  m_fd_conn_map_shared_mutex;
	FD_CONNECT_MAP        m_fd_conn_map;
	//conn_id 与 关联事件句柄
	boost::shared_mutex  m_conn_handle_map_shared_mutex;
	CONN_NETWORK_MAP     m_conn_handle_map;

	std::mutex  		 m_wait_join_fd_list_mutex;
	std::list<int>       m_wait_join_fd_list;

	std::mutex  m_wait_remove_fd_list_mutex;
	std::list<int>       m_wait_remove_fd_list;


private:
	typedef struct SSendDataPack
	{
		SSendDataPack() = default;
		SSendDataPack(unsigned int conn_id, const char* buf, unsigned int buf_len)
			:m_conn_id(conn_id),
			m_data_buf(buf, buf + buf_len)
		{

		}
		unsigned int m_conn_id;
		std::string  m_data_buf;
	}SSendDataPack;

	unsigned int                            m_send_data_threads; //发送线程的数量
	std::vector<CDataQueue<SSendDataPack> > m_send_data_queue; //发送数据队列

};

#endif





