#include "mesh.h"

Mesh::Mesh(OpenGLContext *context)
    : Drawable(context)
{
}

Mesh::~Mesh()
{}

GLenum Mesh::drawMode()
{
    // Since we want every three indices in bufIdx to be
    // read to draw our Drawable, we tell that the draw mode
    // of this Drawable is GL_TRIANGLES

    // If we wanted to draw a wireframe, we would return GL_LINES

    return GL_TRIANGLES;
}

// overrides Drawable's create function
void Mesh::create() {
    std::vector<GLuint> idxVec; // vector of indices
    std::vector<glm::vec4> posVec; // vector of vertex positions
    std::vector<glm::vec4> colorVec; // vector of colors
    std::vector<glm::vec4> normalVec; // vector of normals
    int i = 0;
    //iterate through faces
    for (uPtr<Face> &face: faces) {
        HalfEdge *curr = face->halfedge;

        // obtain normal from cross product
        glm::vec3 vec1 = glm::normalize(curr->next->vertex->pos - curr->vertex->pos);
        glm::vec3 vec2 = glm::normalize(curr->next->next->vertex->pos - curr->next->vertex->pos);
        glm::vec3 normal = glm::normalize(glm::cross(vec1, vec2));

        int edgeCount = 0;
        // iterate through each edge
        do {
            edgeCount += 1;
            posVec.push_back(glm::vec4(curr->vertex->pos, 1));
            normalVec.push_back(glm::vec4(normal, 1));
            colorVec.push_back(glm::vec4(face->color, 1));
            curr = curr->next;
        } while (curr != face->halfedge);

        // push indices of triangles
        for (int j = 1; j < edgeCount - 1; j++) {
            idxVec.push_back(i);
            idxVec.push_back(i + j);
            idxVec.push_back(i + j + 1);
        }
        i += edgeCount;
    }
    count = idxVec.size();

    //send vbo

    generateIdx();
    bindIdx();
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idxVec.size() * sizeof(GLuint), idxVec.data(), GL_STATIC_DRAW);

    generatePos();
    bindPos();
    mp_context->glBufferData(GL_ARRAY_BUFFER, posVec.size() * sizeof(glm::vec4), posVec.data(), GL_STATIC_DRAW);


    generateNor();
    bindNor();
    mp_context->glBufferData(GL_ARRAY_BUFFER, normalVec.size() * sizeof(glm::vec4), normalVec.data(), GL_STATIC_DRAW);


    generateCol();
    bindCol();
    mp_context->glBufferData(GL_ARRAY_BUFFER, colorVec.size() * sizeof(glm::vec4), colorVec.data(), GL_STATIC_DRAW);
}


