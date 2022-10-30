#pragma once
#include "Helper.h"

class Shader
{
    ComPtr<ID3DBlob> vertexShader;
    ComPtr<ID3DBlob> pixelShader;
public:
    virtual ~Shader() = default;
    static std::wstring GetAssetFullPath(std::wstring fileName)
    {
        std::wstring dir = L".\\Shaders\\";
        return dir + fileName;
    }
    virtual void Initialize();
    virtual void Compile();
};


#define STRINGIFY(X) #X    

#define SHADER_FILE(ShaderName) STRINGIFY(CompiledShaders/##ShaderName.h)

#define GLOBAL_GRAPHIC_SHADER_START(ClassName) \
    class ClassName: public Shader { \
        public:\
        ClassName() = default;\
        ~ClassName() = default;
#define GLOBAL_GRAPHIC_SHADER_END() };

#define GLOBAL_GRAPHIC_SHADER_VERTEX_SHADER(VSName) \
    static const unsigned char* getVertexShaderStream() { return shader##VSName; } \
    static SIZE_T getVertexShaderSize() { return sizeof(shader##VSName); } \

#define GLOBAL_GRAPHIC_SHADER_PIXEL_SHADER(PSName) \
    static const unsigned char* getPixelShaderStream() { return shader##PSName; } \
    static SIZE_T getPixelShaderSize() { return sizeof(shader##PSName); } \

#define GLOBAL_GRAPHIC_SHADER_PRE_PIXEL_SHADER(PrePSName) \
    static const unsigned char* getPrePixelShaderStream() { return shader##PrePSName; } \
    static SIZE_T getPrePixelShaderSize() { return sizeof(shader##PrePSName); } \

#define GLOBAL_GRAPHIC_SHADER_WITH_PRE(ClassName, VSName, PSName, PrePSName) \
    GLOBAL_GRAPHIC_SHADER_START(ClassName)\
        GLOBAL_GRAPHIC_SHADER_VERTEX_SHADER(VSName) \
        GLOBAL_GRAPHIC_SHADER_PIXEL_SHADER(PSName) \
        GLOBAL_GRAPHIC_SHADER_PRE_PIXEL_SHADER(PrePSName) \
    GLOBAL_GRAPHIC_SHADER_END()

#define GLOBAL_GRAPHIC_SHADER(ClassName, VSName, PSName) \
    GLOBAL_GRAPHIC_SHADER_START(ClassName)\
    GLOBAL_GRAPHIC_SHADER_VERTEX_SHADER(VSName) \
    GLOBAL_GRAPHIC_SHADER_PIXEL_SHADER(PSName) \
GLOBAL_GRAPHIC_SHADER_END()

// #define IMPLEMENT_GLOBAL_GRAPHIC_SHADER(ClassName) \
//     ComPtr<ID3DBlob> ClassName::vertexShader; \
//     ComPtr<ID3DBlob> ClassName::pixelShader; \
//     void ClassName::Compile() \
//     {\
//         if (vertexShader.Get() != nullptr && pixelShader.Get() != nullptr ) return;\
//         UINT compileFlags;\
//         if(_DEBUG)\
//             compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;\
//         else\
//             compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;\
//         ThrowIfFailed(D3DCompileFromFile(Shader::GetAssetFullPath(GetVSShadeFileName()).c_str(), nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));\
//         ThrowIfFailed(D3DCompileFromFile(Shader::GetAssetFullPath(GetVSShadeFileName()).c_str(), nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));\
//     }\
