#include "sub_viewport_container.h"

namespace Flint {
    std::shared_ptr<SubViewport> SubViewportContainer::get_viewport() {
        return {viewport};
    }
}
