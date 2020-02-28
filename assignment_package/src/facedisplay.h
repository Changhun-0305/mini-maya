#pragma once
#include "drawable.h"
#include "face.h"
#include <vector>

class FaceDisplay : public Drawable
{
protected:
    Face *representedFace = nullptr; // the face it points to

public:
    FaceDisplay(OpenGLContext*);
    ~FaceDisplay();
    GLenum drawMode() override;
    virtual void create() override;
    void updateFace(Face*); // update with new face
};

