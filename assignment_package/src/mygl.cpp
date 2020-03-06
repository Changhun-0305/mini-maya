#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <QFileDialog>
#include <QFile>
#include <fstream>
#include <sstream>
#include <QStringList>


MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomSquare(this), m_mesh(this),
      m_progLambert(this), m_progFlat(this),
      m_glCamera(), vDisplay(this), eDisplay(this), fDisplay(this),
      selectedVertex(nullptr), selectedEdge(nullptr), selectedFace(nullptr)
{
    setFocusPolicy(Qt::StrongFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_mesh.destroy();
    m_geomSquare.destroy();
    vDisplay.destroy();
    eDisplay.destroy();
    fDisplay.destroy();
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.5, 0.5, 0.5, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    m_mesh.destroy();
    m_mesh.createCube();
    m_mesh.create();

    vDisplay.destroy();
    vDisplay.create();

    eDisplay.destroy();
    eDisplay.create();

    fDisplay.destroy();
    fDisplay.create();


    //Create the instances of Cylinder and Sphere.
    m_geomSquare.create();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");


    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    for (auto &pointer : m_mesh.vertices) {
        emit sig_sendVertices(pointer.get());
    }
    for (auto &pointer : m_mesh.edges) {
        emit sig_sendEdges(pointer.get());
    }
    for (auto &pointer : m_mesh.faces) {
        emit sig_sendFaces(pointer.get());
    }

}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_glCamera = Camera(w, h);
    glm::mat4 viewproj = m_glCamera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function update() is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_progFlat.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setViewProjMatrix(m_glCamera.getViewProj());
    m_progLambert.setCamPos(glm::vec3(m_glCamera.eye));
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    //glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    glm::mat4 model = glm::mat4();
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
    //m_progLambert.draw(m_geomSquare);

    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
    //model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));


    m_progLambert.setModelMatrix(model);
    //m_progLambert.draw(m_geomSquare);
    m_progLambert.draw(m_mesh);

    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(model);
    m_progFlat.draw(vDisplay);
    m_progFlat.draw(eDisplay);
    m_progFlat.draw(fDisplay);


    glEnable(GL_DEPTH_TEST);
}


