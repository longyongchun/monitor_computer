/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorFileSystem.h
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


#ifndef MONITOR_FILE_SYSTEM_HPP__
#define MONITOR_FILE_SYSTEM_HPP__

#include "IMonitorFileSystem.h"
#include <memory>
#include <dirent.h>
#include <string>
#include <list>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <queue>
#include <unordered_set>
#include <boost/thread/thread.hpp>
#include <mutex>
class CMonitorFileNodeManager;
struct MonitorDataPack;
struct epoll_event;
struct inotify_event;
class CMonitorFileSystem : public IMonitorFileSystem
{
public:
	CMonitorFileSystem();
	virtual ~CMonitorFileSystem();

public:
	virtual unsigned int init();
	/**
	 * @brief  增加监控节点
	 *
	 * @param file_node       
	 * @param auto_release  是否自动释放 true:此类进行释放file_node false:此类不进行释放file_node
	 * @param is_recursive  是否递归监视目录 true: 递归监视 fasle: 不递归监视
	 *
	 * @return 
	 */
	virtual MonitorStatus add_monitor_node(const char* path, bool is_recursive);

	virtual MonitorStatus start();
protected:
	int create_notify_fd();
	int add_watch(int fd, const std::string& full_path, int parent_wd);
	template <typename FUNC>
	int read_dir(const std::string& full_path, FUNC fun);
	MonitorStatus exe_watch_file_node(int fd, const char* name, bool is_recursive);

	/**
	 * @brief  处理epoll事件
	 *
	 * @param evt
	 * @param evt_cnt
	 */
	void process_event(const epoll_event* evt, int evt_cnt);

	/**
	 * @brief  处理句柄关联的数据
	 *
	 * @param fd
	 */
	bool process_fd_data(int fd);

	/**
	 * @brief  构建网络数据包
	 *
	 * @param evt
	 *
	 * @return 
	 */
	std::shared_ptr<MonitorDataPack> build_network_pack(int fd, const inotify_event* evt);

	/**
	 * @brief  构建创建事件包
	 *
	 * @param wd
	 * @param name
	 * @param len
	 * @param is_dir
	 *
	 * @return 
	 */
	std::shared_ptr<MonitorDataPack> build_create_event_pack(const char* path, const char* name, bool is_dir);
	std::shared_ptr<MonitorDataPack> build_delete_event_pack(const char* path, const char* name, bool is_dir);
	std::shared_ptr<MonitorDataPack> build_open_event_pack(const char* path, const char* name, bool is_dir);
	std::shared_ptr<MonitorDataPack> build_modify_event_pack(const char* path, const char* name, bool is_dir);
	std::shared_ptr<MonitorDataPack> build_access_event_pack(const char* path, const char* name, bool is_dir);
	std::shared_ptr<MonitorDataPack> build_attrib_event_pack(const char* path, const char* name, bool is_dir);

	/**
	 * @brief  格式化当前时间
	 *
	 * @param now_time
	 * @param maxsize
	 * @param format 参考strftime的格式
	 *
	 * @return 
	 */
	char* get_now_time(char* now_time, int maxsize, const char* format);
	
	/**
	 * @brief  循环遍历无效的监控路径 磁盘上有相应的文件，就将其加入到监控中
	 *
	 * @return  返回加入到监控中的路径数量
	 */
	int create_notify_fd_by_invalid_path();

	/**
	 * @brief 处理epoll句柄 主要是删除，增加监控节点 
	 *
	 * @param epfd
	 *
	 * @return 
	 */
	unsigned int process_epoll_fd(int& epfd);

private:
	std::shared_ptr<CMonitorFileNodeManager> m_node_mgr_ptr;
	
	std::mutex								 m_wait_join_fd_mutex;
	std::list<int> 							 m_list_wait_join_fd;   //等待加入监控列表的fd

	boost::shared_mutex						 m_finsh_fd_shared_mutex;
	std::unordered_set<int>  				 m_set_finsh_fd;       //已经完成加入监控列表的fd

	std::mutex								 m_wait_delete_fd_mutex;
	std::list<int>                           m_list_wait_delete_fd; //等待删除监控列表的fd

	std::unordered_set<std::string>          m_invalid_inotify_set;
	std::unordered_set<std::string>          m_inotify_set;

	struct InotifyEvent
	{
		InotifyEvent(int event, int fd) : m_event(event), m_fd(fd){}
		int m_event; 						//参照epoll_ctl中的op参数值
		int m_fd;
	};
	std::list<InotifyEvent>                m_event_list;

};

template <typename FUNC>
int CMonitorFileSystem::read_dir(const std::string& full_path, FUNC fun)
{
	DIR* pdir = opendir(full_path.c_str());
	if (pdir == nullptr)
	{
		return -1;
	}

	int file_count = 0;
	struct dirent* ent;
	while ((ent = readdir(pdir)) != nullptr)
	{
		if (ent->d_type & DT_DIR)
		{
			if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
			{
				continue;
			}
		}
		if (fun(full_path + "/" + ent->d_name, (ent->d_type & DT_DIR) == DT_DIR))
		{
			++file_count;
		}
	}
	closedir(pdir);
	return file_count;
}


#endif



