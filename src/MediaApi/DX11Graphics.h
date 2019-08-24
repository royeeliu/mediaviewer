#pragma once

#include "Types.h"
#include "Uncopyable.h"
#include <d3d11_1.h>

namespace mapi {

class DX11Graphics : Uncopyable
{
public:
	DX11Graphics();
	~DX11Graphics();

	void Initialize(HWND hwnd, Error& err);

public:
	ID3D11Device* GetDevice() const { return m_device; }
	ID3D11DeviceContext* GetDeviceContext() const { return m_deviceContext; }
	IDXGISwapChain* GetSwapChain() const { return m_swapChain; }
	ID3D11RenderTargetView* GetRenderTargetView() const { return m_renderTargetView; }

private:
	void CreateDeviceResouces(Error& err);
	void CreateSwapChain(Error& err);
	void DiscardDeviceResources();

private:
	D3D_DRIVER_TYPE					m_driverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL				m_featureLevel = D3D_FEATURE_LEVEL_11_1;
	HWND							m_window = nullptr;
	CComPtr<ID3D11Device>			m_device;
	CComPtr<ID3D11Device1>			m_device1;
	CComPtr<ID3D11DeviceContext>	m_deviceContext;
	CComPtr<ID3D11DeviceContext1>	m_deviceContext1;
	CComPtr<IDXGISwapChain>			m_swapChain;
	CComPtr<IDXGISwapChain1>		m_swapChain1;
	CComPtr<ID3D11RenderTargetView> m_renderTargetView;
};

};
