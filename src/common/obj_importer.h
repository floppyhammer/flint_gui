#ifndef FLINT_OBJ_IMPORTER_H
#define FLINT_OBJ_IMPORTER_H

#include <memory>
#include <vector>

#include "../resources/surface.h"

namespace Flint::Utils {

class ObjImporter {
public:
    static void load_file(const std::string &filename, std::vector<std::shared_ptr<Surface3d>> &surfaces);
};

} // namespace Flint::Utils

#endif // FLINT_OBJ_IMPORTER_H
