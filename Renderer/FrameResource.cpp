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
    NAME_D3D12_OBJECT(renderTarget);
    renderer->GetRtvHeap()->AllocatorRTV(&rtvHandle, renderTarget.Get(), nullptr);
    

    sceneTexture->Initialize(renderer->GetRtvHeap(), renderer->GetDsvHeap());
    frameView->Initialize(renderer->GetCbvSrvUavHeap());
    
    ThrowIfFailed(commandList->Close());
}

void FrameResource::Bind(Scene* scene)
{
    lightSet.clear();
    objectSet.clear();
    scene->LightIter([this](shared_ptr<Light> light)
    {
        if (!light->IsInitialized()) light->Initialize(GetCommandList());
        light->Update(frameIndex);
        this->lightSet.insert(light);
    });
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

void FrameResource::LightPass()
{
    // auto iter = lightSet.begin();
    // while (iter != lightSet.end())
    // {
    //     iter->get()->InputAssemble(GetCommandList(), frameIndex, GetFrameView());
    //     iter->get()->Render(GetCommandList());
    //     ++iter;
    // }
}

void FrameResource::RenderStart()
{
    commandList->RSSetViewports(1, &viewport);
    commandList->RSSetScissorRects(1, &scissorRect);

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &transition);

    auto rtvHandle = GetRtvCpuHandle();

    
    commandList->ClearRenderTargetView(rtvHandle, Colors::Black, 0, nullptr);
    commandList->ClearDepthStencilView(*sceneTexture->GetDSVCpuHandle(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    auto b = sceneTexture->GetDSVCpuHandle();
    commandList->OMSetRenderTargets(1, sceneTexture->GetRTVCpuHandle(), TRUE,  nullptr);
    
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
    return rtvHandle.GetCpuHandle();
}

D3D12_GPU_DESCRIPTOR_HANDLE FrameResource::GetRtvGpuHandle()
{
    return rtvHandle.GetGpuHandle();
}