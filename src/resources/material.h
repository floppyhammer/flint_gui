#ifndef FLINT_MATERIAL_H
#define FLINT_MATERIAL_H

#include "image_texture.h"

#include <memory>
#include <iostream>

/**
 * Do not use the base class directly.
 */
class MaterialDescSet {
public:
    ~MaterialDescSet();

    /**
     * A descriptor pool is used to allocate descriptor sets of some layout for use in a shader.
     * Do this before creating descriptor sets.
     * @dependency None.
     */
    virtual void createDescriptorPool() = 0;

    /**
     * Allocate descriptor sets in the pool.
     * @dependency Descriptor pool, descriptor set layout.
     */
    virtual void createDescriptorSet() = 0;

    /**
     * Should be called once uniform/texture bindings changed.
     * @dependency Actual resources (buffers, images, image views).
     */
    //virtual void updateDescriptorSet(std::shared_ptr<Material>, std::vector<VkBuffer> &uniformBuffers) = 0;

    [[nodiscard]] VkDescriptorSet getDescriptorSet(uint32_t index) const;

protected:
    /// A descriptor pool maintains a pool of descriptors, from which descriptor sets are allocated.
    VkDescriptorPool descriptorPool{};

    /// Descriptor sets are allocated from descriptor pool objects.
    std::vector<VkDescriptorSet> descriptorSets;
};

class Material2dDescSet : public MaterialDescSet {
public:
    Material2dDescSet();

    void createDescriptorPool() override;

    void createDescriptorSet() override;

    void updateDescriptorSet(const std::shared_ptr<ImageTexture>& p_texture);
};

class Material3dDescSet : public MaterialDescSet {
public:
    Material3dDescSet();

    void createDescriptorPool() override;

    void createDescriptorSet() override;

    void updateDescriptorSet(const std::shared_ptr<ImageTexture>& p_texture);
};

/// Material Resources

class Material : public Resource {
};

class Material2d : public Material {
public:
    Material2d();

    void set_texture(const std::shared_ptr<ImageTexture>& p_texture);
    std::shared_ptr<ImageTexture> get_texture();

    std::shared_ptr<Material2dDescSet> get_desc_set() {
        return desc_set;
    }

private:
    std::shared_ptr<ImageTexture> texture;

    std::shared_ptr<Material2dDescSet> desc_set;
};

class Material3d : public Material {
public:
    Material3d();

    static std::shared_ptr<Material3d> from_default();

    void set_diffuse_texture(std::shared_ptr<ImageTexture> p_texture);

    std::shared_ptr<Material3dDescSet> get_desc_set() {
        return desc_set;
    }

private:
    std::shared_ptr<ImageTexture> diffuse_texture;
    std::shared_ptr<ImageTexture> normal_texture;

    std::shared_ptr<Material3dDescSet> desc_set;
};

#endif //FLINT_MATERIAL_H
