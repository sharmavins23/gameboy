# Game Boy Emulator

This project is a follow-along of
[Low Level Devel's](https://www.youtube.com/@lowleveldevel1712)
[Game Boy emulator series](https://youtube.com/playlist?list=PLVxiWMqQvhg_yk4qy2cSC3457wZJga_e5&si=-9VKWRZGcien0WDQ).

It's a simple project that features a (nearly) cycle-accurate Game Boy emulator,
functional with a few games.

Notably, it features some serious refactoring as well as improvements to logging
and robustness over the original. As the original is both fairly wasteful (in
redundancy) and error-prone, I attempted to test and mitigate these particular
problems with my own implementation.

# Setup

The project was originally created using WSL2, though it can run on Windows with
proper setup (see the original repository for the DLL files - DLLs can simply be
added in with the corresponding folder).

For Linux:

1. `sudo apt-get install cmake build-essential libsdl2-dev libsdl2-ttf-dev`
2. `mkdir build`
3. `cd build`
4. `cmake ..`
5. `make`
6. `gbemu/gbemu ../roms/<RomName>.gb`

# References

-   [Game Boy Pan Docs](https://gbdev.io/pandocs/)
-   [Game Boy CPU (LR35902) Instruction Set and OPCodes](https://www.pastraiser.com/cpu/gameboy/gameboy_opcodes.html)
-   [Game Boy Programming Manual (Ver. 1.1)](https://archive.org/details/GameBoyProgManVer1.1/page/n85/mode/2up)
-   [The Cycle-Accurate Game Boy Docs](https://github.com/rockytriton/LLD_gbemu/blob/main/docs/The%20Cycle-Accurate%20Game%20Boy%20Docs.pdf)
-   [Game Boy: Complete Technical Reference](https://github.com/rockytriton/LLD_gbemu/blob/main/docs/gbctr.pdf)

# License TL;DR

This project is distributed under the MIT license. This is a paraphrasing of a
[short summary](https://tldrlegal.com/license/mit-license).

This license is a short, permissive software license. Basically, you can do
whatever you want with this software, as long as you include the original
copyright and license notice in any copy of this software/source.

## What you CAN do:

-   You may commercially use this project in any way, and profit off it or the
    code included in any way;
-   You may modify or make changes to this project in any way;
-   You may distribute this project, the compiled code, or its source in any
    way;
-   You may incorporate this work into something that has a more restrictive
    license in any way;
-   And you may use the work for private use.

## What you CANNOT do:

-   You may not hold me (the author) liable for anything that happens to this
    code as well as anything that this code accomplishes. The work is provided
    as-is.

## What you MUST do:

-   You must include the copyright notice in all copies or substantial uses of
    the work;
-   You must include the license notice in all copies or substantial uses of the
    work.

If you're feeling generous, give credit to me somewhere in your projects.
