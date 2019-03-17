import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import QtQuick.Window 2.2

import WickedLasers 1.0

Item {
    id: rootItem

    Window {
        id: mainWindow

        width: 640
        height: 480

        RowLayout {
            anchors.fill: parent

            ColumnLayout {
                Layout.alignment: Qt.AlignTop


                Button {
                    focusPolicy: Qt.NoFocus
                    enabled: ldCore.laserController.connectedDevices > 0

                    text: checked ? qsTr("Stop") : qsTr("Start")

                    checked: ldCore.laserController.isActive

                    onClicked: ldCore.laserController.togglePlay()
                }

                ComboBox {
                    id: gameComboBox

                    focusPolicy: Qt.NoFocus


                    model:  ListModel {
                        id: visModeModel
                    }
                    textRole: "display"

                    currentIndex: gameManager.currentGameIndex
                    onActivated: gameManager.currentGameIndex = index

                    Component.onCompleted:  {
                        for (var i = 0; i < gameManager.games.length; i++) {
                            visModeModel.append({"display": gameManager.games[i].title })
                        }
                    }
                }


                Button {
                    focusPolicy: Qt.NoFocus

                    text: checked ? qsTr("Stop") : qsTr("Start")

                    checked: gameManager.activeGame && gameManager.activeGame.isPlaying

                    onClicked: gameManager.activeGame.toggle()
                }
                Button {
                    focusPolicy: Qt.NoFocus

                    text: qsTr("Reset")

                    onClicked: gameManager.activeGame.reset()
                }

                ComboBox {
                    visible: gameManager.activeGame && gameManager.activeGame.levelList.length > 0
                    model: gameManager.activeGame ? gameManager.activeGame.levelList : 0

                    currentIndex: 0

                    onCurrentIndexChanged: gameManager.activeGame.levelIndex = currentIndex
                }

            }

            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true

                LdSimulatorItem {
                    id: simulator
                    anchors.fill: parent

                    autostart: true
                    clearColor: "black"
                }
            }
        }



    }

    Component.onCompleted: {
        console.debug("QML LOADED")

        mainWindow.show()
        setWindow(mainWindow)
    }
}
