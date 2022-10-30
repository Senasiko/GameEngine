#include "Texture.h"
#include "Renderer.h"
#include "Helper.h"

void Texture::Create(D3D12_RESOURCE_DESC* desc, D3D12_RESOURCE_STATES resourceState)
{
    CD3DX12_HEAP_PROPERTIES defaultProp(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &defaultProp,
        D3D12_HEAP_FLAG_NONE,
        desc,
        resourceState,
        nullptr,
        IID_PPV_ARGS(&texture)));
    NAME_D3D12_OBJECT(texture, STRING_TO_LPCWSTR(name));
    state = resourceState;
}

void Texture::Upload(ID3D12GraphicsCommandList* commandList, UINT RowPitch, UINT SlicePitch, void* data)
{

    auto uploadDesc = CD3DX12_RESOURCE_DESC::Buffer(SlicePitch);
    CD3DX12_HEAP_PROPERTIES uploadProp(D3D12_HEAP_TYPE_UPLOAD);
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &uploadProp,
        D3D12_HEAP_FLAG_NONE,
        &uploadDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&textureUpload)));
    NAME_D3D12_OBJECT(textureUpload, STRING_TO_LPCWSTR(name));

    D3D12_SUBRESOURCE_DATA resourceData = {};
    resourceData.pData = data;
    resourceData.RowPitch = RowPitch;
    resourceData.SlicePitch = SlicePitch;
    
    UpdateSubresources(commandList, texture.Get(), textureUpload.Get(), 0, 0, 1, &resourceData);

    Transition(commandList, D3D12_RESOURCE_STATE_GENERIC_READ);
}

// void Texture::UploadFromFile(ID3D12GraphicsCommandList* commandList, D3D12_RESOURCE_DESC* desc, UINT RowPitch, UINT SlicePitch, string filename)
// {
//     unsigned char* data;
//     UINT size;
//     string currentFile = __FILE__;
//     string dirPath = currentFile.substr(0, currentFile.rfind("\\"));
//     string filePath = dirPath.append("\\textures\\").append(filename);
//     std::wstring stemp = std::wstring(filePath.begin(), filePath.end());
//     LPCWSTR sw = stemp.c_str();
//     // ReadDataFromFile(sw, &data, &size);
//     // Upload(commandList, desc, RowPitch, SlicePitch, data);
//     //
// }
void Texture2D::ResolveFromFile(string filename)
{
    UINT size;
    string currentFile = __FILE__;
    string dirPath = currentFile.substr(0, currentFile.rfind("\\"));
    string filePath = dirPath.append("\\textures\\").append(filename);
    std::wstring stemp = std::wstring(filePath.begin(), filePath.end());
    LPCWSTR sw = stemp.c_str();
    ReadDataFromFile(sw, &data, &size);
    // vector<unsigned char> png;
    // vector<unsigned char> image;
    // lodepng::State state;
    // auto error = lodepng::load_file(png, filePath);
    // if (!error)
    // {
    //     error = lodepng::decode(image, width, height, state, png);
    //     data = &image[0];
    // }
    // TexMetadata info = {};
    // auto image = std::make_unique<ScratchImage>();
    // DirectX::LoadFromDDSFile(sw, DDS_FLAGS_NONE, &info, *image);

}

void Texture2D::InitSRV(CbvSrvUavDescriptorHeap* heap, DXGI_FORMAT format) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = format;
    desc.Texture2D.MipLevels = mipLevels;
    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.ResourceMinLODClamp = 0.0f;
    heap->CreateSRV(srvHandle.get(), texture.Get(), &desc);
}

void Texture2D::InitAsRtv(RtvDescriptorHeap* heap, DXGI_FORMAT format, UINT width, UINT height)
{
    this->width = width;
    this->height = height;
    auto desc = CD3DX12_RESOURCE_DESC::Tex2D(format, width, height);
    desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;
    
    D3D12_CLEAR_VALUE optClear;
    optClear.Format = format;
    optClear.Color[0] = 0.0f;
    optClear.Color[1] = 0.0f;
    optClear.Color[2] = 0.0f;
    optClear.Color[3] = 1.0f;
    CD3DX12_HEAP_PROPERTIES heapProp(D3D12_HEAP_TYPE_DEFAULT);
    state = D3D12_RESOURCE_STATE_RENDER_TARGET;
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &desc,
        state,
        &optClear,
        IID_PPV_ARGS(&texture)));
        
    D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
    rtvDesc.Format = format;
    rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2DMS;
    heap->AllocatorRTV(handle.get(), GetResource(), &rtvDesc);
}

void Texture2D::InitAsDsV(DsvDescriptorHeap* heap, UINT width, UINT height)
{
    this->width = width;
    this->height = height;
    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = width;
    depthStencilDesc.Height = height;
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
    state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &heapProp,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilDesc,
        state,
        &optClear,
        IID_PPV_ARGS(&texture)));
    heap->AllocatorDSV(handle.get(), texture.Get());
    handle->heap = heap;

}

