struct Light {
    vec4 corner;
    vec4 v1;
    vec4 v2;
    vec4 normal;
    vec4 emission;
};

struct UniformBufferObject {
    mat4 model_view;
    mat4 projection;
    mat4 model_view_inverse;
    mat4 projection_inverse;
    float x;
    //float focus_distance;
    float fov;
    float y;
    //uint total_number_of_samples;
    uint number_of_samples;
    uint number_of_bounces;
    uint seed;
    float z;
    //bool next_event_estimation;
    bool gamma_correction;
    bool tone_map;
    float exposure;
    bool has_sky;
    uint frame;
    bool debug_normals;
    bool debug_radiance;
};
