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
void CGraphicsEngine::Initialize()
{
#ifdef _DEBUG
	InitDebugLayer();
#endif

	// DXGI�t�@�N�g���̍쐬
	CreateDxgiFactory();

	// D3D12�f�o�C�X���쐬
	CreateD3d12Device();

	// �R�}���h�L���[���쐬
	CreateD3d12CommandQueue();

	// �R�}���h�A���P�[�^���쐬
	CreateD3d12CommandAllocator();

	// �R�}���h���X�g���쐬
	CreateD3d12CommandList();

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

	// �R�}���h���X�g���폜
	if (m_d3d12GraphicsCommandList != nullptr)
	{
		m_d3d12GraphicsCommandList->Release();
		m_d3d12GraphicsCommandList = nullptr;
	}

	// �R�}���h�A���P�[�^���폜
	if (m_d3d12CommandAllocator != nullptr)
	{
		m_d3d12CommandAllocator->Release();
		m_d3d12CommandAllocator = nullptr;
	}

	// �R�}���h�L���[���폜
	if (m_d3d12CommandQueue != nullptr)
	{
		m_d3d12CommandQueue->Release();
		m_d3d12CommandQueue = nullptr;
	}

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

/// @brief �X�V
void CGraphicsEngine::Update()
{
	HRESULT result;

	if (m_d3d12CommandAllocator == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12CommandAllocator��null�ł��B\n");
	}

	if (m_d3d12GraphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::Update m_d3d12GraphicsCommandList��null�ł��B\n");
	}
	
	m_frameIdx = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	// todo ��������GraphicsEngine�ł��ׂĕ`��܂ł�邪�A�R�}���h����͋@�\��������
	// todo Draw�֐��Ȃǂ��쐬���Ď��s���ɋC��t����

	// �R�}���h�A���P�[�^�����Z�b�g
	result = m_d3d12CommandAllocator->Reset();
	assert(SUCCEEDED(result));

	// �R�}���h���X�g�����Z�b�g
	result = m_d3d12GraphicsCommandList->Reset(m_d3d12CommandAllocator, nullptr);
	assert(SUCCEEDED(result));

	// todo PreUpdate�ȂǂŁA�`��O�Ɏ��s
	// �����_�[�^�[�Q�b�g�Ƃ��Ďg�p�ł���悤�ɂȂ�܂ő҂�
	{
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;		//�T�u���\�[�X�̑J��
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		{
			D3D12_RESOURCE_TRANSITION_BARRIER transition;
			transition.pResource = m_renderTarget[m_frameIdx];				//�����_�[�^�[�Q�b�g
			transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;			//�J�ڑO�̏��(Present)
			transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;		//�J�ڌ�̏��(�`��^�[�Q�b�g)
			transition.pResource = m_renderTarget[m_frameIdx];
			barrier.Transition = transition;
		}
		m_d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	// �����_�[�^�[�Q�b�g���Z�b�g
	SetRenderTarget();

	// �o�b�N�o�b�t�@���N���A
	ClearBackBuffer();

	// todo �`��R�}���h��S�Đς񂾌�Ɏ��s
	// �o�b�N�o�b�t�@�̕`�悪��������܂ő҂�
	{
		D3D12_RESOURCE_BARRIER barrier;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;		// �T�u���\�[�X�̑J��
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		{
			D3D12_RESOURCE_TRANSITION_BARRIER transition;
			transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
			transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;		// �J�ڑO�̏��(�`��^�[�Q�b�g)
			transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;				// �J�ڌ�̏��(Present)
			transition.pResource = m_renderTarget[m_frameIdx];
			barrier.Transition = transition;
		}
		m_d3d12GraphicsCommandList->ResourceBarrier(1, &barrier);
	}

	// �R�}���h���X�g���N���[�Y����
	m_d3d12GraphicsCommandList->Close();

	// �R�}���h���X�g�����s
	ID3D12CommandList* commandLists[] = { m_d3d12GraphicsCommandList };
	m_d3d12CommandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

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

/// @brief �R�}���h�L���[���쐬
void CGraphicsEngine::CreateD3d12CommandQueue()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandQueue d3d�f�o�C�X��null�ł�\n");
		return;
	}

	//�R�}���h�L���[���쐬
	D3D12_COMMAND_QUEUE_DESC desc = {};
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;					//�_�C���N�g�R�}���h�L���[
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;		//�ʏ�̗D��x
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;					//GPU�^�C���A�E�g���L��
	desc.NodeMask = 0;											//GPU��1�̂�

	HRESULT result;
	result = m_d3d12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_d3d12CommandQueue));
	if (result != S_OK || m_d3d12CommandQueue == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandQueue D3D12�R�}���h�L���[�̐����Ɏ��s\n");
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
	if (m_d3d12CommandQueue == nullptr)
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
    result = m_dxgiFactory->CreateSwapChain(m_d3d12CommandQueue, &swapChainDesc, &swapChain);
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

/// @brief ���[�g�V�O�l�`�����쐬
void CGraphicsEngine::CreateRootSignature()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateRootSignature d3d�f�o�C�X��null�ł�\n");
		return;
	}

	D3D12_DESCRIPTOR_RANGE descriptorRanges[1] = {};
	descriptorRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;		//�萔�o�b�t�@�Ƃ��Ďg�p
	descriptorRanges[0].NumDescriptors = 1;									//�f�B�X�N���v�^��1��
	descriptorRanges[0].BaseShaderRegister = 0;								//�V�F�[�_�[���W�X�^�̊J�n�C���f�b�N�X��0
	descriptorRanges[0].RegisterSpace = 0;									//���W�X�^�̈�͎g�p���Ȃ��̂�0
	descriptorRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_DESCRIPTOR_TABLE descriptorTable = {};
	descriptorTable.NumDescriptorRanges = 1;		//�f�B�X�N���v�^�����W�̐���1
	descriptorTable.pDescriptorRanges = descriptorRanges;

	D3D12_ROOT_PARAMETER rootParameters[1] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		//�萔�o�b�t�@�Ƃ��Ďg�p
	rootParameters[0].DescriptorTable = descriptorTable;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;		//���ׂẴV�F�[�_�[����A�N�Z�X�\

	//���[�g�V�O�l�`�����쐬
	D3D12_ROOT_SIGNATURE_DESC desc;
	desc.NumParameters = _countof(rootParameters);		//���[�g�p�����[�^��
	desc.pParameters = rootParameters;					//���[�g�p�����[�^�̃|�C���^
	desc.NumStaticSamplers = 0;							//�T���v���[��
	desc.pStaticSamplers = nullptr;						//�T���v���[�̃|�C���^
	desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;		//�I�v�V����

	HRESULT result;
	//���[�g�V�O�l�`���̃V���A���C�Y
	ID3DBlob* pSignature = nullptr;
	result = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &pSignature, nullptr);
	if (result != S_OK || pSignature == nullptr)
	{
		printf("CGraphicsEngine::CreateRootSignature ���[�g�V�O�l�`���̃V���A���C�Y�Ɏ��s\n");
		return;
	}

	//���[�g�V�O�l�`���̍쐬
	result = m_d3d12Device->CreateRootSignature(0, pSignature->GetBufferPointer(), pSignature->GetBufferSize(), IID_PPV_ARGS(&m_d3d12RootSignature));
	if (result != S_OK || m_d3d12RootSignature == nullptr)
	{
		printf("CGraphicsEngine::CreateRootSignature ���[�g�V�O�l�`���̐����Ɏ��s\n");
		return;
	}
}

