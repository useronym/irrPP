#version 140

uniform sampler2D Render;

uniform float PixelSizeX;
uniform float PixelSizeY;

float weights[] = {0.00390625, 0.03125, 0.109375, 0.21875, 0.2734375};


void main()
{
    vec4 blur = vec4(0.0);
    vec2 texcoord = vec2(gl_TexCoord[0]) + vec2(0.0, PixelSizeY/2.0);

    blur += textureOffset(Render, texcoord, ivec2(0, -8)) * weights[0];
    blur += textureOffset(Render, texcoord, ivec2(0, -6)) * weights[1];
    blur += textureOffset(Render, texcoord, ivec2(0, -4)) * weights[2];
    blur += textureOffset(Render, texcoord, ivec2(0, -2)) * weights[3];
    blur += texture2D(Render, vec2(gl_TexCoord[0])) * weights[4];
    blur += textureOffset(Render, texcoord, ivec2(0, 2)) * weights[3];
    blur += textureOffset(Render, texcoord, ivec2(0, 4)) * weights[2];
    blur += textureOffset(Render, texcoord, ivec2(0, 6)) * weights[1];
    blur += textureOffset(Render, texcoord, ivec2(0, 8)) * weights[0];

    gl_FragColor = blur;
}
