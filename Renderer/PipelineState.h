#pragma once
#include "../Core/pch.h"
#include "RootSignature.h"

enum PIPELINE_STATE_DESC_PAST
{
    PIPELINE_STATE_DESC_PAST_INPUT_LAYOUT = 0x01,
    PIPELINE_STATE_DESC_PAST_ROOT_SIGNATURE = 0x01 << 1,
    PIPELINE_STATE_DESC_PAST_SHADERS = 0x01 << 2,
    PIPELINE_STATE_DESC_PAST_RENDER_TARGET = 0x01 << 3,

    PIPELINE_STATE_DESC_PAST_ALL = 0x01 << 4,
};

class PipelineState
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC stateDesc = {};
    ComPtr<ID3D12PipelineState> state;
    unique_ptr<RootSignature> rootSignature = make_unique<RootSignature>();
    string name;
    UINT statePast = 0;
public:
    PipelineState(string name);
    void SetInputLayout(UINT num, D3D12_INPUT_ELEMENT_DESC inputElement[]);
    void SetRootSignature(UINT num, RootSignature::InitParam params[]);
    void SetShaders(D3D12_SHADER_BYTECODE VS, D3D12_SHADER_BYTECODE PS);
    void SetRenderTarget(UINT num, DXGI_FORMAT RTVFormats[]);
    void Create();
    void InputAssemble(ID3D12GraphicsCommandList* commandList, DescBindable* descs[]);

    BOOL Validate()
    {
        return (statePast & (PIPELINE_STATE_DESC_PAST_ALL - 1)) == (PIPELINE_STATE_DESC_PAST_ALL - 1);
    }
};