// initializes cube structure
void Mesh::createCube() {

    //vertices
    uPtr<Vertex> b1;
    uPtr<Vertex> b2;
    uPtr<Vertex> b3;
    uPtr<Vertex> b4;
    uPtr<Vertex> t1;
    uPtr<Vertex> t2;
    uPtr<Vertex> t3;
    uPtr<Vertex> t4;

    //edges
    uPtr<HalfEdge> eb1 = mkU<HalfEdge>();
    uPtr<HalfEdge> eb2 = mkU<HalfEdge>();;
    uPtr<HalfEdge> eb3 = mkU<HalfEdge>();;
    uPtr<HalfEdge> eb4 = mkU<HalfEdge>();;

    uPtr<HalfEdge> et1 = mkU<HalfEdge>();
    uPtr<HalfEdge> et2 = mkU<HalfEdge>();
    uPtr<HalfEdge> et3 = mkU<HalfEdge>();
    uPtr<HalfEdge> et4 = mkU<HalfEdge>();

    uPtr<HalfEdge> er1 = mkU<HalfEdge>();
    uPtr<HalfEdge> er2 = mkU<HalfEdge>();
    uPtr<HalfEdge> er3 = mkU<HalfEdge>();
    uPtr<HalfEdge> er4 = mkU<HalfEdge>();

    uPtr<HalfEdge> el1 = mkU<HalfEdge>();
    uPtr<HalfEdge> el2 = mkU<HalfEdge>();
    uPtr<HalfEdge> el3 = mkU<HalfEdge>();
    uPtr<HalfEdge> el4 = mkU<HalfEdge>();

    uPtr<HalfEdge> ef1 = mkU<HalfEdge>();
    uPtr<HalfEdge> ef2 = mkU<HalfEdge>();
    uPtr<HalfEdge> ef3 = mkU<HalfEdge>();
    uPtr<HalfEdge> ef4 = mkU<HalfEdge>();

    uPtr<HalfEdge> ebk1 = mkU<HalfEdge>();
    uPtr<HalfEdge> ebk2 = mkU<HalfEdge>();
    uPtr<HalfEdge> ebk3 = mkU<HalfEdge>();
    uPtr<HalfEdge> ebk4 = mkU<HalfEdge>();

    // faces
    uPtr<Face> bottom = mkU<Face>();
    uPtr<Face> top = mkU<Face>();
    uPtr<Face> left = mkU<Face>();
    uPtr<Face> right = mkU<Face>();
    uPtr<Face> front = mkU<Face>();
    uPtr<Face> back = mkU<Face>();

    // fill member variables
    b1 = mkU<Vertex>(-0.5, -0.5, 0.5, eb1.get());
    b2 = mkU<Vertex>(0.5, -0.5, 0.5, eb2.get());
    b3 = mkU<Vertex>(0.5, -0.5, -0.5, eb3.get());
    b4 = mkU<Vertex>(-0.5, -0.5, -0.5, eb4.get());
    t1 = mkU<Vertex>(-0.5, 0.5, -0.5, et1.get());
    t2 = mkU<Vertex>(0.5, 0.5, -0.5, et2.get());
    t3 = mkU<Vertex>(0.5, 0.5, 0.5, et3.get());
    t4 = mkU<Vertex>(-0.5, 0.5, 0.5, et4.get());


    eb1->next = eb4.get();
    eb1->sym = ef3.get();
    eb1->face = bottom.get();
    eb1->vertex = b1.get();

    eb2->next = eb1.get();
    eb2->sym = er4.get();
    eb2->face = bottom.get();
    eb2->vertex = b2.get();

    eb3->next = eb2.get();
    eb3->sym = ebk1.get();
    eb3->face = bottom.get();
    eb3->vertex = b3.get();

    eb4->next = eb3.get();
    eb4->sym = el2.get();
    eb4->face = bottom.get();
    eb4->vertex = b4.get();

    et1->next = et4.get();
    et1->sym = ebk3.get();
    et1->face = top.get();
    et1->vertex = t1.get();

    et2->next = et1.get();
    et2->sym = er2.get();
    et2->face = top.get();
    et2->vertex = t2.get();

    et3->next = et2.get();
    et3->sym = ef1.get();
    et3->face = top.get();
    et3->vertex = t3.get();

    et4->next = et3.get();
    et4->sym = el4.get();
    et4->face = top.get();
    et4->vertex = t4.get();

    er1->next = er4.get();
    er1->sym = ef2.get();
    er1->face = right.get();
    er1->vertex = b2.get();

    er2->next = er1.get();
    er2->sym = et2.get();
    er2->face = right.get();
    er2->vertex = t3.get();

    er3->next = er2.get();
    er3->sym = ebk2.get();
    er3->face = right.get();
    er3->vertex = t2.get();

    er4->next = er3.get();
    er4->sym = eb2.get();
    er4->face = right.get();
    er4->vertex = b3.get();

    el1->next = el4.get();
    el1->sym = ef4.get();
    el1->face = left.get();
    el1->vertex = t4.get();

    el2->next = el1.get();
    el2->sym = eb4.get();
    el2->face = left.get();
    el2->vertex = b1.get();

    el3->next = el2.get();
    el3->sym = ebk4.get();
    el3->face = left.get();
    el3->vertex = b4.get();

    el4->next = el3.get();
    el4->sym = et4.get();
    el4->face = left.get();
    el4->vertex = t1.get();

    ef1->next = ef4.get();
    ef1->sym = et3.get();
    ef1->face = front.get();
    ef1->vertex = t4.get();

    ef2->next = ef1.get();
    ef2->sym = er1.get();
    ef2->face = front.get();
    ef2->vertex = t3.get();

    ef3->next = ef2.get();
    ef3->sym = eb1.get();
    ef3->face = front.get();
    ef3->vertex = b2.get();

    ef4->next = ef3.get();
    ef4->sym = el1.get();
    ef4->face = front.get();
    ef4->vertex = b1.get();

    ebk1->next = ebk4.get();
    ebk1->sym = eb3.get();
    ebk1->face = back.get();
    ebk1->vertex = b4.get();

    ebk2->next = ebk1.get();
    ebk2->sym = er3.get();
    ebk2->face = back.get();
    ebk2->vertex = b3.get();

    ebk3->next = ebk2.get();
    ebk3->sym = et1.get();
    ebk3->face = back.get();
    ebk3->vertex = t2.get();

    ebk4->next = ebk3.get();
    ebk4->sym = el3.get();
    ebk4->face = back.get();
    ebk4->vertex = t1.get();

    bottom->halfedge = eb1.get();
    top->halfedge = et1.get();
    left->halfedge = el1.get();
    right->halfedge = er1.get();
    front->halfedge = ef1.get();
    back->halfedge = ebk1.get();


    vertices.push_back(std::move(b1));
    vertices.push_back(std::move(b2));
    vertices.push_back(std::move(b3));
    vertices.push_back(std::move(b4));
    vertices.push_back(std::move(t1));
    vertices.push_back(std::move(t2));
    vertices.push_back(std::move(t3));
    vertices.push_back(std::move(t4));

    edges.push_back(std::move(eb1));
    edges.push_back(std::move(eb2));
    edges.push_back(std::move(eb3));
    edges.push_back(std::move(eb4));
    edges.push_back(std::move(et1));
    edges.push_back(std::move(et2));
    edges.push_back(std::move(et3));
    edges.push_back(std::move(et4));
    edges.push_back(std::move(el1));
    edges.push_back(std::move(el2));
    edges.push_back(std::move(el3));
    edges.push_back(std::move(el4));
    edges.push_back(std::move(er1));
    edges.push_back(std::move(er2));
    edges.push_back(std::move(er3));
    edges.push_back(std::move(er4));
    edges.push_back(std::move(ef1));
    edges.push_back(std::move(ef2));
    edges.push_back(std::move(ef3));
    edges.push_back(std::move(ef4));
    edges.push_back(std::move(ebk1));
    edges.push_back(std::move(ebk2));
    edges.push_back(std::move(ebk3));
    edges.push_back(std::move(ebk4));



    faces.push_back(std::move(bottom));
    faces.push_back(std::move(top));
    faces.push_back(std::move(left));
    faces.push_back(std::move(right));
    faces.push_back(std::move(front));
    faces.push_back(std::move(back));

}


