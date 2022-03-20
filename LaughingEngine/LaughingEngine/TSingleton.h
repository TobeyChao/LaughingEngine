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
		mSingleton = (T*)((size_t)this + offset); // 内存地址偏移正确

		//mSingleton = (T*)this; // 内存地址和上面的相同

		//mSingleton = reinterpret_cast<T*>(this); // 内存地址不对
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