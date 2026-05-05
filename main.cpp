#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QObject>
#include <QVariant>

//#include <hwconnector.h>
//#include <devicebuilder.h>
#include "devicebuilder.h"
#include "hwconnector.h"
#include "storages/ram.h"

#include "sessionmanager.h"
#include "sessionstate.h"
#include "dashboardmetricsmodel.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    SessionManager sessionManager;
    app.setProperty("sessionManagerPtr", QVariant::fromValue(static_cast<QObject *>(&sessionManager)));
    Device device;
    DesktopDevice desktopDevice;
    Ram ram;

    QQmlApplicationEngine engine;
    QQmlContext * context = engine.rootContext();

    qmlRegisterUncreatableType<Device>("DeviceData", 1, 0, "Device", "Access to devices Id");
    qmlRegisterUncreatableMetaObject(SessionStateNs::staticMetaObject, "SessionState", 1, 0, "SessionState", "Session state enum");
    qmlRegisterType<DashboardMetricsModel>("DashboardModels", 1, 0, "DashboardMetricsModel");

    context->setContextProperty("sessionManager", &sessionManager);
    context->setContextProperty("device", &device);
    context->setContextProperty("desktopDevice", &desktopDevice);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
