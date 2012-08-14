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
#include <cstring>
#include "Vec.h"
#include "Transform.h"

#define MAX_POLYGON_VERTICES 12

struct Polygon {
  int vertexCount;
  Vec vertices[MAX_POLYGON_VERTICES];

  Polygon() : vertexCount(0) {}
  Polygon(int count, Vec* data) : vertexCount(count) { 
    memcpy(vertices, data, count*sizeof(Vec)); 
  }

  bool IsClockwise() const;
  bool IsConvex() const;
  float ComputeArea() const;
  Vec ComputeAveragePosition() const;
  
  void EnsureCanonicalOrdering();
  void ReverseOrdering();

  void ApplyTransformation(const Transform &t);
  void ApplyOffset(Vec u);
  
  void ComputeRegularPolygon(int nSides, float radius);  
  void ComputeEccentricVertices(float eccentricity);

  bool Append(Vec u);

  void ComputeStrokeNormals(Vec* outNormals, float weight=1.0f);
};
