

#ifndef ILOG_SYSTEM_HPP__
#define ILOG_SYSTEM_HPP__

class ILogSystem
{
public:
	virtual ~ILogSystem(){};

public:
	virtual unsigned int init() = 0;
	virtual unsigned int write(const char* log_context) = 0;
};

ILogSystem* get_log_system_instance();


#endif




