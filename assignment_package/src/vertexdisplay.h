#pragma once
#include "drawable.h"
#include "vertex.h"
#include <vector>

class VertexDisplay : public Drawable
{
protected:
    // vertex this object represents
    Vertex *representedVertex = nullptr;

public:
    VertexDisplay(OpenGLContext*);
    ~VertexDisplay();
    GLenum drawMode() override;
    virtual void create() override;
    void updateVertex(Vertex*); // updates the represented vertex
};

