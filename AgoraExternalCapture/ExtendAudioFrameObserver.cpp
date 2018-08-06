#include "stdafx.h"
#include "ExtendAudioFrameObserver.h"
#include <iostream>


CExtendAudioFrameObserver::CExtendAudioFrameObserver()
{
}


CExtendAudioFrameObserver::~CExtendAudioFrameObserver()
{
}

bool CExtendAudioFrameObserver::onRecordAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	CAudioCapturePackageQueue::GetInstance()->PopAudioPackage(audioFrame.buffer, &nSize);

	FILE* pFileRecord = fopen("..\\Extendaudio.txt","ab+");
	if (pFileRecord) {

		std::string str = __FUNCTION__ + std::string("\n");
		fwrite(str.data(), 1, str.length(), pFileRecord);
		fclose(pFileRecord);
		pFileRecord = nullptr;
	}
	
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	CAudioPlayPackageQueue::GetInstance()->PushAudioPackage(audioFrame.buffer, nSize);

	FILE* pFileRecord = fopen("..\\Extendaudio.txt", "ab+");
	if (pFileRecord) {

		std::string str = __FUNCTION__ + std::string("\n");
		fwrite(str.data(), 1, str.length(), pFileRecord);
		fclose(pFileRecord);
		pFileRecord = nullptr;
	}

	return true;
}

bool CExtendAudioFrameObserver::onMixedAudioFrame(AudioFrame& audioFrame)
{
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrameBeforeMixing(unsigned int uid, AudioFrame& audioFrame)
{
	return true;
}
