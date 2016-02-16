
// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorFileSystem.cpp
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/07
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#include "MonitorFileSystem.h"
#include "file_node/DirectoryNode.h"
#include "MonitorFileNodeManager.h"
#include "NetworkPack.h"
#include <unistd.h>
#include <sys/inotify.h>
#include <stack>
#include <vector>
#include <functional>
#include <sys/epoll.h>
#include <errno.h>
#include <iostream>
#include <sys/fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <functional>
#include <thread>
#include "FileSystem.h"
#include "thread_pool/ThreadPool.h"
#include "data_queue/MonitorFileSystemDataQueue.h"
#include <future>
#include "../public/log_system/LogManagerSystem.h"
#include <errno.h>
#include <string.h>
CMonitorFileSystem::CMonitorFileSystem()
{
	m_node_mgr_ptr.reset(new(std::nothrow)CMonitorFileNodeManager());
}

CMonitorFileSystem::~CMonitorFileSystem()
{

}

unsigned int CMonitorFileSystem::init()
{
	std::string node_mgr_tag = "monitor_file_system";
	std::string node_tag = "monitor_directory";
	if (THREAD_POOL->create_thread_node_mgr(node_mgr_tag.c_str()) != 0xffffffff)
	{//创建线程节点管理成功
		if (THREAD_POOL->create_thread_node(node_mgr_tag.c_str(), node_tag.c_str(), 2) != 0xffffffff)
		{//创建2个线程的节点成功
			//注册任务
			//1. 注册循环事件监控任务start
			CThreadTask* start_task = new (std::nothrow)CThreadTask();
			if (start_task == nullptr)
			{
				return -1;
			}
			start_task->reset(std::bind(&CMonitorFileSystem::start, this));
			THREAD_POOL->register_thread_task(node_mgr_tag.c_str(), node_tag.c_str(), start_task, true);

			CThreadTask* check_task = new (std::nothrow)CThreadTask();
			if (check_task == nullptr)
			{
				return -1;
			}
			
			auto check_thread_fun = [&]() 
			{
				while (true)
				{
					int num = this->create_notify_fd_by_invalid_path();
					sleep(2);
				}
			};
			check_task->reset(check_thread_fun);
			THREAD_POOL->register_thread_task(node_mgr_tag.c_str(), node_tag.c_str(), check_task, true);
		}
	}
	return -1;
}

MonitorStatus CMonitorFileSystem::add_monitor_node(const char* path, bool is_recursive)
{
	MonitorStatus mon_status = MonitorStatus::EN_MONITOR_ERROR;
	do
	{
		if (path == nullptr)
		{
			WRITE_ERROR_LOG("监控路径为空!!!");
			mon_status = MonitorStatus::EN_MONITOR_ERROR;
			break;
		}
		WRITE_INFO_LOG("正在增加监控路径[%s]", path);

		m_inotify_set.insert(std::string(path));
		if (!FILE_SYSTEM_POINTER->file_exists(path))
		{//文件或目录不存在
			m_invalid_inotify_set.insert(std::string(path));
			WRITE_ERROR_LOG("监控的路径[%s]不存在", path);
			break;
		}
		int fd = this->create_notify_fd();
		if (fd < 0)
		{
			mon_status = MonitorStatus::EN_MONITOR_ERROR;
			WRITE_ERROR_LOG("创建通知句柄失败,错误信息[%s]", strerror(errno));
			break;
		}

		{//加入等待监控列表
			std::unique_lock<std::mutex> write_lock(m_wait_join_fd_mutex);
			m_list_wait_join_fd.push_front(fd);
		}

		m_event_list.push_front(InotifyEvent(EPOLL_CTL_ADD, fd));
		mon_status = this->exe_watch_file_node(fd, path, is_recursive);
	}while (false);

	return mon_status;
}

int CMonitorFileSystem::create_notify_fd()
{
	int fd = inotify_init();
	if (fd >= 0)
	{
		int flag = fcntl(fd, F_GETFL);
		if (flag < 0)
		{
			//todo 打印相应的日志
		}
		flag |= O_NONBLOCK;
		if (fcntl(fd, F_SETFL, flag) < 0)
		{
			//todo 打印相应的日志
		}
	}
	return fd;
}


