precision mediump float;

uniform sampler2D Render;

varying vec2 TexCoord;

void main()
{
    gl_FragColor = texture2D(Render, TexCoord);
}

