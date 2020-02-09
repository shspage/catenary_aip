#ifndef __catenarySuites_H__
#define __catenarySuites_H__

#include "IllustratorSDK.h"
#include "Suites.hpp"
#include <AICurveFittingSuite.h>
#include <AICursorSnap.h>

// AI suite headers

// Suite externs
extern "C" SPBasicSuite *sSPBasic;
extern "C" SPBlocksSuite *sSPBlocks;
extern "C" AIUnicodeStringSuite*	sAIUnicodeString;
extern "C" AIToolSuite* sAITool;
extern "C" AILayerSuite* sAILayer;
extern "C" AIArtSuite* sAIArt;
extern "C" AIPathSuite* sAIPath;
extern "C" AIPathStyleSuite* sAIPathStyle;
extern "C" AICurveFittingSuite* sAICurveFitting;
extern "C" AIDocumentViewSuite* sAIDocumentView;
extern "C" AICursorSnapSuite* sAICursorSnap;
extern "C" AIHitTestSuite* sAIHitTest;

#endif
