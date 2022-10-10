#include "FrameResource.h"
#include "Helper.h"
#include "Renderer.h"
#include "View.h"

void FrameResource::Initialize()
{
    ThrowIfFailed(renderer->m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAlloc)));
    NAME_D3D12_OBJECT(commandAlloc, TEXT("commandAlloc"));

    ThrowIfFailed(renderer->m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAlloc.Get(), nullptr, IID_PPV_ARGS(&commandList)));
    NAME_D3D12_OBJECT(commandList, TEXT("commandList"));

    
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width    = static_cast<FLOAT>(Renderer::displayWidth);
    viewport.Height   = static_cast<FLOAT>(Renderer::displayHeight);
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    scissorRect = { 0, 0, Renderer::displayWidth, Renderer::displayHeight };

    
    ThrowIfFailed(renderer->GetSwapChain()->GetBuffer(frameIndex, IID_PPV_ARGS(&renderTarget)));
    NAME_D3D12_OBJECT(renderTarget, TEXT("renderTarget"));
    renderer->GetRtvHeap()->AllocatorRTV(&rtvHandle, renderTarget.Get(), nullptr);
    

    sceneTexture->Initialize(renderer->GetRtvHeap(), renderer->GetDsvHeap(), renderer->GetCbvSrvUavHeap());
    frameView->Initialize(renderer->GetCbvSrvUavHeap());
    
    ThrowIfFailed(commandList->Close());
}

void FrameResource::Bind(Scene* scene)
{
    lightSet.clear();
    objectSet.clear();
    scene->ObjectIter([this](Scene::SceneItem item) {
        if (!item->IsInitialized()) item->Initialize(GetCommandList());
        item->Update(frameIndex);
        this->objectSet.insert(item);
    });
    scene->LightIter([this](shared_ptr<Light> light)
    {
        if (!light->IsInitialized()) light->Initialize(GetCommandList());
        light->Update(frameIndex);
        this->lightSet.insert(light);
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
    
    ID3D12DescriptorHeap* heap[] = {renderer->GetCbvSrvUavHeap()->GetHeap(), renderer->GetSamplerHeap()};
    commandList->SetDescriptorHeaps(_countof(heap), heap);
    sceneTexture->TransitionForRender(commandList.Get());

}

void FrameResource::PrePass()
{
    PIXBeginEvent(commandList.Get(), 0, "Pre Pass");
    auto dsvHandle = sceneTexture->GetDSVCpuHandle();
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    
    // commandList->ClearRenderTargetView(*sceneTexture->GetRTVCpuHandle(), Colors::Black, 0, nullptr);
    // commandList->OMSetRenderTargets(1, sceneTexture->GetRTVCpuHandle(), TRUE, &dsvHandle);
    
    ID3D12DescriptorHeap* heap[] = {renderer->GetCbvSrvUavHeap()->GetHeap(), renderer->GetSamplerHeap()};
    commandList->SetDescriptorHeaps(_countof(heap), heap);

    auto light = lightSet.begin();
    while (light != lightSet.end())
    {
        auto handle = light->get()->GetShadowMapCpuHandle();
        commandList->OMSetRenderTargets(0, nullptr, FALSE, &handle);
        RenderObjects();
        ++light;
    }
    PIXEndEvent(commandList.Get());
}

void FrameResource::BasePass()
{
    PIXBeginEvent(commandList.Get(), 0, "Base Pass");

    auto dsvHandle = sceneTexture->GetDSVCpuHandle();
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    auto color = Colors::Black;
    color.f[3] = 0.0f;
    auto handles = sceneTexture->GetRTVCpuHandles();
    commandList->ClearRenderTargetView(handles[0], color, 0, nullptr);
    commandList->OMSetRenderTargets(1, &handles[0], FALSE, &dsvHandle);
    RenderObjects();
    PIXEndEvent(commandList.Get());
}

void FrameResource::LightPass()
{
    PIXBeginEvent(commandList.Get(), 0, "Light Pass");

    sceneTexture->TransitionForLightPass(commandList.Get());
    commandList->ClearRenderTargetView(GetRtvCpuHandle(), Colors::Black, 0, nullptr);
    auto handle = GetRtvCpuHandle();
    commandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);
    auto light = lightSet.begin();
    while (light != lightSet.end())
    {
        light->get()->InputAssemble(GetCommandList(), frameIndex, GetFrameView(), sceneTexture.get());
        light->get()->Render(GetCommandList());
        ++light;
    }

    PIXEndEvent(commandList.Get());
}
void FrameResource::RenderObjects() 
{
    auto iter = objectSet.begin();
    while (iter != objectSet.end())
    {
        iter->get()->InputAssemble(GetCommandList(), frameIndex, GetFrameView(), sceneTexture.get());
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