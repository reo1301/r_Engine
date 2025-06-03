#include "application.h"

//インクルード
//グラフィック系
#include "graphics/graphics_engine.h"
#include "graphics/model/model_render.h"

GAME_NAMESPACE_BEGIN

//初期化
CApplication* CApplication::s_instance = nullptr;

/// @brief コンストラクタ
CApplication::CApplication()
{

}

/// @brief デストラクタ
CApplication::~CApplication()
{

}

/// @brief 初期化処理
/// @param _windowWidth ウィンドウの横幅
/// @param _windowHeight ウィンドウの縦幅
void CApplication::Initialize(unsigned int _windowWidth, unsigned int _windowHeight)
{
	//グラフィックスエンジン
	CGraphicsEngine::CreateInstance();
	CGraphicsEngine::GetInstance().Initialize(_windowWidth, _windowHeight);

	// モデルレンダー
	CModelRender::CreateInstance();
	CModelRender::GetInstance().Initialize();

	// モデルの生成
	{
		CShader::ShaderInitData vsInitData;
		vsInitData.filePath = "resource/shader/vertex_shader/simple_vertex_shader.hlsl";
		vsInitData.entryPoint = "main";
		vsInitData.type = CShader::ShaderType_Vertex;

		CShader::ShaderInitData psInitData;
		psInitData.filePath = "resource/shader/pixel_shader/simple_pixel_shader.hlsl";
		psInitData.entryPoint = "main";
		psInitData.type = CShader::ShaderType_Pixel;
		
		// 頂点データ
		float vertexData[] =
		{
			// X   Y      Z     R     G     B     A
			0.0f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f,  -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		};

		// モデル初期化データ
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

/// @brief 終了処理
void CApplication::Finalize()
{
	// モデルレンダー
	CModelRender::GetInstance().Finalize();
	CModelRender::Delete();

	//グラフィックスエンジン
	CGraphicsEngine::GetInstance().Finalize();
	CGraphicsEngine::Delete();
}

/// @brief 前更新
void CApplication::PreUpdate()
{
	CGraphicsEngine::GetInstance().PreUpdate();
}

/// @brief 更新処理
void CApplication::Update()
{
	CGraphicsEngine& graphicsEngine = CGraphicsEngine::GetInstance();
	graphicsEngine.Update();

	CModelRender& modelRender = CModelRender::GetInstance();
	modelRender.Update();
}

/// @brief 描画
void CApplication::Draw()
{
	CGraphicsEngine::GetInstance().Draw();
	CModelRender::GetInstance().Draw();
}

/// @brief 後更新
void CApplication::PostUpdate()
{
	CGraphicsEngine::GetInstance().PostUpdate();
}

/// @brief シングルトンイのインスタンスの生成
void CApplication::CreateInstance()
{
	//インスタンスがまだないとき生成
	if (!s_instance)
	{
		s_instance = new CApplication();
	}
}

/// @brief CApplicationのインスタンス取得
/// @return CApplicationのインスタンスの参照
CApplication& CApplication::GetInstance()
{
	if (!s_instance)
	{
		throw;
	}
	return *s_instance;
}

/// @brief シングルトンのインスタンス削除
void CApplication::Delete()
{
	if (s_instance)
	{
		delete s_instance;
		s_instance = nullptr;
	}
}

GAME_NAMESPACE_END