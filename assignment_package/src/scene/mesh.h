#pragma once
#include "smartpointerhelp.h"
#include "face.h"
#include "vertex.h"
#include "halfedge.h"
#include "vector"
#include "drawable.h"

class Mesh : public Drawable
{
public:
    Mesh(OpenGLContext*);
    ~Mesh(); // destructor
    GLenum drawMode() override;
    virtual void create() override;
    void createCube(); // initializes cube structure

    std::vector<uPtr<Face>> faces; // vector of faces
    std::vector<uPtr<HalfEdge>> edges; // vector of edges
    std::vector<uPtr<Vertex>> vertices; // vector of vertices
};
