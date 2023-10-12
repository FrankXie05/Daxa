#pragma once

#define DAXA_UNIFORM_BUFFER_SLOT0 0
#define DAXA_UNIFORM_BUFFER_SLOT1 1
#define DAXA_UNIFORM_BUFFER_SLOT2 2
#define DAXA_UNIFORM_BUFFER_SLOT3 3
#define DAXA_UNIFORM_BUFFER_SLOT4 4
#define DAXA_UNIFORM_BUFFER_SLOT5 5
#define DAXA_UNIFORM_BUFFER_SLOT6 6
#define DAXA_UNIFORM_BUFFER_SLOT7 7

#define DAXA_GPU_TABLE_SET_BINDING 0
#define DAXA_DECL_UNIFORM_BUFFER_BINDING_SET 1
#define DAXA_STORAGE_BUFFER_BINDING 0
#define DAXA_STORAGE_IMAGE_BINDING 1
#define DAXA_SAMPLED_IMAGE_BINDING 2
#define DAXA_SAMPLER_BINDING 3
#define DAXA_BUFFER_DEVICE_ADDRESS_BUFFER_BINDING 4
#define DAXA_SHADER_DEBUG_BUFFER_BINDING 5
#define DAXA_ID_INDEX_MASK (0x00FFFFFF)
#define DAXA_ID_VERSION_SHIFT (24)

#if defined(_STDC_) // C
#define DAXA_SHADER 0
#elif defined(__cplusplus) // C++
#define DAXA_SHADER 0
#elif defined(GL_core_profile) // GLSL
#define DAXA_SHADER 1
#define DAXA_SHADERLANG DAXA_SHADERLANG_GLSL
#elif defined(__HLSL_VERSION) // HLSL
#define DAXA_SHADER 1
#define DAXA_SHADERLANG DAXA_SHADERLANG_HLSL
#endif

#if DAXA_SHADER
#if !defined(DAXA_ENABLE_SHADER_NO_NAMESPACE)
#define DAXA_ENABLE_SHADER_NO_NAMESPACE 0
#else
#if !defined(DAXA_ENABLE_SHADER_NO_NAMESPACE)
#define DAXA_ENABLE_SHADER_NO_NAMESPACE 1
#endif
#endif
#if !defined(DAXA_ENABLE_SHADER_NO_NAMESPACE)
#define DAXA_ENABLE_SHADER_NO_NAMESPACE 0
#endif

#if DAXA_SHADERLANG == DAXA_SHADERLANG_GLSL
#include <daxa/daxa.glsl>
#elif DAXA_SHADERLANG == DAXA_SHADERLANG_HLSL
#include <daxa/daxa.hlsl>
#endif
#else
#if defined(_STDC_)
#include <daxa/c/daxa.h>

#if !defined(DAXA_UNIFORM_BUFFER_ALIGNMENT)
#define DAXA_UNIFORM_BUFFER_ALIGNMENT 64
#endif // #if !defined(DAXA_UNIFORM_BUFFER_ALIGNMENT)

/// @brief The c++ equivalent of a constant buffer in a file is simply a struct.
#define DAXA_DECL_UNIFORM_BUFFER(SLOT) typedef struct alignas(DAXA_UNIFORM_BUFFER_ALIGNMENT)
/// @brief Buffer ptr enable is ignored in c++.
#define DAXA_DECL_BUFFER_PTR(STRUCT_TYPE)
#define DAXA_DECL_BUFFER_PTR_ALIGN(STRUCT_TYPE, ALIGN)
/// @brief Buffer ptr types map to the buffer device address type in daxa.
#define daxa_RWBufferPtr(x) daxa_BufferDeviceAddress
/// @brief Buffer ptr types map to the buffer device address type in daxa.
#define daxa_BufferPtr(x) daxa_BufferDeviceAddress

#elif defined(__cplusplus)
#include <daxa/daxa.hpp>

#if !defined(DAXA_UNIFORM_BUFFER_ALIGNMENT)
#define DAXA_UNIFORM_BUFFER_ALIGNMENT 64
#endif // #if !defined(DAXA_UNIFORM_BUFFER_ALIGNMENT)

/// @brief The c++ equivalent of a constant buffer in a file is simply a struct.
#define DAXA_DECL_UNIFORM_BUFFER(SLOT) struct alignas(DAXA_UNIFORM_BUFFER_ALIGNMENT)
/// @brief Buffer ptr enable is ignored in c++.
#define DAXA_DECL_BUFFER_PTR(STRUCT_TYPE)
#define DAXA_DECL_BUFFER_PTR_ALIGN(STRUCT_TYPE, ALIGN)
/// @brief Buffer ptr types map to the buffer device address type in daxa.
#define daxa_RWBufferPtr(x) daxa::types::BufferDeviceAddress
/// @brief Buffer ptr types map to the buffer device address type in daxa.
#define daxa_BufferPtr(x) daxa::types::BufferDeviceAddress

#ifndef daxa_i32
#define daxa_i32 daxa::types::i32
#define daxa_u32 daxa::types::u32
#define daxa_f32 daxa::types::f32
#define daxa_f32 daxa::types::f32
#define daxa_f32vec2 daxa::types::f32vec2
#define daxa_f32mat2x2 daxa::types::f32mat2x2
#define daxa_f32mat2x3 daxa::types::f32mat2x3
#define daxa_f32mat2x4 daxa::types::f32mat2x4
#define daxa_f32vec3 daxa::types::f32vec3
#define daxa_f32mat3x2 daxa::types::f32mat3x2
#define daxa_f32mat3x3 daxa::types::f32mat3x3
#define daxa_f32mat3x4 daxa::types::f32mat3x4
#define daxa_f32vec4 daxa::types::f32vec4
#define daxa_f32mat4x2 daxa::types::f32mat4x2
#define daxa_f32mat4x3 daxa::types::f32mat4x3
#define daxa_f32mat4x4 daxa::types::f32mat4x4
#define daxa_i32 daxa::types::i32
#define daxa_u32 daxa::types::u32
#define daxa_i64 daxa::types::i64
#define daxa_u64 daxa::types::u64
#define daxa_i32vec2 daxa::types::i32vec2
#define daxa_u32vec2 daxa::types::u32vec2
#define daxa_i32vec3 daxa::types::i32vec3
#define daxa_u32vec3 daxa::types::u32vec3
#define daxa_i32vec4 daxa::types::i32vec4
#define daxa_u32vec4 daxa::types::u32vec4
#define daxa_BufferId daxa::types::BufferId
#define daxa_ImageViewId daxa::types::ImageViewId
#define daxa_SamplerId daxa::types::SamplerId
#endif

#if DAXA_ENABLE_SHADER_NO_NAMESPACE
using namespace daxa::types;
#endif

#endif
#endif
