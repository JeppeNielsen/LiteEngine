# Sokol Rendering Primitives and Resources

This document explains the core rendering concepts in Sokol's `sokol_gfx.h` and how they relate to textures, render textures, shaders, and draw calls. It is written to match the API used in this repo (see `src/Example/main.cpp`).

## Quick mental model

Sokol separates **resources** (buffers, images, samplers, shaders, pipelines, views) from **passes** (swapchain or offscreen render targets) and **commands** (apply pipeline, bind resources, draw). A typical frame is:

1. `sg_begin_pass(...)`
2. `sg_apply_pipeline(...)`
3. `sg_apply_bindings(...)`
4. `sg_apply_uniforms(...)`
5. `sg_draw(...)`
6. `sg_end_pass()`
7. `sg_commit()`

## Rendering primitives

Primitive types describe how vertex/index data is interpreted for a draw call. They are set on the pipeline via `sg_pipeline_desc.primitive_type`.

Available primitive types (`sg_primitive_type`):

- `SG_PRIMITIVETYPE_POINTS`
- `SG_PRIMITIVETYPE_LINES`
- `SG_PRIMITIVETYPE_LINE_STRIP`
- `SG_PRIMITIVETYPE_TRIANGLES` (default)
- `SG_PRIMITIVETYPE_TRIANGLE_STRIP`

Indexed vs non-indexed rendering is selected by `sg_pipeline_desc.index_type` and the presence of an index buffer binding. The draw call is always `sg_draw(base_element, num_elements, num_instances)`; Sokol decides whether those elements are vertices or indices based on the bound state.

## Buffers

`sg_buffer` represents GPU memory used for vertices, indices, or storage buffers.

Buffer usage (`sg_buffer_usage`):

- `vertex_buffer`: bind in `sg_bindings.vertex_buffers[]`
- `index_buffer`: bind in `sg_bindings.index_buffer`
- `storage_buffer`: bind through a storage-buffer view in `sg_bindings.views[]`
- `immutable`: data never changes from CPU side
- `dynamic_update`: updated occasionally from CPU
- `stream_update`: updated every frame from CPU

Common paths:

- Immutable vertex/index buffers with initial data (see `src/Example/main.cpp`).
- Dynamic or stream buffers updated with `sg_update_buffer` or `sg_append_buffer`.

## Images and textures

`sg_image` is the core image resource. A plain texture and a render texture are both `sg_image` objects; the difference is **usage flags** and **how the image is viewed and bound**.

Image types (`sg_image_type`):

- `SG_IMAGETYPE_2D`
- `SG_IMAGETYPE_CUBE`
- `SG_IMAGETYPE_3D`
- `SG_IMAGETYPE_ARRAY`

Image usage (`sg_image_usage`):

- `color_attachment`: image can be rendered into as a color target
- `depth_stencil_attachment`: image can be used for depth/stencil
- `resolve_attachment`: image can receive MSAA resolves
- `storage_image`: image can be written by compute shaders
- `immutable`, `dynamic_update`, `stream_update`: CPU update intent

Important rules:

- Immutable textures must be created with initial data in `sg_image_desc.data`.
- Attachment or storage images must not be created with initial content; their initial contents are undefined.

## Views and samplers

Sokol uses explicit **views** and **samplers**.

- `sg_view` selects how a resource is bound (texture view, attachment view, storage view).
- `sg_sampler` defines filtering, wrapping, and comparison settings.

You bind views and samplers via `sg_bindings`:

- `sg_bindings.views[]`: texture views, storage buffer views, storage image views
- `sg_bindings.samplers[]`: samplers used alongside texture views

This separation matters for modern APIs (Metal/WebGPU/Vulkan) and allows sharing one image with multiple sampling or attachment interpretations.

## Render textures (offscreen targets)

A “render texture” in Sokol is an `sg_image` created with attachment usage, plus an `sg_view` used as a pass attachment.

Typical flow for a color render texture:

1. Create `sg_image` with `usage.color_attachment = true` and a renderable pixel format.
2. Create a color-attachment view with `sg_make_view(&(sg_view_desc){ .color_attachment.image = my_image })`.
3. Start an offscreen pass with `sg_pass.attachments.colors[0] = color_view`.
4. Render into it.
5. Create a texture view for sampling: `sg_make_view(&(sg_view_desc){ .texture.image = my_image })` and bind it in a later pass.

Example snippet:

```cpp
sg_image offscreen_img = sg_make_image(&(sg_image_desc){
    .width = 512,
    .height = 512,
    .pixel_format = SG_PIXELFORMAT_RGBA8,
    .usage = {
        .color_attachment = true,
        .immutable = true,
    },
});

sg_view offscreen_color = sg_make_view(&(sg_view_desc){
    .color_attachment = { .image = offscreen_img },
});

sg_view offscreen_tex = sg_make_view(&(sg_view_desc){
    .texture = { .image = offscreen_img },
});
```

Depth render textures use `usage.depth_stencil_attachment = true` and a depth format (for example `SG_PIXELFORMAT_DEPTH`).

## Shaders

`sg_shader` packages your vertex/fragment (and optionally compute) code, plus reflection details such as uniforms, sampled images, and samplers.

Key points:

- Sokol does not provide a single shader language; you supply backend-specific sources or bytecode.
- Uniforms are organized into **uniform blocks** and updated with `sg_apply_uniforms(slot, data)`.
- In this repo, the shader is inline GLSL (see `src/Example/main.cpp`).

If you need cross-compilation, Sokol provides a separate toolchain (`sokol-shdc`) to generate backend-specific shader code.

## Pipelines

`sg_pipeline` captures render state and input layout. It includes:

- Shader to use (`sg_pipeline_desc.shader`).
- Vertex layout (`sg_pipeline_desc.layout` and `attrs`).
- Primitive type (`sg_pipeline_desc.primitive_type`).
- Index type (`sg_pipeline_desc.index_type`).
- Raster state (`cull_mode`, `face_winding`).
- Depth/stencil and blend state.
- Sample count and render target formats.

Because the pipeline encodes render target formats and sample count, it must match the pass attachments or swapchain defaults.

## Passes and passes vs swapchain

There are three pass styles:

- Swapchain pass: render to the window backbuffer via `sg_pass.swapchain = sglue_swapchain()`.
- Offscreen render pass: render into attachment views in `sg_pass.attachments`.
- Compute pass: `sg_pass.compute = true` (no draw calls, only compute dispatch).

A render pass includes a `sg_pass_action` that defines load/store behavior and clear values.

## Example in this repo

`src/Example/main.cpp` demonstrates:

- Creating vertex and index buffers.
- Creating a simple GLSL shader.
- Building a pipeline with depth and culling enabled.
- Beginning a swapchain pass and drawing a rotating cube.

Use it as a minimal reference for the draw loop and resource creation order.

## Cheat sheet

Resource creation:

- `sg_make_buffer` for vertices and indices.
- `sg_make_image` for textures or render targets.
- `sg_make_view` for texture/attachment/storage views.
- `sg_make_sampler` for sampling behavior.
- `sg_make_shader` for GPU programs.
- `sg_make_pipeline` for input layout and render state.

Per frame:

- `sg_begin_pass`
- `sg_apply_pipeline`
- `sg_apply_bindings`
- `sg_apply_uniforms`
- `sg_draw`
- `sg_end_pass`
- `sg_commit`
