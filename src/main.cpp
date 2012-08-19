#include "SceneSystem.h"
#include "util/Macros.h"
#include <iostream>
#include <map>


class PrintSystem : public SceneSystem::IComponentManager {
private:
    std::map<ID, const char*> mMessages;

public:
    static const int Id = 0;

    void CreateComponent(ID node) {
        std::cout << "+" << node << std::endl;
        mMessages[node] = "";
    }

    void DestroyComponent(ID node) {
        mMessages.erase(node);
        std::cout << "-" << node << std::endl;
    }

    void SetMessage(ID node, const char* message) {
        ASSERT(mMessages.find(node) != mMessages.end());
        mMessages[node] = message;
    }

    void PrintMessages() {
        for(auto i=mMessages.begin(); i!=mMessages.end(); ++i) {
            std::cout << i->first << ": " << i->second << std::endl;
        }
    }

};

int main(int argc, char* argv[]) {
    using namespace SceneSystem;
    ID root = CreateNode();
    ID child = CreateNode(root);
    ID otherChild = CreateNode();
    AttachNode(root, otherChild);
    std::cout << NodeCount() << std::endl;
    DetachNode(child);
    DestroyNode(root);
    std::cout << NodeCount() << std::endl;
    PrintSystem ps;
    RegisterComponentManager(PrintSystem::Id, &ps);
    AddComponent(child, PrintSystem::Id);
    ps.SetMessage(child, "quack");
    ps.PrintMessages();
    DestroyNode(child);
    return 0;
}
