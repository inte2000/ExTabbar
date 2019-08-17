#include "pch.h"
#include "framework.h"
#include "DebugLog.h"
#include "SystemFunctions.h"
#include "log_inc.h"
#include "tchar.h"

class ShlExtLog
{
public:
	static ShlExtLog& GetLogger() { return s_shlExtLogger; }
	static ShlExtLog s_shlExtLogger;

	void Trace(LPCTSTR evtMsg) { LOG4CPLUS_TRACE(m_logger, evtMsg); }
	void Debug(LPCTSTR evtMsg) { LOG4CPLUS_DEBUG(m_logger, evtMsg); }
	void Info(LPCTSTR evtMsg) { LOG4CPLUS_INFO(m_logger, evtMsg); }
    void Warn(LPCTSTR evtMsg) { LOG4CPLUS_WARN(m_logger, evtMsg); }
    void Error(LPCTSTR evtMsg) { LOG4CPLUS_ERROR(m_logger, evtMsg); }
	void FatalErr(LPCTSTR evtMsg) { LOG4CPLUS_FATAL(m_logger, evtMsg); }

protected:
	ShlExtLog() { InitLogSystem(); }
	~ShlExtLog() { UninitLogSystem(); }
	bool InitLogSystem();
	void UninitLogSystem();

protected:
	Logger m_logger;
};

ShlExtLog ShlExtLog::s_shlExtLogger;

bool ShlExtLog::InitLogSystem()
{
	log4cplus::initialize();

//	BasicConfigurator config;
//	config.configure();
	log4cplus::tstring fileName = GetLocalAppDataPath() + _T("\\TabExplorer.log");
	SharedAppenderPtr appender(new RollingFileAppender(fileName, 2 * 1024 * 1024, 5));
	appender->setName(_T("appender for main log"));

    log4cplus::tstring pattern = _T("%D{%Y-%m-%d %H:%M:%S} - %p:%m %n");
	appender->setLayout(std::unique_ptr<Layout>(new PatternLayout(pattern)));
    Logger::getRoot().addAppender(appender);

    m_logger = Logger::getInstance(LOG4CPLUS_TEXT("main"));
	//m_logger.addAppender(appender);

	m_logger.setLogLevel(ALL_LOG_LEVEL);

    LOG4CPLUS_TRACE(m_logger, _T("TabExplorer Shell Extension object starting!"));

    return true;
}

void ShlExtLog::UninitLogSystem()
{
    LOG4CPLUS_TRACE(m_logger, _T("TabExplorer Shell Extension object stoped!\n\n"));

    log4cplus::deinitialize();
}

void LogTrace(LPCTSTR format, ...)
{
    TCHAR evtMsg[256] = { 0 };

    va_list argList;
    va_start(argList, format);
    int len = _vstprintf_s(evtMsg, 256, format, argList);
    va_end(argList);

    if (len > 0)
    {
        ShlExtLog& logger = ShlExtLog::GetLogger();
        logger.Trace(evtMsg);
    }
}

void LogDebug(LPCTSTR format, ...)
{
    TCHAR evtMsg[256] = { 0 };

    va_list argList;
    va_start(argList, format);
    int len = _vstprintf_s(evtMsg, 256, format, argList);
    va_end(argList);

    if (len > 0)
    {
        ShlExtLog& logger = ShlExtLog::GetLogger();
        logger.Debug(evtMsg);
    }
}

void LogInfo(LPCTSTR format, ...)
{
    TCHAR evtMsg[256] = { 0 };

    va_list argList;
    va_start(argList, format);
    int len = _vstprintf_s(evtMsg, 256, format, argList);
    va_end(argList);

    if (len > 0)
    {
        ShlExtLog& logger = ShlExtLog::GetLogger();
        logger.Info(evtMsg);
    }
}

void LogWarn(LPCTSTR format, ...)
{
    TCHAR evtMsg[256] = { 0 };

    va_list argList;
    va_start(argList, format);
    int len = _vstprintf_s(evtMsg, 256, format, argList);
    va_end(argList);

    if (len > 0)
    {
        ShlExtLog& logger = ShlExtLog::GetLogger();
        logger.Warn(evtMsg);
    }
}

void LogError(LPCTSTR format, ...)
{
    TCHAR evtMsg[256] = { 0 };

    va_list argList;
    va_start(argList, format);
    int len = _vstprintf_s(evtMsg, 256, format, argList);
    va_end(argList);

    if (len > 0)
    {
        ShlExtLog& logger = ShlExtLog::GetLogger();
        logger.Error(evtMsg);
    }
}

void LogFatalErr(LPCTSTR format, ...)
{
    TCHAR evtMsg[256] = { 0 };

    va_list argList;
    va_start(argList, format);
    int len = _vstprintf_s(evtMsg, 256, format, argList);
    va_end(argList);

    if (len > 0)
    {
        ShlExtLog& logger = ShlExtLog::GetLogger();
        logger.FatalErr(evtMsg);
    }
}
