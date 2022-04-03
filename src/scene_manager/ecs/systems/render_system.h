//
// Created by floppyhammer on 4/2/2022.
//

#ifndef FLINT_RENDER_SYSTEM_H
#define FLINT_RENDER_SYSTEM_H

#include "../system.h"
#include "../entity.h"

#include <set>

namespace Flint {
    class RenderSystem : public System {
    public:
        void draw(uint32_t command_buffer);
    };
}

#endif //FLINT_RENDER_SYSTEM_H
