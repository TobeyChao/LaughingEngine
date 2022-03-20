#pragma once
#include <cassert>
template <typename T>
class TSingleton
{
public:
	TSingleton()
	{
		assert(!mSingleton);
		mSingleton = (T*)this;
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