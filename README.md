# catenary.aip

[readme in japanese](https://github.com/shspage/catenary_aip/blob/master/readme_ja.md)

__catenary.aip__ is a plug-in tool for Adobe Illustrator 2020.

Forked from [@Ushio](https://github.com/Ushio) 's repository of an interactive tool for OpenFrameworks. 

## How To Use

* Drag to draw a catenary curve.
* Press SHIFT key while mouse down at the ends of a curve to EDIT it. (If the "curve" has certain conditions. Described later.)
* Press OPTION key while mouse down at the ends of a curve to DUPLICATE and edit it.
* Hit UP arrow key while dragging to SHORTEN the curve.
* Hit DOWN arrow key while dragging to LENGTHEN the curve.

## Note

You can edit or duplicate a path that has the following conditions __as__ a catenary curve.
The length of the path is preserved while it is bending, or unless the original length is unnaturally long as a curve.

* An editable open path which has no fill, has stroke.
* One of the ends is located fairly at the upper bound of the path.
* One of the ends is located fairly at the left bound of the path.
* One of the ends is located fairly at the right bound of the path.


## Install

(After build,)
Place __catenary.aip__ under Illustrator's plug-in folder or additional plug-in folder. Then (re)start Illustrator to find catenary tool at the bottom category of the tools list. 


## Development Environment

Adobe Illustrator 2020 SDK  
Xcode 10.1 / macOS 10.13 sdk / MacOS High Sierra

Visual Studio 2017 / Windows10

## Note For Build

* Place "catenary_aip" folder directly under Illustrator SDK's samplecode folder.
* (Windows) To build with attached VS project file, you must convert the character encoding of source files (.cpp, .h, .hpp) from UTF-8 to Multibyte(cp932).

## License

zlib License  
Copyright (c) 2020 Hiroyuki Sato  
https://github.com/shspage  
See LICENSE-zlib.txt for details.

This software depends on the following resources which may have the licenses differs from this software.

* catenary.hpp (namespace catenary)  
zlib License  
Copyright (c) 2017 ushiostarfish  
https://github.com/Ushio/Catenaly

* Adobe Illustrator 2020 SDK  
Copyright (c) 2020 Adobe. All rights reserved.  
https://www.adobe.io/


