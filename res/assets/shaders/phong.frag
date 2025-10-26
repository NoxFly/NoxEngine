in vec3 v_FragPos;
in vec3 v_Normal;

uniform vec3 u_CameraPos;

uniform vec3 u_Diffuse;
uniform vec3 u_Ambient;
uniform vec3 u_Specular;
uniform float u_Shininess;
uniform float u_Opacity;
uniform bool u_Wireframe;

uniform vec3 u_LightPos;
uniform vec3 u_LightColor;
uniform float u_LightPower;

out vec4 FragColor;

void main() {
    vec3 N = normalize(v_Normal);
    vec3 L = normalize(u_LightPos - v_FragPos);
    vec3 V = normalize(u_CameraPos - v_FragPos);
    vec3 R = reflect(-L, N);

    // Ambient
    vec3 ambient = u_Ambient;

    // Diffuse
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * u_Diffuse * u_LightColor * u_LightPower;

    // Specular
    float spec = pow(max(dot(R, V), 0.0), u_Shininess);
    vec3 specular = spec * u_Specular * u_LightColor * u_LightPower;

    vec3 color = ambient + diffuse + specular;
    FragColor = vec4(color, u_Opacity);
}