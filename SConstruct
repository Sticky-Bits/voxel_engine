VariantDir('build', 'source', duplicate=0)
env = Environment(CPPPATH=["./source/glm:./source/glfw/include:./source/khr/include/:./source/glad/include"], LIBS=["dl", "glfw"])
env.Append(CCFLAGS = '-g')
env.Program(target='build/voxel_engine', source=[
    "build/main.cpp",
    "build/Game.cpp",
    "build/Input.cpp",
    "build/Settings.cpp",
    "build/Window.cpp",
    "build/blocks/ChunkManager.cpp",
    "build/blocks/Chunk.cpp",
    "build/blocks/FastNoise.cpp",
    "build/renderer/Mesh.cpp",
    "build/renderer/Renderer.cpp",
])
