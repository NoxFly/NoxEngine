in vec3 in_Vertex;
in vec3 in_Normal;
in vec3 in_Color;
in vec2 in_TexCoord0;

out vec3 fragPos_worldspace;

out vec3 normal_cameraspace;
out vec3 eyeDir_cameraspace;
out vec3 lightDir_cameraspace;

out vec3 color;
out vec2 coordTexture;

uniform mat4 MVP;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;

void main()
{
    vec4 vertexPos = vec4(in_Vertex, 1.0);
    mat4 MV = V * M;

    gl_Position = MVP * vertexPos;



    fragPos_worldspace = (M * vertexPos).xyz;

    vec3 vertexPos_cameraspace = (MV * vertexPos).xyz;
    eyeDir_cameraspace = vec3(0, 0, 0) - vertexPos_cameraspace;

    vec3 lightPos_cameraspace = (V * vec4(lightPos, 1)).xyz;
    lightDir_cameraspace = lightPos_cameraspace - eyeDir_cameraspace;

    normal_cameraspace = (MV * vec4(in_Normal, 0.0)).xyz;
    
    color = in_Color;
    coordTexture = in_TexCoord0;
}