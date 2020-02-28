#pragma once
#include <halfedge.h>
#include <glm/glm.hpp>
#include <QListWidgetItem>

class HalfEdge;

static int lastFace;

class Face : public QListWidgetItem
{
public:
    Face();
    HalfEdge *halfedge; // any halfedge in the face
    glm::vec3 color; // color
    int id; // id of face
    int vertexCount(); // returns vertex count of face
};

