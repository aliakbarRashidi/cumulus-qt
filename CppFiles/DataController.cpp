/*
* Copyright (C) 2016 - 2017 Adrian Verban <verbanady@gmail.com>
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
#include "DataController.h"

DataController::DataController(QObject *parent) : QObject(parent) {}

void DataController::getDataFromUrl(QString urlString) {
    networkManager = new QNetworkAccessManager(this);
    if (networkManager->networkAccessible() == QNetworkAccessManager::Accessible) {
        connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(readFinished(QNetworkReply*)));
        QNetworkRequest networkRequest;
        networkRequest.setUrl(QUrl(urlString));
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        networkManager->get(networkRequest);
    }
    else {
        emit networkError("Network not accessible!");
    }
}

void DataController::readFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString dataRead = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(dataRead.toUtf8());
        QJsonObject jsonObject = jsonResponse.object();
        emit jsonObjectReady(jsonObject);
    }
    else {
        emit networkError(reply->errorString());
    }
}

void DataController::getJsonArrayDataFromUrl(QString urlString) {
    networkManager = new QNetworkAccessManager(this);
    if (networkManager->networkAccessible() == QNetworkAccessManager::Accessible) {
        connect(networkManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(readJsonArrayFinished(QNetworkReply*)));
        QNetworkRequest networkRequest;
        networkRequest.setUrl(QUrl(urlString));
        networkRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
        networkManager->get(networkRequest);
    }
    else {
        emit networkError("Network not accessible!");
    }
}

void DataController::readJsonArrayFinished(QNetworkReply *reply) {
    if (reply->error() == QNetworkReply::NoError) {
        QString dataRead = (QString)reply->readAll();
        QJsonDocument jsonResponse = QJsonDocument::fromJson(dataRead.toUtf8());
        QJsonArray jsonArray = jsonResponse.array();
        emit jsonArrayReady(jsonArray);
    }
    else {
        emit networkError(reply->errorString());
    }
}
