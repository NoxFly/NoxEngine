in vec4 color;
in vec2 coordTexture;

uniform sampler2D tex;

out vec4 out_Color;

void main()
{
    vec4 tex = texture(tex, coordTexture);
    
    // color : x, y, z, w
    //         r  g  b  a
    // linear interpolation between the texture and the color
    // formula : (1 - t) * a + t * b
    // where t is between 0 and 1, representing the blend
    // between a and b, 0 = a, 1 = b.
    // In our case,
    //      - a is the texture
    //      - b is the color rgb
    //      - t is the color alpha

    out_Color = tex;

    /* float a = color.w;

    vec3 blend = {
        (1 - a) * tex.x + a * color.x, // r
        (1 - a) * tex.y + a * color.y, // g
        (1 - a) * tex.z + a * color.z  // b
    };

    out_Color = vec4(blend, 1.0); */
}