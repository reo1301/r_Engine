#pragma once
#include "model_def.h"

MY_LIB_NAMESPACE_BEGIN

class CModelUtility
{
public:
	/// @brief ���f���̓��̓��C�A�E�g���擾
	/// @param  _outInputLayoutElementList ���̓��C�A�E�g
	/// @param  _outElementNum �v�f��
	/// @param _type �^�C�v
	/// @return ����
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
			printf("GetModelInputLayout _type�������B_type = %u\n", _type);
			isSuccess = false;
		}
		break;
		}

		return isSuccess;
	}

};

MY_LIB_NAMESPACE_END
