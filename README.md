![Celerytest](https://raw.githubusercontent.com/Chlorophytus/celerytest/master/lib/CelerytestPromo.png)

# celerytest
OpenGL 4 game engine! Inspired by Torque Game Engine 

## Usage
```
$ ./build/celerytest w h f
```
- `w` : width
- `h` : height
- `f` : fullscreen flag (1 = on, 0 = off)

## Okay so...
### ...devkits!
I am unable to get/make you a dev kit, but I can point you to what I am planning on targeting: an Odroid-H2+ (Intel Celeron J4115) with 4GB RAM, load Xubuntu LTS 20.04 on it.

However, you could definitely run Celerytest on a new computer. See below.

### ...what do I run/make games on?
#### Compute-based Raytracing OFF
- CPU minimum: A Pentium Silver or Celeron (a CPU with AVX would be nice)
- RAM minimum: 1-2GiB (at least 4GB would be nice)
- GPU minimum: Intel UHD6xx Graphics (a video card would be nice)
#### Compute-based Raytracing ON
- GPU: Have an AMD Vega-based GPU handy.
### ...how do I run this?
- CMake 3.15.x required
- SDL2 (Base / TTF / Image / Net)
- An OpenGL 4.6 runtime should be on your computer.

### ...why?
I was bored. :)
