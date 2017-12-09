#ifndef RESOURCEHOLDER_H
#define RESOURCEHOLDER_H

#include <map>
#include <memory>

#include "noncopyable.h"

template<typename Resource, typename ID>
class ResourceHolder : NonCopyable
{
public:
    using pResourse = std::unique_ptr<Resource>;

    void insert(const ID& key, pResourse value);

    template<typename... Args>
    void constructInPlace(const ID& key, Args&&... args);

    void remove(const ID& key);

    Resource& get(const ID& key);

private:
    std::map<ID, pResourse> m_resources;
};

#include "resourceholder.inl"

#endif // RESOURCEHOLDER_H
