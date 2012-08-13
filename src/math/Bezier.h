// ScriptCanvas: An Artful, Interactive Canvas
// Copyright (C) 2011 max.kaufmann@gmail.com
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

#pragma once
#include "Vec2.h"

struct CubicBezier {
	Vec2 p0;
	Vec2 p1;
	Vec2 p2;
	Vec2 p3;
  
	Vec2 ValueAt(float u) const {
		return 
      ((1.0f-u) * (1.0f-u) * (1.0f-u)) * p0 +
      (3.0f * (1.0f-u) * (1.0f-u) * u) * p1 +
      (3.0f * (1.0f-u) * u * u) * p2 +
      (u * u * u) * p3;
	}
  
	Vec2 DerivAt(float u)const {
		return 3.0f * (
      (-(1.0f-u) * (1.0f-u)) * p0 +
      (1.0f - 4.0f * u + 3.0f * u * u) * p1 +
      (2.0f * u - 3.0f * u * u) * p2 +
      (u * u) * p3
    );
	}
  
};

struct QuadraticBezier {
  Vec2 p0;
  Vec2 p1;
  Vec2 p2;
  
    Vec2 ValueAt(float u) const {
        return ((1.0f-u)*(1.0f-u))*p0 + (2.0f*(1.0f-u)*u)*p1 + (u*u)*p2;
    }
    
    Vec2 DerivAt(float u)const {
        return (2.0f*(1.0f-u))*(p1-p0) + (2.0f*u)*(p2-p1);
    }
};
