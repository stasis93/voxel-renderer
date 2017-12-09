#ifndef OUTLINE_H
#define OUTLINE_H

namespace Geom {
class AABB;
}

namespace Outline
{
void render(const Geom::AABB& box);
}

#endif // OUTLINE_H
