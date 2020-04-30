#ifndef LOG_UTILITY_H
#define LOG_UTILITY_H

int initLog(const char *appPath);
void log_fatal(const char *format, ...);
void log_error(const char *format, ...);
void log_warning(const char *format, ...);
void log_info(const char *format, ...);
void log_debug(const char *format, ...);
void log_trace(const char *format, ...);



#endif // #ifndef LOG_UTILITY_H