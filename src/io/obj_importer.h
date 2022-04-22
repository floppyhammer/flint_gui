#ifndef FLINT_OBJ_IMPORTER_H
#define FLINT_OBJ_IMPORTER_H

#include "../resources/mesh.h"
#include "../render/mvp_buffer.h"

#include <vector>
#include <memory>

namespace Flint {
    class ObjImporter {
    public:
        static void load_file(const std::string &filename,
                              std::vector<std::shared_ptr<Mesh3d>> &meshes,
                              std::vector<std::shared_ptr<Mesh3dDescSet>> &desc_sets,
                              std::vector<std::shared_ptr<Material3d>> &materials);
    };
}

#endif //FLINT_OBJ_IMPORTER_H
