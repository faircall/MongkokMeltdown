#version 330

in vec2 texCoord;
in vec4 fragColor;

uniform float timeOfDay;
uniform vec2 sunPos;

out vec4 fragOut;

void main()
{
	float timeOfDayMax = 24.0f;
	// how to lerp again?
	float timeToUse = (timeOfDayMax - timeOfDay) / timeOfDayMax;
	// actually cosine will be a better choice here
    	fragOut = vec4(0.05f * timeToUse, 0.02f, 0.4f * timeToUse, 1.0f);
}