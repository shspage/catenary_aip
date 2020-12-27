//
//  catenaryPlugin.cpp
//  Adobe Illustrator plug-in tool
//
//  Copyright (c) 2020 Hiroyuki Sato
//  zlib License
//  http://github.com/shspage/catenary_aip
//
//  ----
//  This project depends on the following resources.
//
//  * catenary.hpp (namespace catenary)
//  zlib License
//  Copyright (c) 2017 ushiostarfish
//  https://github.com/Ushio/Catenaly
//
//  * Adobe Illustrator 2020 SDK
//  Copyright (c) 2020 Adobe. All rights reserved.
//  https://www.adobe.io/
//  ----

#include "IllustratorSDK.h"
#include "catenaryPlugin.h"
#include "catenarySuites.h"
#include "SDKErrors.h"
#include <cmath>

#include "catenary.hpp"
#include "mySysUtil.hpp"
#include "myAiUtil.hpp"

#ifndef CHKERR
#define CHKERR aisdk::check_ai_error(error)
#endif

// カーソルスナップ設定（詳細はSDKリファレンス参照）
const static char* const SNAPCURSOR_CTL_MOUSEDOWN_NEW = "ATFGP v i o";
const static char* const SNAPCURSOR_CTL_MOUSEDOWN_HIT = "A v";
const static char* const SNAPCURSOR_CTL_MOUSEUP = "ATFGP v i o";

// MouseUp時に、描画したパスがこれより短い場合は削除する(pt)
const static AIReal MIN_PATH_LENGTH = 1.0;

// MouseDown時に近くの端点を検出する際の許容誤差(px)
const static AIReal HIT_TOLERANCE = 8;

// 曲線をプロットする点の最大数（パス単純化の前工程）(pt)
const static ai::int16 MAX_SEGMENT_COUNT_BEFORE_SIMPLIFY = 200;

// パスを単純化する場合true
const static bool DO_SIMPLIFY = true;

// パス単純化の際の許容誤差(pt?)
const static AIReal CURVEFIT_TOLERANCE = 0.2;

// ドラッグ中に上下矢印キーを押した際の線長変倍率
const static double LENGTH_MULTIPLYER_UP = 0.99;
const static double LENGTH_MULTIPLYER_DOWN = 1.01;

#ifdef WIN_ENV
const static AICharacterEncoding MY_MESSAGE_ENCODING = kAIPlatformCharacterEncoding;
#else
const static AICharacterEncoding MY_MESSAGE_ENCODING = kAIUTF8CharacterEncoding;
#endif

// --------
Plugin* AllocatePlugin(SPPluginRef pluginRef)
{
    return new catenaryPlugin(pluginRef);
}

void FixupReload(Plugin* plugin)
{
    catenaryPlugin::FixupVTable((catenaryPlugin*) plugin);
}

catenaryPlugin::catenaryPlugin(SPPluginRef pluginRef) :
    Plugin(pluginRef),
    fToolHandle(NULL),
    fResourceManagerHandle(NULL),
    fShutdownApplicationNotifier(NULL),
    m_path(nullptr),
    m_length(0),
    m_downpoint({0,0})
{
    strncpy(fPluginName, kcatenaryPluginName, kMaxStringLength);
}

catenaryPlugin::~catenaryPlugin()
{
}

