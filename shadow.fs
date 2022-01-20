#version 330

in vec2 texCoord;
in vec4 fragColor;

uniform sampler2D tex;

out vec4 fragOut;

void main()
{
    vec4 sourceColor = texture(tex, texCoord);
    if (sourceColor.a > 0.0f) {
        sourceColor = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    fragOut = sourceColor;
}