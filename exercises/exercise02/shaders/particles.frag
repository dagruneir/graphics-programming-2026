#version 330 core

out vec4 FragColor;

// (todo) 02.5: Add Color input variable here


void main()
{
	// (todo) 02.3: Compute alpha using the built-in variable gl_PointCoord
	float alpha = 1.0 - length(gl_PointCoord * 2.0 - 1.0);
	FragColor = vec4(1, 1, 1, alpha);
}
