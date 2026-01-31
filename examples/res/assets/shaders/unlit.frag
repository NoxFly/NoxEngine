uniform vec3 u_Diffuse;
uniform float u_Opacity;

out vec4 FragColor;

void main() {
    FragColor = vec4(u_Diffuse, u_Opacity);
}