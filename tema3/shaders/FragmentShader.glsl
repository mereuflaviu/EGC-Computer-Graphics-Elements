#version 430

struct Light {
	vec3 position;
	vec3 color;
	vec3 direction;
	float intensity;
    int type;
};

uniform sampler2D texture_1;

uniform float time;
uniform int shader_type;
// 0 - normal, 1 - water, 2 - emissive, 3 - ui, 4 - moon

uniform Light lights[8];
uniform vec3 color;
uniform vec3 eye_position;

in vec2 texcoord;
in vec3 world_pos;
in vec3 world_normal;

layout(location = 0) out vec4 out_color;

vec3 Point(Light light) {
    vec3 L = normalize(light.position - world_pos);
    vec3 V = normalize(eye_position - world_pos);
    vec3 H = normalize(L + V);
    vec3 N = normalize(world_normal);
    
    float ka = 0;
    vec3 ca = ka * vec3(1, 1, 1);

    float kd = 1;
    vec3 cd = kd * light.color * max(dot(N, L), 0);

    vec3 cs = vec3(0, 0, 0);
    if(dot(N, L) > 0)
    {
        float ks = 1;
        cs = ks * light.color * pow(max(dot(N, H), 0), 25);
    }

    float att = 0;
    float d = distance(light.position, world_pos);
    att = 1 / (d * d);
    att *= light.intensity;
        
    return ca + (cd + cs) * att;
}

vec3 Directional(Light light) {
    vec3 L = -light.direction;
    vec3 V = normalize(eye_position - world_pos);
    vec3 H = normalize(L + V);
    vec3 N = normalize(world_normal);
    
    float ka = 0.05f;
    vec3 ca = ka * vec3(1, 1, 1);

    float kd = 1;
    vec3 cd = kd * light.color * max(dot(N, L), 0);

    vec3 cs = vec3(0, 0, 0);
    if(dot(N, L) > 0)
    {
        float ks = 1;
        cs = ks * light.color * pow(max(dot(N, H), 0), 25);
    }

    float att = light.intensity;

    return ca + (cd + cs) * att;
}

vec3 Spot(Light light) {
    vec3 L = normalize(light.position - world_pos);
    vec3 V = normalize(eye_position - world_pos);
    vec3 H = normalize(L + V);
    vec3 N = normalize(world_normal);
    
    float ka = 0;
    vec3 ca = ka * vec3(1, 1, 1);

    float kd = 1;
    vec3 cd = kd * light.color * max(dot(N, L), 0);

    vec3 cs = vec3(0, 0, 0);
    if(dot(N, L) > 0)
    {
        float ks = 1;
        cs = ks * light.color * pow(max(dot(N, H), 0), 50);
    }

    float att = 0;

    float spot_light = dot(-L, light.direction);
    float spot_light_limit = cos(radians(40));
 
    if (spot_light > spot_light_limit)
    {
        float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
        att = pow(linear_att, 2) * light.intensity;
    }

    return ca + (cd + cs) * att;
}

vec4 normal() {
    vec4 texColor = texture2D(texture_1, texcoord);

    vec3 light = vec3(0.0, 0.0, 0.0);
    
    for(int i = 0; i < 8; i++) {
        if(lights[i].type == 0)
            light += Point(lights[i]);
        else if(lights[i].type == 1)
            light += Spot(lights[i]);
        else if(lights[i].type == 2)
            light += Directional(lights[i]);
    }

    return texColor * vec4(light, 1);
}

vec4 water() {
    vec4 texColor = texture2D(texture_1, texcoord);

    vec3 light = vec3(0.0, 0.0, 0.0);
    
    for(int i = 0; i < 8; i++) {
        if(lights[i].type == 0)
            light += Point(lights[i]);
        else if(lights[i].type == 1)
            light += Spot(lights[i]);
        else if(lights[i].type == 2)
            light += Directional(lights[i]);
    }

    vec2 modulated_coords = texcoord * 15 + time * vec2(0.2, -0.2);
    modulated_coords.x += sin(world_pos.x * 5) * 0.01;
    vec4 color = texture2D(texture_1, modulated_coords); 
    texColor = color;
        

    return texColor * vec4(light, 1);
}

vec4 emissive() {
    return vec4(color, 1) * 1.5f;
}

vec4 ui() {
    return vec4(color, 1);
}

vec4 moon() {
    vec4 texColor = texture2D(texture_1, texcoord);
    return texColor + 0.2f * vec4(color, 1);
}

void main()
{
    switch(shader_type)
    {
        case 0:
            out_color = normal();
            break;
        case 1:
            out_color = water();
            break;
        case 2:
            out_color = emissive();
            break;
        case 3:
            out_color = ui();
            break;
        case 4:
            out_color = moon();
            break;
    }
}
