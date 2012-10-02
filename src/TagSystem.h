#pragma once
#include "Foundation.h"
#include <string>

#define kMaxNameLength	32

// replace w/ a "tag" system that can be one-to-many (linked list?)

namespace TagSystem {

	void Initialize();
	void Destroy();
	void SetName(ID node, std::string name);
	void ClearName(ID node);
	void OnNodeDestroyed(ID node);

	std::string Name(ID node);
	ID FindNode(std::string name);

}
