#ifndef MYGL_H
#define MYGL_H

#include <openglcontext.h>
#include <utils.h>
#include <shaderprogram.h>
#include <scene/squareplane.h>
#include "camera.h"
#include <scene/mesh.h>
#include <vertexdisplay.h>
#include "halfedgedisplay.h"
#include "facedisplay.h"


#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>


class MyGL
    : public OpenGLContext
{
    Q_OBJECT
private:
    SquarePlane m_geomSquare;// The instance of a unit cylinder we can use to render any cylinder
    Mesh m_mesh;
    ShaderProgram m_progLambert;// A shader program that uses lambertian reflection
    ShaderProgram m_progFlat;// A shader program that uses "flat" reflection (no shadowing at all)


    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera m_glCamera;


public:
    explicit MyGL(QWidget *parent = nullptr);
    ~MyGL();

    VertexDisplay vDisplay; // object for displaying a vertex on gui
    HalfEdgeDisplay eDisplay; // object for displaying a edge on gui
    FaceDisplay fDisplay; // object for displaying a face on gui

    Vertex *selectedVertex; // currently selected vertex
    HalfEdge *selectedEdge; // currently selected edge
    Face *selectedFace; // currently selected face

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();
    void computeCentroids(); // get centroids of faces, used in subdivision
    void computeMidPts(); // get midpoints of edges, used in subdivision
    void smoothVertices(); // smooth vertices, used in subdivision
    void splitByMidPt(HalfEdge*); // split by mid points, used in subdivision

    std::map<int, uPtr<Vertex>> centroids; // stores centroids of faces
    std::map<int, uPtr<Vertex>> midPts; // stores midpoints of edges


signals:
    void sig_sendVertices(Vertex*); // send vertices to gui
    void sig_sendEdges(HalfEdge*); // send edges to gui
    void sig_sendFaces(Face*); // send faces to gui



public slots:
    void slot_vertexSelected(QListWidgetItem*); // slot for vertex selection
    void slot_halfEdgeSelected(QListWidgetItem*); // slot for edge selection
    void slot_faceSelected(QListWidgetItem*); // slot for face selection

    void slot_vertexTranslateX(double); // slot for vertex translation in x
    void slot_vertexTranslateY(double); // slot for vertex translation in y
    void slot_vertexTranslateZ(double); // slot for vertex translation in z

    void slot_changeFaceR(double); // slot for color change in r
    void slot_changeFaceG(double); // slot for color change in g
    void slot_changeFaceB(double); // slot for color change in b

    void slot_addVertex(); // slot for adding a vertex to current halfedge
    void slot_triangulate(); // slot for triangulating the current face
    void slot_subdivide(); // slot for subdividing mesh
    void slot_extrude(); // slot for extruding edge
    void slot_readObj(); // slot for reading obj files
    void sendSignalsMesh(); // send signals of mesh

protected:
    void keyPressEvent(QKeyEvent *e);
};


#endif // MYGL_H
