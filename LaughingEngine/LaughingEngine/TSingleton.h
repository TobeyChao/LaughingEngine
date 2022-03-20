#pragma once
#include <cassert>
template <typename T>
class TSingleton
{
public:
	TSingleton()
	{
		assert(!mSingleton);
		size_t offset = (size_t)(T*)1 - (size_t)(TSingleton<T>*)(T*)1;
		mSingleton = (T*)((size_t)this + offset); // �ڴ��ַƫ����ȷ

		//mSingleton = (T*)this; // �ڴ��ַ���������ͬ

		//mSingleton = reinterpret_cast<T*>(this); // �ڴ��ַ����
	}

	virtual ~TSingleton()
	{
		assert(mSingleton);
		mSingleton = nullptr;
	}

	static T& GetInstance()
	{
		assert(mSingleton);
		return (*mSingleton);
	}

	static T* GetInstancePtr()
	{
		return mSingleton;
	}
private:
	TSingleton(const TSingleton<T>&) = delete;
	TSingleton& operator=(const TSingleton<T>&) = delete;

protected:
	static T* mSingleton;
};

template<typename T> T* TSingleton<T>::mSingleton = new T;