#include "EntityComponentSystem.h"
#include "util/CompactPool.h"
#include <vector>
#include <iostream>

class Duck {
public:
	ID id;

private:
	const char* msg;
	
public:
	void SetMessage(const char* aMsg) { msg = aMsg; }
	void Quack() { std::cout << msg << std::endl; }
};

class QuackSystem : public EntityComponentSystem::ISystem  {
private:
	CompactPool<Duck> mPool;
	PoolIndex mIndex[32];
	Duck mDucks[32];

public:
	QuackSystem() : mPool(32, mIndex, mDucks) {
	}

	ID CreateComponent() {
		return mPool.Add();
	}

	void DestroyComponent(ID c) {
		mPool.Remove(c);
	}
	
	Duck& GetDuck(ID c) {
		return mPool[c];
	}
	
	void BatchQuack() {
		for(auto p=mPool.Begin(); p!=mPool.End(); ++p) {
			p->Quack();
		}
	}
	
};

int main(int argc, char* argv[]) {
	using namespace EntityComponentSystem;
	QuackSystem q;
	auto qtype = RegisterComponentType(&q);
	auto e = CreateEntity();
	auto c = AddComponent(e, qtype);
	q.GetDuck(c).SetMessage("Quack!");
	q.BatchQuack();
	DestroyEntity(e);
	q.BatchQuack();
}
