//
//  Overlays.cpp
//  interface/src/ui/overlays
//
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <limits>
#include <Application.h>

#include "BillboardOverlay.h"
#include "Circle3DOverlay.h"
#include "Cube3DOverlay.h"
#include "ImageOverlay.h"
#include "Line3DOverlay.h"
#include "LocalModelsOverlay.h"
#include "LocalVoxelsOverlay.h"
#include "ModelOverlay.h"
#include "Overlays.h"
#include "Rectangle3DOverlay.h"
#include "Sphere3DOverlay.h"
#include "TextOverlay.h"
#include "Text3DOverlay.h"

Overlays::Overlays() : _nextOverlayID(1) {
}

Overlays::~Overlays() {
    
    {
        QWriteLocker lock(&_lock);
        foreach(Overlay* thisOverlay, _overlays2D) {
            delete thisOverlay;
        }
        _overlays2D.clear();
        foreach(Overlay* thisOverlay, _overlays3D) {
            delete thisOverlay;
        }
        _overlays3D.clear();
    }
    
    if (!_overlaysToDelete.isEmpty()) {
        QWriteLocker lock(&_deleteLock);
        do {
            delete _overlaysToDelete.takeLast();
        } while (!_overlaysToDelete.isEmpty());
    }
    
}

void Overlays::init(QGLWidget* parent) {
    _parent = parent;
}

void Overlays::update(float deltatime) {

    {
        QWriteLocker lock(&_lock);
        foreach(Overlay* thisOverlay, _overlays2D) {
            thisOverlay->update(deltatime);
        }
        foreach(Overlay* thisOverlay, _overlays3D) {
            thisOverlay->update(deltatime);
        }
    }

    if (!_overlaysToDelete.isEmpty()) {
        QWriteLocker lock(&_deleteLock);
        do {
            delete _overlaysToDelete.takeLast();
        } while (!_overlaysToDelete.isEmpty());
    }
    
}

void Overlays::render2D() {
    QReadLocker lock(&_lock);
    foreach(Overlay* thisOverlay, _overlays2D) {
        thisOverlay->render();
    }
}

void Overlays::render3D() {
    QReadLocker lock(&_lock);
    if (_overlays3D.size() == 0) {
        return;
    }
    bool myAvatarComputed = false;
    MyAvatar* avatar = NULL;
    glm::quat myAvatarRotation;
    glm::vec3 myAvatarPosition(0.0f);
    float angle = 0.0f;
    glm::vec3 axis(0.0f, 1.0f, 0.0f);
    float myAvatarScale = 1.0f;

    foreach(Overlay* thisOverlay, _overlays3D) {
        glPushMatrix();
        switch (thisOverlay->getAnchor()) {
            case Overlay::MY_AVATAR:
                if (!myAvatarComputed) {
                    avatar = Application::getInstance()->getAvatar();
                    myAvatarRotation = avatar->getOrientation();
                    myAvatarPosition = avatar->getPosition();
                    angle = glm::degrees(glm::angle(myAvatarRotation));
                    axis = glm::axis(myAvatarRotation);
                    myAvatarScale = avatar->getScale();
                    
                    myAvatarComputed = true;
                }
                
                glTranslatef(myAvatarPosition.x, myAvatarPosition.y, myAvatarPosition.z);
                glRotatef(angle, axis.x, axis.y, axis.z);
                glScalef(myAvatarScale, myAvatarScale, myAvatarScale);
                break;
            default:
                break;
        }
        thisOverlay->render();
        glPopMatrix();
    }
}

unsigned int Overlays::addOverlay(const QString& type, const QScriptValue& properties) {
    unsigned int thisID = 0;
    Overlay* thisOverlay = NULL;
    
    bool created = true;
    if (type == "image") {
        thisOverlay = new ImageOverlay();
    } else if (type == "text") {
        thisOverlay = new TextOverlay();
    } else if (type == "text3d") {
        thisOverlay = new Text3DOverlay();
    } else if (type == "cube") {
        thisOverlay = new Cube3DOverlay();
    } else if (type == "sphere") {
        thisOverlay = new Sphere3DOverlay();
    } else if (type == "circle3d") {
        thisOverlay = new Circle3DOverlay();
    } else if (type == "rectangle3d") {
        thisOverlay = new Rectangle3DOverlay();
    } else if (type == "line3d") {
        thisOverlay = new Line3DOverlay();
    } else if (type == "localvoxels") {
        thisOverlay = new LocalVoxelsOverlay();
    } else if (type == "localmodels") {
        thisOverlay = new LocalModelsOverlay(Application::getInstance()->getEntityClipboardRenderer());
    } else if (type == "model") {
        thisOverlay = new ModelOverlay();
    } else if (type == "billboard") {
        thisOverlay = new BillboardOverlay();
    } else {
        created = false;
    }

    if (created) {
        thisOverlay->setProperties(properties);
        thisID = addOverlay(thisOverlay);
    }

    return thisID; 
}

unsigned int Overlays::addOverlay(Overlay* overlay) {
    overlay->init(_parent);

    QWriteLocker lock(&_lock);
    unsigned int thisID = _nextOverlayID;
    _nextOverlayID++;
    if (overlay->is3D()) {
        _overlays3D[thisID] = overlay;
    } else {
        _overlays2D[thisID] = overlay;
    }
    
    return thisID;
}

