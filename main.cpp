#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QObject>

//#include <hwconnector.h>
//#include <devicebuilder.h>
#include "devicebuilder.h"
#include "hwconnector.h"
#include "storages/ram.h"

#include "core.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);
    QCoreApplication::setOrganizationName("HWFront");
    QCoreApplication::setOrganizationDomain("hwfront.local");
    QCoreApplication::setApplicationName("HWFrontQML");

    Core core;
    Device device;
    DesktopDevice desktopDevice;
    Ram ram;

    QQmlApplicationEngine engine;
    QQmlContext * context = engine.rootContext();

    qmlRegisterUncreatableType<Device>("DeviceData", 1, 0, "Device", "Access to devices Id");

    context->setContextProperty("core", &core);
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
