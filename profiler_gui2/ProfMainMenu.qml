/**************************************
*
***************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

Drawer {

    signal drawTestClicked()
    signal openFileClicked()

    Component.onCompleted: {
        drawTestAction.triggered.connect(drawTestClicked)
        openFileAction.triggered.connect(openFileClicked)
    }

    Flickable {
        anchors.fill: parent

        Column {
            id: drawerContents
            anchors.left: parent.left
            width: parent.width
            spacing: 10

            property color labelColor: "#2094f4"
            property color itemsColor: "#505050"
            property color separatorColor: "#c0c0c0"
            property int labelTextSize: 14
            property int itemsTextSize: 18

            Separator { color: drawerContents.separatorColor }

            Label {
                text: "File"
                font.pixelSize: drawerContents.labelTextSize
                color: drawerContents.labelColor
                anchors.left: parent.left
                anchors.leftMargin: 15
            }

            MenuItem {
                id: openFileAction
                anchors.left: parent.left
                width: parent.width
                contentItem: Text {
                    text: "Open profiler file"
                    font.pixelSize: drawerContents.itemsTextSize
                    color: drawerContents.itemsColor
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                }
            }

            Separator { color: drawerContents.separatorColor }

            Label {
                text: "Tests"
                font.pixelSize: drawerContents.labelTextSize
                color: drawerContents.labelColor
                anchors.left: parent.left
                anchors.leftMargin: 15
            }

            MenuItem {
                id: drawTestAction
                anchors.left: parent.left
                width: parent.width
                contentItem: Text {
                    text: "Draw test items"
                    font.pixelSize: drawerContents.itemsTextSize
                    color: drawerContents.itemsColor
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                }
            }

            Separator { color: drawerContents.separatorColor }

            MenuItem {
                id: exitAction
                anchors.left: parent.left
                width: parent.width
                contentItem: Text {
                    text: "Exit"
                    font.pixelSize: drawerContents.itemsTextSize
                    color: drawerContents.itemsColor
                    anchors.left: parent.left
                    anchors.leftMargin: 15
                }
                onTriggered: Qt.quit();
            }
        }

        contentWidth: parent.width
        contentHeight: drawerContents.height
        ScrollIndicator.vertical: ScrollIndicator { width: 14; }
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
