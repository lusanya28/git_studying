import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Window {
    width: 400
    height: 300
    visible: true
    color: "lightgray"
    title: qsTr("Temperature Converter")

    ColumnLayout {
        anchors.centerIn: parent
        anchors.margins: 20
        spacing: 20

        Text {
            text: "Temperature Converter"
            font.pointSize: 16
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        RowLayout {
            spacing: 15
            Layout.fillWidth: true

            Text {
                text: "Celsius:"
                font.pointSize: 12
                Layout.preferredWidth: 80
                color: model.celsiusValid ? "black" : "red"
            }

            TextField {
                id: celsiusField
                placeholderText: "0.00"
                Layout.preferredWidth: 150
                text: controller.formatTemperature(model.celsius)

                background: Rectangle {
                    border.color: model.celsiusValid ? "gray" : "red"
                    border.width: 2
                    radius: 4
                    color: "white"
                }

                onTextChanged: {
                    if (activeFocus) {
                        controller.setCelsiusFromString(text)
                    }
                }
            }

            Text {
                text: "°C"
                font.pointSize: 12
            }
        }

        RowLayout {
            spacing: 15
            Layout.fillWidth: true

            Text {
                text: "Fahrenheit:"
                font.pointSize: 12
                Layout.preferredWidth: 80
                color: model.fahrenheitValid ? "black" : "red"
            }

            TextField {
                id: fahrenheitField
                placeholderText: "32.00"
                Layout.preferredWidth: 150
                text: controller.formatTemperature(model.fahrenheit)

                background: Rectangle {
                    border.color: model.fahrenheitValid ? "gray" : "red"
                    border.width: 2
                    radius: 4
                    color: "white"
                }

                onTextChanged: {
                    if (activeFocus) {
                        controller.setFahrenheitFromString(text)
                    }
                }
            }

            Text {
                text: "°F"
                font.pointSize: 12
            }
        }

        RowLayout {
            spacing: 15
            Layout.fillWidth: true

            Text {
                text: "Kelvin:"
                font.pointSize: 12
                Layout.preferredWidth: 80
                color: model.kelvinValid ? "black" : "red"
            }

            TextField {
                id: kelvinField
                placeholderText: "273.15"
                Layout.preferredWidth: 150
                text: controller.formatTemperature(model.kelvin)

                background: Rectangle {
                    border.color: model.kelvinValid ? "gray" : "red"
                    border.width: 2
                    radius: 4
                    color: "white"
                }

                onTextChanged: {
                    if (activeFocus) {
                        controller.setKelvinFromString(text)
                    }
                }
            }

            Text {
                text: "K"
                font.pointSize: 12
            }
        }

        Text {
            text: "Note: Temperatures below absolute zero are invalid"
            font.pointSize: 10
            color: "gray"
            Layout.alignment: Qt.AlignHCenter
            Layout.topMargin: 20
        }
    }

    Connections {
        target: model

        function onCelsiusChanged() {
            if (!celsiusField.activeFocus) {
                celsiusField.text = controller.formatTemperature(model.celsius)
            }
        }

        function onFahrenheitChanged() {
            if (!fahrenheitField.activeFocus) {
                fahrenheitField.text = controller.formatTemperature(model.fahrenheit)
            }
        }

        function onKelvinChanged() {
            if (!kelvinField.activeFocus) {
                kelvinField.text = controller.formatTemperature(model.kelvin)
            }
        }
    }
}
