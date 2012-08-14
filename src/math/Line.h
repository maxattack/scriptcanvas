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

#include "Vec.h"

struct Line {
  Vec p0;
  Vec p1;
  
  Vec ValueAt(float u) const { return p0 + u * (p1 - p0); }
  Vec DerivAt(float u) const { return p1 - p0; }
  Vec Delta() const { return p1 - p0; }
  void Offset(Vec d) { p0+=d; p1+=d; }
};

bool Intersection(const Line& U, const Line& V, float& u);
bool Intersection(const Line& U, const Line& V, float& u, float& v);