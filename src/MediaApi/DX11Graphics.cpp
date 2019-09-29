#include "pch.h"
#include "DX11Graphics.h"
#include "DX11RenderTarget.h"
#include "Common.h"
#include "ComHelper.h"

#pragma comment(lib, "d3d11.lib")


namespace mapi {

DX11Graphics::DX11Graphics()
{
}

DX11Graphics::~DX11Graphics()
{
	DiscardDeviceResources();
}

void DX11Graphics::Initialize(HWND hwnd, Error& err)
{
	if (!::IsWindow(hwnd))
	{
		err = { MAPIERR_INVALID_ARG };
		return;
	}

	if (m_device != nullptr)
	{
		err = { MAPIERR_INVALID_CALL };
		return;
	}

	m_window = hwnd;
	CreateDeviceResouces(err);

	if (err.code != MAPI_NO_ERROR)
	{
		DiscardDeviceResources();
	}
}

void DX11Graphics::UpdateSwapChain()
{
	RECT clientRect{};
	::GetClientRect(m_window, &clientRect);
	SIZE clientSize{ clientRect.right, clientRect.bottom };

	if (clientSize != m_windowSize)
	{
		m_windowSize = clientSize;
		HRESULT hr = m_swapChain->ResizeBuffers(2, m_windowSize.cx, m_windowSize.cy, DXGI_FORMAT_B8G8R8A8_UNORM, 0);
		ASSERT(SUCCEEDED(hr));
	}
}

void DX11Graphics::CreateDeviceResouces(Error& err)
{
	UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
	//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	static D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};

	static D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	uint32_t numDriverTypes = ARRAYSIZE(driverTypes);
	uint32_t numFeatureLevels = ARRAYSIZE(featureLevels);

	HRESULT hr = S_OK;

	for (uint32_t driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		m_driverType = driverTypes[driverTypeIndex];
		m_featureLevel = featureLevels[0];

		CComPtr<ID3D11Device> device;
		CComPtr<ID3D11DeviceContext> deviceContext;

		hr = ::D3D11CreateDevice(
			nullptr,
			m_driverType,
			nullptr,
			createDeviceFlags,
			featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&device,
			&m_featureLevel,
			&deviceContext
		);

		if (hr == E_INVALIDARG)
		{
			// DirectX 11.0 platforms will not recognize D3D_FEATURE_LEVEL_11_1 so we need to retry without it
			hr = ::D3D11CreateDevice(
				nullptr,
				m_driverType,
				nullptr,
				createDeviceFlags,
				&featureLevels[1],
				numFeatureLevels - 1,
				D3D11_SDK_VERSION,
				&device,
				&m_featureLevel,
				&deviceContext
			);
		}

		if (SUCCEEDED(hr))
		{
			m_device = std::move(device);
			m_deviceContext = std::move(deviceContext);
			break;
		}
	}
	RETURN_VOID_IF(!CHECK_COM_RESULT("D3D11CreateDevice", hr, err));

	CreateSwapChain(err);
}

void DX11Graphics::CreateSwapChain(Error& err)
{
	RECT rect{};
	BOOL br = ::GetClientRect(m_window, &rect);
	if (!br)
	{
		PROCESS_SYSTEM_ERROR("GetClientRect", ::GetLastError(), err);
		return;
	}

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Obtain DXGI factory from device (since we used nullptr for pAdapter above)
	CComQIPtr<IDXGIDevice> dxgiDevice(m_device);

	CComPtr<IDXGIAdapter> adapter;
	HRESULT hr = dxgiDevice->GetAdapter(&adapter);
	RETURN_VOID_IF(!CHECK_COM_RESULT("IDXGIDevice::GetAdapter", hr, err));

	CComPtr<IDXGIFactory1> dxgiFactory;
	hr = adapter->GetParent(IID_PPV_ARGS(&dxgiFactory));
	RETURN_VOID_IF(!CHECK_COM_RESULT("IDXGIAdapter::GetParent", hr, err));

	// Create swap chain
	// DirectX 11.0 systems
	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferCount = 2;
	sd.BufferDesc.Width = width;
	sd.BufferDesc.Height = height;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = m_window;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	hr = dxgiFactory->CreateSwapChain(m_device, &sd, &m_swapChain);
	RETURN_VOID_IF(!CHECK_COM_RESULT("IDXGIFactory1::CreateSwapChain", hr, err));

	// Note this tutorial doesn't handle full-screen swapchains so we block the ALT+ENTER shortcut
	dxgiFactory->MakeWindowAssociation(m_window, DXGI_MWA_NO_ALT_ENTER);

#if 0
	// Create a render target view
	CComPtr<ID3D11Texture2D> backBuffer;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&backBuffer));
	RETURN_VOID_IF(!CHECK_COM_RESULT("IDXGISwapChain::GetBuffer", hr, err));

	hr = m_device->CreateRenderTargetView(backBuffer, nullptr, &m_renderTargetView);
	RETURN_VOID_IF(!CHECK_COM_RESULT("ID3D11Device::CreateRenderTargetView", hr, err));

	m_deviceContext->OMSetRenderTargets(1, &(m_renderTargetView.p), nullptr);
#endif

	// Setup the viewport
	D3D11_VIEWPORT vp{};
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	m_deviceContext->RSSetViewports(1, &vp);

	m_windowSize = SIZE{ width, height };
}

void DX11Graphics::DiscardDeviceResources()
{
	if (m_deviceContext)
	{
		m_deviceContext->ClearState();
	}

	m_renderTargetView = nullptr;
	m_swapChain = nullptr;
	m_swapChain1 = nullptr;
	m_deviceContext = nullptr;
	m_deviceContext1 = nullptr;
	m_device = nullptr;
	m_device1 = nullptr;
}

};
