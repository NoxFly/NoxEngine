in vec3 color; // unused
in vec2 coordTexture;

uniform vec2 textureAndColorOpacity = vec2(0.0, 1.0);
uniform sampler2D tex;
uniform vec3 objectColor;

out vec4 out_Color;

void main()
{
    vec3 materialTexture = mix(vec4(1.0), texture(tex, coordTexture), textureAndColorOpacity.x).rgb;
    vec3 materialAmbientColor = mix(vec3(1.0), objectColor, textureAndColorOpacity.y) * materialTexture;

    out_Color = vec4(materialAmbientColor, 1.0);
}