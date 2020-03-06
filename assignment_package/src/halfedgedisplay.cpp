#include "halfedgedisplay.h"

HalfEdgeDisplay::HalfEdgeDisplay(OpenGLContext *context)
    : Drawable(context)
{
}

HalfEdgeDisplay::~HalfEdgeDisplay()
{}

GLenum HalfEdgeDisplay::drawMode() {
    return GL_LINES;
}

// Creates VBO data to make a visual
// representation of the currently selected Vertex
void HalfEdgeDisplay::create() {
    std::vector<GLuint> idxVec;
    std::vector<glm::vec4> posVec;
    std::vector<glm::vec4> colorVec;

    // fill vectors with two endpoints
    if (representedEdge != nullptr) {
        idxVec.push_back(0);
        idxVec.push_back(1);
        posVec.push_back(glm::vec4(this->representedEdge->vertex->pos, 1));
        posVec.push_back(glm::vec4(this->representedEdge->sym->vertex->pos, 1));
        colorVec.push_back(glm::vec4(1, 1, 0, 1));
        colorVec.push_back(glm::vec4(1, 0, 0, 1));
    }

    count = idxVec.size();
    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxVec.size() * sizeof(GLuint), idxVec.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, posVec.size() * sizeof(glm::vec4), posVec.data(), GL_STATIC_DRAW);

    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colorVec.size() * sizeof(glm::vec4), colorVec.data(), GL_STATIC_DRAW);
}

// updates the edge it represents
void HalfEdgeDisplay::updateEdge(HalfEdge *edge) {
    this->representedEdge = edge;
}
