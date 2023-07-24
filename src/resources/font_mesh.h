#ifndef FLINT_FONT_H
#define FLINT_FONT_H

#include <hb.h>
#include <pathfinder.h>
#include <stb/stb_truetype.h>
#include <ttf2mesh/ttf2mesh.h>

#include <cstdio>
#include <cstdlib>

#include "../render/mesh.h"
#include "common/utils.h"
#include "mesh.h"
#include "resource.h"

using Pathfinder::ColorU;
using Pathfinder::RectF;
using Pathfinder::RectI;
using Pathfinder::Vec2F;

namespace Flint {

class FontMesh : public Resource {
public:
    explicit FontMesh(const std::string &path) {
        ttf_load_from_file(path.c_str(), &font, false);
        if (font == nullptr) {
            abort();
        }
    }

    ~FontMesh() override {
    }

    std::shared_ptr<MeshCpu<Vertex>> create_mesh_from_glyph(uint32_t unicode_char) {
        // Find a glyph in the font file.
        int index = ttf_find_glyph(font, unicode_char);
        if (index < 0) {
            return nullptr;
        }

        // Make mesh object from the glyph.
        ttf_mesh_t *mesh;
        if (ttf_glyph2mesh(&font->glyphs[index], &mesh, TTF_QUALITY_NORMAL, TTF_FEATURES_DFLT) != TTF_DONE) {
            return nullptr;
        }

        ttf_glyph_t *glyph = &font->glyphs[index];

        auto glyph_mesh = std::make_shared<MeshCpu<Vertex>>();

        for (int v = 0; v < mesh->nvert; v++) {
            glyph_mesh->vertices.push_back({{mesh->vert[v].x, mesh->vert[v].y, 0}, {}, {}});
        }

        for (int i = 0; i < mesh->nfaces; i++) {
            glyph_mesh->indices.push_back(mesh->faces[i].v1);
            glyph_mesh->indices.push_back(mesh->faces[i].v2);
            glyph_mesh->indices.push_back(mesh->faces[i].v3);
        }

        ttf_free_mesh(mesh);

        return glyph_mesh;
    }

private:
    std::vector<char> font_data;

private:
    ttf_t *font = nullptr;
};

} // namespace Flint

#endif // FLINT_FONT_H
