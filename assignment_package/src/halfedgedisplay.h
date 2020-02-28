#pragma once
#include "drawable.h"
#include "halfedge.h"
#include <vector>

class HalfEdgeDisplay : public Drawable
{
protected:
    HalfEdge *representedEdge = nullptr;

public:
    HalfEdgeDisplay(OpenGLContext*);
    ~HalfEdgeDisplay();
    GLenum drawMode() override;
    virtual void create() override;
    void updateEdge(HalfEdge*); // updates the edge it represents
};

