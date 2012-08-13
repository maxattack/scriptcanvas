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

#include "Line.h"

static float kColinearSlop = 0.0001f;

bool Intersection(const Line& U, const Line& V, float& u) {
  float norm = (V.p1.y - V.p0.y)*(U.p1.x-U.p0.x) - (V.p1.x-V.p0.x)*(U.p1.y-U.p0.y);
  if (norm > -kColinearSlop && norm < kColinearSlop) {
    // lines are parallel
    return false;
  }
  norm = 1.0f / norm;
  u = ((V.p1.x-V.p0.x)*(U.p0.y-V.p0.y) - (V.p1.y-V.p0.y)*(U.p0.x-V.p0.x)) * norm;
  return true;  
}

bool Intersection(const Line& U, const Line& V, float& u, float& v) {
  float norm = (V.p1.y - V.p0.y)*(U.p1.x-U.p0.x) - (V.p1.x-V.p0.x)*(U.p1.y-U.p0.y);
  if (norm > -kColinearSlop && norm < kColinearSlop) {
    // lines are parallel
    u = 0.0f;
    v = 0.0f;
    return false;
  }
  norm = 1.0f / norm;
  u = ((V.p1.x-V.p0.x)*(U.p0.y-V.p0.y) - (V.p1.y-V.p0.y)*(U.p0.x-V.p0.x)) * norm;
  v = ((U.p1.x-U.p0.x)*(U.p0.y-V.p0.y) - (U.p1.y-U.p0.y)*(U.p0.x-V.p0.x)) * norm;
  return true;  
}
