#include "testtriangulate.h"

TestTriangulate::TestTriangulate()
{
    aiScene* scene = new aiScene();

    scene->mRootNode = new aiNode();

    // single material, not reallyused
    scene->mMaterials = new aiMaterial*[ 1 ];
    scene->mMaterials[ 0 ] = nullptr;
    scene->mNumMaterials = 1;

    scene->mMaterials[ 0 ] = new aiMaterial();

    // meshes node, we need only one
    scene->mMeshes = new aiMesh*[ 1 ];
    scene->mNumMeshes = 1;
    scene->mRootNode->mMeshes = new unsigned int[ 1 ];
    scene->mRootNode->mNumMeshes = 1;

    scene->mMeshes[ 0 ] = new aiMesh();
    scene->mMeshes[ 0 ]->mMaterialIndex = 0;

    scene->mRootNode->mMeshes[ 0 ] = 0;

    // pointer to aiMesh object
    auto pMesh = scene->mMeshes[ 0 ];

    pMesh->mVertices = new aiVector3D[ 9 ];
    //pMesh->mNormals = new aiVector3D[ 0 ];
    pMesh->mNumVertices = 9;

    pMesh->mVertices[0] = aiVector3D(4.26000000000000,	5.66000000000001    ,0);
    pMesh->mVertices[1] = aiVector3D(1.08000000000000, 1.36000000000000    ,0);
    pMesh->mVertices[2] = aiVector3D(2.38000000000000, 0.500000000000001   ,0);
    pMesh->mVertices[3] = aiVector3D(4.14000000000000, 2.80000000000000    ,0);
    pMesh->mVertices[4] = aiVector3D(5.28000000000001, -0.920000000000001  ,0);
    pMesh->mVertices[5] = aiVector3D(7.14000000000001, -0.560000000000001  ,0);
    pMesh->mVertices[6] = aiVector3D(5.60000000000001, 3.36000000000000    ,0);
    pMesh->mVertices[7] = aiVector3D(8.74000000000001, 1.30000000000000    ,0);
    pMesh->mVertices[8] = aiVector3D(9.86000000000001, 2.76000000000000    ,0);

    pMesh->mFaces = new aiFace[ 1 ];
    pMesh->mNumFaces = (unsigned int)(1);

    aiFace &face = pMesh->mFaces[0];

    face.mIndices = new unsigned int[9];
    face.mNumIndices = 9;

    face.mIndices[0] = 0;
    face.mIndices[1] = 1;
    face.mIndices[2] = 2;
    face.mIndices[3] = 3;
    face.mIndices[4] = 4;
    face.mIndices[5] = 5;
    face.mIndices[6] = 6;
    face.mIndices[7] = 7;
    face.mIndices[8] = 8;

    Assimp::Exporter *exporter = new Assimp::Exporter();
    exporter->Export(scene, "collada", "face.collada", aiProcess_Triangulate);
}