/// @brief �R�}���h�A���P�[�^���쐬
void CGraphicsEngine::CreateD3d12CommandAllocator()
{
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandAllocator d3d�f�o�C�X��null�ł�\n");
		return;
	}

	HRESULT result;
	result = m_d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_d3d12CommandAllocator));
	if (result != S_OK || m_d3d12CommandAllocator == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandAllocator �R�}���h�A���P�[�^�̐����Ɏ��s\n");
		return;
	}
}

/// @brief �R�}���h���X�g���쐬
void CGraphicsEngine::CreateD3d12CommandList()
{
	if (m_d3d12CommandAllocator == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandList �R�}���h�A���P�[�^��null�ł�\n");
		return;
	}
	if (m_d3d12Device == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandList d3d�f�o�C�X��null�ł�\n");
		return;
	}

	HRESULT result;
	result = m_d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_d3d12CommandAllocator, nullptr, IID_PPV_ARGS(&m_d3d12GraphicsCommandList));
	if (result != S_OK || m_d3d12GraphicsCommandList == nullptr)
	{
		printf("CGraphicsEngine::CreateD3d12CommandList �R�}���h���X�g�̐����Ɏ��s\n");
		return;
	}

	//�R�}���h���X�g���N���[�Y���Ă���
	m_d3d12GraphicsCommandList->Close();
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
	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;
	m_d3d12GraphicsCommandList->OMSetRenderTargets(1, &currentFrameBufferRTVHandle, false, nullptr);
}

/// @brief �o�b�N�o�b�t�@�̃N���A
void CGraphicsEngine::ClearBackBuffer()
{
	if (m_dxgiSwapChain == nullptr)
	{
		printf("CGraphicsEngine::ClearBackBuffer m_dxgiSwapChain��null�ł��B\n");
		return;
	}

	const unsigned int rtvDescriptorSize = m_d3d12Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE currentFrameBufferRTVHandle = m_d3d12RtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	currentFrameBufferRTVHandle.ptr += rtvDescriptorSize * m_frameIdx;

	// �N���A
	m_d3d12GraphicsCommandList->ClearRenderTargetView(currentFrameBufferRTVHandle, RT_CLEAR_COLOR, 0, nullptr);
}

/// @brief �`��ҋ@
void CGraphicsEngine::WaitDraw()
{
	if (m_d3d12CommandQueue == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw m_d3d12CommandQueue��null�ł�\n");
	}

	if (m_d3d12Fence == nullptr)
	{
		printf("CGraphicsEngine::WaitDraw m_d3d12Fence��null�ł�\n");
	}

	HRESULT result;
	const unsigned long long nowFenceValue = m_fenceValue;
	result = m_d3d12CommandQueue->Signal(m_d3d12Fence, nowFenceValue);
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