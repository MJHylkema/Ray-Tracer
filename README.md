# C++ Ray Tracer (OpenGL)

Build commands used for RayTracer.cpp are:

Compile:
g++ -Wall -c "%f" Cone.cpp Cylinder.cpp Plane.cpp Color.cpp Object.cpp Sphere.cpp Vector.cpp

Build:
g++ -Wall -o "%e" Cone.cpp Cylinder.cpp Plane.cpp Color.cpp "%f" Sphere.cpp Vector.cpp Object.cpp -lGL -lGLU -lglut

With current setup rendering image takes approximately 3minutes on lab computers.
