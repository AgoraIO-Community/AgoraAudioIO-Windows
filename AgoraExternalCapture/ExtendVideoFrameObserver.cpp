#include "stdafx.h"
#include "ExtendVideoFrameObserver.h"
#include <iostream>

#include "../LibYUV/include/libyuv.h"
#ifdef _M_IX86
#pragma comment(lib,"../LibYUV/lib/X64/libyuv.lib")
#elif defined _M_X64
#pragma comment(lib,"../LibYUV/lib/X86/libyuv.lib")
#endif

#include "CBufferMgr.h"

CExtendVideoFrameObserver::CExtendVideoFrameObserver()
{
	m_lpImageBuffer = new BYTE[0x800000];
	m_lpImageBufferRemote = new BYTE[0x800000];
	m_lpImageBufferRemoteTemp = new BYTE[0x800000];
}


CExtendVideoFrameObserver::~CExtendVideoFrameObserver()
{
	delete[] m_lpImageBuffer;
	delete[] m_lpImageBufferRemote;
	delete[] m_lpImageBufferRemoteTemp;
}

bool CExtendVideoFrameObserver::onCaptureVideoFrame(VideoFrame& videoFrame)
{
	int nBufferSize = videoFrame.height * videoFrame.width * 3 /2;
	
	FILE* pFileRecord = fopen("..\\Extendaudio.txt", "ab+");
	if (pFileRecord) {

		std::string str = __FUNCTION__ + std::string("\n");
		fwrite(str.data(), 1, str.length(), pFileRecord);
		fclose(pFileRecord);
		pFileRecord = nullptr;
	}

	int nWidth = 0; int nHeight = 0;
// 	BOOL bSuccess = CVideoPackageQueue::GetInstance()->PopVideoPackage(m_lpImageBuffer, &nBufferSize);
// 	if (!bSuccess)
// 		return false;

	if (!CBufferMgr::getInstance()->popYUVBuffer(0, m_lpImageBuffer, nBufferSize, nWidth, nHeight))
		return false;

	m_lpY = m_lpImageBuffer;
	m_lpU = m_lpImageBuffer + videoFrame.height*videoFrame.width;
	m_lpV = m_lpImageBuffer + 5 * videoFrame.height*videoFrame.width / 4;

	memcpy_s(videoFrame.yBuffer, videoFrame.height*videoFrame.width, m_lpY, videoFrame.height*videoFrame.width);
	videoFrame.yStride = videoFrame.width;
	
	memcpy_s(videoFrame.uBuffer, videoFrame.height*videoFrame.width / 4, m_lpU, videoFrame.height*videoFrame.width / 4);
	videoFrame.uStride = videoFrame.width/2;

	memcpy_s(videoFrame.vBuffer, videoFrame.height*videoFrame.width / 4, m_lpV, videoFrame.height*videoFrame.width / 4);
	videoFrame.vStride = videoFrame.width/2;

	videoFrame.type = FRAME_TYPE_YUV420;
	videoFrame.rotation = 0;

	return true;
}

bool CExtendVideoFrameObserver::onRenderVideoFrame(unsigned int uid, VideoFrame& videoFrame)
{
	int nWidth = videoFrame.width;
	int nHeight = videoFrame.height;
	int nYStride = videoFrame.yStride;
	int nUStride = videoFrame.uStride;
	int nVStride = videoFrame.vStride;

	memcpy_s(m_lpImageBufferRemote, nYStride * nHeight, videoFrame.yBuffer, nYStride*nHeight);
	memcpy_s(m_lpImageBufferRemote + nYStride * nHeight, nUStride * nHeight / 2, videoFrame.uBuffer, nUStride * nHeight / 2);
	memcpy_s(m_lpImageBufferRemote + nYStride * nHeight + nUStride * nHeight / 2, nVStride * nHeight / 2, videoFrame.vBuffer, nVStride * nHeight / 2);
	int nBufferLen = nYStride * nHeight + nUStride *nHeight / 2 + nVStride *nHeight / 2;

	libyuv::ConvertToI420((uint8_t*)m_lpImageBufferRemote, nBufferLen,
		m_lpImageBufferRemoteTemp, nWidth,
		m_lpImageBufferRemoteTemp + nWidth * nHeight, nWidth / 2,
		m_lpImageBufferRemoteTemp + nWidth * nHeight * 5 / 4, nWidth / 2,
		0, 0, nYStride, nHeight,
		nWidth, nHeight, libyuv::kRotate180, libyuv::FOURCC_I420);
	nBufferLen = nWidth * nHeight * 3 / 2;

#if 0
	FILE *pFile = fopen("../remoteyuv.yuv", "ab+");
	if (pFile) {
		fwrite(m_lpImageBufferRemoteTemp, 1, nBufferLen, pFile);
		fclose(pFile);
		pFile = nullptr;
	}
#endif

	CBufferMgr::getInstance()->pushYUVBuffer(uid, (uint8_t*)m_lpImageBufferRemoteTemp, nBufferLen, nWidth, nHeight);

	return true;
}
