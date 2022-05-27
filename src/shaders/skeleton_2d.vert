#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform uMvp {
    mat4 mvp;
} pcs;

layout(binding = 0) uniform mediump sampler2D skeleton_texture;

layout(binding = 1) uniform highp uTransform{
    mat4 skeleton_transform;
    mat4 skeleton_transform_inverse;
};

// We need to change vertex positions according to bone transform and bone weights.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
// UV will stay the same.
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in vec4 bone_indices;
layout(location = 4) in vec4 bone_weights;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    highp vec4 pos = vec4(inPosition.xy, 0.0, 1.0);

    // Must be a valid bone.
    if (bone_weights != vec4(0.0)) {
        // Skeleton transform.
        ivec4 bone_indicesi = ivec4(bone_indices);

        highp mat2x4 m;

        ivec2 tex_offset = ivec2(bone_indicesi.x % 256, (bone_indicesi.x / 256) * 2);
        m = mat2x4(texelFetch(skeleton_texture, tex_offset, 0), texelFetch(skeleton_texture, tex_offset + ivec2(0, 1), 0)) * bone_weights.x;

        tex_offset = ivec2(bone_indicesi.y % 256, (bone_indicesi.y / 256) * 2);
        m += mat2x4(texelFetch(skeleton_texture, tex_offset, 0), texelFetch(skeleton_texture, tex_offset + ivec2(0, 1), 0)) * bone_weights.y;

        tex_offset = ivec2(bone_indicesi.z % 256, (bone_indicesi.z / 256) * 2);
        m += mat2x4(texelFetch(skeleton_texture, tex_offset, 0), texelFetch(skeleton_texture, tex_offset + ivec2(0, 1), 0)) * bone_weights.z;

        tex_offset = ivec2(bone_indicesi.w % 256, (bone_indicesi.w / 256) * 2);
        m += mat2x4(texelFetch(skeleton_texture, tex_offset, 0), texelFetch(skeleton_texture, tex_offset + ivec2(0, 1), 0)) * bone_weights.w;

        mat4 bone_matrix = skeleton_transform * transpose(mat4(m[0], m[1], vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0))) * skeleton_transform_inverse;

        pos = bone_matrix * pos;
    }

    gl_Position = pcs.mvp * pos;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
