#include "mygl.h"
#include <la.h>

#include <iostream>
#include <QApplication>
#include <QKeyEvent>


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
    m_progLambert.setCamPos(m_glCamera.eye);
    m_progFlat.setModelMatrix(glm::mat4(1.f));

    //Create a model matrix. This one rotates the square by PI/4 radians then translates it by <-2,0,0>.
    //Note that we have to transpose the model matrix before passing it to the shader
    //This is because OpenGL expects column-major matrices, but you've
    //implemented row-major matrices.
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2,0,0)) * glm::rotate(glm::mat4(), 0.25f * 3.14159f, glm::vec3(0,1,0));
    //Send the geometry's transformation matrix to the shader
    m_progLambert.setModelMatrix(model);
    //Draw the example sphere using our lambert shader
    //m_progLambert.draw(m_geomSquare);

    //Now do the same to render the cylinder
    //We've rotated it -45 degrees on the Z axis, then translated it to the point <2,2,0>
    model = glm::translate(glm::mat4(1.0f), glm::vec3(2,2,0)) * glm::rotate(glm::mat4(1.0f), glm::radians(-45.0f), glm::vec3(0,0,1));


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
