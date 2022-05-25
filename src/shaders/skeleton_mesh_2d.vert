#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform uMvp {
    mat4 mvp;
} pcs;

// We need to change vertex positions according to bone transform and bone weights.
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
// UV will stay the same.
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in uvec4 bone_indices;
layout(location = 4) in vec4 bone_weights;

layout(binding = 0) uniform mediump sampler2D skeleton_texture;
uniform highp mat4 skeleton_transform;
uniform highp mat4 skeleton_transform_inverse;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = pcs.mvp * vec4(inPosition, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}
