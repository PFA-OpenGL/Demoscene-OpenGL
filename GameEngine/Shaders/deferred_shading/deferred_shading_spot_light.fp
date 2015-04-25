#version 330

out vec4 color;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform sampler2D normal_buffer;
uniform sampler2D depth_buffer;
uniform vec3 eye_pos;

uniform vec3 position_light;
uniform vec3 attenuation_light;
uniform vec3 direction_light;
uniform vec3 color_light;
uniform mat4 matrix_light;
uniform sampler2D shadow_map;

uniform float spot_cut_off;
uniform float exponent_light;

in vec2 interpolated_texCoord;

vec3 getWorldPosition(float depth, vec2 screenPos, mat4 viewProj)
{
	vec4 worldPos = vec4(screenPos * 2.0f - 1.0f, depth * 2.0f - 1.0f, 1.0f);
	worldPos = inverse(viewProj) * worldPos;
	worldPos /= worldPos.w;
	return (vec3(worldPos));
}

void main()
{
	mat4 viewProj = projection_matrix * view_matrix;
	float depth = texture2D(depth_buffer, interpolated_texCoord).x;
	vec3 worldPos = getWorldPosition(depth, interpolated_texCoord, viewProj);
	vec3 lightDir = vec3(position_light - worldPos);
	float dist = length(lightDir);
	vec3 normal = normalize(vec3(texture(normal_buffer, interpolated_texCoord).xyz) * 2.0f - 1.0f);
	float attenuation = attenuation_light.x + attenuation_light.y * dist + attenuation_light.z * dist * dist; 
	lightDir = normalize(lightDir);
	float cosTheta = dot(normal, lightDir);
	float lambert = max(0.0f, cosTheta);
	float effect = max(0.0f, dot(-lightDir, direction_light));
	effect = effect * step(spot_cut_off, effect);
	effect = pow(effect, exponent_light);
	vec3 worldPosToEyes = normalize(eye_pos - worldPos);
	vec3 reflection = reflect(normalize(-lightDir), normal);
	float specularRatio = clamp(pow(max(dot(reflection, worldPosToEyes), 0.0f), 100.f), 0.0f, 1.0f);
	vec4 shadowPos = matrix_light * vec4(worldPos, 1.0f);
	shadowPos = shadowPos * 0.5 + 0.5;
	shadowPos /= shadowPos.w;
	float visibility = 1.0f;
	float bias = clamp(0.005f * tan(acos(cosTheta)), 0.f, 0.01f);
	float get_depth = texture2D(shadow_map, shadowPos.xy).z;
	if (get_depth < (shadowPos.z - bias)) {
		visibility = 0.f;
	}
	color = (vec4(vec3(lambert * color_light), specularRatio) * effect / (attenuation)) * visibility/** 0.001f + vec4(1.0f - (1.f - texture(shadow_map, 1 - interpolated_texCoord).x) * 25.0f, 0.f, 0.f, 0.f)*/;
}