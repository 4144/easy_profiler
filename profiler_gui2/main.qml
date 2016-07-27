/**************************************
*
***************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Controls.Material 2.0
import QtQuick.Layouts 1.3
import easy.profiler.cppext 0.1

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//window containing the application
ApplicationWindow {
    id: window
    width: 640
    height: 480
    visible: true
    title: "Easy Profiler"

    header: ToolBar {
        //Material.foreground: "white"
        RowLayout {
            spacing: 20
            anchors.fill: parent
            ToolButton {
                contentItem: Image {
                    fillMode: Image.Pad
                    horizontalAlignment: Image.AlignHCenter
                    verticalAlignment: Image.AlignVCenter
                    source: "qrc:/resources/menu.png"
                }
                onClicked: menu.open()
            }
        }
    }

    ProfMainMenu {
        id: menu
        width: 300//Math.min(Math.min(window.width, window.height) * 0.65, 300)
        height: window.height

        onOpenFileClicked: {
            console.log("Open action triggered");
        }

        onDrawTestClicked: {
            cppcanvas.test(18000, 40000000, 5)
            var cnvsize = cppcanvas.canvasSize
            content.contentWidth = cnvsize.width
            content.contentHeight = cnvsize.height
            scene.requestPaint()
        }
    }

    //Content Area
    Flickable {
        id: content
        anchors.fill: parent
        contentWidth: 1280
        contentHeight: 720
        //property double scaledWidth: contentWidth * scene.xscale

        ScrollBar.horizontal: ScrollBar {
            id: hbar
            active: vbar.active
            height: 14
            //size: scene.canvasWindow.width / content.scaledWidth
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 1
            onPressedChanged: height = pressed ? 18 : 14;
            Behavior on height { NumberAnimation { duration: 100 } }
//            onSizeChanged: {
//                if (size < 0.01) size = 0.01;
//            }
        }

        ScrollBar.vertical: ScrollBar {
            id: vbar
            active: hbar.active
            width: 14
            anchors.right: parent.right
            anchors.rightMargin: 1
            onPressedChanged: width = pressed ? 18 : 14;
            Behavior on width { NumberAnimation { duration: 100 } }
        }

        MouseArea {
            anchors.fill: parent
            onWheel: {
                var factor = wheel.angleDelta.y > 0 ? 1.25 : 0.8
                scene.xscale *= factor
                //content.contentWidth *= factor
                //console.log(scene.xscale, hbar.size)
                scene.requestPaint()
            }
        }
    }

    Canvas {
        id: scene
        parent: content
        anchors.fill: parent
        contextType: "2d"
        renderStrategy: Canvas.Threaded
        renderTarget: Canvas.FramebufferObject
        canvasSize: Qt.size(content.contentWidth, content.contentHeight)
        canvasWindow: Qt.rect(content.contentX, content.contentY, content.width - 20, content.height - 20)
        property double xscale: 1.0

        property CppCanvas backend: CppCanvas {
            id: cppcanvas
            property color prevColor: Qt.rgba(0, 0, 0, 0)

            onFillRect: {
                if (fill_color != prevColor) {
                    scene.context.fillStyle = fill_color
                    prevColor = fill_color
                }
                scene.context.fillRect(rect_x, rect_y, rect_width, rect_height)
            }

            onSetColor: {
                if (fill_color != prevColor) {
                    scene.context.fillStyle = fill_color
                    prevColor = fill_color
                }
            }
        }

        onPaint: {
            //console.log("repaint")
            //context.scale(xscale, 1.0)
            context.clearRect(region.x, region.y, region.width, region.height) // clear previous picture
            cppcanvas.prevColor = Qt.rgba(0, 0, 0, 0) // reset color
            cppcanvas.paint(region.x / xscale, region.y, region.width / xscale, region.height, xscale) // paint visible scene rect
        }

        onCanvasWindowChanged: requestPaint()
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