MonitorStatus CMonitorFileSystem::exe_watch_file_node(int fd, const char* name, bool is_recursive)
{
	MonitorStatus mon_status = MonitorStatus::EN_MONITOR_ERROR;
	do
	{
		if (name == nullptr)
		{
			mon_status = MonitorStatus::EN_MONITOR_ERROR;
			WRITE_ERROR_LOG("监控路径为空!!!");
			break;
		}
		
		if (is_recursive)
		{
			//递归增加文件节点
			struct DirPack
			{
				DirPack(const std::string& name, int parent_wd)
					: m_name(name), m_parent_wd(parent_wd){}
				std::string m_name;
				int         m_parent_wd;
			};
			std::stack<DirPack> file_stack;
			file_stack.push(DirPack(name, -1));

			auto lambda_fun = [&file_stack](const std::string& name, bool is_dir, int parent_wd)->bool
			{
				if (is_dir)
				{
					file_stack.push(DirPack(name, parent_wd));
					return true;
				}
				return false;
			};
			
			DirPack pack("", -1);
			while (!file_stack.empty())
			{
				pack = file_stack.top();
				file_stack.pop();

				int wd = this->add_watch(fd, pack.m_name, pack.m_parent_wd);
				
				this->read_dir(pack.m_name, std::bind(lambda_fun, std::placeholders::_1, std::placeholders::_2, wd));
			}
		}
		else
		{
			this->add_watch(fd, name, -1);				
		}
		
		mon_status = MonitorStatus::EN_MONITOR_SUCCESS;
	}while (false);

	return mon_status;
}

int CMonitorFileSystem::add_watch(int fd, const std::string& full_path, int parent_wd)
{
	if (fd < 0)
	{
		return -1;
	}

	int wd = inotify_add_watch(fd, full_path.c_str(), IN_ALL_EVENTS);
	
	if (wd < 0)
	{
		WRITE_ERROR_LOG("创建监控句柄失败, 监控路径[%s], 错误信息[%s]", full_path.c_str(), strerror(errno));
	}

	if (wd > 0 && m_node_mgr_ptr)
	{
		m_node_mgr_ptr->insert_file_node(fd, parent_wd, wd, full_path.c_str());
	}

	return wd;
}

MonitorStatus CMonitorFileSystem::start()
{
	WRITE_INFO_LOG("监控文件启动....");
    int epfd = epoll_create(1000000);
	if (epfd < 0)
	{
		WRITE_ERROR_LOG("创建epoll句柄失败,失败信息[%s]", strerror(errno));
		//todo 打印日志
		return MonitorStatus::EN_MONITOR_ERROR;
	}
	std::vector<epoll_event> vec_evt;
	char buf[1024] = {0};
	int evt_cnt = 0;
	while (true)
	{
		//增加，删除监控中的文件
		this->process_epoll_fd(epfd);

		{
			boost::shared_lock<boost::shared_mutex> read_lock(m_finsh_fd_shared_mutex);
			if (m_set_finsh_fd.empty())
			{//没有相应的监控时间
				sleep(1);
				continue;
			}
		}

		vec_evt.resize(m_set_finsh_fd.size());
		int evt_cnt = epoll_wait(epfd, &vec_evt[0], vec_evt.size(), 0x8fffffff);
		if (evt_cnt < 0)
		{
			//todo 打印日志
			if (errno == EINTR)
			{//被信号终端
				continue;
			}
			else
			{//有错误发生
				break;
			}
		}
		std::cout << vec_evt.size() << std::endl;
		std::cout << evt_cnt << std::endl;
		this->process_event(&vec_evt[0], evt_cnt);
	}
	close(epfd);
	return MonitorStatus::EN_MONITOR_SUCCESS;
}

