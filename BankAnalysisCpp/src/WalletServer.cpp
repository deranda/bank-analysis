#include "WalletServer.h"
#include <QDebug>
#include <Logger.h>
#include <QHttpServerResponse>
#include <QHttpServer>

struct WalletServer::Members
{
	Logger lg{};
	const QString strCatsPath = qApp->applicationDirPath() + "/categories.json";
	QMap<QString, QVector<QString>> mpCategories;
	QHttpServer server{};
	QTcpServer tcpServer{};
};

WalletServer::WalletServer(QObject* parent /*= nullptr*/)
	: QObject(parent)
	, m(new Members)
{
	// Read config
	QFile file(m->strCatsPath);
	if (file.open(QIODevice::ReadOnly)) {
		QByteArray settingsData = file.readAll();
		file.close();
		m->mpCategories = parseJsonToMap(settingsData);
	}

	// route URLs
	m->server.route("/*", QHttpServerRequest::Method::Options, this, &WalletServer::handlePreflightRequest);
	m->server.route("/settings", QHttpServerRequest::Method::Options, this, &WalletServer::handlePreflightRequest);
	m->server.route("/settings", QHttpServerRequest::Method::Get, this, &WalletServer::handleGetSettingsRequest);
	m->server.route("/settings", QHttpServerRequest::Method::Post, this, &WalletServer::handlePostSettingsRequest);
	m->server.route("/upload", QHttpServerRequest::Method::Options, this, &WalletServer::handlePreflightRequest);
	m->server.route("/upload", QHttpServerRequest::Method::Post, this, &WalletServer::handlePostUploadRequest);

	// start server
	if (!m->tcpServer.listen(QHostAddress::Any, 8080) || !m->server.bind(&m->tcpServer)) 
	{
		qCritical() << "Failed to bind to port 8080";
		return;
	}
	QString lgMsg = "Server start listeing on port 8080.";
	LOG(m->lg, "WalletServer", Logger::ELogType::eInfo, lgMsg);
	
}

WalletServer::~WalletServer() = default;

Q_SLOT QHttpServerResponse WalletServer::handlePreflightRequest()
{
	QHttpServerResponse resp("", QHttpServerResponse::StatusCode::NoContent);
	addDefaultHeaders(resp);
	return resp;
}

/* -----------------------------------------------------------------------
   ---------------------- handle Settings --------------------------------
   ----------------------------------------------------------------------- */
Q_SLOT QHttpServerResponse WalletServer::handleGetSettingsRequest()
{
	QString lgMsg = "Handle Get Request."; 
	LOG(m->lg, "WalletServer", Logger::ELogType::eInfo, lgMsg);

	QFile file(m->strCatsPath);
	if (file.open(QIODevice::ReadOnly)) 
	{
		QByteArray settingsData = file.readAll();
		file.close();
		m->mpCategories = parseJsonToMap(settingsData);
		lgMsg = "loaded Categories from:" + m->strCatsPath;
		LOG(m->lg, "WalletServer", Logger::ELogType::eInfo, lgMsg);
		QHttpServerResponse resp(settingsData, QHttpServerResponse::StatusCode::Ok);
		addDefaultHeaders(resp);
		return resp;
	}
	QHttpServerResponse resp("Categories not found", QHttpServerResponse::StatusCode::BadRequest);
	addDefaultHeaders(resp);
	return resp;
}

Q_SLOT QHttpServerResponse WalletServer::handlePostSettingsRequest(const QHttpServerRequest& request)
{
	QJsonDocument jsonDoc = QJsonDocument::fromJson(request.body());
	if (!jsonDoc.isNull()) {
		QFile file(m->strCatsPath); 
		if (file.open(QIODevice::WriteOnly)) { 
			file.write(jsonDoc.toJson()); 
			file.close();
			m->mpCategories = parseJsonToMap(request.body());
			QHttpServerResponse resp("Saved Categories", QHttpServerResponse::StatusCode::Ok);
			addDefaultHeaders(resp);
			return resp;
		}
	}
	QHttpServerResponse resp("Invalid types.", QHttpServerResponse::StatusCode::BadRequest);
	addDefaultHeaders(resp);
	return resp;
}

/* -----------------------------------------------------------------------
   --------------------- handle Fileupload -------------------------------
   ----------------------------------------------------------------------- */
