#pragma once
#include <glm/glm.hpp>
#include <halfedge.h>
#include <QListWidgetItem>

class HalfEdge;

static int lastVertex;

class Vertex : public QListWidgetItem
{
public:
    Vertex();
    // constructor with initial positin and halfedge
    Vertex(float x, float y, float z, HalfEdge *edge);
    glm::vec3 pos; // vertex position
    HalfEdge *halfedge; // halfedge that points to the vertex
    int id; // unique id for vertex
};
