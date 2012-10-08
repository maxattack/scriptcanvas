// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2012 max.kaufmann@gmail.com
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

varying vec2 uv;

#ifdef VERTEX

attribute vec2 unit;
uniform float radius;

void main() {
	gl_Position = gl_ModelViewProjectionMatrix * vec4(radius * unit, 0, 1);
	uv = unit;
}


#else

uniform vec4 color;
uniform vec4 irisColor;
uniform float irisRadiusSq;

void main() {
	gl_FragColor = mix(
		color, 
		irisColor, 
		step(irisRadiusSq, dot(uv, uv))
	);
}

#endif
