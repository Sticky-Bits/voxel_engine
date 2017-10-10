voxel_engine: main.cpp
	g++ -o main main.cpp glad.c -I. -I./include -lglfw -ldl
	# -ldl -lX11 -lpthread -lXrandr -lXi -lGL
	# -L/usr/lib
