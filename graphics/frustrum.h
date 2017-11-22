#ifndef FRUSTRUM_H_INCLUDED
#define FRUSTRUM_H_INCLUDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Frustrum
{
public:
    enum Result
    {
        Inside, Outside, Intersect
    };

                        Frustrum() = default;

    void updatePlanes(  const glm::vec3& cam_pos,
                        const glm::vec3& cam_dir,
                        const glm::vec3& cam_up,
                        const glm::vec3& cam_right,
                        float fovy, float aspect,
                        float dNear, float dFar);

    int checkPoint   (const glm::vec3& p) const;
    int checkSphere  (const glm::vec3& center, float rad) const;
//    int checkBox     (/*Box b*/) const;

private:
    float               distanceToPlane(const glm::vec3& point, int plane) const;

private:
    struct Plane
    {
        glm::vec3 norm;
        glm::vec3 point;
    };
    enum class Planes { Near = 0, Far, Top, Bottom, Left, Right };
    Plane m_planes[6];
};

#endif // FRUSTRUM_H_INCLUDED
