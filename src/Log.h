#ifndef _LOG_H
#define _LOG_H

#include <log4cpp/Category.hh>

#define LOG_TRACE(fmt, ...) LOG4CPP_TRACE(logger, log_format(fmt, ## __VA_ARGS__))
#define LOG_DEBUG(fmt, ...) LOG4CPP_DEBUG(logger, log_format(fmt, ## __VA_ARGS__))
#define LOG_INFO(fmt, ...) LOG4CPP_INFO(logger, log_format(fmt, ## __VA_ARGS__))
#define LOG_WARN(fmt, ...) LOG4CPP_WARN(logger, log_format(fmt, ## __VA_ARGS__))
#define LOG_ERROR(fmt, ...) LOG4CPP_ERROR(logger, log_format(fmt, ## __VA_ARGS__))
#define LOG_FATAL(fmt, ...) LOG4CPP_FATAL(logger, log_format(fmt, ## __VA_ARGS__))

const char *log_format(const char *fmt, ...);

#define LOG_INIT(X) \
	static log4cpp::Category& logger(log4cpp::Category::getInstance(X))

#endif 
