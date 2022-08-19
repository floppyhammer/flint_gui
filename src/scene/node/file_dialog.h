#ifndef FLINT_FILE_DIALOG_H
#define FLINT_FILE_DIALOG_H

#include "node.h"

#include <tinyfiledialogs/tinyfiledialogs.h>

namespace Flint {
    class FileDialog : public Node {
    public:
        void show();
    };
}

#endif //FLINT_FILE_DIALOG_H
