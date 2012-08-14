#include "EntityComponentSystem.h"
#include "util/CompactPool.h"
#include "util/PinnedPool.h"
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
    // PinnedPool<Duck> mPool;
    // uint32_t mMask[1];
    Duck mDucks[32];

public:
    QuackSystem() : mPool(32, mIndex, mDucks) {}
    // QuackSystem() : mPool(32, mMask, mDucks) {}

    ID CreateComponent() { return mPool.TakeOut(); }
    void DestroyComponent(ID c) {  mPool.PutBack(c); }
    Duck& GetDuck(ID c) { return mPool[c]; }
    
    void BatchQuack() {
        for(auto p=mPool.Enumerate(); p.Next();) {
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

    auto e0 = CreateEntity();
    auto c0 = AddComponent(e0, qtype);
    q.GetDuck(c0).SetMessage("(>'')>");

    q.BatchQuack();
    DestroyEntity(e);
    q.BatchQuack();
}
