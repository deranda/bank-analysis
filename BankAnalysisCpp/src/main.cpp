#include <QtCore/QCoreApplication>
#include "BackendDefs.h"
#include "WalletServer.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    WalletServer server;

    return app.exec();
}
