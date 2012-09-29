#pragma once
#include "Foundation.h"

namespace NameSystem {

	void Initialize();
	void Destroy();
	void SetName(ID node, const char* name);
	void ClearName(ID node);
	void OnNodeDestroyed(ID node);

	const char* Name(ID node);
	ID FindNode(const char *name);

}
