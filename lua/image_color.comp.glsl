#version 450
layout(location = 0, format = rgba32f) uniform readonly image2D src_surface;
layout(location = 1, format = rgba32f) uniform writeonly image2D dst_surface;
layout(location = 10) uniform vec4 color_point;

void main() {
    ivec2 coord = ivec2(gl_GlobalInvocationID.xy);
    vec4 data = imageLoad(src_surface, coord);
    data *= color_point;
    imageStore(dst_surface, coord, data);
}
