#include "Logger.h"


Logger::Logger() = default;

void Logger::log(const QString& context, ELogType logType, const QString& message, int line) 
{
    QString logTypeStr = logTypeToString(logType);
    QString logMessage = QString("[%1] [%2] [%3]  %4      Line: %5")
        .arg(context)
        .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss"))
        .arg(logTypeStr)
        .arg(message)
        .arg(line);

    qDebug() << logMessage;
}

QString Logger::logTypeToString(ELogType logType) 
{
    switch (logType) {
    case Logger::ELogType::eDebug: return "DEBUG";
    case Logger::ELogType::eInfo: return "INFO";
    case Logger::ELogType::eWarning: return "WARNING";
    case Logger::ELogType::eError: return "ERROR";
    default: return "UNKNOWN";
    }
}