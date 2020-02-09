#ifndef __myAiUtil_hpp__
#define __myAiUtil_hpp__

#include "IllustratorSDK.h"
#include <cmath>

// utility functions which do not depend on suites

namespace myAiUtil{
    // squared distance between p1 and p2
    inline AIReal dist2(const AIRealPoint &p1, const AIRealPoint &p2){
        AIReal dx = p1.h - p2.h;
        AIReal dy = p1.v - p2.v;
        return dx*dx + dy*dy;
    }
    // distance between p1 and p2
    inline AIReal dist(const AIRealPoint &p1, const AIRealPoint &p2){
        return (AIReal)std::sqrt(dist2(p1, p2));
    }
    
    // ----
    // fix v to be within minv and maxv
    inline ai::int16 fixRange(const ai::int16 v, const ai::int16 minv, const ai::int16 maxv) {
        ai::int16 v1 = v < minv ? minv : v;  // max
        return v1 < maxv ? v1 : maxv;  // min
    }
    
    // ----
    // returns black in GrayColor
    inline AIColor getBlack(){
        AIColor gray;
        gray.kind = kGrayColor;
        gray.c.g.gray = 1;
        return gray;
    }
    
    // ----
    // ensures a path style has no fill.
    // If it has no stroke, applies basic stroke properties.
    // style : [out] path style
    inline void ensureStrokeStyle(AIPathStyle &style){
        if (style.fillPaint) {
            style.fillPaint = false;
            if (!style.strokePaint) {
                style.strokePaint = true;
                AIStrokeStyle strokeStyle;
                strokeStyle.color = getBlack();
                strokeStyle.width = 1;
                style.stroke = strokeStyle;
            }
        }
    }

}

#endif /* myAiUtil_hpp */
