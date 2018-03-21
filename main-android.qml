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
import QtQuick 2.7
import QtQuick.Controls 2.1
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import ownTypes.settingsController 1.0

ApplicationWindow {
    id: mainWindow
    visible: true
    title: "Cumulus"

    FontLoader {
        id: ubuntuCondensed
        source: "fonts/Ubuntu-C.ttf"
    }
    FontLoader {
        id: weatherIcons
        source: "fonts/weathericons-regular-webfont.ttf"
    }

    SettingsController {
        id: applicationSettingsController
        onWeatherApiChanged: weatherView.updateWeather()
    }

    Rectangle {
        id: appBody
        anchors.fill: parent
        color: applicationSettingsController.applicationBackground
        focus: true
        Keys.onBackPressed: mainWindow.close()

        ApplicationBar {
            id: applicationBar
            anchors.top: appBody.top
            width: appBody.width
            height: parent.height * 10 / 100
            textColor: applicationSettingsController.textColor
            iconsFont: weatherIcons.name
            textFontFamily: ubuntuCondensed.name
            onRefreshButtonClicked: weatherView.updateWeather()
            windowControlsPos: applicationSettingsController.windowControlsPos
            onMenuButtonClicked: {
                if (bodyView.visible == true) {
                    appView.push(settingsView)
                    settingsView.forceActiveFocus()
                }
                else {
                    appView.pop(null)
                    appBody.forceActiveFocus()
                }
            }
        }

        StackView {
            id: appView
            width: parent.width
            height: parent.height - applicationBar.height
            anchors.top: applicationBar.bottom
            initialItem: bodyView
        }

        Flickable {
            id: bodyView
            contentWidth: width
            contentHeight: height
            interactive: false
            contentX: 0
            MainView {
                id: weatherView
                width: bodyView.width
                height: bodyView.height
                backgroundColor: applicationSettingsController.applicationBackground
                textColor: applicationSettingsController.textColor
                textFontFamily: ubuntuCondensed.name
                iconsFont: weatherIcons.name
                speedUnit: applicationSettingsController.windSpeedUnit
                tempUnit: applicationSettingsController.tempUnit
                pressureUnit: applicationSettingsController.pressureUnit
                onFinishedWeatherUpdate: {
                    applicationBar.animationAlias.stop()
                    timer.interval = 3600000
                }
                onUpdateWeather: {
                    if (applicationBar.animationAlias.running == false) {
                        applicationBar.animationAlias.start()
                        applicationBar.animationAlias.loops = RotationAnimation.Infinite
                    }
                }
                onNetworkError: timer.interval = 60000
                onLocationNameChanged: applicationBar.locationName = name
            }            
        }

        SettingsWindow {
            id: settingsView            
            backgroundColor: applicationSettingsController.applicationBackground
            onBackgroundColorChanged: applicationSettingsController.applicationBackground = backgroundColor
            textColor: applicationSettingsController.textColor
            onTextColorChanged: applicationSettingsController.textColor = textColor
            onWindowControlsChanged: applicationSettingsController.windowControlsPos = windowControls
            onShowCredits: {
                appView.push(creditsView)
                creditsView.forceActiveFocus()
            }
            api: applicationSettingsController.weatherApi
            onApiChanged: applicationSettingsController.weatherApi = api
            temperatureUnit: applicationSettingsController.tempUnit
            onTemperatureUnitChanged: applicationSettingsController.tempUnit = settingsView.temperatureUnit
            speedUnit: applicationSettingsController.windSpeedUnit
            onSpeedUnitChanged: applicationSettingsController.windSpeedUnit = settingsView.speedUnit
            onLocationChanged: {
                weatherView.updateWeather()
                appView.pop()
                bodyView.forceActiveFocus()
            }
            useGps: applicationSettingsController.useGps
            onUseGpsChanged: applicationSettingsController.useGps = useGps
            Keys.onBackPressed: {
                appView.pop()
                bodyView.forceActiveFocus()
            }
        }

        CreditsView {
            id: creditsView
            textColor: applicationSettingsController.textColor
            backgroundColor: applicationSettingsController.applicationBackground
            Keys.onBackPressed: {
                appView.pop()
                settingsView.forceActiveFocus()
            }
        }
    }

    Timer {
        id: timer
        interval: 3600000
        running: true
        repeat: true
        onTriggered: weatherView.updateWeather()
    }
    Component.onCompleted: weatherView.updateWeather()
}
