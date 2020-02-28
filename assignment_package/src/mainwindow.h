#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "vertex.h"
#include "face.h"
#include "halfedge.h"
#include <vector>
#include "smartpointerhelp.h"


namespace Ui {
class MainWindow;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void slot_displayVertices(Vertex*);
    void slot_displayFaces(Face*);
    void slot_displayEdges(HalfEdge*);

private slots:
    void on_actionQuit_triggered();

    void on_actionCamera_Controls_triggered();


private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
