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
	CAudioCapturePackageQueue::GetInstance()->PushAudioPackage(audioFrame.buffer, nSize);

	FILE* pFileRecord = fopen("..\\Extendaudio.pcm","ab+");
	if (pFileRecord) {

		fwrite(audioFrame.buffer, 1, nSize, pFileRecord);
		fclose(pFileRecord);
		pFileRecord = nullptr;
	}
	
	return true;
}

bool CExtendAudioFrameObserver::onPlaybackAudioFrame(AudioFrame& audioFrame)
{
	SIZE_T nSize = audioFrame.channels*audioFrame.samples * 2;
	bool res = CAudioCapturePackageQueue::GetInstance()->PopAudioPackage(audioFrame.buffer, &nSize);

	FILE* pFileRecord = fopen("..\\Extendaudio1.pcm", "ab+");
	if (pFileRecord && res) {
		fwrite(audioFrame.buffer, 1, nSize, pFileRecord);
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
