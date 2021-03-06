/*
* Copyright (C) 2016 2017 Adrian Verban <verbanady@gmail.com>
* Maintainers: Adrian Verban <verbanady@gmail.com>
* Derived from Typhoon by Archisman Panigrahi which is based on Stormcloud by Jono Cooper <jonocooper.com>
* Thanks to all the contributors.
* Using the Ubuntu Condensed font.
* This file is part of Cumulus.
#
* Cumulus is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
#
* Cumulus is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
#
* You should have received a copy of the GNU General Public License
* along with Cumulus.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <QApplication>
#include <QTranslator>
#include "CppFiles/DatabaseHelper.h"
#if defined(Q_OS_ANDROID)
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QtQml>
#include <QIcon>
#include <QProcessEnvironment>

#include "CppFiles/WeatherType.h"
#include "CppFiles/Forecast.h"
#include "CppFiles/Util.h"
#include "CppFiles/SettingsController.h"
#include "CppFiles/TrayController.h"
#include "CppFiles/ThreadWorker.h"
#include "CppFiles/CustomImageProvider.h"
#include "CppFiles/FontImageProvider.h"
#include "CppFiles/Location.h"
void registerQmlType();
#else
#include <QFile>
#include <QThread>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QLibrary>

#include "CppFiles/MainWindow.h"
#include "CppFiles/ThreadWorker.h"
#endif

#define SHLIB_VERSION_NUMBER "1.0.0"
int main(int argc, char *argv[]) {
    QLibrary libcrypto, libssl;
    libcrypto.setFileNameAndVersion(QLatin1String("crypto"), QLatin1String(SHLIB_VERSION_NUMBER));
    libssl.setFileNameAndVersion(QLatin1String("ssl"), QLatin1String(SHLIB_VERSION_NUMBER));
#if (QT_VERSION >= QT_VERSION_CHECK(5, 6, 2))
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);
    QCoreApplication::addLibraryPath(QLibraryInfo::location(QLibraryInfo::LibrariesPath));
    QString applicationName = "Cumulus";
    if (argc > 2 && QString::fromLatin1(argv[1]) == "-i") {
        applicationName = applicationName + "-" + argv[2];
    }
    QApplication::setOrganizationName("Visoft");
    QApplication::setApplicationName(applicationName);
    qRegisterMetaType<QList<location_struct>>("QList<location_struct>");
    qRegisterMetaType<weather_struct>("weather_struct");
    DatabaseHelper *dbHelper = new DatabaseHelper;
    dbHelper->databaseInit();
    delete(dbHelper);
    QTranslator translator;
    QString defaultLocale = QLocale::system().name();
    defaultLocale.truncate(defaultLocale.lastIndexOf("_"));
    QString langPath = ":/translations/";
    if (translator.load(langPath.append("tempestas_").append(defaultLocale).append(".qm"))) {
        app.installTranslator(&translator);
    }
#if defined(Q_OS_ANDROID)
    Util util;
    SettingsController settingsController;
    QQmlApplicationEngine engine;
    engine.addImageProvider(QLatin1String("customimage"), new CustomImageProvider());
    engine.addImageProvider(QLatin1String("fontimage"), new FontImageProvider());
    QQmlContext *context = engine.rootContext();
    context->setContextProperty("util", &util);
    context->setContextProperty("settingsController", &settingsController);
    context->setContextProperty("applicationPath", "file://" + qApp->applicationDirPath() + "/");
    registerQmlType();
    engine.load(QUrl(QLatin1String("qrc:/qml/main-android.qml")));
#else
    MainWindow w;
    w.launchApp();
    if (QFile::exists(QApplication::applicationDirPath() + "/maintenancetool")) {
        ThreadWorker *threadWorker = new ThreadWorker();
        QThread *thread = new QThread();
        threadWorker->moveToThread(thread);
        QObject::connect(thread, SIGNAL(started()), threadWorker, SLOT(updaterTimerStart()));
        QObject::connect(threadWorker, SIGNAL(startUpdateTimerSignal()), threadWorker, SLOT(updaterTimerStart()));
        QObject::connect(threadWorker, SIGNAL(stopUpdatesSearch()), thread, SLOT(quit()));
        QObject::connect(thread, SIGNAL(finished()), threadWorker, SLOT(deleteLater()));
        QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
        thread->start();
    }
#endif
    return app.exec();
}

#if defined(Q_OS_ANDROID)
void registerQmlType() {
    qmlRegisterType<WeatherType>("ownTypes.weather", 1, 9, "Weather");
    qmlRegisterType<SettingsController>("ownTypes.settingsController", 1, 0, "SettingsController");
    qmlRegisterType<TrayController>("ownTypes.TrayController", 0, 3, "TrayController");
    qmlRegisterType<Location>("ownTypes.LocationController", 0, 1, "LocationController");
}
#endif
