#include "graphics/graphics_engine.h"

#include "window/gui_window.h"

MY_LIB_NAMESPACE_BEGIN

#define REFRESH_RATE		60			//���t���b�V�����[�g

CGraphicsEngine* CGraphicsEngine::s_instance = nullptr;

/// @brief �R���X�g���N�^
CGraphicsEngine::CGraphicsEngine()
{

}

/// @brief �f�X�g���N�^
CGraphicsEngine::~CGraphicsEngine()
{

}

/// @brief ����������
/// @param _windowWidth �E�B���h�E�̉���
/// @param _windowHeight �E�B���h�E�̏c��
void CGraphicsEngine::Initialize(unsigned int _windowWidth, unsigned int _windowHeight)
{
	m_windowWidth = _windowWidth;
	m_windowHeight = _windowHeight;

#ifdef _DEBUG
	InitDebugLayer();
#endif

	// DXGI�t�@�N�g���̍쐬
	CreateDxgiFactory();

	// D3D12�f�o�C�X���쐬
	CreateD3d12Device();

	m_commandWrapper.Initialize(m_d3d12Device);

	// �X���b�v�`�F�[�����쐬
	CreateSwapChain();

	// RTV�p�̃f�B�X�N���v�^�q�[�v���쐬
	CreateD3d12RtvDescriptorHeap();

	// �����_�[�^�[�Q�b�g���쐬
	CreateRenderTarget();

	// �t�F���X���쐬
	CreateD3d12Fence();
}

/// @brief �I������
void CGraphicsEngine::Finalize()
{
	// �`��܂�
	WaitDraw();

	// �t�F���X���폜
	if (m_d3d12Fence != nullptr)
	{
		m_d3d12Fence->Release();
		m_d3d12Fence = nullptr;
	}

	// RTV���폜
	for (auto& renderTarget : m_renderTarget)
	{
		if (renderTarget != nullptr)
		{
			renderTarget->Release();
			renderTarget = nullptr;
		}
	}

	// RTV�p�̃f�B�X�N���v�^�q�[�v���폜
	if (m_d3d12RtvDescriptorHeap != nullptr)
	{
		m_d3d12RtvDescriptorHeap->Release();
		m_d3d12RtvDescriptorHeap = nullptr;
	}

	// �X���b�v�`�F�[�����폜
	if (m_dxgiSwapChain != nullptr)
	{
		m_dxgiSwapChain->Release();
		m_dxgiSwapChain = nullptr;
	}

	// �R�}���h
	m_commandWrapper.Finalize();

	// D3D12�f�o�C�X���폜
	if (m_d3d12Device != nullptr)
	{
		m_d3d12Device->Release();
		m_d3d12Device = nullptr;
	}

	// DXGI�t�@�N�g�����폜
	if (m_dxgiFactory != nullptr)
	{
		m_dxgiFactory->Release();
		m_dxgiFactory = nullptr;
	}
}

/// @brief �O�X�V
void CGraphicsEngine::PreUpdate()
{
	// �R�}���h�̃��Z�b�g
	m_commandWrapper.Reset();

	m_frameIdx = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	// �����_�[�^�[�Q�b�g�Ƃ��Ďg�p�ł���悤�ɂȂ�܂ő҂�(Present -> �`��^�[�Q�b�g)
	m_commandWrapper.ResourceBarrier(m_renderTarget[m_frameIdx], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	ID3D12GraphicsCommandList* graphicsCommandList = GetD3dGraphicsCommandList();
	if (graphicsCommandList != nullptr)
	{
		// �r���[�|�[�g�̐ݒ�
		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<float>(m_windowWidth);
		viewport.Height = static_cast<float>(m_windowHeight);
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		graphicsCommandList->RSSetViewports(1, &viewport);

		// �V�U�[��`�̐ݒ�
		D3D12_RECT rect;
		rect.left = 0;
		rect.top = 0;
		rect.right = m_windowWidth;
		rect.bottom = m_windowHeight;
		graphicsCommandList->RSSetScissorRects(1, &rect);
	}
}

/// @brief �X�V
void CGraphicsEngine::Update()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12Device��null�ł��B\n");
		return;
	}

	if (m_d3d12RtvDescriptorHeap == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12RtvDescriptorHeap��null�ł��B\n");
		return;
	}

	// �����_�[�^�[�Q�b�g���Z�b�g
	SetRenderTarget();

	// �o�b�N�o�b�t�@���N���A
	ClearBackBuffer();
}

