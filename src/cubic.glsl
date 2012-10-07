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

#ifdef VERTEX

attribute vec4 parameterAndSide;
uniform mat4 positionMatrix;
uniform mat4 normalMatrix;
uniform vec4 strokeVector;


void main() {
	vec4 parameter = vec4(parameterAndSide.xyz, 1);
	gl_Position = gl_ModelViewProjectionMatrix * vec4((
		(positionMatrix * parameter) + 
		(dot(strokeVector, parameter) * parameterAndSide.w) * normalize(normalMatrix * parameter)
	).xyz, 1);
}

#else

uniform vec4 color;

void main() {
	gl_FragColor = color;	
}

#endif
