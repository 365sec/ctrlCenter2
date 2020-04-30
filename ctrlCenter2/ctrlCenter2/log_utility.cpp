#include <boost/log/trivial.hpp>

#include <boost/log/common.hpp> 
#include <boost/log/expressions.hpp> 
#include <boost/log/attributes.hpp> 
#include <boost/log/sources/logger.hpp> 
#include <boost/log/support/date_time.hpp>
#include <boost/log/sinks/sync_frontend.hpp> 
#include <boost/log/sinks/text_file_backend.hpp> 
#include <boost/log/trivial.hpp>

#include "global.h"

namespace logging = boost::log; 
namespace attrs = boost::log::attributes; 
namespace src = boost::log::sources; 
namespace sinks = boost::log::sinks; 
namespace expr = boost::log::expressions; 
namespace keywords = boost::log::keywords;

#include "log_utility.h"

#define MAX_PATH  1024


#ifdef WIN32
#define DIRECTORY_SEPARATOR_STR "\\"
#else
#define DIRECTORY_SEPARATOR_STR "/"
#endif

int initLog(const char *appPath)
{
	char tmpStr[MAX_PATH];

	// init log utility
	try 
	{ 
		// Create a text file sink 
#ifndef CFG_MDL_UPGRADE_SVR
		sprintf(tmpStr, "%slogs%s", appPath, DIRECTORY_SEPARATOR_STR);
#else
		sprintf(tmpStr, "%supdate_logs%s", appPath, DIRECTORY_SEPARATOR_STR);
#endif
		strcat(tmpStr, "%Y%m%d%H%M%S_%5N.log");
		typedef sinks::synchronous_sink<sinks::text_file_backend> file_sink; 
		boost::shared_ptr<file_sink> sink(new file_sink(
			keywords::open_mode = std::ios::app,
			keywords::auto_flush = true,
			keywords::file_name = tmpStr,      // file name pattern
			keywords::rotation_size = 100*1024*1024	// rotation size, in characters
			)); 

		sprintf(tmpStr, "%slogs", appPath);
		// Set up where the rotated files will be stored 
		sink->locked_backend()->set_file_collector(sinks::file::make_collector( 
			keywords::target = tmpStr,                          // where to store rotated files 
			keywords::max_size = 380 * 1024 * 1024,              // maximum total size of the stored files, in bytes 
			keywords::min_free_space = 10 * 1024 * 1024        // minimum free space on the drive, in bytes
			)); 

		// Upon restart, scan the target directory for files matching the file_name pattern 
		sink->locked_backend()->scan_for_files(); 

		sink->set_formatter(
			expr::format("%1%: [%2%] - %3%") 
			% expr::attr< unsigned int >("RecordID") 
			% expr::attr< boost::posix_time::ptime >("TimeStamp") 
			% expr::smessage
			); 

		// Add it to the core 
		logging::core::get()->add_sink(sink); 

		// Add some attributes too 
		logging::core::get()->add_global_attribute("TimeStamp", attrs::local_clock()); 
		logging::core::get()->add_global_attribute("RecordID", attrs::counter< unsigned int >()); 

		logging::core::get()->set_filter(boost::log::trivial::severity >= g_log_level);
		
		return 0;
	} 
	catch (std::exception &) 
	{ 
		printf("init boost::log failed\n");
		return -1;
	}
}

void log_fatal(const char *format, ...)
{
	char buf[4096] = {0};
	va_list ap;

	va_start(ap, format);
	vsnprintf (buf, 4096, format, ap);
	va_end(ap);

	try
	{
		BOOST_LOG_TRIVIAL(fatal) << buf;
	}
	catch(std::exception &)
	{
	}
}

void log_error(const char *format, ...)
{
	char buf[4096] = {0};
	va_list ap;

	va_start(ap, format);
	vsnprintf (buf, 4096, format, ap);
	va_end(ap);

	try
	{
		BOOST_LOG_TRIVIAL(error) << buf;
	}
	catch(std::exception &)
	{
	}
}

void log_warning(const char *format, ...)
{
	char buf[4096] = {0};
	va_list ap;

	va_start(ap, format);
	vsnprintf (buf, 4096, format, ap);
	va_end(ap);
	try
	{
		BOOST_LOG_TRIVIAL(warning) << buf;
	}
	catch(std::exception &)
	{
	}
}

void log_info(const char *format, ...)
{
	char buf[4096] = {0};
	va_list ap;

	va_start(ap, format);
	vsnprintf (buf, 4096, format, ap);
	va_end(ap);

	try
	{
		BOOST_LOG_TRIVIAL(info) << buf;
	}
	catch(std::exception &)
	{
	}
}

void log_debug(const char *format, ...)
{
	char buf[4096] = {0};
	va_list ap;

	va_start(ap, format);
	vsnprintf (buf, 4096, format, ap);
	va_end(ap);

	try
	{
		BOOST_LOG_TRIVIAL(debug) << buf;
	}
	catch(std::exception &)
	{
	}
}

void log_trace(const char *format, ...)
{
	char buf[4096] = {0};
	va_list ap;

	va_start(ap, format);
	vsnprintf (buf, 4096, format, ap);
	va_end(ap);

	try
	{
		BOOST_LOG_TRIVIAL(trace) << buf;
	}
	catch(std::exception &)
	{
	}
}
