#pragma once

#include "d3dx12.h"
#include <unordered_map>

class DescriptorManager
{
public:
	struct SRVKey
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;

		/*
		DXGI_FORMAT Format;
		D3D12_SRV_DIMENSION ViewDimension;
		UINT Shader4ComponentMapping;
		union
		{
			D3D12_BUFFER_SRV Buffer;
			D3D12_TEX1D_SRV Texture1D;
			D3D12_TEX1D_ARRAY_SRV Texture1DArray;
			D3D12_TEX2D_SRV Texture2D;
			D3D12_TEX2D_ARRAY_SRV Texture2DArray;
			D3D12_TEX2DMS_SRV Texture2DMS;
			D3D12_TEX2DMS_ARRAY_SRV Texture2DMSArray;
			D3D12_TEX3D_SRV Texture3D;
			D3D12_TEXCUBE_SRV TextureCube;
			D3D12_TEXCUBE_ARRAY_SRV TextureCubeArray;
			D3D12_RAYTRACING_ACCELERATION_STRUCTURE_SRV RaytracingAccelerationStructure;
		} 	;
		*/

		bool operator==(const SRVKey& k) const
		{
			return
				desc.Format == k.desc.Format &&
				desc.ViewDimension == k.desc.ViewDimension &&
				desc.Shader4ComponentMapping == k.desc.Shader4ComponentMapping &&

				desc.Buffer.FirstElement == k.desc.Buffer.FirstElement &&
				desc.Buffer.Flags == k.desc.Buffer.Flags &&
				desc.Buffer.NumElements == k.desc.Buffer.NumElements &&
				desc.Buffer.StructureByteStride == k.desc.Buffer.StructureByteStride &&

				desc.Texture1D.MipLevels == k.desc.Texture1D.MipLevels &&
				desc.Texture1D.MostDetailedMip == k.desc.Texture1D.MostDetailedMip &&
				desc.Texture1D.ResourceMinLODClamp == k.desc.Texture1D.ResourceMinLODClamp &&

				desc.Texture1DArray.ArraySize == k.desc.Texture1DArray.ArraySize &&
				desc.Texture1DArray.FirstArraySlice == k.desc.Texture1DArray.FirstArraySlice &&
				desc.Texture1DArray.MipLevels == k.desc.Texture1DArray.MipLevels &&
				desc.Texture1DArray.MostDetailedMip == k.desc.Texture1DArray.MostDetailedMip &&

				desc.Texture2D.MipLevels == k.desc.Texture2D.MipLevels &&
				desc.Texture2D.MostDetailedMip == k.desc.Texture2D.MostDetailedMip &&
				desc.Texture2D.PlaneSlice == k.desc.Texture2D.PlaneSlice &&
				desc.Texture2D.ResourceMinLODClamp == k.desc.Texture2D.ResourceMinLODClamp &&

				desc.Texture2DArray.ArraySize == k.desc.Texture2DArray.ArraySize &&
				desc.Texture2DArray.FirstArraySlice == k.desc.Texture2DArray.FirstArraySlice &&
				desc.Texture2DArray.MipLevels == k.desc.Texture2DArray.MipLevels &&
				desc.Texture2DArray.MostDetailedMip == k.desc.Texture2DArray.MostDetailedMip &&
				desc.Texture2DArray.PlaneSlice == k.desc.Texture2DArray.PlaneSlice &&
				desc.Texture2DArray.ResourceMinLODClamp == k.desc.Texture2DArray.ResourceMinLODClamp &&

				desc.Texture2DMS.UnusedField_NothingToDefine == k.desc.Texture2DMS.UnusedField_NothingToDefine &&

				desc.Texture2DMSArray.ArraySize == k.desc.Texture2DMSArray.ArraySize &&
				desc.Texture2DMSArray.FirstArraySlice == k.desc.Texture2DMSArray.FirstArraySlice &&

				desc.Texture3D.MipLevels == k.desc.Texture3D.MipLevels &&
				desc.Texture3D.MostDetailedMip == k.desc.Texture3D.MostDetailedMip &&
				desc.Texture3D.ResourceMinLODClamp == k.desc.Texture3D.ResourceMinLODClamp &&

				desc.TextureCube.MipLevels == k.desc.TextureCube.MipLevels &&
				desc.TextureCube.MostDetailedMip == k.desc.TextureCube.MostDetailedMip &&
				desc.TextureCube.ResourceMinLODClamp == k.desc.TextureCube.ResourceMinLODClamp &&

				desc.TextureCubeArray.First2DArrayFace == k.desc.TextureCubeArray.First2DArrayFace &&
				desc.TextureCubeArray.MipLevels == k.desc.TextureCubeArray.MipLevels &&
				desc.TextureCubeArray.MostDetailedMip == k.desc.TextureCubeArray.MostDetailedMip &&
				desc.TextureCubeArray.NumCubes == k.desc.TextureCubeArray.NumCubes &&
				desc.TextureCubeArray.ResourceMinLODClamp == k.desc.TextureCubeArray.ResourceMinLODClamp &&

				desc.RaytracingAccelerationStructure.Location == k.desc.RaytracingAccelerationStructure.Location &&

				handle.ptr == k.handle.ptr;
		}
	};




	struct RTVKey
	{
		D3D12_RENDER_TARGET_VIEW_DESC desc;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;

		/*
		DXGI_FORMAT Format;
		D3D12_RTV_DIMENSION ViewDimension;
		union
		{
			D3D12_BUFFER_RTV Buffer;
			D3D12_TEX1D_RTV Texture1D;
			D3D12_TEX1D_ARRAY_RTV Texture1DArray;
			D3D12_TEX2D_RTV Texture2D;
			D3D12_TEX2D_ARRAY_RTV Texture2DArray;
			D3D12_TEX2DMS_RTV Texture2DMS;
			D3D12_TEX2DMS_ARRAY_RTV Texture2DMSArray;
			D3D12_TEX3D_RTV Texture3D;
		};
		*/

		bool operator==(const RTVKey& k) const
		{
			return
				desc.Format == k.desc.Format &&
				desc.ViewDimension == k.desc.ViewDimension &&

				desc.Buffer.FirstElement == k.desc.Buffer.FirstElement &&
				desc.Buffer.NumElements == k.desc.Buffer.NumElements &&

				desc.Texture1D.MipSlice == k.desc.Texture1D.MipSlice &&

				desc.Texture1DArray.ArraySize == k.desc.Texture1DArray.ArraySize &&
				desc.Texture1DArray.FirstArraySlice == k.desc.Texture1DArray.FirstArraySlice &&
				desc.Texture1DArray.MipSlice == k.desc.Texture1DArray.MipSlice &&

				desc.Texture2D.MipSlice == k.desc.Texture2D.MipSlice &&
				desc.Texture2D.PlaneSlice == k.desc.Texture2D.PlaneSlice &&

				desc.Texture2DArray.ArraySize == k.desc.Texture2DArray.ArraySize &&
				desc.Texture2DArray.FirstArraySlice == k.desc.Texture2DArray.FirstArraySlice &&
				desc.Texture2DArray.MipSlice == k.desc.Texture2DArray.MipSlice &&
				desc.Texture2DArray.PlaneSlice == k.desc.Texture2DArray.PlaneSlice &&

				desc.Texture2DMS.UnusedField_NothingToDefine == k.desc.Texture2DMS.UnusedField_NothingToDefine &&

				desc.Texture2DMSArray.ArraySize == k.desc.Texture2DMSArray.ArraySize &&
				desc.Texture2DMSArray.FirstArraySlice == k.desc.Texture2DMSArray.FirstArraySlice &&

				desc.Texture3D.FirstWSlice == k.desc.Texture3D.FirstWSlice &&
				desc.Texture3D.MipSlice == k.desc.Texture3D.MipSlice &&
				desc.Texture3D.WSize == k.desc.Texture3D.WSize &&

				handle.ptr == k.handle.ptr;
		}
	};

	struct DSVKey
	{
		D3D12_DEPTH_STENCIL_VIEW_DESC desc;
		D3D12_CPU_DESCRIPTOR_HANDLE handle;


		/*
		DXGI_FORMAT Format;
		D3D12_DSV_DIMENSION ViewDimension;
		D3D12_DSV_FLAGS Flags;
		union
		{
			D3D12_TEX1D_DSV Texture1D;
			D3D12_TEX1D_ARRAY_DSV Texture1DArray;
			D3D12_TEX2D_DSV Texture2D;
			D3D12_TEX2D_ARRAY_DSV Texture2DArray;
			D3D12_TEX2DMS_DSV Texture2DMS;
			D3D12_TEX2DMS_ARRAY_DSV Texture2DMSArray;
			} 	;
		} 	D3D12_DEPTH_STENCIL_VIEW_DESC;
		*/

		bool operator==(const DSVKey& k) const
		{
			return
				desc.Flags == k.desc.Flags &&
				desc.ViewDimension == k.desc.ViewDimension &&
				desc.Format == k.desc.Format &&

				desc.Texture1D.MipSlice == k.desc.Texture1D.MipSlice &&

				desc.Texture1DArray.ArraySize == k.desc.Texture1DArray.ArraySize &&
				desc.Texture1DArray.FirstArraySlice == k.desc.Texture1DArray.FirstArraySlice &&
				desc.Texture1DArray.MipSlice == k.desc.Texture1DArray.MipSlice &&

				desc.Texture2D.MipSlice == k.desc.Texture2D.MipSlice &&

				desc.Texture2DArray.ArraySize == k.desc.Texture2DArray.ArraySize &&
				desc.Texture2DArray.FirstArraySlice == k.desc.Texture2DArray.FirstArraySlice &&
				desc.Texture2DArray.MipSlice == k.desc.Texture2DArray.MipSlice &&

				desc.Texture2DMS.UnusedField_NothingToDefine == k.desc.Texture2DMS.UnusedField_NothingToDefine &&

				desc.Texture2DMSArray.ArraySize == k.desc.Texture2DMSArray.ArraySize &&
				desc.Texture2DMSArray.FirstArraySlice == k.desc.Texture2DMSArray.FirstArraySlice &&

				handle.ptr == k.handle.ptr;
		};
	};

private:
	DescriptorManager() {}
	~DescriptorManager() {}
public:

	static DescriptorManager& getInstance()
	{
		static DescriptorManager instance;
		return instance;
	}


private:
	std::unordered_map<>

};