/// @brief �`��
void CGraphicsEngine::Draw()
{

}

/// @brief ��X�V
void CGraphicsEngine::PostUpdate()
{
	// �o�b�N�o�b�t�@�̕`�悪��������܂ő҂�(�`��^�[�Q�b�g -> Present)
	m_commandWrapper.ResourceBarrier(m_renderTarget[m_frameIdx], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	// �R�}���h���s
	m_commandWrapper.Execute();

	// ��ʂ̃t���b�v
	m_dxgiSwapChain->Present(1, 0);

	WaitDraw();
}

#ifdef _DEBUG

/// @brief �f�o�b�O���C���[�̏�����
void CGraphicsEngine::InitDebugLayer() const
{
	ID3D12Debug* debugController = nullptr;

	//�f�o�b�O�R���g���[���[�̐���
	HRESULT result = S_OK;
	result = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));

	if (result != S_OK || debugController == nullptr)
	{
		printf("CGraphicsEngine::InitDebugLayer �f�o�b�O�R���g���[���[�̐����Ɏ��s\n");
		return;
	}

	//�f�o�b�O�R���g���[���[�̗L����
	debugController->EnableDebugLayer();

	debugController->Release();
}

#endif

/// @brief DXGI�I�u�W�F�N�g�̐����Ɏg�p����DXGI�t�@�N�g���̍쐬
void CGraphicsEngine::CreateDxgiFactory()
{
	//DXGI�t�@�N�g���̐���
	HRESULT result = S_OK;
	result = CreateDXGIFactory1(IID_PPV_ARGS(&m_dxgiFactory));

	if (result != S_OK || m_dxgiFactory == nullptr)
	{
		printf("CGraphicsEngine::CreateDxgiFactory DXGI�t�@�N�g���̐����Ɏ��s\n");
		return;
	}
}

/// @brief D3D12�f�o�C�X���쐬
void CGraphicsEngine::CreateD3d12Device()
{
	//D3D12�f�o�C�X�̐���
	HRESULT result = S_OK;
	//WARP�A�_�v�^�͎w�肵�Ȃ�
	result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_d3d12Device));

	if (result != S_OK || m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12Device D3D12�f�o�C�X�̐����Ɏ��s\n");
		return;
	}
}

