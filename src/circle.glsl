#ifdef VERTEX

attribute vec2 unit;
uniform float radius;

void main() {
	gl_Position = gl_ModelViewProjectionMatrix * vec4(
		radius * unit, 0, 1
	);
}


#else

uniform vec4 color;

void main() {
	gl_FragColor = color;
}

#endif
