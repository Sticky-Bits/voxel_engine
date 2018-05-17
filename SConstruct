VariantDir('build', 'src', duplicate=0)
env = Environment(CPPPATH=["./include"], LIBS=["dl", "glfw"])
env.Program(target='build/voxel_engine', source=["build/main.cpp", "build/chunk.cpp", "build/glad.cpp"])
