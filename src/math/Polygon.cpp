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

#include "Polygon.h"
#include "Line.h"

bool Polygon::IsClockwise() const {
  float result = 0.0f;
  for (int i=0; i<vertexCount-1; ++i) {
    result += Cross(vertices[i], vertices[i+1]);
  }
  return result < 0.0f;
}

bool Polygon::IsConvex() const {
  if (Cross(vertices[0], vertices[1]) > 0) {
    for(int i=1; i<vertexCount-1; ++i) {
      if (Cross(vertices[i], vertices[i+1]) < 0) {
        return false;
      }
    }
  } else {
    for(int i=1; i<vertexCount-1; ++i) {
      if (Cross(vertices[i], vertices[i+1]) > 0) {
        return false;
      }
    }
  }
  return true;
}

void Polygon::EnsureCanonicalOrdering() {
  if (IsClockwise()) {
    ReverseOrdering();
  }
}

void Polygon::ReverseOrdering() {
  int h = vertexCount >> 1;
  Vec2 p;
  for(int i=0; i<h; ++i) {
    p = vertices[i];
    vertices[i] = vertices[vertexCount-1-i];
    vertices[vertexCount-1-i] = p;
  }
}

Vec2 Polygon::ComputeAveragePosition() const {
  Vec2 result = vec2(0,0);
  for(int i=0; i<vertexCount; ++i) {
    result += vertices[i];
  }
  result /= vertexCount;
  return result;
}

float Polygon::ComputeArea() const {
  float result = 0.0f;
  for (int i=0; i<vertexCount-1; ++i) {
    result += Cross(vertices[i], vertices[i+1]);
  }
  return fabs(0.5f * result);
}

void Polygon::ApplyTransformation(const Transform &t) {
  for(int i=0; i<vertexCount; ++i) {
    vertices[i] = t.TransformPoint(vertices[i]);
  }
}

void Polygon::ApplyOffset(Vec2 u) {
  for(int i=0; i<vertexCount; ++i) {
    vertices[i] += u;
  }
}

void Polygon::ComputeRegularPolygon(int nSides, float radius) {
  vertexCount = nSides;
  Vec2 curr;
  if (nSides % 2 == 0) {
    curr = Polar(radius, kTau / (float) nSides);
  } else {
    curr = Polar(radius, kTau / 4.0f);
  }
  Vec2 rotor = Polar(1.0f, kTau / (float) nSides );
  for(int i=0; i<nSides; ++i) {
    vertices[i] = curr;
    curr *= rotor;
  }
}

void Polygon::ComputeEccentricVertices(float eccentricity) {
  int n = vertexCount >> 1;
  for(int i=0; i<n; ++i) {
    Vec2 p0 = vertices[i+i];
    Vec2 p1 = vertices[(i+i+2)%vertexCount];
    vertices[i+i+1] = (0.5f * (p0 + p1)) + eccentricity * ((p1 - p0).Clockwise());
  }
}

bool Polygon::Append(Vec2 u) {
  if (vertexCount == MAX_POLYGON_VERTICES) {
    return false;
  }
  vertices[vertexCount] = u;
  vertexCount++;
  return true;
}

void Polygon::ComputeStrokeNormals(Vec2* outNormals, float weight) {
  EnsureCanonicalOrdering();
  float _strokeOffsets[vertexCount<<1];
  Vec2* strokeOffsets = (Vec2*) _strokeOffsets;
  for(int i=0; i<vertexCount; ++i) {
    strokeOffsets[i] = 
      ((0.5f * weight) * (vertices[(i+1)%vertexCount] - vertices[i]))
      .Normalized()
      .Anticlockwise();
  }
  for(int i=0; i<vertexCount; ++i) {
    Line l0;
    Line l1;
    l0.p0 = vertices[i];
    l0.p1 = vertices[(i+1)%vertexCount];
    l1.p0 = l0.p1;
    l1.p1 = vertices[(i+2)%vertexCount];
    l0.Offset(strokeOffsets[i]);
    l1.Offset(strokeOffsets[(i+1)%vertexCount]);
    float u;
    outNormals[(i+1)%vertexCount] = 
      l0.ValueAt(Intersection(l0, l1, u)?u:1.0f) - vertices[(i+1)%vertexCount];
  }
}

