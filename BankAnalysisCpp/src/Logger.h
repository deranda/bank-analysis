#pragma once
#include <QString>
#include <QDateTime>
#include <QDebug>

class Logger {
public:
    enum class ELogType {
        eDebug,
        eInfo,
        eWarning,
        eError
    };

    Logger();

    void log(const QString& context, ELogType logType, const QString& message, int line) ;

    QString logTypeToString(ELogType logType) ;
};

// Macro to simplify logging with line number
#define LOG(logger, context, logType, message) logger.log(context, logType, message, __LINE__)