unsigned int CMonitorFileSystem::process_epoll_fd(int& epfd)
{
	struct epoll_event evt;
	memset(&evt, 0, sizeof(evt));
	//监控文件描述符的可读与发生错误的事件
	evt.events = EPOLLIN | EPOLLERR;

	std::list<int> join_fd_succees_list;
	{//监控需要加入监控列表的fd
		std::unique_lock<std::mutex> write_lock(m_wait_join_fd_mutex);
		for (std::list<int>::iterator it = m_list_wait_join_fd.begin(), end_it = m_list_wait_join_fd.end();
					it != end_it;)
		{
			int fd = *it;
			evt.data.fd = fd;
			std::cout << fd << std::endl;
			if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &evt) >= 0)
			{
				join_fd_succees_list.push_front(fd);
				m_list_wait_join_fd.erase(it++);
			}
			else
			{
				++it;
			}
		}
	}

	
	std::list<int> delete_fd_success_fd;
	{//删除需要删除的监控列表中的fd
		std::unique_lock<std::mutex> write_lock(m_wait_delete_fd_mutex);
		for (std::list<int>::iterator it = m_list_wait_delete_fd.begin(), end_it = m_list_wait_delete_fd.end();
					it != end_it;)
		{
			int fd = *it;
			evt.data.fd = fd;
			if (epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &evt) >= 0)
			{
				delete_fd_success_fd.push_front(fd);
				m_list_wait_delete_fd.erase(it++);
			}
			else
			{
				++it;
			}
		}
		m_list_wait_delete_fd.clear();
	}

	{//将加入成功的fd加入到已经加入的监控列表中 将删除成功的fd从监控列表中删除
		boost::unique_lock<boost::shared_mutex> write_lock(m_finsh_fd_shared_mutex);
		for (auto& fd : join_fd_succees_list)
		{
			if (!m_set_finsh_fd.insert(fd).second)
			{//插入失败
				;
			}
		}
		for (auto& fd : delete_fd_success_fd)
		{
			if (m_set_finsh_fd.erase(fd) == 0)
			{//移除失败
				;
			}
			close(fd);
		}
	}
	return 0;
}

void CMonitorFileSystem::process_event(const epoll_event* evt, int evt_cnt)
{
	std::list<std::future<bool> > list_future;
	for (int i = 0; i < evt_cnt; ++i)
	{//处理有事件发生的文件句柄
		if (evt[i].events & EPOLLERR)
		{//文件描述符发生错误
			//todo 打印日志
				
		}

		if (evt[i].events & EPOLLIN)
		{//有读事件产生 异步执行
			list_future.push_back(std::async(std::bind(&CMonitorFileSystem::process_fd_data, this, evt[i].data.fd)));		
		}
	}

	//等待所有线程返回
	for (auto& fut : list_future)
	{
		fut.wait();
	}
	
}



bool CMonitorFileSystem::process_fd_data(int fd)
{
	int queue_size = 0;
	if (ioctl(fd, FIONREAD, &queue_size) < 0)
	{//fd管理的文件对象没有数据可以读 直接返回
		return false;
	}
	std::vector<char> vec_buf(queue_size, '\0');
	size_t len = read(fd, &vec_buf[0], queue_size);
	if (len == -1)
	{//
		if (errno == EAGAIN)
		{//没有数据准备好

		}
		else if (errno == EINTR)
		{//被信号中断

		}
		else
		{//有错误发生
			//todo 打印日志
		}
	}
	else
	{
		int start = 0;
		while (start < queue_size)
		{
			//组包
			struct inotify_event* inotify_evt = (struct inotify_event*)&vec_buf[start];
			std::shared_ptr<MonitorDataPack> data_pack = this->build_network_pack(fd, inotify_evt);
			if (data_pack)
			{
				MONITOR_DATA_QUEUE->push_network_pack(data_pack);
			}
		
			start += sizeof(inotify_event) + inotify_evt->len;
		}
	}

	return true;
}



std::shared_ptr<MonitorDataPack> CMonitorFileSystem::build_network_pack(int fd, const inotify_event* evt)
{
	std::shared_ptr<MonitorDataPack> data_pack;
	
	do
	{
		if (evt == nullptr)
		{
			break;
		}

		bool is_dir = evt->mask & IN_ISDIR;

		std::string path;
		if (m_node_mgr_ptr)
		{
			std::weak_ptr<IFileNode> file_node = m_node_mgr_ptr->get_file_node(fd, evt->wd);
			if (file_node.lock())
			{
				path = file_node.lock()->get_name();
			}
		}

		if (evt->mask & IN_CREATE)
		{
			data_pack = this->build_create_event_pack(path.c_str(), evt->name, is_dir);			
			std::cout << "create " << evt->name << std::endl;
			break;
		}
		
		if (evt->mask & IN_DELETE)
		{
			data_pack = this->build_delete_event_pack(path.c_str(), evt->name, is_dir);			
			std::cout << "delete " << evt->name << std::endl;
			break;
		}

		if (evt->mask & IN_OPEN)
		{
			data_pack = this->build_open_event_pack(path.c_str(), evt->name, is_dir);			
			std::cout << "open " << evt->name << std::endl;
			break;	
		}

		if (evt->mask & IN_MODIFY)
		{
			data_pack = this->build_modify_event_pack(path.c_str(), evt->name, is_dir);			
			std::cout << "modify " << evt->name << std::endl;
			break;	
		}

		if (evt->mask & IN_ACCESS)
		{
			data_pack = this->build_access_event_pack(path.c_str(), evt->name, is_dir);			
			std::cout << "access " << evt->name << std::endl;
			break;	
		}

		if (evt->mask & IN_ATTRIB)
		{
			data_pack = this->build_attrib_event_pack(path.c_str(), evt->name, is_dir);			
			std::cout << "attrib " << evt->name << std::endl;
			break;	
		}

		if (evt->mask & IN_MOVED_FROM)
		{
			std::cout << "move from " << evt->name << std::endl;
			break;
		}

		if (evt->mask & IN_MOVED_TO)
		{
			std::cout << "move to " << evt->name << std::endl;
			break;
		}

		if (evt->mask & IN_MOVE_SELF)
		{
			std::cout << "move self " << evt->name << std::endl;
			break;
		}

		if (evt->mask & IN_DELETE_SELF)
		{
			std::cout << "delete self " << evt->name << std::endl;
			break;
		}

		if (evt->mask & IN_CLOSE_WRITE)
		{//关闭文件，写数据
			std::cout << "close write " << evt->name << std::endl;
			break;
		}

		if (evt->mask & IN_CLOSE_NOWRITE)
		{//关闭文件，没有写数据
			std::cout << "close nowrite " << evt->name << std::endl;
			break;
		}


	}while(false);

	return data_pack;
}



