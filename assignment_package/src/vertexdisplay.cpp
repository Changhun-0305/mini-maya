#include "vertexdisplay.h"

VertexDisplay::VertexDisplay(OpenGLContext *context)
    : Drawable(context)
{
}

VertexDisplay::~VertexDisplay()
{}

GLenum VertexDisplay::drawMode() {
    return GL_POINTS;
}

// Creates VBO data to make a visual
// representation of the currently selected Vertex
void VertexDisplay::create() {
    std::vector<GLuint> idxVec;
    std::vector<glm::vec4> posVec;
    std::vector<glm::vec4> colorVec;
    if (representedVertex != nullptr) {
        idxVec.push_back(0);
        posVec.push_back(glm::vec4(this->representedVertex->pos, 1));
        colorVec.push_back(glm::vec4(1, 1, 1, 1));
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

// updates the represented vertex
void VertexDisplay::updateVertex(Vertex *vertex) {
    this->representedVertex = vertex;
}
