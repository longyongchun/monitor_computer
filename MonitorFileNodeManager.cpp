/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           MonitorFileNodeManager.cpp
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

#include "MonitorFileNodeManager.h"
#include "file_node/DirectoryNode.h"

CMonitorFileNodeManager::CMonitorFileNodeManager()
{

}
CMonitorFileNodeManager::~CMonitorFileNodeManager()
{

}



bool CMonitorFileNodeManager::insert_file_node(int fd, int parent_wd, int wd, const char* path)
{
	if (parent_wd == -1)
	{
		return this->insert_root_node(fd, wd, path);
	}
	else
	{
		return this->insert_child_node(fd, parent_wd, wd, path);
	}
}

bool CMonitorFileNodeManager::insert_child_node(int fd, int parent_wd, int wd, const char* path)
{
	bool insert_ret = false;
	do
	{
		if (m_fd_node_map.find(fd) == m_fd_node_map.end())
		{//该节点已经存在 
			break;
		}

		if (m_wd_node_map.find(parent_wd) == m_wd_node_map.end())
		{
			break;
		}
		std::shared_ptr<FileNodePack> parent_node_pack = m_wd_node_map[parent_wd].lock();
		if (!parent_node_pack)
		{
			break;
		}
		
		FileNodePack* file_node_pack = new (std::nothrow)FileNodePack;
		if (file_node_pack == nullptr)
		{
			break;
		}
		std::shared_ptr<FileNodePack> node_pack(file_node_pack);
		node_pack->m_wd = wd; 
		node_pack->m_parent_node = parent_node_pack;

		IFileNode* file_node = new(std::nothrow)CDirectoryNode(path);
		if (file_node == nullptr)
		{
			break;
		}
		node_pack->m_file_node.reset(file_node);
		parent_node_pack->m_child_nodes.push_front(node_pack);		

		m_wd_node_map.insert(WD_NODE_MAP::value_type(wd, node_pack));
		insert_ret = true;

	}while (false);
	return insert_ret;
}


bool CMonitorFileNodeManager::insert_root_node(int fd, int wd, const char* path)
{
	bool insert_ret = false;
	do
	{
		if (m_fd_node_map.find(fd) != m_fd_node_map.end())
		{//该节点已经存在 
			break;
		}
		FileNodePack* file_node_pack = new (std::nothrow)FileNodePack;
		if (file_node_pack == nullptr)
		{
			break;
		}
		std::shared_ptr<FileNodePack> node_pack(file_node_pack);
		node_pack->m_wd = wd;
		IFileNode* file_node = new(std::nothrow)CDirectoryNode(path);
		if (file_node == nullptr)
		{
			break;
		}
		node_pack->m_file_node.reset(file_node);
		insert_ret = m_fd_node_map.insert(FD_NODE_MAP::value_type(fd, node_pack)).second;

		m_wd_node_map.insert(WD_NODE_MAP::value_type(wd, node_pack));
	}while (false);

	return insert_ret;
}


bool CMonitorFileNodeManager::file_node_is_exists(const IFileNode* file_node)const
{
	if (file_node == nullptr)
	{
		return false;
	}
	std::shared_ptr<IFileNode> tmp_node(const_cast<IFileNode*>(file_node), [](IFileNode* node){});
	return m_set_node.find(std::weak_ptr<IFileNode>(tmp_node)) != m_set_node.end();
}


std::weak_ptr<IFileNode> CMonitorFileNodeManager::get_file_node(int fd, int wd)const
{
	std::weak_ptr<IFileNode> file_node;
	do
	{
		NOTIFY_MAP::const_iterator it_notify = m_notify_map.find(fd);
	 	if (it_notify == m_notify_map.end())
		{
			break;
		}
		const WATCH_FILE_MAP& watch_map = it_notify->second;
		WATCH_FILE_MAP::const_iterator it_watch_map = watch_map.find(wd);
		if (it_watch_map == watch_map.end())
		{
			break;
		}
		file_node = it_watch_map->second;
	}while (false);

	return file_node;
}




