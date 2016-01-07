#include "sketch_converter.h"
#include <QDebug>


SketchConverter::SketchConverter() {

}

QSharedPointer<SketchLine> SketchConverter::addLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint) {
    QSharedPointer<SketchLine> sketchLine(new SketchLine(line, linesPerPoint));

    meshes << sketchLine;

    return sketchLine;
}

QSharedPointer<SketchJoint> SketchConverter::addJoint(QObject* point, QList<QObject*> lines) {
    QSharedPointer<SketchJoint> sketchJoint (new SketchJoint(point, lines));

    meshes << sketchJoint;

    return sketchJoint;
}

QSharedPointer<SketchPoint> SketchConverter::addPoint(QObject* point) {
    QSharedPointer<SketchPoint> sketchPoint (new SketchPoint(point));

    meshes << sketchPoint;

    return sketchPoint;
}

QVariant SketchConverter::exportToFile(QObject* sketch, QString file) {
    QString error;
    bool result = this->exportToFile(sketch, file, error);

    if(result) {
        return true;
    }
    else {
        return error;
    }
}


bool SketchConverter::exportToFile(QObject* sketch, QString file, QString& error) {
#ifdef CARPENTER_DEBUG
    qDebug() << "SketchConverter: exportToFile() called";
#endif


    meshes.clear();
#ifdef CARPENTER_DEBUG
    qDebug() << "SketchConverter: size of meshes" << meshes.size();
#endif

    // get the store from sketch
    QVariant maybeStore = sketch->property("store");

    if(!maybeStore.isValid()) {
        error = "Cannot find Sketch to convert";
        return false;
    }

    QVariantMap store = maybeStore.value<QVariantMap>();

    if(!store.contains("points")) {
        error =  "Sketch doesn't contain points";
        return false;
    }

    if(!store.contains("lines")) {
        error = "Sketch doesn't contain lines";
        return false;
    }

    QVariantList points = store["points"].value<QVariantList>();
    QVariantList lines = store["lines"].value<QVariantList>();
    // collect the line per node
    QMap<QObject*, QList<QObject*>> linesPerPoint;

    foreach(QVariant rawPoint, points) {
        this->addPoint(rawPoint.value<QObject*>());
    }

    foreach(QVariant line, lines) {
        QObject* lineObject = line.value<QObject*>();

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
        QSharedPointer<SketchLine> newLine = this->addLine(lineObject, linesPerPoint);

        if(!(*newLine).isValid()) {
            error = newLine->getErrorMessage();
            return false;
        }
    }

    // create all the joints
    foreach(QObject* point, linesPerPoint.keys()) {
        QList<QObject*> lines = linesPerPoint[point];

        if(lines.size() > 0) {
            QSharedPointer<SketchJoint> newJoint = this->addJoint(point,lines);

            if(!(*newJoint).isValid()) {
                error = newJoint->getErrorMessage();
                return false;
            }
        }
    }
#ifdef CARPENTER_DEBUG
    qDebug() << "SketchConverter: linesPerPoint: " << linesPerPoint;
#endif

    // create a scene
    QSharedPointer<aiScene> scene = this->generateScene();

    Assimp::Exporter exporter;
    aiReturn state = exporter.Export(scene.data(), "collada", file.toStdString().c_str(), aiProcess_Triangulate);


    if(state == AI_SUCCESS) {
        return true;
    }
    else if(state == AI_OUTOFMEMORY) {
        error = "Pas assez de mémoire vive pour terminer l'opération";
        return false;
    }
    else {
        error = "Une erreur s'est produire lors de l'exportation";
        return false;
    }
}

QSharedPointer<aiScene> SketchConverter::generateScene() {
    QSharedPointer<aiScene> scene(new aiScene());

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
        QSharedPointer<SketchMesh> mesh = meshes.at(i);

        scene->mMeshes[ i ] = new aiMesh();
        scene->mMeshes[ i ]->mMaterialIndex = 0;

        scene->mRootNode->mMeshes[ i ] = i;

        // pointer to aiMesh object
        auto pMesh = scene->mMeshes[ i ];

        int verticesCount = mesh->getVertices().size();

        pMesh->mVertices = new aiVector3D[verticesCount];
        pMesh->mNumVertices = verticesCount;

        for(int j = 0; j < verticesCount; j++) {
            QVector3D vertex = mesh->getVertices().at(j);
            pMesh->mVertices[j] = aiVector3D( vertex.x() * 0.1, vertex.y() * 0.1, vertex.z() * 0.1 );
            //pMesh->mVertices[j] = aiVector3D(vertex.x(), vertex.y(), vertex.z());
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
        }
    }

    return scene;
}

