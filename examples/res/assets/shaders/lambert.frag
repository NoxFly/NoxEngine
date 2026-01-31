in vec3 v_FragPos;
in vec3 v_Normal;

uniform vec3 u_Diffuse;
uniform vec3 u_Ambient;
uniform float u_Opacity;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform float u_LightPower;

out vec4 FragColor;

void main() {
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(u_LightPos - v_FragPos);

    vec3 ambient = u_Ambient;
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * u_Diffuse * u_LightColor * u_LightPower;

    vec3 color = ambient + diffuse;
    FragColor = vec4(color, u_Opacity);
}