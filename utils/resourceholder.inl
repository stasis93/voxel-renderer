#include "resourceholder.h"
#include <cassert>

template<typename Resource, typename ID>
void ResourceHolder<Resource, ID>::insert(const ID& key, pResourse value)
{
    auto res = m_resources.emplace(key, std::move(value));
    assert(res.second);
}

template<typename Resource, typename ID>
template<typename... Args>
void ResourceHolder<Resource, ID>::constructInPlace(const ID& key, Args&&... args)
{
    pResourse ptr = std::make_unique<Resource>(std::forward<Args>(args)...);
    insert(key, std::move(ptr));
}

template<typename Resource, typename ID>
void ResourceHolder<Resource, ID>::remove(const ID& key)
{
    auto iter = m_resources.find(key);
    assert(iter != m_resources.end());
    m_resources.erase(iter);
}

template<typename Resource, typename ID>
Resource& ResourceHolder<Resource, ID>::get(const ID& key)
{
    auto iter = m_resources.find(key);
    assert(iter != m_resources.end());
    return *iter->second;
}

template<typename Resource, typename ID>
void ResourceHolder<Resource, ID>::clear()
{
    m_resources.clear();
}
