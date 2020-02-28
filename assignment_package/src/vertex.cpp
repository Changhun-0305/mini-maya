#include "vertex.h"

Vertex::Vertex()
    : QListWidgetItem()
{
    id = lastVertex++;
    this->setText(QString::number(this->id));

}

// constructor with initial positin and halfedge
Vertex::Vertex(float x, float y, float z, HalfEdge *edge)
    : QListWidgetItem(), pos(glm::vec3(x, y, z)), halfedge(edge)
{
    id = lastVertex++;
    this->setText(QString::number(this->id));
}
