#version 140

uniform sampler2D Render;
uniform sampler2D Tex0;

void main()
{
    vec2 texCoord = vec2(gl_TexCoord[0]);
    gl_FragColor = texture2D(Render, texCoord) + texture2D(Tex0, texCoord);
}
