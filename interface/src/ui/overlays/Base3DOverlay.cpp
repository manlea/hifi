//
//  Base3DOverlay.cpp
//  interface/src/ui/overlays
//
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

// include this before QGLWidget, which includes an earlier version of OpenGL
#include "InterfaceConfig.h"

#include <QGLWidget>
#include <SharedUtil.h>

#include "Base3DOverlay.h"

const glm::vec3 DEFAULT_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
const float DEFAULT_LINE_WIDTH = 1.0f;
const bool DEFAULT_IS_SOLID = false;
const bool DEFAULT_IS_DASHED_LINE = false;

Base3DOverlay::Base3DOverlay() :
    _position(DEFAULT_POSITION),
    _lineWidth(DEFAULT_LINE_WIDTH),
    _rotation(),
    _isSolid(DEFAULT_IS_SOLID),
    _isDashedLine(DEFAULT_IS_DASHED_LINE),
    _ignoreRayIntersection(false)
{
}

Base3DOverlay::~Base3DOverlay() {
}

void Base3DOverlay::setProperties(const QScriptValue& properties) {
    Overlay::setProperties(properties);

    QScriptValue position = properties.property("position");

    // if "position" property was not there, check to see if they included aliases: start, point, p1
    if (!position.isValid()) {
        position = properties.property("start");
        if (!position.isValid()) {
            position = properties.property("p1");
            if (!position.isValid()) {
                position = properties.property("point");
            }
        }
    }

    if (position.isValid()) {
        QScriptValue x = position.property("x");
        QScriptValue y = position.property("y");
        QScriptValue z = position.property("z");
        if (x.isValid() && y.isValid() && z.isValid()) {
            glm::vec3 newPosition;
            newPosition.x = x.toVariant().toFloat();
            newPosition.y = y.toVariant().toFloat();
            newPosition.z = z.toVariant().toFloat();
            setPosition(newPosition);
        }
    }

    if (properties.property("lineWidth").isValid()) {
        setLineWidth(properties.property("lineWidth").toVariant().toFloat());
    }

    QScriptValue rotation = properties.property("rotation");

    if (rotation.isValid()) {
        glm::quat newRotation;

        // size, scale, dimensions is special, it might just be a single scalar, or it might be a vector, check that here
        QScriptValue x = rotation.property("x");
        QScriptValue y = rotation.property("y");
        QScriptValue z = rotation.property("z");
        QScriptValue w = rotation.property("w");


        if (x.isValid() && y.isValid() && z.isValid() && w.isValid()) {
            newRotation.x = x.toVariant().toFloat();
            newRotation.y = y.toVariant().toFloat();
            newRotation.z = z.toVariant().toFloat();
            newRotation.w = w.toVariant().toFloat();
            setRotation(newRotation);
        }
    }

    if (properties.property("isSolid").isValid()) {
        setIsSolid(properties.property("isSolid").toVariant().toBool());
    }
    if (properties.property("isFilled").isValid()) {
        setIsSolid(properties.property("isSolid").toVariant().toBool());
    }
    if (properties.property("isWire").isValid()) {
        setIsSolid(!properties.property("isWire").toVariant().toBool());
    }
    if (properties.property("solid").isValid()) {
        setIsSolid(properties.property("solid").toVariant().toBool());
    }
    if (properties.property("filled").isValid()) {
        setIsSolid(properties.property("filled").toVariant().toBool());
    }
    if (properties.property("wire").isValid()) {
        setIsSolid(!properties.property("wire").toVariant().toBool());
    }

    if (properties.property("isDashedLine").isValid()) {
        setIsDashedLine(properties.property("isDashedLine").toVariant().toBool());
    }
    if (properties.property("dashed").isValid()) {
        setIsDashedLine(properties.property("dashed").toVariant().toBool());
    }
    if (properties.property("ignoreRayIntersection").isValid()) {
        setIgnoreRayIntersection(properties.property("ignoreRayIntersection").toVariant().toBool());
    }
}

bool Base3DOverlay::findRayIntersection(const glm::vec3& origin, const glm::vec3& direction,
                                                        float& distance, BoxFace& face) const {
    return false;
}


void Base3DOverlay::drawDashedLine(const glm::vec3& start, const glm::vec3& end) {

    glBegin(GL_LINES);

    // draw each line segment with appropriate gaps
    const float DASH_LENGTH = 0.05f;
    const float GAP_LENGTH = 0.025f;
    const float SEGMENT_LENGTH = DASH_LENGTH + GAP_LENGTH;
    float length = glm::distance(start, end);
    float segmentCount = length / SEGMENT_LENGTH;
    int segmentCountFloor = (int)glm::floor(segmentCount);

    glm::vec3 segmentVector = (end - start) / segmentCount;
    glm::vec3 dashVector = segmentVector / SEGMENT_LENGTH * DASH_LENGTH;
    glm::vec3 gapVector = segmentVector / SEGMENT_LENGTH * GAP_LENGTH;

    glm::vec3 point = start;
    glVertex3f(point.x, point.y, point.z);
    for (int i = 0; i < segmentCountFloor; i++) {
        point += dashVector;
        glVertex3f(point.x, point.y, point.z);

        point += gapVector;
        glVertex3f(point.x, point.y, point.z);
    }
    glVertex3f(end.x, end.y, end.z);

    glEnd();

}