Q_SLOT QHttpServerResponse WalletServer::handlePostUploadRequest(const QHttpServerRequest& request)
{
	QString data = QString(request.body());
	qDebug() << "\n\nbody:\n" << data << "\n";
	auto headers = request.headers();
	auto contVal = QString(headers.value("content-type").toByteArray());
	if (!headers.contains("content-type") || !contVal.contains("multipart/form-data"))
		return getBadRequestResponse("Invalid request format.");

	QString strBoundary = contVal.split("boundary=").at(1);

	QByteArray csvData; 
	defs::EBanks eBank{ defs::EBanks::eUndefined }; 
	QList<QString> lstData = data.split(strBoundary);
	for (const auto& strPart : lstData)
	{
		if (strPart.contains("Content-Disposition: form-data; name=\"bank\"")) 
		{
			// Extract the bank field
			QByteArray baPart = strPart.toUtf8();
			int pos = baPart.indexOf("\r\n\r\n") + 4;
			QString strBank = QString::fromUtf8(baPart.mid(pos).trimmed());
			strBank.remove('\n');
			strBank.remove('\r');
			strBank.chop(2);
			eBank = defs::bankFromString(strBank);
		}
		else if (strPart.contains("Content-Disposition: form-data; name=\"csvFile\"")) 
		{
			// Extract the CSV file data
			QByteArray baPart = strPart.toUtf8(); 
			int pos = baPart.indexOf("\r\n\r\n") + 4; 
			csvData = baPart.mid(pos); 
			csvData.chop(2); // Remove trailing boundary 
		}
	}

	if (eBank == defs::EBanks::eUndefined || csvData.isEmpty())
		return getBadRequestResponse("Failed reading data: 'Bank'.");

	QFile file("uploaded.csv"); 
	if (file.open(QIODevice::WriteOnly)) 
	{
		file.write(csvData); 
		file.close(); 
	}
	QJsonObject jsResult = categorizeCSV(eBank, "uploaded.csv");
	if(jsResult.isEmpty())
		return getBadRequestResponse("Failed reading data: 'CSV-File'.");
	QByteArray dataJsForm = QJsonDocument(jsResult).toJson(QJsonDocument::Compact);


	QHttpServerResponse resp(dataJsForm, QHttpServerResponse::StatusCode::Ok);
	addDefaultHeaders(resp); 
	return resp; 
}

QJsonObject WalletServer::categorizeCSV(const defs::EBanks eBank, const QString& filePath)
{
	QMap<QString, int> mpCells{};
	int iNoFields{ 0 };
	int iIgnoreLines{ 0 };
	switch (eBank)
	{
	case defs::EBanks::eGLS:
		mpCells = defs::mpCellsGLS;
		iNoFields = defs::cNoFieldGLS;
		iIgnoreLines = defs::cIngoreLinesGLS;
		break;
	case defs::EBanks::eDKB:
		break;
	default:
		return {};
	}

	QFile file(filePath);
	QJsonObject JsObResult;
	if (!file.open(QIODevice::ReadOnly)) return JsObResult;

	QTextStream stream(&file);
	QJsonArray JsArCategories;
	QStringList lstEntries = stream.readAll().split("\n").mid(iIgnoreLines);
	for (const auto& strLine : lstEntries)
	{
		QStringList fields = strLine.split(";");
		if (fields.size() != iNoFields) continue;

		QString strReference = fields[mpCells["Name Zahlungsbeteiligter"]] + " " + fields[mpCells["Verwendungszweck"]];

		QJsonObject JsObEntry;
		JsObEntry["Datum"] = fields[mpCells["Buchungstag"]];
		JsObEntry["Betrag"] = QString::number(-1 * fields[mpCells["Betrag"]].replace(",", ".").toDouble()).replace(".", ",");
		JsObEntry["Beschreibung"] = strReference;
		JsObEntry["Kategorie"] = categorize(strReference);
		JsArCategories.append(JsObEntry);
	}

	JsObResult["transactions"] = JsArCategories;
	return JsObResult;
}

QString WalletServer::categorize(const QString& strReference)
{
	QString strCategory = "None";
	for (const auto& strKey : m->mpCategories.keys())
		for (const auto& strIndication : m->mpCategories[strKey])
			if (strReference.contains(strIndication, Qt::CaseInsensitive))
				return strKey;
	return "None";
}


/* -----------------------------------------------------------------------
   --------------------- Utils -------------------------------
   ----------------------------------------------------------------------- */
QMap<QString, QVector<QString>> WalletServer::parseJsonToMap(const QByteArray& jsonData)
{
	QMap<QString, QVector<QString>> result{};

	// Parse the JSON data
	QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData);
	if (!jsonDoc.isObject()) return result;

	QJsonObject jsonObject = jsonDoc.object();

	// Iterate through the keys in the JSON object
	for (const auto& key : jsonObject.keys()) {
		QJsonArray jsonArray = jsonObject.value(key).toArray();
		QVector<QString> values{};

		// Convert each element in the array to QString and add it to the QVector
		for (const auto& value : jsonArray) {
			if (value.isString()) values.append(value.toString());
		}
		result.insert(key, values);
	}
	return result;
}

void WalletServer::addDefaultHeaders(QHttpServerResponse& response)
{
	QHttpHeaders headers = response.headers();
	if (!headers.contains(QHttpHeaders::WellKnownHeader::AccessControlAllowOrigin)) headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowOrigin, "*");
	if (!headers.contains(QHttpHeaders::WellKnownHeader::AccessControlAllowMethods)) headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowMethods, "GET, POST, OPTIONS");
	if (!headers.contains(QHttpHeaders::WellKnownHeader::AccessControlAllowHeaders)) headers.append(QHttpHeaders::WellKnownHeader::AccessControlAllowHeaders, "Content-Type, Authorization");
	if (!headers.contains(QHttpHeaders::WellKnownHeader::AccessControlMaxAge)) headers.append(QHttpHeaders::WellKnownHeader::AccessControlMaxAge, "86400");
	response.setHeaders(headers);
}

QHttpServerResponse WalletServer::getBadRequestResponse(const QString& strMsg)
{
	QHttpServerResponse resp(strMsg, QHttpServerResponse::StatusCode::BadRequest);
	addDefaultHeaders(resp);
	return resp;
}