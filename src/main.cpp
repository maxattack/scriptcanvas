#include "SceneSystem.h"
#include "util/CompactPool.h"
#include "util/PinnedPool.h"
#include <vector>
#include <iostream>

struct Duck {
    const char* msg;
    void SetMessage(const char* aMsg) { msg = aMsg; }
    void Quack() { std::cout << msg << std::endl; }
};

class QuackSystem : public SceneSystem::IComponentFactory  {
private:
    StaticPinnedPool<Duck, 32> mPool;

public:
    ID CreateComponent() { return mPool.TakeOut(); }
    void DestroyComponent(ID c) {  mPool.PutBack(c); }
    
    Duck& GetDuck(ID c) { return mPool[c]; }
    
    void BatchQuack() {
        for(auto p=mPool.Enumerate(); p.Next();) { p->Quack(); }
    }
    
};

int main(int argc, char* argv[]) {
    using namespace SceneSystem;
    QuackSystem q;
    auto qtype = RegisterComponentType(&q);
    auto e = CreateEntity();
    auto c = AddComponent(e, qtype);
    q.GetDuck(c).SetMessage("Quack!");

    auto e0 = CreateEntity();
    auto c0 = AddComponent(e0, qtype);
    q.GetDuck(c0).SetMessage("(>'')>");

    q.BatchQuack();
    DestroyEntity(e);
    q.BatchQuack();
}
