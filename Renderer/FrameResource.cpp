#include "FrameResource.h"
#include "Helper.h"
#include "Renderer.h"
#include "View.h"

void FrameResource::Initialize()
{
    ThrowIfFailed(renderer->m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAlloc)));
    NAME_D3D12_OBJECT(commandAlloc);

    ThrowIfFailed(renderer->m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAlloc.Get(), nullptr, IID_PPV_ARGS(&commandList)));
    NAME_D3D12_OBJECT(commandList);

    
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = static_cast<FLOAT>(Renderer::displayWidth);
    viewport.Height   = static_cast<FLOAT>(Renderer::displayHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    scissorRect = { 0, 0, Renderer::displayWidth, Renderer::displayHeight };

    
    ThrowIfFailed(renderer->GetSwapChain()->GetBuffer(frameIndex, IID_PPV_ARGS(&renderTarget)));
    renderer->m_device->CreateRenderTargetView(renderTarget.Get(), nullptr, GetRtvCpuHandle());
    NAME_D3D12_OBJECT(renderTarget);


    // dsv
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = static_cast<UINT>(Renderer::displayWidth);
    depthStencilDesc.Height = static_cast<UINT>(Renderer::displayHeight);
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &optClear,
        IID_PPV_ARGS(&dsBuffer)));

    
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Texture2D.MipSlice = 0;
    renderer->m_device->CreateDepthStencilView(dsBuffer.Get(), &dsvDesc, GetDsvCpuHandle());

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(dsBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    commandList->ResourceBarrier(1, &transition);

    frameView->Initialize(renderer->GetCbvSrvUavHeap());

    ThrowIfFailed(commandList->Close());
}

void FrameResource::Bind(Scene* scene)
{
    objectSet.clear();
    scene->ObjectIter([this](Scene::SceneItem item) {
        if (!item->IsInitialized()) item->Initialize(GetCommandList());
        item->Update(frameIndex);
        this->objectSet.insert(item);
    });
}

void FrameResource::Update(Scene* scene, View* view)
{
    frameView->UpdateFromOther(view);
    frameView->UpdateBuffer(Renderer::displayWidth, Renderer::displayHeight);
    Bind(scene);
}

void FrameResource::ResetCommandList()
{
    ThrowIfFailed(commandAlloc->Reset());
    ThrowIfFailed(commandList->Reset(commandAlloc.Get(), nullptr));
}

void FrameResource::RenderStart()
{
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &transition);

    auto rtvHandle = GetRtvCpuHandle();
    auto dsvHandle = GetDsvCpuHandle();

    commandList->ClearRenderTargetView(rtvHandle, Colors::Black, 0, nullptr);
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    
    ID3D12DescriptorHeap* heap[] = {renderer->GetCbvSrvUavHeap()->GetHeap(), renderer->GetSamplerHeap()};
    commandList->SetDescriptorHeaps(_countof(heap), heap);

}

void FrameResource::RenderObjects()
{
    auto iter = objectSet.begin();
    while (iter != objectSet.end())
    {
        iter->get()->InputAssemble(GetCommandList(), frameIndex, GetFrameView());
        iter->get()->Render(GetCommandList());
        ++iter;
    }
}

void FrameResource::RenderEnd()
{
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    commandList->ResourceBarrier(1, &transition);

    ThrowIfFailed(commandList->Close());
}

D3D12_CPU_DESCRIPTOR_HANDLE FrameResource::GetRtvCpuHandle()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE start(renderer->GetRtvHeap()->GetCPUDescriptorHandleForHeapStart());
    return start.Offset(frameIndex * renderer->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
}

D3D12_GPU_DESCRIPTOR_HANDLE FrameResource::GetRtvGpuHandle()
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE start(renderer->GetRtvHeap()->GetGPUDescriptorHandleForHeapStart());
    return start.Offset(frameIndex * renderer->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));
}

D3D12_CPU_DESCRIPTOR_HANDLE FrameResource::GetDsvCpuHandle()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE start(renderer->GetDsvHeap()->GetCPUDescriptorHandleForHeapStart());
    return start.Offset(frameIndex * renderer->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
}

D3D12_GPU_DESCRIPTOR_HANDLE FrameResource::GetDsvGpuHandle()
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE start(renderer->GetDsvHeap()->GetGPUDescriptorHandleForHeapStart());
    return start.Offset(frameIndex * renderer->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV));
}
