#pragma once

//�R�s�[�֎~�N���X
//�R�s�[���֎~�������N���X�́A���̃N���X��private�Ōp�����Ďg�p���Ă�������
class Noncopyable
{
protected:
	Noncopyable() {}

private:
	void operator =(const Noncopyable& /*src*/) {}
	Noncopyable(const Noncopyable& /*src*/) {}
};