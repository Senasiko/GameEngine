#include "Helper.h"
#include "PipelineState.h"
#include "Renderer.h"

PipelineState::PipelineState(string name): name(name)
{
    CD3DX12_DEPTH_STENCIL_DESC depthStencilDesc(D3D12_DEFAULT);
    depthStencilDesc.DepthEnable = TRUE;
    depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    depthStencilDesc.StencilEnable = FALSE;

    stateDesc.pRootSignature = rootSignature->GetRootSignature();
    stateDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    stateDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    stateDesc.DepthStencilState = depthStencilDesc;
    stateDesc.SampleMask = UINT_MAX;
    stateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    stateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    stateDesc.SampleDesc.Count = 1;
    stateDesc.SampleDesc.Quality = 0;
}

void PipelineState::SetInputLayout(UINT num, D3D12_INPUT_ELEMENT_DESC inputElement[])
{
    D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
    inputLayoutDesc.pInputElementDescs = inputElement;
    inputLayoutDesc.NumElements = num;

    stateDesc.InputLayout = inputLayoutDesc;
    statePast += PIPELINE_STATE_DESC_PAST_INPUT_LAYOUT;
}

void PipelineState::SetRootSignature(UINT num, RootSignature::InitParam params[])
{
    rootSignature->Initialize(num, params);
    
    stateDesc.pRootSignature = rootSignature->GetRootSignature();
    statePast += PIPELINE_STATE_DESC_PAST_ROOT_SIGNATURE;
}

void PipelineState::SetShaders(D3D12_SHADER_BYTECODE VS, D3D12_SHADER_BYTECODE PS)
{
    stateDesc.VS = VS;
    stateDesc.PS = PS;
    
    statePast += PIPELINE_STATE_DESC_PAST_SHADERS;
}

void PipelineState::SetRenderTarget(UINT num, DXGI_FORMAT RTVFormats[])
{
    stateDesc.NumRenderTargets = num;
    for (auto i = 0; i < num; i++)
    {
        stateDesc.RTVFormats[i] = RTVFormats[i];
    }
    
    statePast += PIPELINE_STATE_DESC_PAST_RENDER_TARGET;
}


void PipelineState::Create()
{
    if (!Validate())
    {
        cout << "pipeline state fail" << endl;
        return;
    }
    ThrowIfFailed(renderer->m_device->CreateGraphicsPipelineState(&stateDesc, IID_PPV_ARGS(&state)));
    NAME_D3D12_OBJECT(state, STRING_TO_LPCWSTR(name));
}

void PipelineState::InputAssemble(ID3D12GraphicsCommandList* commandList, DescBindable* descs[])
{
    commandList->SetPipelineState(state.Get());
    rootSignature->SetGraphicsRootSignature(descs, commandList);
}