ASErr catenaryPlugin::StartupPlugin( SPInterfaceMessage *message )
{
    ASErr error = kNoErr;
    try{
        error = Plugin::StartupPlugin(message); CHKERR;
        error = AddNotifier(message); CHKERR;
        error = AddTool(message); CHKERR;
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch (...)
    {
        error = kCantHappenErr;
    }

    return error;
}

ASErr catenaryPlugin::PostStartupPlugin(){
    ASErr error = kNoErr;
    try {
        error = sAIUser->CreateCursorResourceMgr(fPluginRef, &fResourceManagerHandle); CHKERR;
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch (...)
    {
        error = kCantHappenErr;
    }
    return error;
}

ASErr catenaryPlugin::ShutdownPlugin(SPInterfaceMessage *message)
{
    ASErr error = kNoErr;
    error = Plugin::ShutdownPlugin(message);
    return error;
}

ASErr catenaryPlugin::AddNotifier(SPInterfaceMessage *message) {
    ASErr error = kNoErr;
    error = sAINotifier->AddNotifier(fPluginRef,
        "catenary " kAIApplicationShutdownNotifier,
        kAIApplicationShutdownNotifier,
        &fShutdownApplicationNotifier);
    return error;
}

ASErr catenaryPlugin::Notify(AINotifierMessage* message) {
    ASErr error = kNoErr;
    try {
        if (message->notifier == fShutdownApplicationNotifier)
        {
            if (fResourceManagerHandle != NULL)
            {
                sAIUser->DisposeCursorResourceMgr(fResourceManagerHandle);
                fResourceManagerHandle = NULL;
            }
        }
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch (...)
    {
        error = kCantHappenErr;
    }
    return error;
}

AIErr catenaryPlugin::AddTool(SPInterfaceMessage* message) {
    AIErr error = kNoErr;
    AIAddToolData toolData;
    toolData.title = ai::UnicodeString::FromRoman("catenary");
    toolData.tooltip = ai::UnicodeString::FromRoman("catenary");
    toolData.sameGroupAs = kNoTool;
    toolData.sameToolsetAs = kNoTool;
    toolData.normalIconResID = kcatenaryToolIconResourceID;
    toolData.darkIconResID = kcatenaryToolIconResourceID;
    toolData.iconType = ai::IconType::kSVG;

    error = sAITool->GetToolNumberFromName(kcatenaryTool, &toolData.sameGroupAs); CHKERR;
    error = sAITool->GetToolNumberFromName(kcatenaryTool, &toolData.sameToolsetAs); CHKERR;

    ai::int32 options = kToolWantsToTrackCursorOption;
    error = sAITool->AddTool(message->d.self, kcatenaryTool, toolData,
        options, &fToolHandle); CHKERR;
    return error;

}

ASErr catenaryPlugin::EditTool(AIToolMessage *message){
    ASErr error = kNoErr;
    try {
        sAIUser->MessageAlert(ai::UnicodeString("設定項目は特にありません。\n----\n"
                                                "カテナリー曲線（カテナリーきょくせん、英: catenary）または懸垂曲線"
                                                "（けんすいきょくせん）または懸垂線（けんすいせん）とは、ロープや"
                                                "電線などの両端を持って垂らしたときにできる曲線である。"
                                                "(Wikipedia)", MY_MESSAGE_ENCODING));
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch (...)
    {
        error = kCantHappenErr;
    }
    return error;
}

// ----------------------
// src : original point  ex. message->cursor
// dst : [out] snapped point
// event : message->event
// control : snap setting (see SDK reference for details)
static AIErr snapCursor(const AIRealPoint &src, AIRealPoint &dst, AIEvent* event, const char* control) {
    AIErr error = kNoErr;
    AIDocumentViewHandle view;
    error = sAIDocumentView->GetNthDocumentView(0, &view); CHKERR;
    return sAICursorSnap->Track(view, src, event, control, &dst);
}

// set a segment (with no handle) at index segNumber
static ASErr setSegment(AIArtHandle art, ai::int16 segNumber, const AIRealPoint &p) {
    AIPathSegment seg;
    seg.p = seg.in = seg.out = p;
    return sAIPath->SetPathSegments(art, segNumber, 1, &seg);
}

// remove art if it is very short in length or is an isolated point
static ASErr removeTinyPath(AIArtHandle art) {
    ASErr error = kNoErr;
    AIReal length;
    error = sAIPath->GetPathLength(art, &length, 1); CHKERR;
    if (length < MIN_PATH_LENGTH) {
        error = sAIArt->DisposeArt(art); CHKERR;
    }
    return error;
}

// 連続した点にフィッティングしたベジェ曲線のセグメントを取得する。
// points : source points
// segs : [out] output PathSegments
static ASErr getSimplifiedSegments(const std::vector<AIRealPoint> &points, std::vector<AIPathSegment> &segs) {
    ASErr error = kNoErr;
    if(points.size() < 1) return error;

    // 元になる点を準備
    AICurveFittingTrajectorySampleVector trajectory;
    trajectory.size = (int)points.size();
    std::vector<AICurveFittingTrajectorySample> samples;

    for (auto itr = points.begin(); itr != points.end(); ++itr) {
        const AIRealPoint& p = *itr;
        AICurveFittingTrajectorySample sample;
        sample.cut = kAICurveFittingSmoothCut;
        sample.x = p.h;
        sample.y = p.v;
        samples.push_back(sample);
    }
    /*samples[0].cut = kAICurveFittingNoCut;
    samples.back().cut = kAICurveFittingNoCut;*/
    trajectory.first = samples.data();
    
    // フィッティング
    AICurveFittingPathSegmentVector dest;
    dest.open = true;
    dest.first = nullptr;
    try {
        error = sAICurveFitting->FitTrajectory(&trajectory, 0, 1, 100,
            CURVEFIT_TOLERANCE, &dest); CHKERR;
        if(dest.first != nullptr){
            AIPathSegment* seg = dest.first;
            for (ai::int32 i = 0, iEnd = dest.size; i < iEnd; i++) {
                segs.push_back(*(seg++));
            }
        }
    }
    catch (...) {
        if(dest.first) sSPBasic->FreeBlock(dest.first);
        throw;
    }
    
    if (dest.first)
        error = sSPBasic->FreeBlock(dest.first);
    return error;
}

// ----
// 指定した点の近くに端点があるパスと端点のインデックスを取得する。
// cpoint : a point to test hit, ex. message->cursor
// art, segmentIndex : [out] result
// is_hit : [out] true if hit
AIErr catenaryPlugin::checkForPathEndHit(const AIRealPoint cpoint, AIArtHandle &art,
    ai::int16 &segmentIndex, bool &is_hit)
{
    AIErr error = kNoErr;
    is_hit = false;
    AIReal tolerance = HIT_TOLERANCE;
    
    AIHitRef hitRef = NULL;
    AIToolHitData toolHitData;
    SDK_ASSERT(sAIHitTest);
    
    error = sAIHitTest->HitTestEx(NULL, &cpoint, tolerance, kEndPHitRequest, &hitRef); CHKERR;
    error = sAIHitTest->GetHitData(hitRef, &toolHitData); CHKERR;
    error = sAIHitTest->Release(hitRef); CHKERR;
    
    if (toolHitData.hit && toolHitData.object != NULL) {
        art = toolHitData.object;
        segmentIndex = toolHitData.segment;
        is_hit = true;
    }
    return error;
}

// MouseDown時にshiftかoptionキーを押していて、かつクリック点がパスの端点にヒットした場合の処理。
// ヒットしたパスが一定の条件にある場合、カテナリー曲線として編集(shift)または複製して編集(option)する。
// 条件に合わない場合、編集対象は設定されず、ToolMouseDown で新規パス生成の処理に移ることになる。
// art : a path to check (a result of hit-test)
// segIndex : (a result of hit-test)
// adjustMode : true if adjust-mode, otherwise it duplicates a path
AIErr catenaryPlugin::setTragetIfCatenary(const AIArtHandle &art, ai::int16 segIndex, bool adjustMode) {
    ASErr error = kNoErr;
    m_path = nullptr;
    try{
        // 操作対象にするパスの条件
        // ・塗りなし、線あり、オープンパス
        // ・パス全体の上端がいずれかの端点のy座標とほぼ等しい
        // ・パス全体の左端が左側の端点のx座標とほぼ等しい
        // ・パス全体の右端が右側の端点のx座標とほぼ等しい
        // （端は線幅を考慮しない）

        // "kEndPHitRequest"はクローズパスに hit しないので、
        // オープンパスかどうかの確認は省略。
        ai::int16 segcount;
        error = sAIPath->GetPathSegmentCount(art, &segcount); CHKERR;
        AIPathStyle style;
        AIBoolean hasAdvFill;
        error = sAIPathStyle->GetPathStyle(art, &style, &hasAdvFill); CHKERR;
        
        if (segcount > 1 && !style.fillPaint && style.strokePaint) {
            AIRealRect bounds;
            error = sAIArt->GetArtTransformBounds(art, NULL, kNoStrokeBounds, &bounds); CHKERR;
            AIPathSegment seg;
            error = sAIPath->GetPathSegments(art, 0, 1, &seg); CHKERR;
            AIRealPoint p1 = seg.p;
            error = sAIPath->GetPathSegments(art, segcount - 1, 1, &seg); CHKERR;
            AIRealPoint p2 = seg.p;

            if (std::fmax(p1.v, p2.v) < bounds.top + 1.0
                && std::fmin(p1.h, p2.h) > bounds.left - 1.0
                && std::fmax(p1.h, p2.h) < bounds.right + 1.0){

                if(adjustMode){
                    m_path = art;
                } else {
                    error = sAIArt->DuplicateArt(art, kPlaceAbove, art, &m_path); CHKERR;
                }
                
                AIReal length;
                error = sAIPath->GetPathLength(m_path, &length, 1); CHKERR;
                // 常識的には無いが、bounds内に超長大なパスが詰め込まれている場合も
                // 条件に合致するので、長さの上限を儲ける。
                AIReal maxLength = abs(bounds.top - bounds.bottom) * 2 + (bounds.right - bounds.left);
                m_length = std::fmin(length, maxLength);
                
                if(segIndex == 0){
                    error = sAIPath->ReversePathSegments(m_path); CHKERR;
                    m_downpoint = p2;
                } else {
                    m_downpoint = p1;
                }
            }
        }
    } catch(...){
        m_path = nullptr;
        throw;
    }
    return error;
}

// カレントレイヤーが編集可能な場合に isAvailable を true に設定する。
// サブレイヤーの場合、親レイヤーのいずれかが編集不可であれば false になる。
// isAvailable [out] set true if the current layer is editable
static ASErr isLayerAvailable(bool *isAvailable) {
    if (isAvailable == nullptr) return 1;
    ASErr error = kNoErr;
    *isAvailable = true;
    AILayerHandle layer;
    error = sAILayer->GetCurrentLayer(&layer); CHKERR;

    AIBoolean isEditable, isVisible;
    while (layer != NULL && *isAvailable) {
        error = sAILayer->GetLayerEditable(layer, &isEditable); CHKERR;
        error = sAILayer->GetLayerVisible(layer, &isVisible); CHKERR;
        *isAvailable = isEditable && isVisible;
        if (*isAvailable) {
            AILayerHandle parentLayer;
            error = sAILayer->GetLayerParent(layer, &parentLayer); CHKERR;
            layer = parentLayer;
        }
    }
    return error;
}

ASErr catenaryPlugin::ToolMouseDown( AIToolMessage* message ){
    ASErr error = kNoErr;
    try{
        m_path = nullptr;
        
        // option または shift が押されている場合、クリック点から編集対象になるパスの取得を試みる。
        bool optionKeyDown = (message->event->modifiers & aiEventModifiers_optionKey);
        bool shiftKeyDown = (message->event->modifiers & aiEventModifiers_shiftKey);
        
        AIRealPoint cpoint = message->cursor;
        
        if (optionKeyDown || shiftKeyDown) {
            error = snapCursor(message->cursor, cpoint, message->event, SNAPCURSOR_CTL_MOUSEDOWN_HIT); CHKERR;
            AIArtHandle hitart;
            ai::int16 segIndex;
            bool is_hit;
            error = checkForPathEndHit(cpoint, hitart, segIndex, is_hit);
            if (is_hit) {
                error = setTragetIfCatenary(hitart, segIndex, shiftKeyDown); CHKERR;
            }
        }

        // 新規パス生成（shiftまたはoptionキーがupか、または編集対象が設定されなかった場合）
        if(m_path == nullptr){
            bool isAvailable;
            error = isLayerAvailable(&isAvailable); CHKERR;

            if(isAvailable){
                error = snapCursor(message->cursor, cpoint, message->event, SNAPCURSOR_CTL_MOUSEDOWN_NEW); CHKERR;
                // new art
                AIArtHandle art;
                error = sAIArt->NewArt(kPathArt, kPlaceDefault, nullptr, &art); CHKERR;
                
                AIPathStyle style;
                AIPathStyleMap styleMap;
                AIDictionaryRef dictRef;
                AIBoolean outHasAdvFill;
                error = sAIPathStyle->GetCurrentPathStyle(&style, &styleMap, &dictRef, &outHasAdvFill); CHKERR;
                if (style.fillPaint) {
                    error = sAIPathStyle->GetInitialPathStyle(&style); CHKERR;
                    myAiUtil::ensureStrokeStyle(style);
                }
                error = sAIPathStyle->SetPathStyle(art, &style); CHKERR;
                
                error = sAIPath->SetPathSegmentCount(art, 2); CHKERR;
                error = setSegment(art, 0, cpoint); CHKERR;
                error = setSegment(art, 1, cpoint); CHKERR;
                m_length = 0;
                m_downpoint = cpoint;
                m_path = art;
            }
        }
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch(...)
    {
        error = kCantHappenErr;
    }
    return error;
}

// MouseDown点とlastPointの間にカテナリー曲線を描画する。
// 線がたわんでいれば、線長はドラッグの間、一定になる。
// 線が直線になる場合は、線長は２点間の距離に更新される。
// lastPoint : the last point of curve (ex. message->cursor)
ASErr catenaryPlugin::drawCatenary(AIRealPoint lastPoint){
    ASErr error = kNoErr;
    if (m_path) {
        try{
            // 上下矢印キーの状態によって線の長さを変化させる。
            // UPで短く、DOWNで長くしているので、
            // 線の下端の位置を変化させるとも言える。
            double lengthMultiplyer = 1.0;
            if(mySysUtil::isUpArrowDown()){
                lengthMultiplyer = LENGTH_MULTIPLYER_UP;
            } else if(mySysUtil::isDownArrowDown()){
                lengthMultiplyer = LENGTH_MULTIPLYER_DOWN;
            }
        
            ai::int16 segCount;
            error = sAIPath->GetPathSegmentCount(m_path, &segCount); CHKERR;
            
            AIReal lengthBetweenEnds = myAiUtil::dist(m_downpoint, lastPoint);
            
            if(m_length <= lengthBetweenEnds || lengthBetweenEnds == 0){
                m_length = lengthBetweenEnds;
                if(segCount > 2){
                    error = sAIPath->SetPathSegmentCount(m_path, 2); CHKERR;
                }
                error = setSegment(m_path, 1, lastPoint); CHKERR;
            }
            else
            {
                m_length *= lengthMultiplyer;

                ai::int16 outSegCount = (ai::int16)lengthBetweenEnds * 2;
                outSegCount = myAiUtil::fixRange(outSegCount, 2, MAX_SEGMENT_COUNT_BEFORE_SIMPLIFY);

                // カテナリー曲線上の点をプロットする。
                AIReal sign = lastPoint.h < m_downpoint.h ? -1 : 1;

                AIRealPoint firstPoint = m_downpoint;
                std::vector<AIRealPoint> points;
                points.push_back(firstPoint);
                
                double width = abs(lastPoint.h - firstPoint.h);
                catenary::Curve cv = catenary::curve(width,
                                                     lastPoint.v - firstPoint.v,
                                                     m_length);
                
                // 曲率が大きいと単純化で滑らかな曲線にならないので、
                // 横方向に拡大した点をプロットし、単純化後に元に戻す。xratioはその倍率。
                double xratio = cv.a > 1 ? 2 : 2 / std::pow(std::fmax(0.01, cv.a),2);
                
                for (ai::int16 i = 1; i < outSegCount; i++) {
                    // 曲線上に等間隔で点を生成する場合
                    //double x = cv.x_for_s(m_length * i / outSegCount);
                    // x を等間隔に変化させる場合
                    double x = width * i / outSegCount;

                    double y = cv.evaluate(x);
                    
                    points.push_back(AIRealPoint{ sign * x * xratio + firstPoint.h, y + firstPoint.v});
                }
                points.push_back(AIRealPoint{ sign * width * xratio + firstPoint.h, lastPoint.v});

                // プロットした点にフィッティングしたパスのセグメントを取得する。
                std::vector<AIPathSegment> simpSegs;
                if(DO_SIMPLIFY){
                    error = getSimplifiedSegments(points, simpSegs); CHKERR;
                }
                else {
                    for (auto itr = points.begin(); itr != points.end(); ++itr) {
                        AIPathSegment seg;
                        seg.p = seg.in = seg.out = *itr;
                        simpSegs.push_back(seg);
                    }
                }
                
                // セグメントを設定する。
                if(simpSegs.size() > 0){
                    for (auto itr = simpSegs.begin(); itr != simpSegs.end(); ++itr) {
                        AIPathSegment& seg = *itr;
                        seg.in.h  = (seg.in.h - firstPoint.h) / xratio + firstPoint.h;
                        seg.p.h   = (seg.p.h - firstPoint.h)  / xratio + firstPoint.h;
                        seg.out.h = (seg.out.h - firstPoint.h) / xratio + firstPoint.h;
                    }
                    
                    if (simpSegs.size() != segCount) {
                        error = sAIPath->SetPathSegmentCount(m_path, simpSegs.size()); CHKERR;
                    }
                    
                    error = sAIPath->SetPathSegments(m_path, 0, simpSegs.size(), simpSegs.data()); CHKERR;
                }
            }
        }
        catch (ai::Error& ex) {
            m_path = nullptr;
            error = ex;
        }
        catch (...)
        {
            m_path = nullptr;
            error = kCantHappenErr;
        }
    }
    return error;
}

ASErr catenaryPlugin::ToolMouseDrag(AIToolMessage * message){
    return drawCatenary(message->cursor);
}

ASErr catenaryPlugin::ToolMouseUp(AIToolMessage * message){
    ASErr error = kNoErr;
    try{
        if(m_path){
            AIRealPoint lastPoint;
            error = snapCursor(message->cursor, lastPoint, message->event, SNAPCURSOR_CTL_MOUSEUP); CHKERR;
            error = drawCatenary(lastPoint); CHKERR;

            error = removeTinyPath(m_path); CHKERR;
        }
    }
    catch(ai::Error& ex) {
        error = ex;
    }
    catch(...)
    {
        error = kCantHappenErr;
    }
    m_path = nullptr;
    return error;
}

ASErr catenaryPlugin::TrackToolCursor(AIToolMessage* message){
    ASErr error = kNoErr;
    try{
        if(sAIUser != NULL){
            error = sAIUser->SetSVGCursor(kcatenaryToolCursorResourceID,
                                          fResourceManagerHandle); CHKERR;
        }
    }
    catch (ai::Error& ex) {
        error = ex;
    }
    catch(...)
    {
        error = kCantHappenErr;
    }
    return error;
}
