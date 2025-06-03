#include "application.h"

//�C���N���[�h
//�O���t�B�b�N�n
#include "graphics/graphics_engine.h"
#include "graphics/model/model_render.h"

GAME_NAMESPACE_BEGIN

//������
CApplication* CApplication::s_instance = nullptr;

/// @brief �R���X�g���N�^
CApplication::CApplication()
{

}

/// @brief �f�X�g���N�^
CApplication::~CApplication()
{

}

/// @brief ����������
/// @param _windowWidth �E�B���h�E�̉���
/// @param _windowHeight �E�B���h�E�̏c��
void CApplication::Initialize(unsigned int _windowWidth, unsigned int _windowHeight)
{
	//�O���t�B�b�N�X�G���W��
	CGraphicsEngine::CreateInstance();
	CGraphicsEngine::GetInstance().Initialize(_windowWidth, _windowHeight);

	// ���f�������_�[
	CModelRender::CreateInstance();
	CModelRender::GetInstance().Initialize();

	// ���f���̐���
	{
		CShader::ShaderInitData vsInitData;
		vsInitData.filePath = "resource/shader/vertex_shader/simple_vertex_shader.hlsl";
		vsInitData.entryPoint = "main";
		vsInitData.type = CShader::ShaderType_Vertex;

		CShader::ShaderInitData psInitData;
		psInitData.filePath = "resource/shader/pixel_shader/simple_pixel_shader.hlsl";
		psInitData.entryPoint = "main";
		psInitData.type = CShader::ShaderType_Pixel;
		
		// ���_�f�[�^
		float vertexData[] =
		{
			// X   Y      Z     R     G     B     A
			0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		};

		// ���f���������f�[�^
		CModel::ModelInitData modelInitData;
		modelInitData.vertexData.vertexDataPtr = vertexData;
		modelInitData.vertexData.vertexDataSize = sizeof(vertexData);
		modelInitData.vertexData.vertexDataStride = sizeof(float) * 7;
		modelInitData.vsInitData = vsInitData;
		modelInitData.psInitData = psInitData;
		modelInitData.inputLayoutType = MODEL_INPUT_LAYOUT_TYPE_SIMPLE;
		CModelRender::GetInstance().CreateModel(modelInitData);
	}
}

/// @brief �I������
void CApplication::Finalize()
{
	// ���f�������_�[
	CModelRender::GetInstance().Finalize();
	CModelRender::Delete();

	//�O���t�B�b�N�X�G���W��
	CGraphicsEngine::GetInstance().Finalize();
	CGraphicsEngine::Delete();
}

/// @brief �O�X�V
void CApplication::PreUpdate()
{
	CGraphicsEngine::GetInstance().PreUpdate();
}

/// @brief �X�V����
void CApplication::Update()
{
	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	graphicsEngine.Update();

	CModelRender& modelRender = CModelRender::GetInstance();
	modelRender.Update();
}

/// @brief �`��
void CApplication::Draw()
{
	CGraphicsEngine::GetInstance().Draw();
	CModelRender::GetInstance().Draw();
}

/// @brief ��X�V
void CApplication::PostUpdate()
{
	CGraphicsEngine::GetInstance().PostUpdate();
}

/// @brief �V���O���g���C�̃C���X�^���X�̐���
void CApplication::CreateInstance()
{
	//�C���X�^���X���܂��Ȃ��Ƃ�����
	if (!s_instance)
	{
		s_instance = new CApplication();
	}
}

/// @brief CApplication�̃C���X�^���X�擾
/// @return CApplication�̃C���X�^���X�̎Q��
CApplication& CApplication::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief �V���O���g���̃C���X�^���X�폜
void CApplication::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

GAME_NAMESPACE_END