/// @brief �X���b�v�`�F�[�����쐬
void CGraphicsEngine::CreateSwapChain()
{
	if (m_dxgiFactory == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain dxgi�t�@�N�g����null�ł�\n");
		return;
	}
	ID3D12CommandQueue* graphicsCommandQueue = m_commandWrapper.GetCommandQueue();
	if (graphicsCommandQueue == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain �R�}���h�L���[��null�ł�\n");
		return;
	}

	const CGUIWindow& guiWindow = CGUIWindow::GetInstance();
	DXGI_MODE_DESC modeDesc = {};
	{
		modeDesc.Width = guiWindow.GetWindowWidth();		//�𑜓x�̉���
		modeDesc.Height = guiWindow.GetWindowHeight();		//�𑜓x�̏c��
	}
	{
		//���t���b�V�����[�g
		DXGI_RATIONAL refreshRate = {};
		refreshRate.Numerator = REFRESH_RATE;
		refreshRate.Denominator = 1;
		modeDesc.RefreshRate = refreshRate;
	}
	modeDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;							//R8G8B8A8�̕����Ȃ����K�������^
	modeDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;		//�X�L�������C���w��Ȃ�
	modeDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;							//�X�P�[�����O�w��Ȃ�

	DXGI_SAMPLE_DESC sampleDesc = {};
	sampleDesc.Count = 1;		//�s�N�Z�����Ƃ̃}���`�T���v����
	sampleDesc.Quality = 0;		//�}���`�T���v���͂��Ȃ�

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferDesc = modeDesc;
	swapChainDesc.SampleDesc = sampleDesc;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;					//�����_�[�^�[�Q�b�g�Ƃ��Ďg�p
	swapChainDesc.BufferCount = RTV_NUM;											//�o�b�t�@�[��
	swapChainDesc.OutputWindow = guiWindow.GetInstance().GetGuiWindowHandle();		//�o�̓E�B���h�E�̃n���h��
	swapChainDesc.Windowed = true;													//�S��ʕ\��
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;						//���]��Ƀo�b�N�o�b�t�@�̓��e��j��

	//�X���b�v�`�F�[�����쐬
	HRESULT result;
	IDXGISwapChain* swapChain = nullptr;
    result = m_dxgiFactory->CreateSwapChain(graphicsCommandQueue, &swapChainDesc, &swapChain);
	if (result != S_OK || swapChain == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain �X���b�v�`�F�[���̍쐬�Ɏ��s\n");
		return;
	}

	result = swapChain->QueryInterface(IID_PPV_ARGS(&m_dxgiSwapChain));
	if (result != S_OK || m_dxgiSwapChain == nullptr)
	{
		printf("CGraphicsEngine::CreateSwapChain �X���b�v�`�F�[���̌^�ϊ��Ɏ��s\n");
		return;
	}
	swapChain->Release();

	m_frameIdx = m_dxgiSwapChain->GetCurrentBackBufferIndex();
}

/// @brief RTV�p�f�B�X�N���v�^�q�[�v���쐬
void CGraphicsEngine::CreateD3d12RtvDescriptorHeap()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12DescriptorHeap d3d�f�o�C�X��null�ł�\n");
		return;
	}

	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;		//�����_�[�^�[�Q�b�g�r���[�Ƃ��Ďg�p
	desc.NumDescriptors = RTV_NUM;					//�t���[���o�b�t�@�ƃo�b�N�o�b�t�@��2
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	desc.NodeMask = 0;

	HRESULT result;
	result = m_d3d12Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_d3d12RtvDescriptorHeap));
	if (result != S_OK || m_d3d12RtvDescriptorHeap == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12DescriptorHeap �f�B�X�N���v�^�q�[�v�̐����Ɏ��s\n");
		return;
	}
}

/// @brief �����_�[�^�[�Q�b�g�̍쐬
void CGraphicsEngine::CreateRenderTarget()
{
	//�X���b�v�`�F�[���̃o�b�t�@���f�B�X�N���v�^�q�[�v�ɓo�^
	D3D12_CPU_DESCRIPTOR_HANDLE handle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	for (unsigned int idx = 0; idx < RTV_NUM; idx++)
	{
		HRESULT result;
		result = m_dxgiSwapChain->GetBuffer(idx, IID_PPV_ARGS(&m_renderTarget[idx]));
		if (result != S_OK)
		{
			printf("CGraphicsEngine::CreateRenderTarget �X���b�v�`�F�[���̃o�b�t�@�A�N�Z�X�Ɏ��s idx = %u\n", idx);
			continue;
		}
		//�����_�[�^�[�Q�b�g�r���[���쐬
		m_d3d12Device->CreateRenderTargetView(m_renderTarget[idx], nullptr, handle);

		//�n���h���̎w���|�C���^���f�B�X�N���v�^�����Z
		handle.ptr += rtvDescriptorSize;
	}
}

