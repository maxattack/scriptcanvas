#ifdef VERTEX

attribute vec4 parameterAndSide;

uniform float thickness;
uniform mat4 hermite;
uniform mat4 hermiteDeriv;

void main() {
	vec4 parameter = vec4(parameterAndSide.xyz, 1);
	float side = parameterAndSide.w;
	vec4 normal = normalize(hermiteDeriv * parameter);
	normal = normal.yxzw;
	normal .x = -normal.x;
	gl_Position = gl_ModelViewProjectionMatrix * vec4((
		(hermite * parameter) + (thickness * side) * normal
	).xy, 0, 1);
}

#else

uniform vec4 color;

void main() {
	gl_FragColor = color;	
}

#endif
