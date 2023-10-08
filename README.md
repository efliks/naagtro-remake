# NAAGTRO 2.0

In 2001 I wrote a simple intro for NAAG, the now-defunct New Age Assembler Group 
that I was a member of.

This is a reimplementation in C of the original NAAG intro. 22 years later
I really don't want to deal with Assembler anymore, but I still want to
play around with MS-DOS and old demoscene stuff, and to contribute to the retro
community.

The remake is mostly accurate with two exceptions: the scroller font and the light
trajectory are different. The scroller text is kept original.

<div align="center">
    <img src="showcase.gif" />
</div>

## Installation instructions

To compile NAAGTRO, there are two options.

Option #1, cross compile from Linux to DOS using [DJGPP](https://github.com/andrewwutw/build-djgpp). 
Enter the build directory, and run the following commands. Replace the path to 
DJGPP with your own.

    naagtro > DJGPP_DIR=/home/mikolaj/local/opt/djgpp-12.1.0
    naagtro > source ${DJGPP_DIR}/setenv
    naagtro > mkdir build
    naagtro > cd build
    
    naagtro/build > cmake -D CMAKE_C_COMPILER=${DJGPP_DIR}/i586-pc-msdosdjgpp/bin/gcc ..
    naagtro/build > make

Option #2, it is possible to build natively under DOS or Win9x. Once you have DJGPP installed, 
make sure to add these lines to your C:\AUTOEXEC.BAT:

    set PATH=C:\DJGPP\BIN;%PATH%
    set DJGPP=C:\DJGPP\DJGPP.ENV

Then, simply call make in the main directory:

    C:\NAAGTRO > make -f makefile.dos

## Q&A

1. I built naagtro.exe, what now?

    DJGPP generates code for protected mode so you need a DOS extender. Download
    [CWSDPMI](https://web.archive.org/web/20151217064947/http://homer.rice.edu/~sandmann/cwsdpmi/csdpmi7b.zip),
    unpack it and place CWSDPMI.exe in the build directory.

2. How do I run NAAGTRO in 2023 and beyond?

    If you don't have access to a DOS PC, use emulation, for example [DOSBox](http://www.dosbox.com/).
    Personally for development purposes I switch between DOSBox and VirtualBox 
    with Windows 98, and for running the final product I use a Pentium II system.

3. Why CMake, is that not overkill?

    No. CMake files are understood by IDEs, for example my personal favorite QtCreator.
