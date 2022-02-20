#version 330

in vec2 texCoord;
in vec4 fragColor;

uniform float timeOfDay;
uniform vec2 sunPos;
uniform vec2 horizonPos;

uniform float screenHeight;
uniform float screenWidth;

out vec4 fragOut;

void main()
{
	float timeOfDayMax = 24.0f;
	// how to lerp again?
	float timeToUse = (timeOfDayMax - timeOfDay) / timeOfDayMax;
	// actually cosine will be a better choice here
	float distToHorizon = pow(texCoord.y, abs((horizonPos.y/screenHeight) - texCoord.y));
    	// fragOut = vec4((0.05f * timeToUse) - distToHorizon, 0.02f, 0.4f * timeToUse, 1.0f);
	fragOut = vec4(timeToUse * distToHorizon, 0.02f, 0.4f, 1.0f);
}