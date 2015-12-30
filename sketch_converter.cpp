#include "sketch_converter.h"
#include <QDebug>


SketchConverter::SketchConverter() {

}

SketchLine* SketchConverter::addLine(QObject* line, QMap<QObject*, QList<QObject*>> linesPerPoint) {
    SketchLine* sketchLine = new SketchLine(line, linesPerPoint);

    meshes << sketchLine;

    return sketchLine;
}

SketchJoint* SketchConverter::addJoint(QObject* point, QList<QObject*> lines) {
    SketchJoint* sketchJoint = new SketchJoint(point, lines);

    meshes << sketchJoint;

    return sketchJoint;
}

aiScene* SketchConverter::generateScene() {
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

QVariant SketchConverter::exportToFile(QObject* sketch, QString file) {
    qDebug() << "exportToFile() called";
    meshes.clear();
    qDebug() << "size of meshes" << meshes.size();

    // get the store from sketch
    QVariant maybeStore = sketch->property("store");

    if(!maybeStore.isValid()) {
        return "Cannot find Sketch to convert";
    }

    QVariantMap store = maybeStore.value<QVariantMap>();

    if(!store.contains("points")) {
        return "Sketch doesn't contain points";
    }

    if(!store.contains("lines")) {
        return "Sketch doesn't contain lines";
    }

    QVariantList points = store["points"].value<QVariantList>();
    QVariantList lines = store["lines"].value<QVariantList>();
    // collect the line per node
    QMap<QObject*, QList<QObject*>> linesPerPoint;

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
        SketchLine* newLine = this->addLine(lineObject, linesPerPoint);

        if(!newLine->isValid()) {
            return newLine->getErrorMessage();
        }
    }

    // create all the joints
    foreach(QObject* point, linesPerPoint.keys()) {
        QList<QObject*> lines = linesPerPoint[point];

        if(lines.size() > 0) {
            SketchJoint* newJoint = this->addJoint(point,lines);

            if(!newJoint->isValid()) {
                return newJoint->getErrorMessage();
            }
        }
    }

    qDebug() << "linesPerPoint: " << linesPerPoint;

    // create a scene
    aiScene* scene = this->generateScene();

    Assimp::Exporter *exporter = new Assimp::Exporter();
    aiReturn state = exporter->Export(scene, "collada", file.toStdString().c_str(), aiProcess_Triangulate);

    delete exporter;
    delete scene;

    if(state == AI_SUCCESS) {
        return true;
    }
    else if(state == AI_OUTOFMEMORY) {
        return "Pas assez de mémoire vive pour terminer l'opération";
    }
    else {
        return "Une erreur s'est produire lors de l'exportation";
    }
}

