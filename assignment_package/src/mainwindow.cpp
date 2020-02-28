#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include "mygl.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    // send vertices to gui
    connect(ui->mygl, SIGNAL(sig_sendVertices(Vertex*)),
            this, SLOT(slot_displayVertices(Vertex*)));
    // send edges to gui
    connect(ui->mygl, SIGNAL(sig_sendEdges(HalfEdge*)),
            this, SLOT(slot_displayEdges(HalfEdge*)));
    // send faces to gui
    connect(ui->mygl, SIGNAL(sig_sendFaces(Face*)),
            this, SLOT(slot_displayFaces(Face*)));
    // vertex is selected in gui
    connect(ui->vertsListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_vertexSelected(QListWidgetItem*)));
    // edge is selected in gui
    connect(ui->halfEdgesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_halfEdgeSelected(QListWidgetItem*)));
    // face is selected in gui
    connect(ui->facesListWidget, SIGNAL(itemClicked(QListWidgetItem*)),
            ui->mygl, SLOT(slot_faceSelected(QListWidgetItem*)));
    // vertex is translated x in gui
    connect(ui->vertPosXSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_vertexTranslateX(double)));
    // vertex is translated y in gui
    connect(ui->vertPosYSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_vertexTranslateY(double)));
    // vertex is translated z in gui
    connect(ui->vertPosZSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_vertexTranslateZ(double)));
    // face color is changed in r
    connect(ui->faceRedSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeFaceR(double)));
    // face color is changed in g
    connect(ui->faceGreenSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeFaceG(double)));
    // face color is changed in b
    connect(ui->faceBlueSpinBox, SIGNAL(valueChanged(double)),
            ui->mygl, SLOT(slot_changeFaceB(double)));
    // vertex is added in halfedge
    connect(ui->addVertexBtn, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_addVertex()));
    // current face is triangulated
    connect(ui->triangulateBtn, SIGNAL(clicked(bool)),
            ui->mygl, SLOT(slot_triangulate()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    CameraControlsHelp* c = new CameraControlsHelp();
    c->show();
}

void MainWindow::slot_displayVertices(Vertex *vertex) {
    ui->vertsListWidget->addItem(vertex);
}

void MainWindow::slot_displayEdges(HalfEdge *edge) {
    ui->halfEdgesListWidget->addItem(edge);
}


void MainWindow::slot_displayFaces(Face *face) {
    ui->facesListWidget->addItem(face);
}

