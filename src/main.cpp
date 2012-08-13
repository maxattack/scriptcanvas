#include "EntityComponentSystem.h"
#include <vector>
#include <iostream>

using namespace EntityComponentSystem;

class Duck {
private:
	const char* msg;
	
public:
	Duck() : msg(0) {}
	void SetMessage(const char* aMsg) { msg = aMsg; }
	void Quack() { std::cout << msg << std::endl; }
};

class QuackSystem : public ISystem {
private:
	std::vector<Duck> mDucks;
public:
	ID AddComponent() {
		Duck result;
		mDucks.push_back(result);
		return mDucks.size()-1;
	}

	void DestroyComponent(ID c) {
		// Just leak :P
	}
	
	Duck& GetDuck(ID c) {
		return mDucks[c];
	}
	
	void BatchQuack() {
		for(auto p=mDucks.begin(); p!=mDucks.end(); ++p) {
			p->Quack();
		}
	}
	
};

int main(int argc, char* argv[]) {
	QuackSystem q;
	auto qtype = RegisterSystem(&q);
	auto e = CreateEntity();
	auto c = AddComponent(e, qtype);
	q.GetDuck(GetID(c)).SetMessage("Quack!");
	q.BatchQuack();
}