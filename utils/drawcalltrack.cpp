#include "drawcalltrack.h"
#include <glad/glad.h>
#include <iostream>

namespace DrawCallTrack
{
int drawCallCount;
int triangleCount;
int getDrawCallCount()
{
    return drawCallCount;
}
int getTriangleCount()
{
    return triangleCount;
}
void resetCount()
{
    drawCallCount = 0;
    triangleCount = 0;
}
void glDrawArrays_track(unsigned mode, int index, int count)
{
    drawCallCount++;
    if (mode == GL_TRIANGLES)
        triangleCount += count / 3;
    else if (mode == GL_TRIANGLE_STRIP || mode == GL_TRIANGLE_FAN)
        triangleCount += count - 2;
    glad_glDrawArrays(mode, index, count);
}
}