void MyGL::keyPressEvent(QKeyEvent *e)
{
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_glCamera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_glCamera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_glCamera.RotateAboutRight(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_glCamera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_1) {
        m_glCamera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        m_glCamera.fovy -= amount;
    } else if (e->key() == Qt::Key_W) {
        m_glCamera.TranslateAlongLook(amount);
    } else if (e->key() == Qt::Key_S) {
        m_glCamera.TranslateAlongLook(-amount);
    } else if (e->key() == Qt::Key_D) {
        m_glCamera.TranslateAlongRight(amount);
    } else if (e->key() == Qt::Key_A) {
        m_glCamera.TranslateAlongRight(-amount);
    } else if (e->key() == Qt::Key_Q) {
        m_glCamera.TranslateAlongUp(-amount);
    } else if (e->key() == Qt::Key_E) {
        m_glCamera.TranslateAlongUp(amount);
    } else if (e->key() == Qt::Key_R) {
        m_glCamera = Camera(this->width(), this->height());
    } else if (e->key() == Qt::Key_N) {
        slot_halfEdgeSelected(selectedEdge->next);
    } else if (e->key() == Qt::Key_M) {
        slot_halfEdgeSelected(selectedEdge->sym);
    } else if (e->key() == Qt::Key_F) {
        slot_faceSelected(selectedEdge->face);
    } else if (e->key() == Qt::Key_V) {
        slot_vertexSelected(selectedEdge->vertex);
    } else if ((e->modifiers() & Qt::ShiftModifier) && e->key() == Qt::Key_H) {
        slot_halfEdgeSelected(selectedFace->halfedge);
    } else if (e->key() == Qt::Key_H) {
        slot_halfEdgeSelected(selectedVertex->halfedge);
    }
    m_glCamera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

// slot for vertex selection
void MyGL::slot_vertexSelected(QListWidgetItem* v) {
    Vertex *vertex = dynamic_cast<Vertex*>(v);
    if (vertex) {
        selectedVertex = vertex;
        vDisplay.updateVertex(vertex);
        vDisplay.destroy();
        vDisplay.create();
        selectedEdge = nullptr;
        eDisplay.updateEdge(nullptr);
        fDisplay.updateFace(nullptr);
        selectedFace = nullptr;
        eDisplay.destroy();
        eDisplay.create();
        fDisplay.destroy();
        fDisplay.create();
        this->update();
    }
}

// slot for edge selection
void MyGL::slot_halfEdgeSelected(QListWidgetItem* e) {
    HalfEdge *edge = dynamic_cast<HalfEdge*>(e);

    if (edge) {
        selectedEdge = edge;
        eDisplay.updateEdge(edge);
        eDisplay.destroy();
        eDisplay.create();
        selectedVertex = nullptr;
        vDisplay.updateVertex(nullptr);
        fDisplay.updateFace(nullptr);
        selectedFace = nullptr;
        vDisplay.destroy();
        vDisplay.create();
        fDisplay.destroy();
        fDisplay.create();
        this->update();
    }
}

// slot for face selection
void MyGL::slot_faceSelected(QListWidgetItem* f) {
    Face *face = dynamic_cast<Face*>(f);

    if (face) {
        selectedFace = face;
        fDisplay.updateFace(face);
        fDisplay.destroy();
        fDisplay.create();
        this->update();
        selectedEdge = nullptr;
        eDisplay.updateEdge(nullptr);
        vDisplay.updateVertex(nullptr);
        selectedVertex = nullptr;
        eDisplay.destroy();
        eDisplay.create();
        vDisplay.destroy();
        vDisplay.create();
    }
}

// slot for vertex translation in x
void MyGL::slot_vertexTranslateX(double x) {
    if (selectedVertex != nullptr) {
        selectedVertex->pos.x = x;
        m_mesh.destroy();
        m_mesh.create();
        vDisplay.destroy();
        vDisplay.create();
        this->update();
    }
}

// slot for vertex translation in y
void MyGL::slot_vertexTranslateY(double x) {
    if (selectedVertex != nullptr) {
        selectedVertex->pos.y = x;
        m_mesh.destroy();
        m_mesh.create();
        vDisplay.destroy();
        vDisplay.create();
        this->update();
    }
}

// slot for vertex translation in z
void MyGL::slot_vertexTranslateZ(double x) {
    if (selectedVertex != nullptr) {
        selectedVertex->pos.z = x;
        m_mesh.destroy();
        m_mesh.create();
        vDisplay.destroy();
        vDisplay.create();
        this->update();
    }
}

// slot for color change in r
void MyGL::slot_changeFaceR(double x) {
    if (selectedFace != nullptr) {
        selectedFace->color.r = x;
        m_mesh.destroy();
        m_mesh.create();
        fDisplay.destroy();
        fDisplay.create();
        this->update();
    }
}

// slot for color change in g
void MyGL::slot_changeFaceG(double x) {
    if (selectedFace != nullptr) {
        selectedFace->color.g = x;
        m_mesh.destroy();
        m_mesh.create();
        fDisplay.destroy();
        fDisplay.create();
        this->update();
    }
}

// slot for color change in b
void MyGL::slot_changeFaceB(double x) {
    if (selectedFace != nullptr) {
        selectedFace->color.b = x;
        m_mesh.destroy();
        m_mesh.create();
        fDisplay.destroy();
        fDisplay.create();
        this->update();
    }
}

// slot for adding a vertex to current halfedge
void MyGL::slot_addVertex() {
    if (selectedEdge != nullptr) {
        Vertex* v1 = selectedEdge->vertex;
        Vertex* v2 = selectedEdge->sym->vertex;
        HalfEdge *h1 = selectedEdge;
        HalfEdge *h2 = selectedEdge->sym;
        uPtr<Vertex> v3 = mkU<Vertex>();
        v3->pos = glm::vec3((v1->pos.x + v2->pos.x) / 2,
                            (v1->pos.y + v2->pos.y) / 2,
                            (v1->pos.z + v2->pos.z) / 2);
        uPtr<HalfEdge> e1 = mkU<HalfEdge>();
        uPtr<HalfEdge> e2 = mkU<HalfEdge>();
        e1->face = h1->face;
        e2->face = h2->face;
        e1->sym = h2;
        e2->sym = h1;
        h1->sym = e2.get();
        h2->sym = e1.get();
        e1->next = h1->next;
        e2->next = h2->next;
        h1->next = e1.get();
        h2->next = e2.get();
        e1->vertex = h1->vertex;
        h1->vertex = v3.get();
        e2->vertex = h2->vertex;
        h2->vertex = v3.get();
        emit sig_sendEdges(e1.get());
        emit sig_sendEdges(e2.get());
        emit sig_sendVertices(v3.get());
        m_mesh.edges.push_back(std::move(e1));
        m_mesh.edges.push_back(std::move(e2));
        m_mesh.vertices.push_back(std::move(v3));
        m_mesh.destroy();
        m_mesh.create();
        eDisplay.destroy();
        eDisplay.create();
        this->update();
    }
}

// slot for triangulating the current face
void MyGL::slot_triangulate() {
    if (selectedFace != nullptr) {
        int count = selectedFace->vertexCount();
        Face *currFace = selectedFace;
        // keep triangulating until every parts are triangles
        for (int i = 0; i < count - 3; i++) {
            HalfEdge *edge = currFace->halfedge;
            uPtr<HalfEdge> e1 = mkU<HalfEdge>();
            uPtr<HalfEdge> e2 = mkU<HalfEdge>();
            e1->vertex = edge->vertex;
            e2->vertex = edge->next->next->vertex;
            e1->sym = e2.get();
            e2->sym = e1.get();
            uPtr<Face> f2 = mkU<Face>();
            f2->color = glm::vec3(currFace->color.r,
                                  currFace->color.g,
                                  currFace->color.b);
            e1->face = f2.get();
            edge->next->face = f2.get();
            edge->next->next->face = f2.get();
            e2->face = currFace;
            f2->halfedge = e1.get();
            e2->next = edge->next->next->next;
            edge->next->next->next = e1.get();
            e1->next = edge->next;
            edge->next = e2.get();
            emit sig_sendEdges(e1.get());
            emit sig_sendEdges(e2.get());
            emit sig_sendFaces(f2.get());
            m_mesh.edges.push_back(std::move(e1));
            m_mesh.edges.push_back(std::move(e2));
            m_mesh.faces.push_back(std::move(f2));
        }
    }
    m_mesh.destroy();
    m_mesh.create();
    fDisplay.destroy();
    fDisplay.create();
    this->update();
}


/// slot for subdividing mesh
void MyGL::slot_subdivide() {
    // compute centroids of faces
    computeCentroids();
    // compute midpoints of edges
    computeMidPts();
    // smooth vertices of original vertices
    smoothVertices();

    std::map<int, HalfEdge*> nextMap;

    std::vector<HalfEdge*> toSplit;
    for (uPtr<HalfEdge> &e : m_mesh.edges) {
        if (std::find(toSplit.begin(), toSplit.end(), e->sym) == toSplit.end()) {
            toSplit.push_back(e.get());
        }
    }
    for (HalfEdge *e : toSplit) {
        splitByMidPt(e);
    }
    for (uPtr<HalfEdge> &e : m_mesh.edges) {
        nextMap[e->id] = e->next;
    }


    std::vector<uPtr<Face>> newFaces;
    // iterate through each face and subdivide
    for (uPtr<Face> &face : m_mesh.faces) {
        HalfEdge *curr = face->halfedge;
        HalfEdge *start = face->halfedge;
        uPtr<Vertex> ct = std::move(centroids[face->id]);
        int count = face->vertexCount() / 2;
        std::vector<uPtr<HalfEdge>> newEdges;

        // make new edges
        for (int i = 0; i < count; i++) {
            uPtr<HalfEdge> e1 = mkU<HalfEdge>();
            uPtr<HalfEdge> e2 = mkU<HalfEdge>();
            if (i != 0) {
                e2->sym = newEdges[newEdges.size()-2].get();
                newEdges[newEdges.size()-2]->sym = e2.get();
            }
            newEdges.push_back(std::move(e1));
            newEdges.push_back(std::move(e2));
        }
        newEdges[newEdges.size()-2]->sym = newEdges[1].get();
        newEdges[1]->sym = newEdges[newEdges.size()-2].get();
        int track = 0;


        //quadrangulate faces
        do {
            HalfEdge *nextEdge = curr->next;
            HalfEdge *e1 = newEdges[track].get();
            HalfEdge *e2 = newEdges[track+1].get();

            nextEdge->next = e1;
            e1->next = e2;
            e2->next = curr;
            if (nextMap[nextEdge->id] == face->halfedge) {
                e1->face = face.get();
                e2->face = face.get();
                face->halfedge = curr;
            } else {
                uPtr<Face> newFace = mkU<Face>();
                curr->face = newFace.get();
                nextEdge->face = newFace.get();
                e1->face = newFace.get();
                e2->face = newFace.get();
                newFace->halfedge = curr;
                newFaces.push_back(std::move(newFace));
            }
            e1->vertex = ct.get();
            e2->vertex = curr->sym->vertex;
            ct->halfedge = e1;
            track += 2;
            curr = nextMap[nextEdge->id];
        } while (curr != start);

        emit sig_sendVertices(ct.get());
        m_mesh.vertices.push_back(std::move(ct));
        for (uPtr<HalfEdge> &e : newEdges) {
            emit sig_sendEdges(e.get());
            m_mesh.edges.push_back(std::move(e));
        }
    }
    for (uPtr<Face> &f : newFaces) {
        emit sig_sendFaces(f.get());
        m_mesh.faces.push_back(std::move(f));
    }
    m_mesh.destroy();
    m_mesh.create();
    this->update();


}

// stores midpoints of edges
void MyGL::splitByMidPt(HalfEdge *edge) {
    HalfEdge *h1 = edge;
    HalfEdge *h2 = edge->sym;
    uPtr<Vertex> v3 = mkU<Vertex>();
    v3->pos = glm::vec3(midPts[edge->id]->pos);
    uPtr<HalfEdge> e1 = mkU<HalfEdge>();
    uPtr<HalfEdge> e2 = mkU<HalfEdge>();
    e1->face = h1->face;
    e2->face = h2->face;
    e1->sym = h2;
    e2->sym = h1;
    h1->sym = e2.get();
    h2->sym = e1.get();
    e1->next = h1->next;
    e2->next = h2->next;
    h1->next = e1.get();
    h2->next = e2.get();
    e1->vertex = h1->vertex;
    h1->vertex = v3.get();
    e2->vertex = h2->vertex;
    h2->vertex = v3.get();
    h1->face->halfedge = e1.get();
    h2->face->halfedge = e2.get();
    emit sig_sendEdges(e1.get());
    emit sig_sendEdges(e2.get());
    emit sig_sendVertices(v3.get());
    m_mesh.edges.push_back(std::move(e1));
    m_mesh.edges.push_back(std::move(e2));
    m_mesh.vertices.push_back(std::move(v3));
}

// smooth vertices, used in subdivision
void MyGL::smoothVertices() {
    std::vector<glm::vec3> newVtxPos;

    // iterate through vertices and smooth them
    for (uPtr<Vertex> &vertex : m_mesh.vertices) {
        glm::vec3 sum_e = glm::vec3(0.0, 0.0, 0.0);
        glm::vec3 sum_f = glm::vec3(0.0, 0.0, 0.0);
        int n = 0;
        for (uPtr<HalfEdge> &edge : m_mesh.edges) {
            if (edge->vertex == vertex.get()) {
                n += 1;
                sum_e += midPts[edge->id]->pos;
                sum_f += centroids[edge->face->id]->pos;
            }
        }
        // new position
        glm::vec3 newPos = glm::vec3(vertex->pos[0] * (n - 2) / n,
                                     vertex->pos[1] * (n - 2) / n,
                                     vertex->pos[2] * (n - 2) / n) +
                           glm::vec3(sum_e[0] / (n*n), sum_e[1] / (n*n), sum_e[2] / (n*n)) +
                           glm::vec3(sum_f[0] / (n*n), sum_f[1] / (n*n), sum_f[2] / (n*n));
        newVtxPos.push_back(newPos);
    }
    int track = 0;
    for (uPtr<Vertex> &vertex : m_mesh.vertices) {
        vertex->pos = glm::vec3(newVtxPos[track][0], newVtxPos[track][1], newVtxPos[track][2]);
        track += 1;
    }
}

// split by mid points, used in subdivision
void MyGL::computeMidPts() {
    midPts.clear();
    for (uPtr<Face> &face : m_mesh.faces) {
        HalfEdge *curr = face->halfedge;
        do {
            uPtr<Vertex> v = mkU<Vertex>();
            glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);
            pos += curr->vertex->pos;
            pos += curr->sym->vertex->pos;
            pos += centroids[face->id]->pos;
            if (curr->sym != nullptr) {
                pos += centroids[curr->sym->face->id]->pos;
                pos[0] = pos[0] / 4.0;
                pos[1] = pos[1] / 4.0;
                pos[2] = pos[2] / 4.0;
            } else {
                pos[0] = pos[0] / 3.0;
                pos[1] = pos[1] / 3.0;
                pos[2] = pos[2] / 3.0;
            }
            v->pos = pos;
            midPts[curr->id] = std::move(v);
            curr = curr->next;
        } while (curr != face->halfedge);
    }
}

