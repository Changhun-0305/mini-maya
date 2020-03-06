#pragma once
#include "face.h"
#include "vertex.h"
#include <QListWidgetItem>


class Face;
class Vertex;

static int lastHalfEdge;

class HalfEdge : public QListWidgetItem
{
public:
    HalfEdge();
    HalfEdge *next; // next halfedge it points to
    HalfEdge *sym; // symmetric halfedge
    Face *face; // face that the edge is on
    Vertex *vertex;
    int id;
    HalfEdge* prevEdge();


};