/// @brief �t�F���X�̍쐬
void CGraphicsEngine::CreateD3d12Fence()
{
	HRESULT result;
	result = m_d3d12Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_d3d12Fence));
	if (result != S_OK || m_d3d12Fence == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12Fence �t�F���X�̍쐬�Ɏ��s\n");
		return;
	}

	m_fenceValue = 1;
	
	// �������s�����̃C�x���g�n���h�����쐬
	m_fenceEvent = CreateEvent(nullptr, false, false, nullptr);
	if (m_fenceEvent == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12Fence �t�F���X�C�x���g�n���h���̍쐬�Ɏ��s\n");
		return;
	}
}

/// @brief �����_�[�^�[�Q�b�g�̃Z�b�g
void CGraphicsEngine::SetRenderTarget()
{
	ID3D12GraphicsCommandList* graphicsCommandList = GetD3dGraphicsCommandList();
	if (graphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::SetRenderTarget graphicsCommandList��null�ł�\n");
		return;
	}

	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;
	graphicsCommandList->OMSetRenderTargets(1, &currentFrameBufferRTVHandle, false, nullptr);
}

/// @brief �o�b�N�o�b�t�@�̃N���A
void CGraphicsEngine::ClearBackBuffer()
{
	ID3D12GraphicsCommandList* graphicsCommandList = GetD3dGraphicsCommandList();
	if (graphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::SetRenderTarget graphicsCommandList��null�ł�\n");
		return;
	}

	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;

	// �N���A
	graphicsCommandList->ClearRenderTargetView(currentFrameBufferRTVHandle, RT_CLEAR_COLOR, 0, nullptr);
}

/// @brief �`��ҋ@
void CGraphicsEngine::WaitDraw()
{
	ID3D12CommandQueue* graphicsCommandQueue = m_commandWrapper.GetCommandQueue();
	if (graphicsCommandQueue == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw graphicsCommandQueue��null�ł�\n");
	}

	if (m_d3d12Fence == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw m_d3d12Fence��null�ł�\n");
	}

	HRESULT result;
	const unsigned long long nowFenceValue = m_fenceValue;
	result = graphicsCommandQueue->Signal(m_d3d12Fence, nowFenceValue);
	m_fenceValue++;

	if (result != S_OK)
	{
		printf("CGraphicsEngine::WaitDraw �`��ҋ@�̓����Ɏ��s\n");
		return;
	}

	const unsigned long long compValue = m_d3d12Fence->GetCompletedValue();
	if (compValue < nowFenceValue)
	{
		result = m_d3d12Fence->SetEventOnCompletion(nowFenceValue, m_fenceEvent);
		if (result != S_OK)
		{
			printf("CGraphicsEngine::WaitDraw �t�F���X������̒l�ɒB�����ۂɔ�������C�x���g�̎w��Ɏ��s\n");
			return;
		}

		WaitForSingleObject(m_fenceEvent, INFINITE);
	}
}

/// @brief D3D�f�o�C�X���擾
/// @return D3D�f�o�C�X
ID3D12Device* CGraphicsEngine::GetD3dDevice()
{
	return m_d3d12Device;
}

/// @brief D3D�O���t�B�b�N�X�R�}���h���X�g���擾
/// @return D3D�O���t�B�b�N�X�R�}���h���X�g
ID3D12GraphicsCommandList* CGraphicsEngine::GetD3dGraphicsCommandList()
{
	return m_commandWrapper.GetCommandList();
}

/// @brief �V���O���g���̃C���X�^���X����
void CGraphicsEngine::CreateInstance()
{
	if (!s_instance)
	{
		s_instance = new CGraphicsEngine();
	}
}

/// @brief �V���O���g���̃C���X�^���X�擾
/// @return GraphicsEngine�̃C���X�^���X�̎Q��
CGraphicsEngine& CGraphicsEngine::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CGraphicsEngine::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

MY_LIB_NAMESPACE_END