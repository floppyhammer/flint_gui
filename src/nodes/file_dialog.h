#ifndef FLINT_NODE_FILE_DIALOG_H
#define FLINT_NODE_FILE_DIALOG_H

#include <optional>

#include "node.h"

namespace Flint {

class FileDialog final : public Node {
public:
    std::optional<std::string> show();
};

} // namespace Flint

#endif // FLINT_NODE_FILE_DIALOG_H
