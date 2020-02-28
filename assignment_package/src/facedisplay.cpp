#include "facedisplay.h"

FaceDisplay::FaceDisplay(OpenGLContext *context)
    : Drawable(context)
{
}

FaceDisplay::~FaceDisplay()
{}

// represented in lines
GLenum FaceDisplay::drawMode() {
    return GL_LINES;
}

// Creates VBO data to make a visual
// representation of the currently selected Vertex
void FaceDisplay::create() {
    std::vector<GLuint> idxVec;
    std::vector<glm::vec4> posVec;
    std::vector<glm::vec4> colorVec;
    if (representedFace != nullptr) {
        int track = 0;
        HalfEdge *currEdge = representedFace->halfedge;
        // iterate through faces
        do {
            idxVec.push_back(track);
            idxVec.push_back(track + 1);
            track += 2;
            posVec.push_back(glm::vec4(currEdge->vertex->pos, 1));
            posVec.push_back(glm::vec4(currEdge->sym->vertex->pos, 1));
            glm::vec4 newColor = glm::vec4(1 - representedFace->color[0],
                                  1 - representedFace->color[1],
                                  1 - representedFace->color[2],
                                  1);
            colorVec.push_back(newColor);
            colorVec.push_back(newColor);
            currEdge = currEdge->next;

        } while (currEdge != representedFace->halfedge);
    }
    // vbo update
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

void FaceDisplay::updateFace(Face *face) {
    this->representedFace = face;
}
