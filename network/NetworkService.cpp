/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           NetworkService.cpp
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

#include "NetworkService.h"
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <future>
#include <functional>
#include "../thread_pool/ThreadPool.h"

CNetworkService::CNetworkService()
		:m_node_mgr_tag("network_service_client"),
		m_node_tag("network_service_client_node"),
		m_send_data_tag("network_service_send_data_node")
{
	m_send_data_threads = 10;
	m_send_data_queue.resize(m_send_data_threads);
}

CNetworkService::~CNetworkService()
{

}

unsigned int CNetworkService::init()
{
	if (THREAD_POOL->create_thread_node_mgr(m_node_mgr_tag.c_str()) != 0xffffffff)
	{//创建线程节点管理成功
		if (THREAD_POOL->create_thread_node(m_node_mgr_tag.c_str(), m_node_tag.c_str(), 5) != 0xffffffff)
		{//创建5个线程的节点成功 其中4个线程用来处理网络包 1个线程负责发送网络包
			return 0;
		}

		if (THREAD_POOL->create_thread_node(m_node_mgr_tag.c_str(), m_send_data_tag.c_str(), m_send_data_threads) != 0xffffffff)
		{//10个线程处理发送数据
			return 0;
		}
	}

	return -1;
}

unsigned int CNetworkService::start()
{
	//开启对外连接的网络服务
	CThreadTask* start_task = new (std::nothrow)CThreadTask();
	if (start_task == nullptr)
	{
		return -1;
	}
	start_task->reset(std::bind(&CNetworkService::start_network_service, this));
	THREAD_POOL->register_thread_task(m_node_mgr_tag.c_str(), m_node_tag.c_str(), start_task, true);

	//创建处理监控网络包
	for (int i = 0; i < 4; ++i)
	{
		start_task = new (std::nothrow)CThreadTask();
		if (start_task == nullptr)
		{
			continue;
		}
		start_task->reset(std::bind(&CNetworkService::process_monitor_network_pack, this));
		THREAD_POOL->register_thread_task(m_node_mgr_tag.c_str(), m_node_tag.c_str(), start_task, true);
	}

	//发送数据线程函数
	auto lambda_send_data = [&](int thread_id)
	{
		while (true)
		{
			while (this->send_network_data(thread_id) == 0)
			{
				continue;
			}
			//包发完了 睡一秒
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};
	
	//注册线程函数
	for (int i = 0; i < m_send_data_threads; ++i)
	{
		start_task = new (std::nothrow)CThreadTask();
		if (start_task == nullptr)
		{
			continue;
		}
		start_task->reset(std::bind(lambda_send_data, i));
		THREAD_POOL->register_thread_task(m_node_mgr_tag.c_str(), m_send_data_tag.c_str(), start_task, true);
	}

	return 0;
}
	
unsigned int CNetworkService::send_data(unsigned int conn_id, const char* data_buf, unsigned int data_len)
{
	bool result = m_send_data_queue[conn_id % m_send_data_threads].push_data(SSendDataPack(conn_id, (const char*)data_buf, data_len));
	return result ? 0 : -1;
}

int CNetworkService::start_network_service()
{
	//保证epoll_create创建成功
	int epfd = epoll_create(10000);	
	while (epfd < 0)
	{
		//休息1秒在尝试创建epoll
		std::this_thread::sleep_for(std::chrono::seconds(1));
		//todo 打印失败日志
		epfd = epoll_create(10000);
	}

	while (true)
	{
		if (this->process_network_fd(epfd) < 0)
		{
			//todo 打印日志
		}
		//加入或删除网络套接字失败，并不影响后续的处理 继续监控事件
		if (this->wait_and_process_network_event(epfd) < 0)
		{
			//todo 打印日志
		}
	}

	return 0;
}

int CNetworkService::wait_and_process_network_event(int epfd)
{
	//每次最大监控1000个事件
	static std::vector<epoll_event> vec_max_evt(1000);

	int evt_cnt = epoll_wait(epfd, &vec_max_evt[0], vec_max_evt.size(), 0x8fffffff);
	if (evt_cnt < 0)
	{
		//todo 打印日志
		if (errno == EINTR)
		{//被信号终端
			return 0;
		}
		else
		{//有错误发生
			return -1;
		}
	}
	
	//为了提高并发策略，每100个网络事件都执行一个异步读操作
	const int process_cell = 100;
	std::list<std::future<int> > list_future;
	for (int i = 0; i < evt_cnt; i += process_cell)
	{//处理有事件发生的文件句柄
		using namespace std::placeholders;
		list_future.push_back(std::async(std::launch::async, 
							std::bind(&CNetworkService::process_network_event, this, std::placeholders::_1, std::placeholders::_2), &vec_max_evt[i], std::min(evt_cnt, process_cell)));
	}

	//等待所有线程返回
	for (auto& fut : list_future)
	{
		fut.wait();
	}
    
	return 0;
}

int CNetworkService::process_network_event(const epoll_event* ep_evt, unsigned int evt_size)
{
	for (int i = 0; i < evt_size; ++i)
	{
		const epoll_event& evt = ep_evt[i];

		if (evt.events & EPOLLOUT)
		{//写事件
			this->process_network_write_event(evt.data.fd);
		}

		if (evt.events & EPOLLIN)
		{//读事件
			this->process_network_read_event(evt.data.fd); 
		}
		
		if (evt.events & EPOLLERR)
		{//错误事件
			this->process_network_error_event(evt.data.fd);
		}

		if (evt.events & EPOLLRDHUP)
		{//关闭事件
			this->process_network_close_event(evt.data.fd);
		}
	}
	return 0;
}


int CNetworkService::process_network_write_event(int fd)
{
	return 0;
}

int CNetworkService::process_network_read_event(int fd)
{
	return 0;
}

int CNetworkService::process_network_error_event(int fd)
{
	return 0;
}

int CNetworkService::process_network_close_event(int fd)
{
	return 0;
}

std::shared_ptr<IEventHandle> CNetworkService::get_process_network_event_handle(int fd, EventFlag event_flag)
{
	{//获取连接
		unsigned int conn_id = -1;
		{
			boost::shared_lock<boost::shared_mutex> read_lock(m_fd_conn_map_shared_mutex);
			auto it = m_fd_conn_map.left.find(fd);
			if (it != m_fd_conn_map.left.end())
			{
				conn_id = it->second;
			}
		}

		if (conn_id != 0xffffffff)
		{
			boost::shared_lock<boost::shared_mutex> read_lock(m_conn_handle_map_shared_mutex);
			CONN_NETWORK_MAP::iterator it_conn_handle_map = m_conn_handle_map.find(conn_id);
			if (it_conn_handle_map != m_conn_handle_map.end())
			{
				std::shared_ptr<NetworkProcessPack> pack = it_conn_handle_map->second;
				boost::shared_lock<boost::shared_mutex> read_lock(pack->m_event_handle_map_shared_mutex);
				typedef NetworkProcessPack::EVENT_HANDLE_MAP::iterator EVENT_HANDLE_IT;
				EVENT_HANDLE_IT it_event_handle = pack->m_event_handle_map.find(event_flag);
				if (it_event_handle != pack->m_event_handle_map.end())
				{
					return it_event_handle->second;
				}
			}
		}
	}
	return std::shared_ptr<IEventHandle>();
}

int CNetworkService::process_network_fd(int epfd)
{
	struct epoll_event evt;
	memset(&evt, 0, sizeof(evt));
	//监控文件描述符的可读,发生错误,可写, 关闭的事件
	evt.events = EPOLLIN | EPOLLERR | EPOLLOUT | EPOLLRDHUP;

	//判断空时不需要加锁 if语句里面进行加锁 效率更高
	if (!m_wait_join_fd_list.empty())
	{//监控需要加入监控列表的fd
		std::unique_lock<std::mutex> write_lock(m_wait_join_fd_list_mutex);
		for (std::list<int>::iterator it = m_wait_join_fd_list.begin(), end_it = m_wait_join_fd_list.end();
					it != end_it;)
		{
			int fd = *it;
			evt.data.fd = fd;
			if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) >= 0)
			{
				m_wait_join_fd_list.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}
	
	//判断空时不需要加锁 if语句里面进行加锁 效率更高
	if (!m_wait_remove_fd_list.empty())
	{//删除需要删除的监控列表中的fd
		std::unique_lock<std::mutex> write_lock(m_wait_remove_fd_list_mutex);
		for (std::list<int>::iterator it = m_wait_remove_fd_list.begin(), end_it = m_wait_remove_fd_list.end();
					it != end_it;)
		{
			int fd = *it;
			evt.data.fd = fd;
			if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &evt) >= 0)
			{
				m_wait_remove_fd_list.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	return 0;

}

int CNetworkService::process_monitor_network_pack()
{

	//while (true)
	{
	//	std::cout << "测试" << std::endl;		
	
	}
	return 0;	
}


int CNetworkService::client_socket()
{
	int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd < 0)
	{
		return -1;
	}
	return sock_fd;
}

int CNetworkService::client_connect(int sock_fd, const char* server_ip, short port)
{
	sockaddr_in ser_addr;
	bzero(&ser_addr, sizeof(ser_addr));
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_port = htons(port);
	ser_addr.sin_addr.s_addr = inet_addr(server_ip);
	if (::connect(sock_fd, (sockaddr*)&ser_addr, sizeof(ser_addr)) < 0)
	{
		return -1;
	}

	return 0;
}

unsigned int CNetworkService::connect(const char* ip_addr, short port)
{
	int client_fd = this->client_socket();
	if (client_fd < 0)
	{
		return -1;
	}
	if (client_connect(client_fd, ip_addr, port) < 0)
	{
		return -1;
	}
	unsigned int conn_id = -1;
	while ((conn_id = generate_connect_id()) == 0xffffffff)
	{
		;
	}

	{
		bool result = false;
		{
			boost::unique_lock<boost::shared_mutex> write_lock(m_fd_conn_map_shared_mutex);
			//todo 返回值后续判断
			result = m_fd_conn_map.insert(FD_CONNECT_MAP::value_type(client_fd, conn_id)).second;
		}
		
		if (result)
		{
			boost::unique_lock<boost::shared_mutex> write_lock(m_conn_handle_map_shared_mutex);
			std::shared_ptr<NetworkProcessPack> net_pack_ptr(new (std::nothrow)NetworkProcessPack(conn_id));
			if (net_pack_ptr)
			{
				if (m_conn_handle_map.insert(CONN_NETWORK_MAP::value_type(conn_id, net_pack_ptr)).second)
				{//插入成功
					return conn_id;
				}
			}	
		}
	}

	return -1;

}
	
unsigned int CNetworkService::register_event_handle(unsigned int conn_id, EventFlag event_flag, IEventHandle* event_handle, bool auto_release)
{
	if (event_handle == nullptr)
	{
		return -1;
	}
	{
		boost::shared_lock<boost::shared_mutex>   read_lock(m_conn_handle_map_shared_mutex);
		CONN_NETWORK_MAP::iterator it_conn_map = m_conn_handle_map.find(conn_id);
		if (it_conn_map != m_conn_handle_map.end())
		{
			std::shared_ptr<IEventHandle> p_event_handle;
			if (auto_release)
			{
				p_event_handle.reset(event_handle);
			}
			else
			{
				p_event_handle.reset(event_handle, [](IEventHandle*){});
			}
			{
				boost::unique_lock<boost::shared_mutex>    write_lock(it_conn_map->second->m_event_handle_map_shared_mutex);
				typedef NetworkProcessPack::EVENT_HANDLE_MAP::value_type VALUE_TYPE;
				bool result = it_conn_map->second->m_event_handle_map.insert(VALUE_TYPE(event_flag, p_event_handle)).second;
				return result ? 0 : -1;
			}
		}
	}
	return -1;
}


unsigned int CNetworkService::generate_connect_id()const
{
	static unsigned int start_id = time(NULL);
	return ++start_id;
}


int CNetworkService::send_network_data(unsigned int queue_id)
{
	if (queue_id >= m_send_data_queue.size())
	{
		return -1;
	}
	//获取队列元素
	SSendDataPack data_pack;
	if (m_send_data_queue[queue_id].pop_data(data_pack))
	{
		//获取sockfd
		int sock_fd = -1;
		{
			boost::shared_lock<boost::shared_mutex> read_lock(m_fd_conn_map_shared_mutex);
			auto it = m_fd_conn_map.right.find(data_pack.m_conn_id);
			if (it != m_fd_conn_map.right.end())
			{
				sock_fd = it->second;
			}
		}
		if (sock_fd != -1)
		{//发送数据
			unsigned data_len = data_pack.m_data_buf.size();
			if (write(sock_fd, data_pack.m_data_buf.c_str(), data_len) == data_len)
			{//发送数据失败
				return 0;
			}
			return -1;
		}
	}
	return -1;
}







