#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "canvas.hpp"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    qmlRegisterType<CppCanvas>("easy.profiler.cppext", 0, 1, "CppCanvas");
    qmlRegisterType<Real>("easy.profiler.cppext", 0, 1, "Real");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}