// get centroids of faces, used in subdivision
void MyGL::computeCentroids() {
    centroids.clear();
    for (uPtr<Face> &face : m_mesh.faces) {
        uPtr<Vertex> v = mkU<Vertex>();
        glm::vec3 pos = glm::vec3(0.0, 0.0, 0.0);
        HalfEdge *curr = face->halfedge;
        int count = 0;
        do {
            pos += curr->vertex->pos;
            curr = curr->next;
            count += 1;
        } while (curr != face->halfedge);
        pos[0] = pos[0] / count;
        pos[1] = pos[1] / count;
        pos[2] = pos[2] / count;
        v->pos = pos;
        centroids[face->id] = std::move(v);
    }
}

// slot for extruding edge
void MyGL::slot_extrude() {
    if (selectedFace != nullptr) {
        int count = selectedFace->vertexCount();
        std::vector<uPtr<HalfEdge>> vertEdges;
        std::vector<uPtr<HalfEdge>> topEdges;
        std::vector<uPtr<Vertex>> vertices;
        HalfEdge *curr = selectedFace->halfedge;
        HalfEdge *startEdge = selectedFace->halfedge;
        glm::vec3 vec1 = glm::normalize(curr->next->vertex->pos - curr->vertex->pos);
        glm::vec3 vec2 = glm::normalize(curr->next->next->vertex->pos - curr->next->vertex->pos);
        glm::vec3 normal = glm::normalize(glm::cross(vec1, vec2));

        // initialize new vertices and edges
        for (int i = 0; i < count; i++) {
            uPtr<HalfEdge> v1 = mkU<HalfEdge>();
            uPtr<HalfEdge> v2 = mkU<HalfEdge>();
            uPtr<HalfEdge> top = mkU<HalfEdge>();
            uPtr<Vertex> v = mkU<Vertex>();
            v->pos = glm::vec3(startEdge->vertex->pos) + normal;
            startEdge = startEdge->next;
            if (i != 0) {
                v2->sym = vertEdges[(vertEdges.size()-2)].get();
                vertEdges[vertEdges.size()-2]->sym = v2.get();
            }
            vertEdges.push_back(std::move(v1));
            vertEdges.push_back(std::move(v2));
            topEdges.push_back(std::move(top));
            vertices.push_back(std::move(v));
        }
        vertEdges[1]->sym = vertEdges[vertEdges.size()-2].get();
        vertEdges[vertEdges.size()-2]->sym = vertEdges[1].get();

        // extrude each face
        Vertex *startVertex = curr->sym->vertex;
        for (int i = 0; i < count; i++) {
            HalfEdge *HE1 = curr;
            HalfEdge *HE2 = HE1->sym;
            Vertex *v1 = HE1->vertex;
            Vertex *v2 = HE2->vertex;
            Vertex *v3 = vertices[i].get();
            Vertex *v4 = vertices[(i-1) % vertices.size()].get();
            uPtr<HalfEdge> HE1B = mkU<HalfEdge>();
            HalfEdge *HE2B = topEdges[i].get();
            HalfEdge *HE3 = vertEdges[i*2].get();
            HalfEdge *HE4 = vertEdges[i*2+1].get();
            uPtr<Face> f = mkU<Face>();
            HE1->vertex = v3;
            HE1->prevEdge()->vertex = v4;
            HE1B->sym = HE1;
            HE2B->sym = HE2;
            HE1->sym = HE1B.get();
            HE2->sym = HE2B;
            HE1B->vertex = v4;
            if (i == count - 1) {
                HE2B->vertex = startVertex;
            } else {
                HE2B->vertex = v1;
            }
            HE1B->face = f.get();
            HE2B->face = f.get();
            HE3->face = f.get();
            HE4->face = f.get();
            HE3->vertex = v3;
            HE4->vertex = v2;
            HE1B->next = HE4;
            HE4->next = HE2B;
            HE2B->next = HE3;
            HE3->next = HE1B.get();
            f->halfedge = HE1B.get();
            v3->halfedge = HE1;
            v4->halfedge = HE2;
            curr = curr->next;
            emit sig_sendEdges(HE1B.get());
            emit sig_sendFaces(f.get());
            m_mesh.edges.push_back(std::move(HE1B));
            m_mesh.faces.push_back(std::move(f));
        }


        // send signals to gui
        for (uPtr<HalfEdge> &edge : vertEdges) {
            emit sig_sendEdges(edge.get());
            m_mesh.edges.push_back(std::move(edge));
        }
        for (uPtr<HalfEdge> &edge : topEdges) {
            emit sig_sendEdges(edge.get());
            m_mesh.edges.push_back(std::move(edge));
        }
        for (uPtr<Vertex> &vertex : vertices) {
            emit sig_sendVertices(vertex.get());
            m_mesh.vertices.push_back(std::move(vertex));
        }
        fDisplay.destroy();
        fDisplay.create();
        m_mesh.destroy();
        m_mesh.create();
        this->update();

    }
}

