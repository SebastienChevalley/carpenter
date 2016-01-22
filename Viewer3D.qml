import QtQuick 2.4
import QtQuick.Window 2.2
import QtQuick.Layouts 1.2
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtMultimedia 5.5
import QtGraphicalEffects 1.0
import Qt.labs.folderlistmodel 2.1

import Qt3D 2.0
import Qt3D.Renderer 2.0
import QtQuick 2.1 as QQ2
import QtQuick.Scene3D 2.0


Rectangle {
    id: viewer3d
    anchors.fill: parent
    color: "white"
    z: 1250
    visible: false
    property Mesh mesh: mesh

    Button {
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.margins: 20
        text: "Menu"
        style: RoundedButton {
            icon: "\uf060";
        }
        z: 100


        onClicked: {
            welcomeScreen.visible = true
            viewer3d.visible = false
            console.log("hide viewer3d")
        }
    }
    Scene3D {
        id: scene3d
        anchors.fill: parent
        anchors.margins: 10
        focus: true
        aspects: "input"

        Entity {
        id: root

        // Render from the mainCamera
        components: [
            FrameGraph {
                activeFrameGraph: ForwardRenderer {
                    id: renderer
                    camera: mainCamera
                }
            }
        ]

        Camera {
            id: mainCamera
            projectionType: CameraLens.PerspectiveProjection
            fieldOfView: 22.5
            aspectRatio: mainForm.width / mainForm.height
            nearPlane:   0.01
            farPlane:    1000.0
            viewCenter: Qt.vector3d( 0.0, 0.0, 0.0 )
            upVector:   Qt.vector3d( 0.0, 1.0, 0.0 )
            position: Qt.vector3d( 0.0, 0.0, 15.0 )

        }


        Configuration  {
            controlledCamera: mainCamera
        }

        Material {
            id: wireframeMaterial

            property color ambient:  Qt.rgba( 0.2, 0.0, 0.0, 1.0 )
            property color diffuse: Qt.rgba( 0.8, 0.0, 0.0, 1.0 )
            property color specular: Qt.rgba( 0.95, 0.95, 0.95, 1.0 )
            property real shininess: 150.0
            property real lineWidth: 1.5
            property color lineColor: Qt.rgba( 0.0, 0.0, 0.0, 1.0 )

            parameters: [
                Parameter { name: "ambient"; value: Qt.vector3d(wireframeMaterial.ambient.r, wireframeMaterial.ambient.g, wireframeMaterial.ambient.b) },
                Parameter { name: "diffuse"; value: Qt.vector3d(wireframeMaterial.diffuse.r, wireframeMaterial.diffuse.g, wireframeMaterial.diffuse.b) },
                Parameter { name: "specular"; value: Qt.vector3d(wireframeMaterial.specular.r, wireframeMaterial.specular.g, wireframeMaterial.specular.b) },
                Parameter { name: "shininess"; value: wireframeMaterial.shininess },
                Parameter { name: "line.width"; value: wireframeMaterial.lineWidth },
                Parameter { name: "line.color"; value: wireframeMaterial.lineColor }
            ]

            effect: Effect {

                parameters: [
                    Parameter { name: "ambient";   value: Qt.vector3d( 0.1, 0.1, 0.1 ) },
                    Parameter { name: "diffuse";   value: Qt.vector3d( 0.7, 0.7, 0.7 ) },
                    Parameter { name: "specular";  value: Qt.vector3d( 0.95, 0.95, 0.95 ) },
                    Parameter { name: "shininess"; value: 150.0 }
                ]

                techniques: [
                    Technique {
                        openGLFilter {
                            api: OpenGLFilter.Desktop
                            profile: OpenGLFilter.Core
                            majorVersion: 3
                            minorVersion: 1
                        }

                        annotations: [ Annotation { name: "renderingStyle"; value: "forward" } ]

                        parameters: [
                            Parameter { name: "light.position"; value: Qt.vector4d( 0.0, 0.0, 0.0, 1.0 ) },
                            Parameter { name: "light.intensity"; value: Qt.vector3d( 1.0, 1.0, 1.0 ) },
                            Parameter { name: "line.width"; value: 1.0 },
                            Parameter { name: "line.color"; value: Qt.vector4d( 1.0, 1.0, 1.0, 1.0 ) }
                        ]

                        renderPasses: [
                            RenderPass {

                                bindings: [
                                    ParameterMapping { parameterName: "ambient";  shaderVariableName: "ka"; bindingType: ParameterMapping.Uniform },
                                    ParameterMapping { parameterName: "diffuse";  shaderVariableName: "kd"; bindingType: ParameterMapping.Uniform },
                                    ParameterMapping { parameterName: "specular"; shaderVariableName: "ks"; bindingType: ParameterMapping.Uniform }
                                ]

                                shaderProgram: ShaderProgram {
                                    vertexShaderCode:   loadSource("qrc:/shaders/robustwireframe.vert")
                                    geometryShaderCode: loadSource("qrc:/shaders/robustwireframe.geom")
                                    fragmentShaderCode: loadSource("qrc:/shaders/robustwireframe.frag")
                                }
                            }
                        ]
                    }
                ]
            }

        }

        Entity {
            id: trefoilKnot

            property alias x: translation.dx
            property alias y: translation.dy
            property alias z: translation.dz
            property alias scale: scaleTransform.scale
            property alias theta: thetaRotation.angle
            property alias phi: phiRotation.angle
            property Material material: wireframeMaterial

            components: [ transform, mesh, trefoilKnot.material ]

            Transform {
                id: transform
                Translate { id: translation }
                Scale { id: scaleTransform }
                Rotate{ id: thetaRotation; axis: Qt.vector3d( 1.0, 0.0, 0.0 ) }
                Rotate{ id: phiRotation;   axis: Qt.vector3d( 0.0, 1.0, 0.0 ) }
            }

            Mesh {
                id: mesh
                source: "file:///c:/Users/Sfimx/Documents/build-carpenter-Desktop_Qt_5_5_1_MinGW_32bit-Debug/output-1453431627860.final.obj"
            }
        }
    }
    }

    MouseArea {
        anchors.fill: parent
        z: 88
    }
}

