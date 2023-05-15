==============================================================================

        ay2sna - A .AY to .SNA file converter
        v1.00

        Copyright (c) 2007 Laurent de Soras

==============================================================================



Contents:

1. Legal
2. What is ay2sna ?
3. Using ay2sna
4. Compilation
5. History
6. Contact



1. Legal
--------

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Check the file license.txt to get full information about the license.



2. What is ay2sna ?
-------------------

.AY files are music files from Amstrad CPC and Sinclair Spectrum computers.
They contain music data as well as the code required to play it back on the
original computer. This program converts these files into .SNA memory snapshot
files for Amstrad CPC emulators (CPCEmu, WinApe, etc.), in order to be
executed and debugged. It works only for AY files from Amstrad CPC.



3. Using ay2sna
---------------

ay2sna runs from the command line. Full C++ source code is provided, as well
as a compiled version for Microsoft Windows (Win32).

Usage:

   %s [options] <ay_filename>

Options:

   -s <song_number>
      Selects the song number, ranging from 1 to N, where N is the number of
      songs stored in the .AY file.
      Default is the "main" song, indicated in the .AY file.

   -o <sna_filename>
      Selects the snapshot filename.
      Default is the same filename as input file, with the ".sna" extension
      appended.



4. Compilation and testing
--------------------------

Drop the following files into your project or makefile :
*.cpp
*.h
*.hpp

ay2sna may be compiled in two versions: release and debug. Debug version
has checks that could slow down the code. Define NDEBUG to set the Release
mode. For example, the command line to compile ay2sna on GCC would look like:

Debug mode:
g++ -o ay2sna_debug.exe -Wall *.cpp z80/*.c

Release mode:
g++ -o ay2sna_release.exe -DNDEBUG -O3 *.cpp z80/*.c

ay2sna is portable and should compile and work on any system.



5. History
----------

v1.00 (2007.04.04)
    - Initial release.



6. Contact
----------

Please address any comment, bug report or flame to:

Laurent de Soras
laurent.de.soras@club-internet.fr
http://ldesoras.free.fr
