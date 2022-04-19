#pragma once
#include "ColorBuffer.h"
#include "DepthBuffer.h"

namespace Graphics
{
	extern DXGI_FORMAT g_DefaultHdrColorFormat;
	extern DXGI_FORMAT g_DefaultDepthStencilFormat;

	/// <summary>
	/// ��ȾĿ��
	/// </summary>
	extern ColorBuffer g_SceneColorBuffer;  // R11G11B10_FLOAT

	/// <summary>
	/// ���ģ�建��
	/// </summary>
	extern DepthBuffer g_SceneDepthBuffer;  // 

	/// <summary>
	/// ��ʼ����Ⱦ����
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void InitializeRenderingBuffers(uint32_t width, uint32_t height);

	/// <summary>
	/// �ı仺������С
	/// </summary>
	/// <param name="width"></param>
	/// <param name="height"></param>
	void ResizeDisplayDependentBuffers(uint32_t width, uint32_t height);

	/// <summary>
	/// ���ٻ�����
	/// </summary>
	void DestroyRenderingBuffers();
};