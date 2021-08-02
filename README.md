## Sound on the Gameboy Advance

This is a mirror of Deku's Game Boy Advance sound mixer tutorial, which is currently hosted at https://deku.gbadev.org/program/sound1.html.

The main reason for the mirror is that the examples don't compile on my setup, and I wanted to fix them all up. ATM this is a work in progress. I also want to tweak the site presentation to my preferences, mainly white text on a black background. For a more complete list of changes, see the [actual tutorial page intro](https://stuij.github.io/deku-sound-tutorial).

I wrote Deku an email to ask for permission to mirror the site, and he was very happy to grant it. He also stated his preference for a public domain license, so this whole repo is now covered by a CC0 license.

The code for the repo should compile on Linux, OSX and on Windows in a POSIX environment like MSYS2.

You can compile all the example code in one go, by executing `make` in the `code` directory. At the top of the Makefile in that directory, you can configure the paths to the relevant GBA toolchain programs, like gcc. There are two programs, `Converter` and `FreqTable`, which are compiled for the host system. These are expected to find g++ from the `PATH` set in the environment.

The rendered HTML for this tutorial: https://stuij.github.io/deku-sound-tutorial