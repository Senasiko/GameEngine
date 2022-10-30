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
    frameView->UpdateBuffer();
    Bind(scene);
}

void FrameResource::ResetCommandList()
{
    ThrowIfFailed(commandAlloc->Reset());
    ThrowIfFailed(commandList->Reset(commandAlloc.Get(), nullptr));
}

void FrameResource::RenderStart()
{
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(renderTarget.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    commandList->ResourceBarrier(1, &transition);
    
    ID3D12DescriptorHeap* heap[] = {renderer->GetCbvSrvUavHeap()->GetHeap(), renderer->GetSamplerHeap()};
    commandList->SetDescriptorHeaps(_countof(heap), heap);
    sceneTexture->TransitionForRender(commandList.Get());

}

void FrameResource::PrePass()
{
    PIXBeginEvent(commandList.Get(), 0, "Pre Pass");
    ID3D12DescriptorHeap* heap[] = {renderer->GetCbvSrvUavHeap()->GetHeap(), renderer->GetSamplerHeap()};
    commandList->SetDescriptorHeaps(_countof(heap), heap);

    auto iter = lightSet.begin();
    while (iter != lightSet.end())
    {
        auto light = iter->get();
        light->TransitionForPrePass(commandList.Get());
        commandList->RSSetViewports(1, light->GetShadowView()->GetViewport());
        commandList->RSSetScissorRects(1, light->GetShadowView()->GetScissorRect());
        auto dsvHandle = light->GetDSVCpuHandle();
        // auto rtvHandle = GetRtvCpuHandle();
        commandList->ClearDepthStencilView(light->GetDSVCpuHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
        commandList->OMSetRenderTargets(0, nullptr, FALSE, &dsvHandle);
        RenderObjects(light->GetShadowView(), TRUE);
        ++iter;
    }
    PIXEndEvent(commandList.Get());
}
 
void FrameResource::BasePass()
{
    PIXBeginEvent(commandList.Get(), 0, "Base Pass");

    commandList->RSSetViewports(1, frameView->GetViewport());
    commandList->RSSetScissorRects(1, frameView->GetScissorRect());
    
    auto dsvHandle = sceneTexture->GetDSVCpuHandle();
    commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
    auto handles = sceneTexture->GetRTVCpuHandles();
    for (auto handle : handles)
    {
        commandList->ClearRenderTargetView(handle, Colors::Black, 0, nullptr);
    }
    commandList->OMSetRenderTargets(handles.size(), handles.data(), FALSE, &dsvHandle);
    RenderObjects(GetFrameView(), FALSE);
    PIXEndEvent(commandList.Get());
}

void FrameResource::LightPass()
{
    PIXBeginEvent(commandList.Get(), 0, "Light Pass");

    sceneTexture->TransitionForLightPass(commandList.Get());
    commandList->ClearRenderTargetView(GetRtvCpuHandle(), Colors::Black, 0, nullptr);
    auto handle = GetRtvCpuHandle();
    commandList->OMSetRenderTargets(1, &handle, FALSE, nullptr);
    auto iter = lightSet.begin();
    while (iter != lightSet.end())
    {
        auto light = iter->get();
        light->TransitionForLightPass(commandList.Get());
        light->InputAssemble(GetCommandList(), frameIndex, GetFrameView(), sceneTexture.get(), FALSE);
        light->Render(GetCommandList());
        ++iter;
    }

    PIXEndEvent(commandList.Get());
}

void FrameResource::RenderObjects(View* view, BOOLEAN bIsPre) 
{
    auto iter = objectSet.begin();
    while (iter != objectSet.end())
    {
        iter->get()->InputAssemble(GetCommandList(), frameIndex, view, sceneTexture.get(), bIsPre);
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