std::shared_ptr<MonitorDataPack> CMonitorFileSystem::build_create_event_pack(const char* path, const char* name, bool is_dir)
{
	std::shared_ptr<MonitorDataPack> data_pack;
	do
	{
		if (path == nullptr || name == nullptr)
		{
			break;
		}
		std::string full_path(path);
		full_path += "/";
		full_path += name;
		
		int total_len = sizeof(MonitorDataPack) + full_path.size();
		char* buf = new(std::nothrow)char[total_len];
		data_pack = std::shared_ptr<MonitorDataPack>((MonitorDataPack*)buf, [](MonitorDataPack* dp){delete[] (char*)dp;});

		this->get_now_time(data_pack->m_op_date, sizeof(data_pack->m_op_date), "%Y%m%d%H%M%S");
		data_pack->m_head.m_event_id = is_dir ? EN_CREATE_DIRECTORY : EN_CREATE_FILE;
		data_pack->m_head.m_total_len = total_len; 
		strncpy(data_pack->m_context, full_path.c_str(), full_path.size());

	}while (false);

	return data_pack;
}



std::shared_ptr<MonitorDataPack> CMonitorFileSystem::build_delete_event_pack(const char* path, const char* name, bool is_dir)
{
	std::shared_ptr<MonitorDataPack> data_pack;
	do
	{
		if (path == nullptr || name == nullptr)
		{
			break;
		}
		std::string full_path(path);
		full_path += "/";
		full_path += name;
		
		int total_len = sizeof(MonitorDataPack) + full_path.size();
		char* buf = new(std::nothrow)char[total_len];
		data_pack = std::shared_ptr<MonitorDataPack>((MonitorDataPack*)buf, [](MonitorDataPack* dp){delete[] (char*)dp;});

		this->get_now_time(data_pack->m_op_date, sizeof(data_pack->m_op_date), "%Y%m%d%H%M%S");
		data_pack->m_head.m_event_id = is_dir ? EN_DELETE_DIRECTORY : EN_DELETE_FILE;
		data_pack->m_head.m_total_len = total_len; 
		strncpy(data_pack->m_context, full_path.c_str(), full_path.size());

	}while (false);

	return data_pack;
}


std::shared_ptr<MonitorDataPack> CMonitorFileSystem::build_open_event_pack(const char* path, const char* name, bool is_dir)
{
	std::shared_ptr<MonitorDataPack> data_pack;
	do
	{
		if (path == nullptr || name == nullptr)
		{
			break;
		}
		std::string full_path(path);
		full_path += "/";
		full_path += name;
		
		int total_len = sizeof(MonitorDataPack) + full_path.size();
		char* buf = new(std::nothrow)char[total_len];
		data_pack = std::shared_ptr<MonitorDataPack>((MonitorDataPack*)buf, [](MonitorDataPack* dp){delete[] (char*)dp;});

		this->get_now_time(data_pack->m_op_date, sizeof(data_pack->m_op_date), "%Y%m%d%H%M%S");
		data_pack->m_head.m_event_id = is_dir ? EN_OPEN_DIRECTORY : EN_OPEN_FILE;
		data_pack->m_head.m_total_len = total_len; 
		strncpy(data_pack->m_context, full_path.c_str(), full_path.size());

	}while (false);	
	
	return data_pack;
}

