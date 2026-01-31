#include <chunks/common>

in vec3 v_FragPos;
in vec3 v_Normal;

uniform vec3 u_Diffuse;
uniform vec3 u_Ambient;
uniform float u_Opacity;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform float u_LightPower;

uniform float u_Metalness;
uniform float u_Roughness;

out vec4 FragColor;

float DistributionGGX(vec3 N, vec3 H, float roughness) {
    float a = roughness * roughness;
    float a2 = a*a;
    float NdotH = max(dot(N,H),0.0);
    float NdotH2 = NdotH*NdotH;

    float num = a2;
    float denom = (NdotH2*(a2-1.0)+1.0);
    denom = PI * denom * denom;

    return num / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
    float r = (roughness + 1.0);
    float k = (r*r)/8.0;
    return NdotV / (NdotV*(1.0 - k) + k);
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness) {
    float NdotV = max(dot(N,V),0.0);
    float NdotL = max(dot(N,L),0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, roughness);
    float ggx2 = GeometrySchlickGGX(NdotL, roughness);
    return ggx1 * ggx2;
}

vec3 fresnelSchlick(float cosTheta, vec3 F0) {
    return F0 + (1.0 - F0)*pow(1.0 - cosTheta,5.0);
}

void main() {
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(-v_FragPos); // camera at origin
    vec3 L = normalize(u_LightPos - v_FragPos);
    vec3 H = normalize(V + L);

    vec3 F0 = mix(vec3(0.04), u_Diffuse, u_Metalness);

    float NDF = DistributionGGX(N, H, u_Roughness);
    float G   = GeometrySmith(N, V, L, u_Roughness);
    vec3 F    = fresnelSchlick(max(dot(H,V),0.0), F0);

    float NdotL = max(dot(N,L),0.0);
    vec3 numerator = NDF * G * F;
    float denom = 4.0 * max(dot(N,V),0.0) * NdotL + 0.001;
    vec3 specular = numerator / denom;

    vec3 kS = F;
    vec3 kD = vec3(1.0) - kS;
    kD *= 1.0 - u_Metalness;

    vec3 ambient = u_Ambient;
    vec3 diffuse = kD * u_Diffuse / PI;

    vec3 radiance = u_LightColor * u_LightPower;
    vec3 color = ambient + (diffuse + specular) * radiance * NdotL;

    FragColor = vec4(color, u_Opacity);
}