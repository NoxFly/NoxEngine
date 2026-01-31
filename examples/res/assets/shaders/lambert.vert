in vec3 a_Position;
in vec3 a_Normal;

uniform mat4 u_Model;
uniform mat4 u_View;
uniform mat4 u_Projection;

out vec3 v_FragPos;
out vec3 v_Normal;

void main() {
    vec4 worldPos = u_Model * vec4(a_Position, 1.0);
    v_FragPos = worldPos.xyz;
    v_Normal = mat3(transpose(inverse(u_Model))) * a_Normal;
    gl_Position = u_Projection * u_View * worldPos;
}