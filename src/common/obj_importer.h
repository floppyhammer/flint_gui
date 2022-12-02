#ifndef FLINT_OBJ_IMPORTER_H
#define FLINT_OBJ_IMPORTER_H

#include <memory>
#include <vector>

#include "../render/mvp.h"
#include "../resources/surface.h"

namespace Flint {

class ObjImporter {
public:
    static void load_file(const std::string &filename, std::vector<std::shared_ptr<Surface3d>> &surfaces);
};

} // namespace Flint

#endif // FLINT_OBJ_IMPORTER_H
