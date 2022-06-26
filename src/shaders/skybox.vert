#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(push_constant) uniform uWvp {
    mat4 wvp;
} pcs;

layout(location = 0) in vec3 inPosition;

layout(location = 0) out vec3 fragTexCoord;

void main() {
    vec4 wvpPos = pcs.wvp * vec4(inPosition, 1.0);
    gl_Position = wvpPos.xyww;

    fragTexCoord = inPosition;
}