std::shared_ptr<MonitorDataPack> CMonitorFileSystem::build_modify_event_pack(const char* path, const char* name, bool is_dir)
{
	std::shared_ptr<MonitorDataPack> data_pack;
	do
	{
		if (path == nullptr || name == nullptr)
		{
			break;
		}
		std::string full_path(path);
		full_path += "/";
		full_path += name;
		
		int total_len = sizeof(MonitorDataPack) + full_path.size();
		char* buf = new(std::nothrow)char[total_len];
		data_pack = std::shared_ptr<MonitorDataPack>((MonitorDataPack*)buf, [](MonitorDataPack* dp){delete[] (char*)dp;});

		this->get_now_time(data_pack->m_op_date, sizeof(data_pack->m_op_date), "%Y%m%d%H%M%S");
		data_pack->m_head.m_event_id = is_dir ? EN_MODIFY_DIRECTORY : EN_MODIFY_FILE;
		data_pack->m_head.m_total_len = total_len; 
		strncpy(data_pack->m_context, full_path.c_str(), full_path.size());

	}while (false);	
	
	return data_pack;	
}


std::shared_ptr<MonitorDataPack> CMonitorFileSystem::build_access_event_pack(const char* path, const char* name, bool is_dir)
{
	std::shared_ptr<MonitorDataPack> data_pack;
	do
	{
		if (path == nullptr || name == nullptr)
		{
			break;
		}
		std::string full_path(path);
		full_path += "/";
		full_path += name;
		
		int total_len = sizeof(MonitorDataPack) + full_path.size();
		char* buf = new(std::nothrow)char[total_len];
		data_pack = std::shared_ptr<MonitorDataPack>((MonitorDataPack*)buf, [](MonitorDataPack* dp){delete[] (char*)dp;});

		this->get_now_time(data_pack->m_op_date, sizeof(data_pack->m_op_date), "%Y%m%d%H%M%S");
		data_pack->m_head.m_event_id = is_dir ? EN_ACCESS_DIRECTORY : EN_ACCESS_FILE;
		data_pack->m_head.m_total_len = total_len; 
		strncpy(data_pack->m_context, full_path.c_str(), full_path.size());

	}while (false);	
	
	return data_pack;	
}


std::shared_ptr<MonitorDataPack> CMonitorFileSystem::build_attrib_event_pack(const char* path, const char* name, bool is_dir)
{
	std::shared_ptr<MonitorDataPack> data_pack;
	do
	{
		if (path == nullptr || name == nullptr)
		{
			break;
		}
		std::string full_path(path);
		full_path += "/";
		full_path += name;
		
		int total_len = sizeof(MonitorDataPack) + full_path.size();
		char* buf = new(std::nothrow)char[total_len];
		data_pack = std::shared_ptr<MonitorDataPack>((MonitorDataPack*)buf, [](MonitorDataPack* dp){delete[] (char*)dp;});
		this->get_now_time(data_pack->m_op_date, sizeof(data_pack->m_op_date), "%Y%m%d%H%M%S");
		data_pack->m_head.m_event_id = is_dir ? EN_ATTRIB_DIRECTORY : EN_ATTRIB_FILE;
		data_pack->m_head.m_total_len = total_len; 
		strncpy(data_pack->m_context, full_path.c_str(), full_path.size());

	}while (false);	
	
	return data_pack;	
}

char* CMonitorFileSystem::get_now_time(char* now_time, int maxsize, const char* format)
{
	if (now_time == nullptr)
	{
		return nullptr;
	}

	time_t now_time_t = time(nullptr);
	tm tm_now_time;
	localtime_r(&now_time_t, &tm_now_time);

	strftime(now_time, maxsize, format, &tm_now_time);
	
	return now_time;
}



int CMonitorFileSystem::create_notify_fd_by_invalid_path()
{
	std::list<std::string> list_inotify_path;
	for (auto& full_path : m_invalid_inotify_set)
	{
		if (FILE_SYSTEM_POINTER->file_exists(full_path.c_str()))
		{
			if (this->add_monitor_node(full_path.c_str(), true) == MonitorStatus::EN_MONITOR_SUCCESS)
			{
				list_inotify_path.push_front(full_path);
			}
		}
	}

	//删除已经添加的路径
	for (auto& full_path : list_inotify_path)
	{
		m_invalid_inotify_set.erase(full_path);
	}

	return list_inotify_path.size();
}




