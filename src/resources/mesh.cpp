#include "mesh.h"

#include "../io/obj_importer.h"

Mesh2d::Mesh2d(const std::string &path) : Resource(path) {

}

Mesh3d::Mesh3d(const std::string &path) : Resource(path) {
    Flint::ObjImporter::load_file(path, surfaces);
}
