#ifndef __WINTER_LOG_H__
#define __WINTER_LOG_H__

#include <string>
#include <stdint.h>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
#include "util.h"
#include "singleton.h"


#define WINTER_LOG_LEVEL(logger, level) \
    winter::LogEventWrap(winter::LogEvent::ptr(new winter::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, winter::GetThreadId(),\
                winter::GetFiberId(), time(0)))).getSS()

#define WINTER_LOG_DEBUG(logger) WINTER_LOG_LEVEL(logger, winter::LogLevel::DEBUG)
#define WINTER_LOG_INFO(logger) WINTER_LOG_LEVEL(logger, winter::LogLevel::INFO)
#define WINTER_LOG_WARN(logger) WINTER_LOG_LEVEL(logger, winter::LogLevel::WARN)
#define WINTER_LOG_ERROR(logger) WINTER_LOG_LEVEL(logger, winter::LogLevel::ERROR)
#define WINTER_LOG_FATAL(logger) WINTER_LOG_LEVEL(logger, winter::LogLevel::FATAL)

#define WINTER_LOG_FMT_LEVEL(logger, level, fmt, ...) \
    if(logger->getLevel() <= level) \
        winter::LogEventWrap(winter::LogEvent::ptr(new winter::LogEvent(logger, level, \
                        __FILE__, __LINE__, 0, winter::GetThreadId(),\
                winter::GetFiberId(), time(0)))).getEvent()->format(fmt, __VA_ARGS__)

#define WINTER_LOG_FMT_DEBUG(logger, fmt, ...) WINTER_LOG_FMT_LEVEL(logger, winter::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define WINTER_LOG_FMT_INFO(logger, fmt, ...)  WINTER_LOG_FMT_LEVEL(logger, winter::LogLevel::INFO, fmt, __VA_ARGS__)
#define WINTER_LOG_FMT_WARN(logger, fmt, ...)  WINTER_LOG_FMT_LEVEL(logger, winter::LogLevel::WARN, fmt, __VA_ARGS__)
#define WINTER_LOG_FMT_ERROR(logger, fmt, ...) WINTER_LOG_FMT_LEVEL(logger, winter::LogLevel::ERROR, fmt, __VA_ARGS__)
#define WINTER_LOG_FMT_FATAL(logger, fmt, ...) WINTER_LOG_FMT_LEVEL(logger, winter::LogLevel::FATAL, fmt, __VA_ARGS__)

#define WINTER_LOG_ROOT() winter::LoggerMgr::GetInstance()->getRoot()

namespace winter{

class Logger;

//日志级别
class LogLevel{
    public:
        enum Level{
            UNKNOW = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };

        static const char* ToString(LogLevel::Level level);
};

//日志事件
class LogEvent{
    public:
        typedef std::shared_ptr<LogEvent> ptr;
        LogEvent(std::shared_ptr<Logger> logger, LogLevel::Level level, 
                const char* file, int32_t line, uint32_t elapse,
                uint32_t thread_id, uint32_t fiber_id, uint64_t time);

        const char* getFile() const {return m_file;}
        int32_t getLine() const {return m_line;}
        uint32_t getElapse() const {return m_elapse;}
        uint32_t getThreadId() const {return m_threadId;}
        uint32_t getFiberId() const {return m_fiberId;}
        uint64_t getTime() const {return m_time;}
        std::string getContent() const{return m_ss.str();}
        std::shared_ptr<Logger> getLogger() const {return m_logger;}
        LogLevel::Level getLevel() const {return m_level;}

        std::stringstream& getSS() {return m_ss;}
        void format(const char* fmt, ...);
        void format(const char* fmt, va_list al);
    private:
        std::shared_ptr<Logger> m_logger;
        LogLevel::Level m_level;

        const char* m_file = nullptr;   //文件名
        int32_t m_line = 0;             //行号
        uint32_t m_elapse = 0;          //程序启动开始到现在的毫秒数
        uint32_t m_threadId = 0;        //线程id
        uint32_t m_fiberId = 0;         //协程id
        uint64_t m_time;                //时间戳
        std::stringstream m_ss;

};

class LogEventWrap{
    public:
        LogEventWrap(LogEvent::ptr e);
        ~LogEventWrap();
        LogEvent::ptr getEvent() const { return m_event;}
        std::stringstream& getSS();
    private:
        LogEvent::ptr m_event;
};

//格式器
class LogFormatter{
    public:
        typedef std::shared_ptr<LogFormatter> ptr;
        LogFormatter(const std::string& pattern);

        std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
    public:
        class FormatItem{
            public:
            typedef std::shared_ptr<FormatItem> ptr;
            virtual ~FormatItem(){}
            virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        };
        void init();
    private:
        std::string m_pattern;
        std::vector<FormatItem::ptr> m_items;
};

//日志输出地
class LogAppender{
    public:
        typedef std::shared_ptr<LogAppender> ptr;
        virtual ~LogAppender(){}
        
        virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
        
        void setFormatter(LogFormatter::ptr val){m_formatter = val;}
        LogFormatter::ptr getFormatter() const {return m_formatter;}

        LogLevel::Level getLevel() const { return m_level;}
        void setLevel(LogLevel::Level val) { m_level = val;}
    protected:
        LogLevel::Level m_level = LogLevel::DEBUG;
        LogFormatter::ptr m_formatter;
};

//日志器
class Logger : public std::enable_shared_from_this<Logger>{
    public:
        typedef std::shared_ptr<Logger> ptr;

        Logger(const std::string& name = "root");
        void log(LogLevel::Level level, LogEvent::ptr event);

        void debug(LogEvent::ptr event);
        void info(LogEvent::ptr event);
        void warn(LogEvent::ptr event);
        void fatal(LogEvent::ptr event);
        void error(LogEvent::ptr event);

        void addAppender(LogAppender::ptr appender);
        void deleteAppender(LogAppender::ptr appender);
        LogLevel::Level getLevel() const {return m_level;}
        void setLevel(LogLevel::Level val) {m_level = val;}

        const std::string& getName() const { return m_name;}
    private:
        std::string m_name;                         //日志名称
        LogLevel::Level m_level;                    //日志级别
        std::list<LogAppender::ptr> m_appenders;    //Appender集合
        LogFormatter::ptr m_formatter;
};

//输出控制台
class StdoutLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        void log(Logger::ptr logger, LogLevel::Level, LogEvent::ptr event) override;

};

//输出到文件
class FileLogAppender : public LogAppender{
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        void log(Logger::ptr logger, LogLevel::Level, LogEvent::ptr event) override;
        bool reopen();
    private:
        std::string m_filename;
        std::ofstream m_filestream;
};

class LoggerManager{
public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();
    Logger::ptr getRoot() const { return m_root;}
private:
    std::map<std::string, Logger::ptr> m_loggers;
    Logger::ptr m_root;
};

typedef winter::Singleton<LoggerManager> LoggerMgr;


}

#endif