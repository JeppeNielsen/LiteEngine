#pragma once

#if defined(__EMSCRIPTEN__)
#ifndef SOKOL_GLES3
#define SOKOL_GLES3
#endif
#ifndef SOKOL_VALIDATE_NON_FATAL
#define SOKOL_VALIDATE_NON_FATAL
#endif
#else
#ifndef SOKOL_GLCORE
#define SOKOL_GLCORE
#endif
#endif

#ifndef SOKOL_NO_ENTRY
#define SOKOL_NO_ENTRY
#endif
