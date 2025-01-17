//
//  Circle3DOverlay.h
//  interface/src/ui/overlays
//
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#ifndef hifi_Circle3DOverlay_h
#define hifi_Circle3DOverlay_h

#include "Planar3DOverlay.h"

class Circle3DOverlay : public Planar3DOverlay {
    Q_OBJECT
    
public:
    Circle3DOverlay();
    ~Circle3DOverlay();
    virtual void render();
    virtual void setProperties(const QScriptValue& properties);

    float getStartAt() const { return _startAt; }
    float getEndAt() const { return _endAt; }
    float getOuterRadius() const { return _outerRadius; }
    float getInnerRadius() const { return _innerRadius; }
    bool getHasTickMarks() const { return _hasTickMarks; }
    float getMajorTickMarksAngle() const { return _majorTickMarksAngle; }
    float getMinorTickMarksAngle() const { return _minorTickMarksAngle; }
    float getMajorTickMarksLength() const { return _majorTickMarksLength; }
    float getMinorTickMarksLength() const { return _minorTickMarksLength; }
    xColor getMajorTickMarksColor() const { return _majorTickMarksColor; }
    xColor getMinorTickMarksColor() const { return _minorTickMarksColor; }
    
    void setStartAt(float value) { _startAt = value; }
    void setEndAt(float value) { _endAt = value; }
    void setOuterRadius(float value) { _outerRadius = value; }
    void setInnerRadius(float value) { _innerRadius = value; }
    void setHasTickMarks(bool value) { _hasTickMarks = value; }
    void setMajorTickMarksAngle(float value) { _majorTickMarksAngle = value; }
    void setMinorTickMarksAngle(float value) { _minorTickMarksAngle = value; }
    void setMajorTickMarksLength(float value) { _majorTickMarksLength = value; }
    void setMinorTickMarksLength(float value) { _minorTickMarksLength = value; }
    void setMajorTickMarksColor(const xColor& value) { _majorTickMarksColor = value; }
    void setMinorTickMarksColor(const xColor& value) { _minorTickMarksColor = value; }

    virtual bool findRayIntersection(const glm::vec3& origin, const glm::vec3& direction, float& distance, BoxFace& face) const;
    
protected:
    float _startAt;
    float _endAt;
    float _outerRadius;
    float _innerRadius;
    bool _hasTickMarks;
    float _majorTickMarksAngle;
    float _minorTickMarksAngle;
    float _majorTickMarksLength;
    float _minorTickMarksLength;
    xColor _majorTickMarksColor;
    xColor _minorTickMarksColor;
};

 
#endif // hifi_Circle3DOverlay_h
