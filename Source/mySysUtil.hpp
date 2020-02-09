#ifndef __mySysUtil_hpp__
#define __mySysUtil_hpp__

#ifdef MAC_ENV
#include <Carbon/Carbon.h>  // need to add Carbon.Framework to Build Phases/Link Binary With Libraries
#else
#include <windows.h>
#endif

namespace mySysUtil{
#ifdef MAC_ENV
    inline bool isVKeyDown(uint16_t vKey){
        uint8_t index = vKey / 32;
        uint8_t shift = vKey % 32;
        KeyMap keystate;
        GetKeys(keystate);
        return keystate[index].bigEndianValue & (1 << shift);
    }
    
    inline bool isUpArrowDown(){
        return isVKeyDown(kVK_UpArrow);
    }
    inline bool isDownArrowDown(){
        return isVKeyDown(kVK_DownArrow);
    }
    inline bool isLeftArrowDown(){
        return isVKeyDown(kVK_LeftArrow);
    }
    inline bool isRightArrowDown(){
        return isVKeyDown(kVK_RightArrow);
    }
#else
    inline bool isUpArrowDown(){
        return GetKeyState(VK_UP) & 0x80;
    }
    inline bool isDownArrowDown(){
        return GetKeyState(VK_DOWN) & 0x80;
    }
    inline bool isLeftArrowDown(){
        return GetKeyState(VK_LEFT) & 0x80;
    }
    inline bool isRightArrowDown(){
        return GetKeyState(VK_RIGHT) & 0x80;
    }
#endif

}

#endif /* mySysUtul_hpp */
