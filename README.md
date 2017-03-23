## Invaders

[![Build Status](https://travis-ci.org/MoleskiCoder/invaders.svg?branch=master)](https://travis-ci.org/MoleskiCoder/invaders)

Space Invaders Emulator written in C++

Uses SDL2 to provide graphics and SDL_mixer for sound effects.

### Features

* 2 player controls
* Coloured gel screen
* Sound effects
* Event driven Intel 8080 emulator
* Basic CP/M emulation (enough to run CPU tests)
* Intel 8080 profiler
* Intel 8080 disassembler

### To be done

* SDL Game Controller

## Compiling

### Windows

Compile with Visual Studio 2015 (via the solution)

#### Prerequisites

### Linux

make opt

#### Prerequisites

apt-get install build-essential libsdl2-dev libsdl2-mixer-dev

## Running

src/invaders

### Keyboard Controls

* 3: Insert Coin
* 1: 1P Start
* 2: 2P Start
* z: Left 1P
* x: Right 1P
* \\: Fire 1P
* ,: Left 2P
* .: Right 2P
* /: Fire 2P
