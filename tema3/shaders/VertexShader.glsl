#version 430

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

out vec2 texcoord;
out vec3 world_pos;
out vec3 world_normal;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform int shader_type;
// 0 - normal, 1 - water, 2 - emissive, 3 - ui, 4 - moon

void main()
{
    texcoord = v_texture_coord;
    world_pos = (Model * vec4(v_position,1)).xyz;
    world_normal = normalize(mat3(Model) * v_normal );
       
    if(shader_type == 3) 
        gl_Position = Model * vec4(v_position, 1.0);
    else 
        gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
