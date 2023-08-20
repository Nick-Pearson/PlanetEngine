# Graphics Objects

```mermaid
graph GRAPHICS;
    MeshInstance-->Mesh;
    MeshInstance-->Material;
    Mesh-->Vertex;
    Material-->PixelShader;
    Material-->Textures;
```

## Instance

Actual instances of a renderable type, with all the position, material and geometry required to render
Full description required to render that item

Types:
 - [x] `MeshInstace`: mesh placed in the world to be rendered (could be extended to support gpu instancing?)
 - [ ] `LineInstance`: for perfect anti aliased line rendering
 - [ ] `UIInstance`: for game style ui rendering

## Mesh

 - [x] `Mesh`: description of verticies and triangles to join
 - [x] `Vertex`: description of kind of vertex, can include addional info for the shader

## Shading

 - [x] `Material`: shader bound to a set of input variables
 - [x] `PixelShader`: shader program and metadata such as required input variables

## Textures

Each texture dimension has its own class
Each texture can be marked as GPU writable

 - [ ] `Texture1D`
 - [ ] `Texture2D`
 - [ ] `Texture3D`