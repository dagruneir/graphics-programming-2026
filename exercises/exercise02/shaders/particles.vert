#version 330 core

layout (location = 0) in vec2 ParticlePosition;
// (todo) 02.X: Add more vertex attributes
layout (location = 1) in float ParticleSize;
layout (location = 2) in float ParticleBirth;
layout (location = 3) in float ParticleDuration;
layout (location = 4) in vec4 ParticleColor;
layout (location = 5) in vec2 ParticleVelocity;

// (todo) 02.5: Add Color output variable here
out vec4 Color;

// (todo) 02.X: Add uniforms
uniform float CurrentTime;
uniform vec2 Gravity;


void main()
{
	gl_PointSize = ParticleSize;
	float age = CurrentTime - ParticleBirth;
	if (age > ParticleDuration)
	{gl_PointSize = 0.0f;}
	Color = ParticleColor;

	vec2 xt = ParticlePosition + (ParticleVelocity * age) + 0.5 * Gravity * (age * age);
	gl_Position = vec4(xt, 0.0, 1.0);
}
