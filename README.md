This is a simple CHIP-8 emulator. Most of it is based on this spec:
http://devernay.free.fr/hacks/chip8/C8TECH10.HTM. Though I also used these blogs
for a few implementation tips:

* http://www.multigesture.net/articles/how-to-write-an-emulator-chip-8-interpreter/
* http://www.codeslinger.co.uk/pages/projects/chip8.html

It uses SDL2 for graphics, input, and sound and Catch2 for its test suite.

Build with make, run with the file name of a CHIP-8 ROM, and provide input using
123QWEASDZXC.
