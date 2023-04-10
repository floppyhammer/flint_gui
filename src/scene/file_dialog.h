#ifndef FLINT_FILE_DIALOG_H
#define FLINT_FILE_DIALOG_H

#include "node.h"
#include "tinyfiledialogs/tinyfiledialogs.h"

namespace Flint::Scene {

class FileDialog : public Node {
public:
    std::string show();
};

} // namespace Flint

#endif // FLINT_FILE_DIALOG_H
