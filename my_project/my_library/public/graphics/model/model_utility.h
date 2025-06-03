#pragma once
#include "model_def.h"

MY_LIB_NAMESPACE_BEGIN

class CModelUtility
{
public:
	/// @brief モデルの入力レイアウトを取得
	/// @param  _outInputLayoutElementList 入力レイアウト
	/// @param  _outElementNum 要素数
	/// @param _type タイプ
	/// @return 成否
	static bool GetModelInputLayout(const MODEL_INPUT_LAYOUT_ELEMENT*& _outInputLayoutElementList, unsigned int& _outElementNum, MODEL_INPUT_LAYOUT_TYPE _type)
	{
		bool isSuccess = true;
		switch (_type)
		{
		case MODEL_INPUT_LAYOUT_TYPE_SIMPLE:
		{
			_outInputLayoutElementList = MODEL_INPUT_ELEMENT_SINPLE;
			_outElementNum = _countof(MODEL_INPUT_ELEMENT_SINPLE);
		}
		break;

		default:
		{
			printf("GetModelInputLayout _typeが無効。_type = %u\n", _type);
			isSuccess = false;
		}
		break;
		}

		return isSuccess;
	}

};

MY_LIB_NAMESPACE_END
