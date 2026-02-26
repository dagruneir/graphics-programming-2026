#version 330 core

in vec3 WorldPosition;
in vec3 WorldNormal;
in vec2 TexCoord;
in float Height;

out vec4 FragColor;

uniform vec4 Color;
uniform vec2 ColorTextureScale;

uniform sampler2D GrassTexture;
uniform sampler2D DirtTexture;
uniform sampler2D RockTexture;
uniform sampler2D SnowTexture;

uniform vec2 Range1;
uniform vec2 Range2;
uniform vec2 Range3;


float blend(vec2 range, float h)
{
    if (h <= range.x) return 0.0;
    if (h >= range.y) return 1.0;
    return (h - range.x) / (range.y - range.x);
}

void main()
{
	vec4 GrassTex = texture(GrassTexture, TexCoord * ColorTextureScale);
	vec4 DirtTex = texture(DirtTexture, TexCoord * ColorTextureScale);
	vec4 RockTex = texture(RockTexture, TexCoord * ColorTextureScale);
	vec4 SnowTex = texture(SnowTexture, TexCoord * ColorTextureScale);

	float blend1 = blend(Range1, Height);
	float blend2 = blend(Range2, Height);
	float blend3 = blend(Range3, Height);
	
	vec4 dirtToGrass = mix(DirtTex, GrassTex, blend1);
	vec4 grassToRock = mix(GrassTex, RockTex, blend2);
	vec4 rockToSnow = mix(RockTex, SnowTex, blend3);
	
	if (Height <= Range1.x)
			FragColor = DirtTex * Color;
		else if (Height <= Range1.y)
			FragColor = dirtToGrass * Color;
		else if (Height <= Range2.x)
			FragColor = GrassTex * Color;
		else if (Height <= Range2.y)
			FragColor = grassToRock * Color;
		else if (Height <= Range3.x)
			FragColor = RockTex * Color;
		else if (Height <= Range3.y)
			FragColor = rockToSnow * Color;
		else
			FragColor = SnowTex * Color;
	}


