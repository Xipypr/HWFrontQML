#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QIcon>
#include <QObject>

#include "hwconnector.h"

#include "languagemanager.h"

#include "../core/sessionmanager.h"
#include "../core/sessionstate.h"
#include "../models/dashboardmetricsmodel.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QCoreApplication::setOrganizationName(QStringLiteral("HWFrontQML"));
    QCoreApplication::setApplicationName(QStringLiteral("HWFrontQML"));

    QGuiApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/icons/app/icon_512.png"));

    QQmlApplicationEngine engine;
    QQmlContext * context = engine.rootContext();
    LanguageManager languageManager(&engine);
    SessionManager sessionManager;
    if (sessionManager.persistSessionState()) {
        sessionManager.restoreSessionsState();
    }

    qmlRegisterUncreatableMetaObject(SessionStateNs::staticMetaObject, "SessionState", 1, 0, "SessionState", "Session state enum");
    qmlRegisterType<DashboardMetricsModel>("DashboardModels", 1, 0, "DashboardMetricsModel");

    context->setContextProperty("sessionManager", &sessionManager);
    context->setContextProperty("languageManager", &languageManager);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
