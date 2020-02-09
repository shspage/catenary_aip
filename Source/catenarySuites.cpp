#include "IllustratorSDK.h"
#include "catenarySuites.h"

// Suite externs
extern "C"
{
    SPBlocksSuite*            sSPBlocks = NULL;
    AIUnicodeStringSuite*    sAIUnicodeString = NULL;
    AIToolSuite* sAITool = NULL;
    AILayerSuite* sAILayer = NULL;
    AIArtSuite* sAIArt = NULL;
    AIPathSuite* sAIPath = NULL;
    AIPathStyleSuite* sAIPathStyle = NULL;
    AICurveFittingSuite* sAICurveFitting = NULL;
    AIDocumentViewSuite* sAIDocumentView = NULL;
    AICursorSnapSuite* sAICursorSnap = NULL;
    AIHitTestSuite* sAIHitTest = NULL;
}

// Import suites
ImportSuite gImportSuites[] = 
{
    kSPBlocksSuite, kSPBlocksSuiteVersion, &sSPBlocks,
    kAIUnicodeStringSuite, kAIUnicodeStringVersion, &sAIUnicodeString,
    kAIToolSuite, kAIToolVersion, &sAITool,
    kAILayerSuite, kAILayerSuiteVersion, &sAILayer,
    kAIArtSuite, kAIArtSuiteVersion, &sAIArt,
    kAIPathSuite, kAIPathSuiteVersion, &sAIPath,
    kAIPathStyleSuite, kAIPathStyleSuiteVersion, &sAIPathStyle,
    kAICurveFittingSuite, kAICurveFittingSuiteVersion, &sAICurveFitting,
    kAIDocumentViewSuite, kAIDocumentViewSuiteVersion, &sAIDocumentView,
    kAICursorSnapSuite, kAICursorSnapSuiteVersion, &sAICursorSnap,
    kAIHitTestSuite, kAIHitTestSuiteVersion, &sAIHitTest,
    nullptr, 0, nullptr
};