// send signals of mesh
void MyGL::sendSignalsMesh() {
    for (uPtr<HalfEdge> &edge : m_mesh.edges) {
        emit sig_sendEdges(edge.get());
    }
    for (uPtr<Face> &face : m_mesh.faces) {
        emit sig_sendFaces(face.get());
    }
    for (uPtr<Vertex> &vertex : m_mesh.vertices) {
        emit sig_sendVertices(vertex.get());
    }
}

// slot for reading obj files
void MyGL::slot_readObj() {
    QString filename = QFileDialog::getOpenFileName(0, QString("Load obj"), QDir::currentPath().append(QString("../..")), QString("*.obj"));
    QFile file(filename);
    std::vector<glm::vec4> posVec;
    std::map<std::pair<int, int>, HalfEdge*> symmap;
    int faceCount = 0;

    // if file is valid
    if (file.exists()) {
        m_mesh.edges.clear();
        m_mesh.faces.clear();
        m_mesh.vertices.clear();
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            while (!file.atEnd()) {
                QString line = file.readLine().trimmed();
                QStringList lineParts = line.split(QRegularExpression("\\s+"));
                // read through line
                if (lineParts.count() > 0) {
                    if (lineParts[0].compare("v", Qt::CaseInsensitive) == 0) {
                        uPtr<Vertex> v = mkU<Vertex>();
                        v->pos = glm::vec3(lineParts[1].toFloat(), lineParts[2].toFloat(), lineParts[3].toFloat());
                        m_mesh.vertices.push_back(std::move(v));
                    } else if (lineParts[0].compare("f", Qt::CaseInsensitive) == 0) {
                        uPtr<Face> face = mkU<Face>();
                        std::vector<uPtr<HalfEdge>> edges;
                        std::vector<QStringList> stringLists;

                        // initialize new edges
                        for (int i = 1; i < lineParts.size(); i++) {
                            uPtr<HalfEdge> e = mkU<HalfEdge>();
                            QStringList segParts = lineParts[i].split("/");
                            stringLists.push_back(segParts);
                            e->vertex = m_mesh.vertices[segParts[0].toInt() - 1].get();
                            m_mesh.vertices[segParts[0].toInt() - 1]->halfedge = e.get();
                            edges.push_back(std::move(e));
                        }
                        int count = lineParts.size() - 1;

                        // iterate through each face
                        for (int i = 1; i < lineParts.size(); i++) {
                            QStringList segParts = stringLists[i - 1];
                            posVec.push_back(glm::vec4(m_mesh.vertices[segParts[0].toInt() - 1]->pos, 1));
                            HalfEdge *edge = edges[i-1].get();
                            edge->next = edges[(i) % count].get();
                            edge->face = face.get();
                            std::pair<int, int> sympair;
                            std::pair<int, int> currpair;
                            if (i == 1) {
                                 sympair = std::make_pair(segParts[0].toInt() - 1, stringLists[count-1][0].toInt() - 1);
                                 currpair = std::make_pair(stringLists[count-1][0].toInt()-1, segParts[0].toInt() - 1);
                            } else {
                                sympair = std::make_pair(segParts[0].toInt() - 1, stringLists[(i-2) % count][0].toInt() - 1);
                                currpair = std::make_pair(stringLists[(i-2) % count][0].toInt()-1, segParts[0].toInt() - 1);
                            }
                            if (symmap.find(sympair) == symmap.end()) {
                                symmap[currpair] = edge;
                            } else {
                                edge->sym = symmap[sympair];
                                symmap[sympair]->sym = edge;
                            }
                        }

                        // set halfedge of new face
                        face->halfedge = edges[0].get();
                        faceCount += lineParts.size() - 1;
                        m_mesh.faces.push_back(std::move(face));
                        for (uPtr<HalfEdge> &edge : edges) {
                            m_mesh.edges.push_back(std::move(edge));
                        }
                    }
                }
            }
            file.close();
            m_mesh.destroy();
            m_mesh.create();
            sendSignalsMesh();
            update();
        }
    }
}
