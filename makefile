voxel_engine: main.cpp
	g++ -o voxel_engine main.cpp glad.c FastNoise.cpp -I. -I./include -lglfw -ldl
	# -ldl -lX11 -lpthread -lXrandr -lXi -lGL
	# -L/usr/lib
