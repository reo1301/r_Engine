#pragma once

//コピー禁止クラス
//コピーを禁止したいクラスは、このクラスをprivateで継承して使用してください
class Noncopyable
{
protected:
	Noncopyable() {}

private:
	void operator =(const Noncopyable& /*src*/) {}
	Noncopyable(const Noncopyable& /*src*/) {}
};