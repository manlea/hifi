#version 120

//
//  metavoxel_heightfield_base.vert
//  vertex shader
//
//  Created by Andrzej Kapolka on 8/20/14.
//  Copyright 2014 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

// the height texture
uniform sampler2D heightMap;

// the distance between height points in texture space
uniform float heightScale;

// the scale between height and color textures
uniform float colorScale;

// the interpolated normal
varying vec4 normal;

void main(void) {
    // transform and store the normal for interpolation
    vec2 heightCoord = gl_MultiTexCoord0.st;
    vec4 neighborHeights = vec4(texture2D(heightMap, heightCoord - vec2(heightScale, 0.0)).r,
        texture2D(heightMap, heightCoord + vec2(heightScale, 0.0)).r,
        texture2D(heightMap, heightCoord - vec2(0.0, heightScale)).r,
        texture2D(heightMap, heightCoord + vec2(0.0, heightScale)).r);
    vec4 neighborsZero = step(1.0 / 255.0, neighborHeights);
    normal = normalize(gl_ModelViewMatrix * vec4(
        (neighborHeights.x - neighborHeights.y) * neighborsZero.x * neighborsZero.y, heightScale,
        (neighborHeights.z - neighborHeights.w) * neighborsZero.z * neighborsZero.w, 0.0));
    
    // add the height to the position
    float height = texture2D(heightMap, heightCoord).r;
    gl_Position = gl_ModelViewProjectionMatrix * (gl_Vertex + vec4(0.0, height, 0.0, 0.0));
    
    // the zero height should be invisible
    gl_FrontColor = vec4(1.0, 1.0, 1.0, step(height, 0.0));
    
    // pass along the scaled/offset texture coordinates
    gl_TexCoord[0] = (gl_MultiTexCoord0 - vec4(heightScale, heightScale, 0.0, 0.0)) * colorScale;
}
