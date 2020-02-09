#ifndef __catenaryPlugin_h__
#define __catenaryPlugin_h__

#include "Plugin.hpp"
#include "catenaryID.h"

Plugin* AllocatePlugin(SPPluginRef pluginRef);

void FixupReload(Plugin* plugin);

class catenaryPlugin : public Plugin
{
public:
    catenaryPlugin(SPPluginRef pluginRef);
    virtual ~catenaryPlugin();
  
    FIXUP_VTABLE_EX(catenaryPlugin, Plugin);
  
    ASErr StartupPlugin( SPInterfaceMessage * message );
    ASErr ShutdownPlugin( SPInterfaceMessage * message );
    ASErr PostStartupPlugin();
    ASErr Notify(AINotifierMessage* message);
    ASErr ToolMouseDown( AIToolMessage* message );
    ASErr ToolMouseDrag(AIToolMessage * message);
    ASErr ToolMouseUp(AIToolMessage * message);
    ASErr TrackToolCursor(AIToolMessage* message);
    ASErr EditTool(AIToolMessage *message);
    
private:
    AIArtHandle m_path;
    double m_length;
    AIRealPoint m_downpoint;
    AIToolHandle fToolHandle;
    AIResourceManagerHandle fResourceManagerHandle;
    AINotifierHandle fShutdownApplicationNotifier;
    
    ASErr AddNotifier(SPInterfaceMessage *message);
    AIErr AddTool(SPInterfaceMessage* message);

    AIErr checkForPathEndHit(const AIRealPoint cpoint, AIArtHandle &art, ai::int16 &segmentIndex, bool &is_hit);
    AIErr setTragetIfCatenary(const AIArtHandle &art, ai::int16 segIndex, bool adjustMode);
    ASErr drawCatenary(AIRealPoint lastPoint);
};

#endif
