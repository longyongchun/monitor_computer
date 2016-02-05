/// ****************************************************************************
///
/// ****************************************************************************
///
/// @file           NetworkPack.h
/// @brief          
/// @attention
/// @todo
/// @version        1.0v
/// @author         longyongchun
/// @date           2016/01/09
/// @note
/// <author>        <time>          <version >          <desc> \n
///
/// ****************************************************************************

#ifndef NETWORK_PACK_HPP__
#define NETWORK_PACK_HPP__

#pragma pack(push,1)

enum EventID
{
	EN_UNKNOWN, 			//未知事件
	EN_CREATE_FILE, 		//创建文件
	EN_CREATE_DIRECTORY, 	//创建目录
	EN_DELETE_FILE,		 	//删除文件
	EN_DELETE_DIRECTORY, 	//删除目录
	EN_OPEN_DIRECTORY,		//打开目录
	EN_OPEN_FILE,			//打开文件
	EN_MODIFY_DIRECTORY,	//修改目录
	EN_MODIFY_FILE,			//修改文件
	EN_ACCESS_DIRECTORY,    //阅读目录
	EN_ACCESS_FILE, 		//阅读文件
	EN_ATTRIB_DIRECTORY,    //目录属性被修改
	EN_ATTRIB_FILE,         //文件属性被修改
};

typedef struct NetworkHeadPack
{
	unsigned int m_total_len; //总长度
	unsigned int m_network_event;
	unsigned int m_seq_num;   //序号
}NetworkHeadPack;


//网络包的头
typedef struct MonitorNetworkHead
{
	unsigned int m_total_len;    //总长度，包含头
	unsigned int m_event_id;     //发生的事件ID
}MonitorNetworkHead;

//监控文件的网络数据包
typedef struct MonitorDataPack
{
	MonitorNetworkHead m_head;
	char 			   m_op_date[14];  //yyyymmddhhmmss
	char 	           m_context[0];
}MonitorDataPack;


#endif

