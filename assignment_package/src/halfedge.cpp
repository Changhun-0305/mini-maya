#include "halfedge.h"

// constructor
HalfEdge::HalfEdge()
    : QListWidgetItem(), sym(nullptr)
{
    id = lastHalfEdge++;
    this->setText(QString::number(this->id));
}

// returns previous edge
HalfEdge* HalfEdge::prevEdge() {
    HalfEdge *curr = this;
    do {
        curr = curr->next;
    } while (curr->next != this);
    return curr;
}
