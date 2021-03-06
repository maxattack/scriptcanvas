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

#pragma once
#include "Foundation.h"
#include <string>

#define kMaxNameLength	32

// Needs some love or needs to die

namespace TagSystem {

	void Initialize();
	void Destroy();
	void SetName(ID node, std::string name);
	void ClearName(ID node);
	void OnNodeDestroyed(ID node);

	std::string Name(ID node);
	ID FindNode(std::string name);

}
