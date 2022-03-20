#pragma once

#include "ColorBuffer.h"

namespace Graphics
{
	/// <summary>
	/// 渲染目标
	/// </summary>
	extern ColorBuffer g_SceneColorBuffer;  // R11G11B10_FLOAT

	/// <summary>
	/// 初始化渲染缓冲
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void InitializeRenderingBuffers(uint32_t width, uint32_t height);

	/// <summary>
	/// 改变缓冲区大小
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void ResizeDisplayDependentBuffers(uint32_t width, uint32_t height);

	/// <summary>
	/// 销毁缓冲区
	/// </summary>
	void DestroyRenderingBuffers();
};