output: ClothSimulator.cpp
	g++ -g -std=gnu++11 -Wall ClothSimulator.cpp Cloth.cpp Camera.cpp LinkConstraint.cpp Plane.cpp PointMass.cpp Shader.cpp SphereCollider.cpp -o ClothSim -lGL -lSDL2 -Iglm -lGLEW

windows:
	i686-w64-mingw32-g++ -nostdinc++ -std=gnu++11 -Wall ClothSimulator.cpp Cloth.cpp Camera.cpp LinkConstraint.cpp Plane.cpp PointMass.cpp Shader.cpp SphereCollider.cpp -o WinClothSim -I/usr/include/glm -I/usr/include/GL -I/usr/include/SDL2 -L/usr/lib/x86_64-linux-gnu -lGL -lSDL2 -lGLEW -I/usr/lib/gcc/i686-w64-mingw32/5.3-win32/include/c++ -I/usr/lib/gcc/i686-w64-mingw32/5.3-win32/include/c++/i686-w64-mingw32
