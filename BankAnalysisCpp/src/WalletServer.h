#pragma once
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QTextStream>
#include <QJsonArray>
#include <QByteArray>
#include "BackendDefs.h"
#include <memory>
#include <QHttpServerResponse>

class WalletServer : public QObject
{
    Q_OBJECT
public:
    explicit WalletServer(QObject* parent = nullptr);
    ~WalletServer();

protected:
    QMap<QString, QVector<QString>> parseJsonToMap(const QByteArray& jsonData);
    QJsonObject categorizeCSV(const defs::EBanks eBank, const QString& filePath);
    QString categorize(const QString& description);

    void addDefaultHeaders(QHttpServerResponse& response);
    QHttpServerResponse getBadRequestResponse(const QString& strMsg);

private slots:
    QHttpServerResponse handlePreflightRequest();

    QHttpServerResponse handleGetSettingsRequest();
    QHttpServerResponse handlePostSettingsRequest(const QHttpServerRequest& request);

    QHttpServerResponse handlePostUploadRequest(const QHttpServerRequest& request);



private:
    struct Members;
    std::unique_ptr<Members> m{};
};