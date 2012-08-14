#include "EntityComponentSystem.h"
#include "util/Table.h"
#include <vector>
#include <iostream>

using namespace EntityComponentSystem;

class Duck {
public:
	ID id;

private:
	const char* msg;
	
public:
	Duck() : msg(0) {}
	void SetMessage(const char* aMsg) { msg = aMsg; }
	void Quack() { std::cout << msg << std::endl; }
};

class QuackSystem : public ISystem, Table<Duck,32> {
public:
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
	QuackSystem q;
	auto qtype = RegisterSystem(&q);
	auto e = CreateEntity();
	auto c = AddComponent(e, qtype);
	q.GetDuck(c).SetMessage("Quack!");
	q.BatchQuack();
	DestroyEntity(e);
	q.BatchQuack();
}