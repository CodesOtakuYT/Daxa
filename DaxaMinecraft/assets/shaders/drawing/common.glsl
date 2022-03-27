
layout(push_constant) uniform Push {
    uint globals_sb;
    uint output_image_i;

    uint chunk_image_0i;
    uint chunk_image_1i;
} p;

layout(set = 0, binding = 3, r32ui) uniform readonly uimage3D input_images[];
layout(set = 0, binding = 3, rgba8) uniform writeonly image2D output_images[];

#include "utils/common.glsl"
