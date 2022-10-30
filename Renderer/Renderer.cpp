#include "Renderer.h"
#include "Mesh.h"
#include "../App/packages/WinPixEventRuntime.1.0.220124001/Include/WinPixEventRuntime/pix3.h"

LONG Renderer::displayWidth;
LONG Renderer::displayHeight;

const unique_ptr<Renderer> renderer = make_unique<Renderer>();

void Renderer::Initialize(LONG width, LONG height)
{
    displayWidth = width;
    displayHeight = height;
    scene = make_unique<Scene>();
    view = make_unique<View>();
    auto camera = Camera();
    camera.LookAt(XMVectorSet(0.0f, 60.f, 0.f, 1.0f), XMVectorSet(0.0f, 0.f, 0.f, 1.0f), XMVectorSet(1.f, 0.f, 0.f, 1.f));
    camera.UpdateScreenSize(renderer->displayWidth, renderer->displayHeight);
    view->UpdateCamera(camera);
    LoadCommonAssets();
    LoadAssets();
}

void Renderer::LoadCommonAssets()
{
    #if defined(DEBUG) || defined(_DEBUG)
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
        debugController->EnableDebugLayer();
    #endif
    // create device
    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));

    ComPtr<IDXGIAdapter1> pAdapter;
    ComPtr<IDXGIAdapter1> bestAdapter;
    SIZE_T maxSize = 0;
    for (uint32_t Idx = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(Idx, &pAdapter); ++Idx)
    {
        DXGI_ADAPTER_DESC1 desc;
        pAdapter->GetDesc1(&desc);
        
        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            continue;

        if (desc.DedicatedVideoMemory < maxSize)
            continue;
        maxSize = desc.DedicatedVideoMemory;
        bestAdapter = pAdapter.Detach();
    }
    
    ThrowIfFailed(D3D12CreateDevice(
        bestAdapter.Get(),
        D3D_FEATURE_LEVEL_12_1,
        IID_PPV_ARGS(&m_device)
        ));
    
    // create queue
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(m_device->CreateCommandQueue(
        &queueDesc,
        IID_PPV_ARGS(&m_commandQueue)
    ));

    // fence
    ThrowIfFailed(m_device->CreateFence(
        m_fenceValue,
        D3D12_FENCE_FLAG_NONE,
        IID_PPV_ARGS(&m_fence)
    ));
    m_fenceValue++;
    m_fenceEvent = CreateEventW( nullptr, FALSE, FALSE, nullptr);
    
    
    // create chain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = displayWidth;
    swapChainDesc.Height = displayHeight;
    swapChainDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),   
        WindowsWindow::windowInst,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
        ));
    ThrowIfFailed(factory->MakeWindowAssociation(WindowsWindow::windowInst, DXGI_MWA_NO_ALT_ENTER));
    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
    
    rtvHeap->Initialize();
    cbvSrvUavHeap->Initialize();
    sampler->Initialize();
    dsvHeap->Initialize();

}

void Renderer::LoadAssets()
{

    vector<ID3D12CommandList*> lists = {};
    
    for (UINT i = 0; i < FrameCount; i++)
    {
        frameResources[i] = new FrameResource(i);
        auto currentFrame = frameResources[i];
        
        currentFrame->Initialize();
        lists.push_back(currentFrame->commandList.Get());
    }

    m_commandQueue->ExecuteCommandLists(FrameCount, lists.data());

    WaitForGpu();
}


void Renderer::WaitForGpu()
{
    const UINT64 fenceToWaitFor = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
    m_fenceValue++;
    ThrowIfFailed(m_fence->SetEventOnCompletion(fenceToWaitFor, m_fenceEvent));
    WaitForSingleObject(m_fenceEvent, INFINITE);

}

void Renderer::Update()
{
    const auto currentFrame = frameResources[m_frameIndex];
    if (currentFrame->fenceValue > m_fence->GetCompletedValue())
    {
        auto waitFrameEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        m_fence->SetEventOnCompletion(frameResources[m_frameIndex]->fenceValue, waitFrameEvent);
        WaitForSingleObject(waitFrameEvent, INFINITE);
        CloseHandle(waitFrameEvent);
    }
    view->UpdateViewSize(renderer->displayWidth, renderer->displayHeight);

    currentFrame->ResetCommandList();
    currentFrame->Update(scene.get(), view.get());
    currentFrame->RenderStart();
    currentFrame->PrePass();
    currentFrame->BasePass();
    currentFrame->LightPass();
    currentFrame->RenderEnd();
    ID3D12CommandList* list[] = { currentFrame->commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(list), list);
    Present();
    
    frameResources[m_frameIndex]->fenceValue = m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
    NAME_D3D12_OBJECT(m_commandQueue, TEXT("commandQueue"));

    m_fenceValue++;
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Renderer::Present()
{
    m_swapChain->Present(1, 0);
}

void Renderer::PrePass()
{
    auto currentFrame = frameResources[m_frameIndex];
    currentFrame->PrePass();
    // ID3D12CommandList* list[] = { currentFrame->commandList.Get() };
    // m_commandQueue->ExecuteCommandLists(_countof(list), list);
    // WaitForGpu();
}

void Renderer::BasePass()
{
    auto currentFrame = frameResources[m_frameIndex];
    currentFrame->BasePass();
    // ID3D12CommandList* list[] = { currentFrame->commandList.Get() };
    // m_commandQueue->ExecuteCommandLists(_countof(list), list);
    // WaitForGpu();
}

void Renderer::LightPass()
{
    
}

