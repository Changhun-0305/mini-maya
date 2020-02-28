#include "face.h"

Face::Face()
    : QListWidgetItem(), color(0, 0, 0)
{
    id = lastFace++;
    // set text of widget item
    this->setText(QString::number(this->id));
}

// returns vertex count of face
int Face::vertexCount() {
    int count = 0;
    HalfEdge* curr = halfedge;
    do {
        count += 1;
        curr = curr->next;
    } while(curr != halfedge);
    return count;
}
