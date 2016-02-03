/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorFileNodeManager.h
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

#ifndef MONITOR_FILE_NODE_MANAGER_HPP__
#define MONITOR_FILE_NODE_MANAGER_HPP__

#include <unordered_map>
#include <memory>
#include <list>
#include <unordered_set>
#include <string>
#include "file_node/IFileNode.h"

class CMonitorFileNodeManager
{
public:
	CMonitorFileNodeManager();
	virtual ~CMonitorFileNodeManager();

public:
	/**
	 * @brief 插入节点 
	 *
	 * @param fd
	 * @param parent_wd  -1: 表示根节点
	 * @param wd         
	 * @param path
	 *
	 * @return 
	 */
	bool insert_file_node(int fd, int parent_wd, int wd, const char* path);

	bool file_node_is_exists(const IFileNode* file_node)const;
	
	std::weak_ptr<IFileNode> get_file_node(int fd, int wd)const;

protected:
	bool insert_root_node(int fd, int wd, const char* path);
	bool insert_child_node(int fd, int parent_wd, int wd, const char* path);

protected:
	typedef std::unordered_map<int, std::weak_ptr<IFileNode> > WATCH_FILE_MAP;
	typedef std::unordered_map<int, WATCH_FILE_MAP> NOTIFY_MAP;
	typedef std::list<std::shared_ptr<IFileNode> > FILE_NODE_LIST;

	struct SetEqual
	{
		bool operator()(const std::weak_ptr<IFileNode>& node1, const IFileNode* node2)const
		{
			std::shared_ptr<IFileNode> tmp_node = node1.lock();
			return tmp_node && node2 != nullptr && tmp_node->get_name() == node2->get_name();
		}
		bool operator()(const IFileNode* node1, const std::weak_ptr<IFileNode>& node2)const
		{
			return operator()(node2, node1);
		}
		bool operator()(const std::weak_ptr<IFileNode>& node1, const std::weak_ptr<IFileNode>& node2)const
		{
			std::shared_ptr<IFileNode> tmp_node1 = node1.lock();
			std::shared_ptr<IFileNode> tmp_node2 = node2.lock();
			return tmp_node1 && tmp_node2 && tmp_node1->get_name() == tmp_node2->get_name();
		}
	};

	struct HashFun
	{
		size_t operator()(const std::weak_ptr<IFileNode>& file_node)const
		{
			const std::shared_ptr<IFileNode> node = file_node.lock();
			if (node)
			{
				return std::hash<std::string>()(node->get_name());
			}
			return -1;
		}
	};

	typedef std::unordered_set<std::weak_ptr<IFileNode>, HashFun, SetEqual> FILE_NODE_SET;
	
	struct FileNodePack
	{
		std::weak_ptr<FileNodePack> 			  m_parent_node; //父节点
		std::list<std::shared_ptr<FileNodePack> > m_child_nodes; //孩子节点集
		int 									  m_wd;          //监控句柄
		std::shared_ptr<IFileNode>				  m_file_node;
	};

	typedef std::unordered_map<int, std::shared_ptr<FileNodePack> > FD_NODE_MAP; 
	typedef std::unordered_map<int, std::weak_ptr<FileNodePack> >   WD_NODE_MAP;
private:
	//存储节点的map表
	NOTIFY_MAP m_notify_map;
	//存储节点的集合 主要用于查找节点
	FILE_NODE_SET  m_set_node;
	//底层存储节点的链表
	FILE_NODE_LIST m_list_node;

	FD_NODE_MAP m_fd_node_map;
	WD_NODE_MAP m_wd_node_map;
};

#endif
