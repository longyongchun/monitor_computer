/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           NetworkServiceBusiness.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/02/02
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#ifndef NETWORK_SERVICE_BUSINESS_HPP__
#define NETWORK_SERVICE_BUSINESS_HPP__

#include "../IBusiness.h"
#include <memory>
#include <list>

class INetworkService;
struct MonitorDataPack;

class CNetworkServiceBusiness : public IBusiness
{
public:
	CNetworkServiceBusiness();
	virtual ~CNetworkServiceBusiness();

public:
	virtual unsigned int init();
	virtual unsigned int start();

private:
	int register_send_data_task(unsigned int conn_id);
	int send_data_task(unsigned int conn_id);

	int build_monitor_network_data(char* buf, unsigned int buf_len, const std::shared_ptr<MonitorDataPack>& data_pack)const;	

private:
	std::shared_ptr<INetworkService> m_net_service;

	std::list<unsigned int> m_conn_list;

	std::string m_node_mgr_tag;
	std::string m_node_tag;
};

#endif




