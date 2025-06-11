# Simple OpenGL Drawing Program

A lightweight C++ drawing application built with **OpenGL** and **GLFW**, featuring:

- Mouse-based freehand drawing
- Multiple brush colors and sizes (keyboard controlled)
- PNG saving using [`stb_image_write.h`](https://github.com/nothings/stb)
- Color preview in Upper Right Corner

---

##  Built With

- C++17 
- OpenGL (4.6)
- GLFW 3
- GLAD
- [`stb_image_write.h`](https://github.com/nothings/stb)

##  Build Instructions on Linux
$ cd src

$ g++ draw.cpp glad.c -lglfw -lGL -o ../bin/draw

##  Controls
- Mouse Left	Draw
- C	Cycle brush color
- J / K	Decrease / Increase brush size
- S	Save drawing to PNG (with timestamp)
- R	Clear canvas
- Q	Exit

##  Demo


https://github.com/user-attachments/assets/9ed0e529-e4d5-484e-9672-3287793ab03f


##  License
BSD LICENSE
