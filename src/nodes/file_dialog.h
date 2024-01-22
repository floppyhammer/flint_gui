#ifndef FLINT_FILE_DIALOG_H
#define FLINT_FILE_DIALOG_H

#include "node.h"

namespace Flint {

class FileDialog : public Node {
public:
    std::string show();
};

} // namespace Flint

#endif // FLINT_FILE_DIALOG_H