bool Overlays::editOverlay(unsigned int id, const QScriptValue& properties) {
    Overlay* thisOverlay = NULL;
    QWriteLocker lock(&_lock);
    if (_overlays2D.contains(id)) {
        thisOverlay = _overlays2D[id];
    } else if (_overlays3D.contains(id)) {
        thisOverlay = _overlays3D[id];
    }
    if (thisOverlay) {
        thisOverlay->setProperties(properties);
        return true;
    }
    return false;
}

void Overlays::deleteOverlay(unsigned int id) {
    Overlay* overlayToDelete;

    {
        QWriteLocker lock(&_lock);
        if (_overlays2D.contains(id)) {
            overlayToDelete = _overlays2D.take(id);
        } else if (_overlays3D.contains(id)) {
            overlayToDelete = _overlays3D.take(id);
        } else {
            return;
        }
    }

    QWriteLocker lock(&_deleteLock);
    _overlaysToDelete.push_back(overlayToDelete);
}

unsigned int Overlays::getOverlayAtPoint(const glm::vec2& point) {
    QReadLocker lock(&_lock);
    QMapIterator<unsigned int, Overlay*> i(_overlays2D);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        unsigned int thisID = i.key();
        Overlay2D* thisOverlay = static_cast<Overlay2D*>(i.value());
        if (thisOverlay->getVisible() && thisOverlay->isLoaded() && thisOverlay->getBounds().contains(point.x, point.y, false)) {
            return thisID;
        }
    }
    return 0; // not found
}

RayToOverlayIntersectionResult Overlays::findRayIntersection(const PickRay& ray) {
    float bestDistance = std::numeric_limits<float>::max();
    RayToOverlayIntersectionResult result;
    QMapIterator<unsigned int, Overlay*> i(_overlays3D);
    i.toBack();
    while (i.hasPrevious()) {
        i.previous();
        unsigned int thisID = i.key();
        Base3DOverlay* thisOverlay = static_cast<Base3DOverlay*>(i.value());
        if (thisOverlay->getVisible() && !thisOverlay->getIgnoreRayIntersection() && thisOverlay->isLoaded()) {
            float thisDistance;
            BoxFace thisFace;
            if (thisOverlay->findRayIntersection(ray.origin, ray.direction, thisDistance, thisFace)) {
                if (thisDistance < bestDistance) {
                    bestDistance = thisDistance;
                    result.intersects = true;
                    result.distance = thisDistance;
                    result.face = thisFace;
                    result.overlayID = thisID;
                    result.intersection = ray.origin + (ray.direction * thisDistance);
                }
            }
        }
    }
    return result;
}

RayToOverlayIntersectionResult::RayToOverlayIntersectionResult() : 
    intersects(false), 
    overlayID(-1),
    distance(0),
    face(),
    intersection()
{ 
}

QScriptValue RayToOverlayIntersectionResultToScriptValue(QScriptEngine* engine, const RayToOverlayIntersectionResult& value) {
    QScriptValue obj = engine->newObject();
    obj.setProperty("intersects", value.intersects);
    obj.setProperty("overlayID", value.overlayID);
    obj.setProperty("distance", value.distance);

    QString faceName = "";    
    // handle BoxFace
    switch (value.face) {
        case MIN_X_FACE:
            faceName = "MIN_X_FACE";
            break;
        case MAX_X_FACE:
            faceName = "MAX_X_FACE";
            break;
        case MIN_Y_FACE:
            faceName = "MIN_Y_FACE";
            break;
        case MAX_Y_FACE:
            faceName = "MAX_Y_FACE";
            break;
        case MIN_Z_FACE:
            faceName = "MIN_Z_FACE";
            break;
        case MAX_Z_FACE:
            faceName = "MAX_Z_FACE";
            break;
        default:
        case UNKNOWN_FACE:
            faceName = "UNKNOWN_FACE";
            break;
    }
    obj.setProperty("face", faceName);
    QScriptValue intersection = vec3toScriptValue(engine, value.intersection);
    obj.setProperty("intersection", intersection);
    return obj;
}

void RayToOverlayIntersectionResultFromScriptValue(const QScriptValue& object, RayToOverlayIntersectionResult& value) {
    value.intersects = object.property("intersects").toVariant().toBool();
    value.overlayID = object.property("overlayID").toVariant().toInt();
    value.distance = object.property("distance").toVariant().toFloat();

    QString faceName = object.property("face").toVariant().toString();
    if (faceName == "MIN_X_FACE") {
        value.face = MIN_X_FACE;
    } else if (faceName == "MAX_X_FACE") {
        value.face = MAX_X_FACE;
    } else if (faceName == "MIN_Y_FACE") {
        value.face = MIN_Y_FACE;
    } else if (faceName == "MAX_Y_FACE") {
        value.face = MAX_Y_FACE;
    } else if (faceName == "MIN_Z_FACE") {
        value.face = MIN_Z_FACE;
    } else if (faceName == "MAX_Z_FACE") {
        value.face = MAX_Z_FACE;
    } else {
        value.face = UNKNOWN_FACE;
    };
    QScriptValue intersection = object.property("intersection");
    if (intersection.isValid()) {
        vec3FromScriptValue(intersection, value.intersection);
    }
}

bool Overlays::isLoaded(unsigned int id) {
    QReadLocker lock(&_lock);
    Overlay* overlay = _overlays2D.value(id);
    if (!overlay) {
        _overlays3D.value(id);
    }
    if (!overlay) {
        return false; // not found
    }

    return overlay->isLoaded();
}

