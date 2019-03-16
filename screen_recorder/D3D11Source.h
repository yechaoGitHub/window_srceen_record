#pragma once

//��Ҫdxgi1.2

#include <dxgi.h>
#include <d3d11_1.h>
#include <dxgi1_2.h>
#include <stdint.h>

#pragma comment ( lib, "D3D11.lib")
#pragma comment ( lib, "dxgi.lib")

class D3D11Source
{
public:
	D3D11Source();
	~D3D11Source();

	bool Initial();
	bool Capture();
	bool GetData(uint8_t *data[4], int linesize[4]);
	bool GetSize(uint32_t *width, uint32_t *height);
	bool ReleaseData();
	bool Release();

private:
	uint32_t __width;
	uint32_t __height;

	const D3D_FEATURE_LEVEL		__feature_level;
	IDXGIFactory1				*__focatory;
	IDXGIAdapter1				*__adapter;
	ID3D11Device				*__device;
	ID3D11Device1				*__device1;
	ID3D11DeviceContext			*__context;
	IDXGIOutput					*__output;
	IDXGIOutput1				*__output1;
	IDXGIOutputDuplication		*__duplication;
	IDXGIResource				*__resourec;
	ID3D11Texture2D				*__texture;
	ID3D11Texture2D				*__texture_cpoy;
};

