#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform uTransform {
    mat4 skeletonTransform;
    mat4 skeletonTransformInverse;
};

layout(binding = 0) uniform mediump sampler2D skeletonTexture;

// We need to change vertex positions according to bone transform and bone weights.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
// UV will stay the same.
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 inBoneIndices;
layout(location = 4) in vec4 inBoneWeights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    // Relative to the root bone.
    highp vec4 pos = vec4(inPosition.xy, 0.0, 1.0);

    // Must be a valid bone.
    if (inBoneWeights != vec4(0.0)) {
        // Skeleton transform.
        ivec4 boneIndicesi = ivec4(inBoneIndices);

        highp mat2x4 m;

        // Get bone transform data from the texture.

        ivec2 tex_offset = ivec2(boneIndicesi.x % 256, (boneIndicesi.x / 256) * 2);
        m = mat2x4(texelFetch(skeletonTexture, tex_offset, 0), texelFetch(skeletonTexture, tex_offset + ivec2(0, 1), 0)) * inBoneWeights.x;

        tex_offset = ivec2(boneIndicesi.y % 256, (boneIndicesi.y / 256) * 2);
        m += mat2x4(texelFetch(skeletonTexture, tex_offset, 0), texelFetch(skeletonTexture, tex_offset + ivec2(0, 1), 0)) * inBoneWeights.y;

        tex_offset = ivec2(boneIndicesi.z % 256, (boneIndicesi.z / 256) * 2);
        m += mat2x4(texelFetch(skeletonTexture, tex_offset, 0), texelFetch(skeletonTexture, tex_offset + ivec2(0, 1), 0)) * inBoneWeights.z;

        tex_offset = ivec2(boneIndicesi.w % 256, (boneIndicesi.w / 256) * 2);
        m += mat2x4(texelFetch(skeletonTexture, tex_offset, 0), texelFetch(skeletonTexture, tex_offset + ivec2(0, 1), 0)) * inBoneWeights.w;

        // Move to origin, do transform, move back to skeleton position.
        mat4 boneMatrix = mat4(m[0], m[1], vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0));

        pos = boneMatrix * pos;
    }

    // To global coordinates.
    gl_Position = skeletonTransform * pos;

    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
