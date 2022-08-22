# Textures

Engine public
 - [ ] `Texture1D`
 - [x] `Texture2D`
 - [ ] `Texture3D`
 - [ ] `ComputeTexture1D`
 - [x] `ComputeTexture2D`
 - [x] `ComputeTexture3D`

Engine private
 - [x] `TextureWriter` - ability to write to a file
 - [ ] `TextureStreamer` - responsible for deciding which mips to load to gpu memory

Editor
 - [x] `TextureImporter` - creation of textures from files

Renderer
 - [ ] `TextureLoader` - loads a texture into gpu memory
 - [ ] `LoadedMipHandle` - reference to a loaded mip