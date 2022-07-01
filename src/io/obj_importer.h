#ifndef FLINT_OBJ_IMPORTER_H
#define FLINT_OBJ_IMPORTER_H

#include "../resources/surface.h"
#include "../render/mvp.h"

#include <vector>
#include <memory>

namespace Flint {
    class ObjImporter {
    public:
        static void load_file(const std::string &filename,
                              std::vector<std::shared_ptr<Surface3d>> &surfaces);
    };
}

#endif //FLINT_OBJ_IMPORTER_H
