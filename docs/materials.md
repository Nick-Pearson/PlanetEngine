# Materials 

Engine public
 - [ ] `PixelShader`: shader string and metadata about inputs
 - [x] `Material`: shader bound to input variables

Engine private
 - [x] `TextureWriter` - ability to write to a file
 - [ ] `TextureStreamer` - responsible for deciding which mips to load to gpu memory

Editor
 - [ ] `ShaderLoader` - uses spirv cross to precompile shaders and get metrics
 - [ ] `MaterialViewer` - preview of material
 - [ ] `ShaderEditor` - edit shaders without reloading editor

Renderer
 - [ ] `D3DPixelShader` - fully compiled shader with root signature
 - [ ] `D3DRootSignature` - description of shader signature
 - [ ] `D3DMaterial` - pixel shader, descriptor table + other variables