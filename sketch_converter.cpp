#include "sketch_converter.h"
#include <QDebug>


SketchConverter::SketchConverter() {

}

void SketchConverter::addPoint(QObject* point) {
    //meshes << new SketchPoint(point);
}

void SketchConverter::addLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint) {
    meshes << new SketchLine(line, linesPerPoint);
}

void SketchConverter::addJoint(QObject* point, QList<QObject*> lines) {
    meshes << new SketchJoint(point, lines);
}

void SketchConverter::exportToFile(QObject* sketch, QString file) {
    qDebug() << "exportToFile() called";
    meshes.clear();
    qDebug() << "size of meshes" << meshes.size();

    // get the store from sketch
    QVariant maybeStore = sketch->property("store");

    if(!maybeStore.isValid()) {
        qDebug() << "SketchConverter invalid store property";
    }

    QVariantMap store = maybeStore.value<QVariantMap>();

    if(!store.contains("points")) {
        qDebug() << "SketchConverter store doesn't contains points";
    }

    if(!store.contains("lines")) {
        qDebug() << "SketchConverter store doesn't contains lines";
    }

    QVariantList points = store["points"].value<QVariantList>();
    QVariantList lines = store["lines"].value<QVariantList>();
    // cllect the line per node
    QMap<QObject*, QList<QObject*>> linesPerPoint;

    /*foreach(QVariant point, points) {
        //qDebug() << "SketchConverter insert point";
        this->addPoint(point.value<QObject*>());
    }*/

    foreach(QVariant line, lines) {
        QObject* lineObject = line.value<QObject*>();
        //qDebug() << "SketchConverter insert line";
        //this->addLine(lineObject);

        QObject* startPoint = lineObject->property("startPoint").value<QObject*>();
        QObject* endPoint = lineObject->property("endPoint").value<QObject*>();

        if(!linesPerPoint.contains(startPoint)) {
            linesPerPoint.insert(startPoint, QList<QObject*>());
        }
        if(!linesPerPoint.contains(endPoint)) {
            linesPerPoint.insert(endPoint, QList<QObject*>());
        }

        QList<QObject*> startList = linesPerPoint.value(startPoint);
        QList<QObject*> endList = linesPerPoint.value(endPoint);

        startList.append(lineObject);
        endList.append(lineObject);

        linesPerPoint.insert(startPoint, startList);
        linesPerPoint.insert(endPoint, endList);
    }

    foreach(QVariant line, lines) {
        QObject* lineObject = line.value<QObject*>();
        this->addLine(lineObject, linesPerPoint);
    }

    // create all the joints
    foreach(QObject* point, linesPerPoint.keys()) {
        QList<QObject*> lines = linesPerPoint[point];

        if(lines.size() > 0) {
            this->addJoint(point,lines);
        }
    }

    qDebug() << linesPerPoint;

    // create a scene
    aiScene* scene = new aiScene();

    scene->mRootNode = new aiNode();

    // single material, not reallyused
    scene->mMaterials = new aiMaterial*[ 1 ];
    scene->mMaterials[ 0 ] = nullptr;
    scene->mNumMaterials = 1;

    scene->mMaterials[ 0 ] = new aiMaterial();

    // meshes node, we need only one
    scene->mMeshes = new aiMesh*[ meshes.size() ];
    scene->mNumMeshes = meshes.size();
    scene->mRootNode->mMeshes = new unsigned int[ meshes.size() ];
    scene->mRootNode->mNumMeshes = meshes.size();


    // per mesh initialization, set the material to 0
    for(int i = 0; i < meshes.size(); i++) {
        SketchMesh* mesh = meshes.at(i);

        scene->mMeshes[ i ] = new aiMesh();
        scene->mMeshes[ i ]->mMaterialIndex = 0;

        scene->mRootNode->mMeshes[ i ] = i;

        // pointer to aiMesh object
        auto pMesh = scene->mMeshes[ i ];

        int verticesCount = mesh->getVertices().size();

        pMesh->mVertices = new aiVector3D[ verticesCount ];
        //pMesh->mNormals = new aiVector3D[ 0 ];
        pMesh->mNumVertices = verticesCount;

        for(int j = 0; j < verticesCount; j++) {
            QVector3D vertex = mesh->getVertices().at(j);
            pMesh->mVertices[j] = aiVector3D( vertex.x() * 0.1, vertex.y() * 0.1, vertex.z() * 0.1 );

            //qDebug() << "vertices" << j << vertex;
        }

        pMesh->mFaces = new aiFace[ mesh->getFaces().size() ];
        pMesh->mNumFaces = (unsigned int)(mesh->getFaces().size());

        for(int j = 0; j < mesh->getFaces().size(); j++) {
            QList<int> meshFace = mesh->getFaces().at(j);

            aiFace &face = pMesh->mFaces[j];

            face.mIndices = new unsigned int[meshFace.size()];
            face.mNumIndices = meshFace.size();

            for(int i = 0; i < meshFace.size(); i++) {
                face.mIndices[i] = meshFace.at(i);
            }

            //qDebug() << "faces" << j << meshFace;
        }
    }

    Assimp::Exporter *exporter = new Assimp::Exporter();
    exporter->Export(scene, "collada", file.toStdString().c_str(), aiProcess_Triangulate);

    delete exporter;
    //delete scene->mMaterials[0];
    //delete scene->mMaterials;
    /*for(int i = 0; i < meshes.size(); i++) {
        SketchMesh* mesh = meshes.at(i);

        auto pMesh = scene->mMeshes[ i ];

        int verticesCount = mesh->getVertices().size();

        for(int j = 0; j < mesh->getFaces().size(); j++) {
            aiFace &face = pMesh->mFaces[j];

            delete[] face.mIndices;
        }

        delete[] pMesh->mFaces;

        delete[] pMesh->mVertices;
        delete scene->mMeshes[ i ];
    }*/
    //delete[] scene->mMeshes;
    //delete[] scene->mRootNode->mMeshes;
    delete scene;
}

