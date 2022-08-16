#include "Texture.h"
#include "Renderer.h"
#include "Helper.h"

void Texture::Create(D3D12_RESOURCE_DESC* desc)
{
    CD3DX12_HEAP_PROPERTIES defaultProp(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(renderer->m_device->CreateCommittedResource(
        &defaultProp,
        D3D12_HEAP_FLAG_NONE,
        desc,
        D3D12_RESOURCE_STATE_COPY_DEST,
        nullptr,
        IID_PPV_ARGS(&texture)));
    NAME_D3D12_OBJECT(texture);
    state = D3D12_RESOURCE_STATE_COPY_DEST;
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
    NAME_D3D12_OBJECT(textureUpload);

    D3D12_SUBRESOURCE_DATA resourceData = {};
    resourceData.pData = data;
    resourceData.RowPitch = RowPitch;
    resourceData.SlicePitch = SlicePitch;
    
    UpdateSubresources(commandList, texture.Get(), textureUpload.Get(), 0, 0, 1, &resourceData);

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(texture.Get(), state, D3D12_RESOURCE_STATE_GENERIC_READ);
    commandList->ResourceBarrier(1, &transition);
    state = D3D12_RESOURCE_STATE_GENERIC_READ;
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

void Texture2D::CreateSRV(CbvSrvUavDescriptorHeap* heap) const
{
    D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
    desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    desc.Format = format;
    desc.Texture2D.MipLevels = mipLevels;
    desc.Texture2D.MostDetailedMip = 0;
    desc.Texture2D.ResourceMinLODClamp = 0.0f;
    heap->CreateSRV(handle.get(), texture.Get(), &desc);
}
