#pragma once
#include "D3D12Include.h"

class FD3D12Resource
{
public:

	ComPtr<ID3D12Resource> Resource;
};

class FD3D12TextureResource : public FD3D12Resource
{

};

class FD3D12BufferResource : public FD3D12Resource
{

};

class FD3D12RenderTargetResource : public FD3D12Resource
{

};

class FD3D12DepthStencilTargetResource : public FD3D12Resource
{

};