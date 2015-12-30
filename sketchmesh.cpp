#include "sketchmesh.h"

bool SketchMesh::isValid() {
    return this->valid;
}
QString SketchMesh::error() {
    return this->errorMessage;
}
void SketchMesh::setValid(bool valid) {
    this->valid = valid;
}
void SketchMesh::setErrorMessage(QString message) {
    this->setValid(false);
    this->errorMessage = message;
}
QString SketchMesh::getErrorMessage() {
    return this->errorMessage;
}
