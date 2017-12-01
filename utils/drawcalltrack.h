#ifndef DRAWCALLTRACK_H_INCLUDED
#define DRAWCALLTRACK_H_INCLUDED

#define TRACK_GL_DRAWCALLS

#ifdef TRACK_GL_DRAWCALLS
    #define glDrawArrays_(mode, index, count) DrawCallTrack::glDrawArrays_track(mode, index, count)
#else
    #define glDrawArrays_(mode, index, count) glad_glDrawArrays(mode, index, count)
#endif

namespace DrawCallTrack
{
int getDrawCallCount();
int getTriangleCount();
void resetCount();
void glDrawArrays_track(unsigned mode, int index, int count);
}

#endif // DRAWCALLTRACK_H_INCLUDED

