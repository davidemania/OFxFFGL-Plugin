Notes, please read
=====================================

Introduction
------------
Davide Manià - software@cogitamus.it

The code in this repository is quite old (from 2014) and is not supposed to run out of the box now.

The addon is meant for OpenFrameworks and its purpose is to run an OF application as a plugin for VJ host software (at the time I tested it successfully with Resolume Arena 4).

This version is for OSX, I remember building it on Windows too (with Microsoft Visual Studio) but at the moment I can't find the VS project files (settings were a little tricky). Hopefully I'll be able to add them later.

License
-------
The code for this addon and for the included FFGL-Plugin is offered like openFrameworks itself under the [MIT License](https://en.wikipedia.org/wiki/MIT_License). Read `license.md` for details.

Installation
------------
This code is experimental and offered "as is".

Dependencies
------------
None IIRC

Compatibility
------------
The code worked with OF 0.8.4 and XCode 5 on Mac.

As stated above it can run on Windows too. Will add the relevant files when I find them

Known issues
------------
It is not possible to use 3D in the plugins (no depth test) and for some reason ofDrawBitmapString() never worked.

Version history
------------

### Version 0.1 (January 2014):
First draft, by Davide Manià



