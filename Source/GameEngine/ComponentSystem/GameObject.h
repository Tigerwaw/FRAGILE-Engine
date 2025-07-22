#pragma once
#include "EngineDefines.h"

#include "Component.h"

class GameObject final
{
public:
    GameObject() = default;
    virtual ~GameObject() = default;
    void Update();
    void SetActive(bool aActive);
    bool GetActive() const { return myIsActive; }
    void SetStatic(bool aStatic);
    bool GetStatic() const { return myIsStatic; }

    void SetName(const std::string& aName) { myName = aName; }
    const std::string& GetName() const { return myName; }

    void SetID(const unsigned aID) { myID = aID; }
    const unsigned GetID() const { return myID; }
    void SetNetworkID(const unsigned aNetworkID) { myNetworkID = aNetworkID; }
    const unsigned GetNetworkID() const { return myNetworkID; }

    // COMPONENTS
    template <typename T, typename... Args>
    const std::shared_ptr<T> AddComponent(Args&&... args);

    template <typename T>
    const bool RemoveComponent();

    template <typename T>
    const std::shared_ptr<T> GetComponent();

    template <typename T>
    const std::vector<std::shared_ptr<T>> GetComponents();
    // --

private:

    std::vector<std::shared_ptr<Component>> myComponents;
    bool myIsActive = true;
    bool myIsStatic = false;
    float myTimeAlive = 0;

    std::string myName;
    unsigned myID = 0;
    unsigned myNetworkID = 0;
};

template<class T, typename... Args>
const std::shared_ptr<T> GameObject::AddComponent(Args&&... args)
{
    std::shared_ptr<Component> newComponent = myComponents.emplace_back(std::make_shared<T>(std::forward<Args>(args)...));

    if (newComponent.get())
    {
        newComponent->gameObject = this;
        newComponent->Start();

        return std::dynamic_pointer_cast<T>(newComponent);
    }

    return std::shared_ptr<T>();
}

template<typename T>
inline const bool GameObject::RemoveComponent()
{
    std::shared_ptr<T> comp = GetComponent<T>();
    if (comp)
    {
        for (size_t i = 0; i < myComponents.size(); i++)
        {
            if (comp == myComponents[i])
            {
                myComponents.erase(myComponents.begin() + i);
                return true;
            }
        }
    }

    return false;
}

template<typename T>
inline const std::shared_ptr<T> GameObject::GetComponent()
{
    PIXScopedEvent(PIX_COLOR_INDEX(6), "Component System Get Component");

    for (auto& comp : myComponents)
    {
        std::shared_ptr<T> castedComp = std::dynamic_pointer_cast<T>(comp);
        if (castedComp.get())
        {
            return castedComp;
        }
    }

    return std::shared_ptr<T>();
}

template<typename T>
inline const std::vector<std::shared_ptr<T>> GameObject::GetComponents()
{
    std::vector<std::shared_ptr<T>> componentVector;

    for (auto& comp : myComponents)
    {
        std::shared_ptr<T> castedComp = std::dynamic_pointer_cast<T>(comp);
        if (castedComp.get())
        {
            componentVector.emplace_back(castedComp);
        }
    }

    return componentVector;
}
