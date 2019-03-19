#include "D3D11Source.h"


D3D11Source::D3D11Source():
	__feature_level(D3D_FEATURE_LEVEL_11_1),
	__focatory(nullptr),
	__adapter(nullptr),
	__device(nullptr),
	__device1(nullptr),
	__context(nullptr),
	__output(nullptr),
	__output1(nullptr),
	__duplication(nullptr),
	__resourec(nullptr),
	__texture(nullptr),
	__texture_cpoy(nullptr)

{

}

D3D11Source::~D3D11Source()
{

}

bool D3D11Source::Initial()
{
	D3D_FEATURE_LEVEL used_feature_level(D3D_FEATURE_LEVEL_11_1);
	
	HRESULT hr = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&__focatory);
	if (FAILED(hr))
		goto error;

	hr = __focatory->EnumAdapters1(0, &__adapter);
	if (FAILED(hr))
		goto error;

	hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, D3D11_CREATE_DEVICE_BGRA_SUPPORT, &__feature_level, 1,
		D3D11_SDK_VERSION, &__device, &used_feature_level, &__context); 
	if (FAILED(hr))
		goto error;

	hr = __device->QueryInterface(__uuidof(ID3D11Device1), (void**)&__device1);
	if (FAILED(hr))
		goto error;

	hr = __adapter->EnumOutputs(0, &__output);
	if (FAILED(hr))
		goto error;

	hr = __output->QueryInterface(__uuidof(IDXGIOutput1), (void**)&__output1);
	if (FAILED(hr))
		goto error;

	hr = __output1->DuplicateOutput(__device, &__duplication);
	if (FAILED(hr))
		goto error;

	return true;

error:
	return false;
}

bool D3D11Source::Capture()
{
	static DXGI_OUTDUPL_FRAME_INFO info = { 0 };
	static D3D11_TEXTURE2D_DESC tex_desc = { 0 };

	memset(&info, 0, sizeof(info));
	HRESULT hr = __duplication->AcquireNextFrame(0, &info, &__resourec);
	if (FAILED(hr))
		goto error;

	hr = __resourec->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&__texture);
	if (FAILED(hr))
		goto error;

	if (__texture_cpoy == nullptr) 
	{
		__texture->GetDesc(&tex_desc);
		tex_desc.Usage = D3D11_USAGE_STAGING;
		tex_desc.BindFlags = 0;
		tex_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tex_desc.MiscFlags = 0;

		hr = __device->CreateTexture2D(&tex_desc, NULL, &__texture_cpoy);
		if (FAILED(hr))
			goto error;
	}

	__context->CopyResource(__texture_cpoy, __texture);
	__texture->Release();
	__resourec->Release();
	hr = __duplication->ReleaseFrame();
	
	if (FAILED(hr))
		goto error;

	return true;
	
error:
	return false;
}

bool D3D11Source::GetData(uint8_t *data[4], uint32_t linesize[4])
{
	static D3D11_MAPPED_SUBRESOURCE map = { 0 };

	__context->Map(__texture_cpoy, 0, D3D11_MAP_READ, 0, &map);

	data[0] = (uint8_t*)map.pData;
	linesize[0] = map.RowPitch / sizeof(uint32_t);

	return true;
}

bool D3D11Source::ReleaseData()
{
	__context->Unmap(__texture_cpoy, 0);

	return true;
}

bool D3D11Source::GetScreenSize(uint32_t &width, uint32_t &height)
{
	if (__output == nullptr)
		return false;

	DXGI_OUTPUT_DESC desc = { 0 };
	HRESULT hr = __output->GetDesc(&desc);
	if (SUCCEEDED(hr)) 
	{
		width = desc.DesktopCoordinates.left = desc.DesktopCoordinates.right;
		height = desc.DesktopCoordinates.bottom - desc.DesktopCoordinates.top;
		return true;
	}
	else 
	{
		return false;
	}
}

bool D3D11Source::Release()
{
	if (__focatory)
		__focatory->Release();
	if (__adapter)
		__adapter->Release();
	if (__device)
		__device->Release();
	if(__context)
		__context->Release();
	if (__output)
		__output->Release();
	if (__duplication)
		__duplication->Release();
	if (__resourec)
		__resourec->Release();
	if (__texture)
		__texture->Release();
	if (__texture_cpoy)
		__texture_cpoy->Release();

	return true;
}

