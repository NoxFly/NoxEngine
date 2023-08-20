in vec3 normal_cameraspace;
in vec3 fragPos_worldspace;
in vec3 eyeDir_cameraspace;
in vec3 lightDir_cameraspace;

in vec3 color; // unused
in vec2 coordTexture;

out vec4 out_Color;

uniform sampler2D tex;
uniform vec2 textureAndColorOpacity = vec2(0.0, 1.0);
uniform vec3 lightPos;
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float lightPower;
uniform float specPower;


void main()
{
    // Material properties
    vec3 materialDiffuseColor = mix(vec3(1.0), texture(tex, coordTexture).rgb, textureAndColorOpacity.x);
	vec3 materialAmbientColor = mix(vec3(1.0), objectColor, textureAndColorOpacity.y) * materialDiffuseColor;
	vec3 materialSpecularColor = vec3(specPower);

    
    float distance = length(lightPos - fragPos_worldspace);
    distance *= distance;

    vec3 lightColPow = lightColor * lightPower;
    

    // Ambiant
    vec3 ambiante = materialAmbientColor;

    
    // diffuse
    vec3 norm = normalize(normal_cameraspace);
    vec3 lightDir = normalize(lightDir_cameraspace);
    float cosTheta = clamp(dot(norm, lightDir), 0, 1);

    vec3 diffuse = materialDiffuseColor * lightColPow * cosTheta / distance;


    // specular
    vec3 viewDir = normalize(eyeDir_cameraspace);
    vec3 reflectDir = reflect(-lightDir, norm);
    float cosAlpha = clamp(dot(viewDir, reflectDir), 0, 1);

    vec3 specular = materialSpecularColor * lightColPow * pow(cosAlpha, 5.0) / distance;



    out_Color = vec4(ambiante + diffuse + specular, 1.0);
}
