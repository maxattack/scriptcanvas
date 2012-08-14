#include "EntityComponentSystem.h"
#include "util/Table.h"
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

class QuackSystem : public EntityComponentSystem::ISystem, Table<Duck> {
private:
	IndexRecord mIndex[32];
	Duck mDucks[32];

public:
	QuackSystem() : Table(32, mIndex, mDucks) {
	}

	ID CreateComponent() {
		return Add();
	}

	void DestroyComponent(ID c) {
		Remove(c);
	}
	
	Duck& GetDuck(ID c) {
		return (*this)[c];
	}
	
	void BatchQuack() {
		for(auto p=Begin(); p!=End(); ++p) {
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
