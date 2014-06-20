attribute vec3 inVertexPosition;
attribute vec2 inTexCoord0;

varying vec2 TexCoord;

void main()
{
    gl_Position = vec4(inVertexPosition, 1.0);
    TexCoord = inTexCoord0;
}
