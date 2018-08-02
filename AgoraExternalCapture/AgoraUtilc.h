#ifndef __AGORAUTILC_H__
#define __AGORAUTILC_H__

#pragma warning(disable:4793)

#include <assert.h>
#include <atlstr.h>
#include <iostream>
#include <map>
#include <list>
#pragma warning(disable:4244)
#pragma warning(disable:4715)
#pragma  warning(disable:4800)
#pragma  warning(disable:4172)

//function
namespace AgoraSdkCWrapperUtilc
{
	extern std::string gStrInstance;
	struct CAgoraWrapperUtilc{
	public:

		static std::string getAbsoluteDir()
		{
			TCHAR path[MAX_PATH] = { 0 };
			GetModuleFileName(nullptr, path, MAX_PATH);

			std::string filePath = CStringA(path).GetBuffer();
			return filePath.substr(0, filePath.rfind("\\") + 1);
		}

		static std::string int2str(int nNum)
		{
			char str[MAX_PATH] = { 0 };
			_itoa_s(nNum, str, 10);
			return str;
		}

		static std::string float2str(float fValue)
		{
			char str[MAX_PATH] = { 0 };
			sprintf_s(str, "%f", fValue);
			return str;
		}

		static int str2int(const std::string &str)
		{
			return atoi(str.c_str());
		}

		static int str2long(const std::string &str)
		{
			return atoll(str.data());
		}

		static float str2float(const std::string &str)
		{
			return (float)atof(str.c_str());
		}

		static CString s2cs(const std::string &str)
		{
			return CString(str.c_str());
		}

		static std::string cs2s(const CString &str)
		{
			CString sTemp(str);
			return CStringA(sTemp.GetBuffer()).GetBuffer();
		}

		static std::string utf82gbk(const char *utf8)
		{
			std::string str;

			if (utf8 != NULL)
			{
				int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
				std::wstring strW;

				strW.resize(len);

				MultiByteToWideChar(CP_UTF8, 0, utf8, -1, (LPWSTR)strW.data(), len);

				len = WideCharToMultiByte(936, 0, strW.data(), len - 1, NULL, 0, NULL, NULL);

				str.resize(len);

				WideCharToMultiByte(936, 0, strW.data(), -1, (LPSTR)str.data(), len, NULL, NULL);
			}

			return str;
		}

		static std::string gbk2utf8(const char *gbk)
		{
			std::string str;

			if (gbk != NULL)
			{
				int len = MultiByteToWideChar(936, 0, gbk, -1, NULL, 0);
				std::wstring strW;

				strW.resize(len);

				MultiByteToWideChar(936, 0, gbk, -1, (LPWSTR)strW.data(), len);

				len = WideCharToMultiByte(CP_UTF8, 0, strW.data(), len - 1, NULL, 0, NULL, NULL);

				str.resize(len);

				WideCharToMultiByte(CP_UTF8, 0, strW.data(), -1, (LPSTR)str.data(), len, NULL, NULL);
			}

			return str;
		}

		static std::string gbk2utf8(const std::string &gbk)
		{
			return gbk2utf8(gbk.c_str());
		}

		static std::string utf82gbk(const std::string &utf8)
		{
			return utf82gbk(utf8.c_str());
		}

		static void PASCAL AgoraOutDebugStr(LPCTSTR lpFormat, ...){

			TCHAR szBuffer[1024] = { _T("\0") };
			va_list args;
			_tcsnccat(szBuffer, _T("\n"), 1);
			va_start(args, lpFormat);
			_vsnwprintf(szBuffer + _tcslen(szBuffer), sizeof(szBuffer) / sizeof(TCHAR) - _tcslen(szBuffer), lpFormat, args);
			va_end(args);

			OutputDebugString(szBuffer);
		}
	};

	class CFileIO
	{
	public:

		CFileIO::CFileIO() :
			fileHandle_(nullptr)
			, filelimitLine_(0)
			, isLog_(false)
		{
		}

		CFileIO::~CFileIO()
		{
			fileHandle_ = nullptr;
			filelimitLine_ = 0;
			isLog_ = false;
		}

		void CFileIO::openLog(const std::string &filepath, int fileFlage = OPEN_ALWAYS )
		{
			assert(nullptr == fileHandle_);
			fileHandle_ = CreateFile(
				CString(filepath.c_str()),
				GENERIC_READ | GENERIC_WRITE,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				nullptr,
				/*CREATE_NEW | OPEN_ALWAYS | TRUNCATE_EXISTING*/fileFlage,
				FILE_ATTRIBUTE_NORMAL,
				nullptr);
			if (INVALID_HANDLE_VALUE == fileHandle_)
			{
				fileHandle_ = nullptr;
				printf("Create LogFile Failed !!!!!!\n");
				int retCode = GetLastError();
				if (ERROR_ALREADY_EXISTS == retCode)
				{
					printf("LogFile Already Exists !!!!\n");
				}
			}

			isLog_ = true;
			SetFilePointer(fileHandle_, 0, nullptr, FILE_END);
		}

		void CFileIO::openMedia(const std::string &filepath, int fileFlage = CREATE_ALWAYS)
		{
			assert(nullptr == fileHandle_);
			fileHandle_ = CreateFile(CString(filepath.c_str()),
				GENERIC_WRITE | GENERIC_READ,
				FILE_SHARE_WRITE | FILE_SHARE_READ,
				nullptr,
				fileFlage,
				FILE_ATTRIBUTE_NORMAL,
				nullptr);
			if (INVALID_HANDLE_VALUE == fileHandle_)
			{
				fileHandle_ = nullptr;
				printf("Create MediaFile Failed !!!!!!\n");
				int retCode = GetLastError();
				if (ERROR_ALREADY_EXISTS == retCode)
				{
					printf("MediaFile Alreadly exists !!!!\n");
				}
			}
		}

		void CFileIO::openReadFile(const std::string &filePath)
		{
			openLog(filePath,OPEN_ALWAYS);
			FlushFileBuffers(fileHandle_);
			SetFilePointer(fileHandle_, 0, nullptr, FILE_BEGIN);
		}

		void CFileIO::close()
		{
			CloseHandle(fileHandle_);
			fileHandle_ = nullptr;
		}

		int CFileIO::write(char *bufferIn, int bufferLen)
		{
			if (fileHandle_)
			{
				DWORD writenLen = 0;
				int res = WriteFile(fileHandle_, bufferIn, (DWORD)bufferLen, &writenLen, nullptr);
				if (0 == res)
				{
					printf("write buffer failed..retCode: %d!!!!!\n", GetLastError());
				}
				return int(writenLen);
			}

			return 0;
		}

		int CFileIO::write(std::string bufferStr)
		{
			filelimitLine_++;
			if (isLog_)
			{
				bufferStr += "\r\n";
			}
			if (isLog_ && 100 == filelimitLine_)
			{//limit
				SetFilePointer(fileHandle_, 0, nullptr, FILE_BEGIN);
				SetEndOfFile(fileHandle_);
				filelimitLine_ = 0;
			}

			return write((char*)bufferStr.c_str(), bufferStr.length());
		}

		int CFileIO::read(char *bufferOut, int bufferLen)
		{
			DWORD readLen = 0;
			int res = ReadFile(fileHandle_, bufferOut, bufferLen, &readLen, nullptr);
			if (0 == res)
			{
				printf("read buffer from file failed!!!,retCode: %d\n", GetLastError());
				return int(readLen);
			}
			return readLen;
		}

		std::string CFileIO::readLine()
		{
			std::string strLine;
			char readTxt[2] = { 0 };
			DWORD readLen = 0; DWORD dwValue = 0;
			BOOL res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
			int nerr = GetLastError();
			std::string chKey = "\r";
			while (true)
			{
				if (res && 1 == readLen && chKey == readTxt)
				{
					res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
					chKey = "\n";
					if (res && 1 == readLen && chKey == readTxt)
					{
						break;
					}
					else
					{
						printf("read error ,cloud read '\r\n'\n");
						return "";
					}
				}
				else if (res && 1 == readLen &&  chKey != readTxt)
				{
					strLine += readTxt;
					//dwValue = SetFilePointer(fileHandle_, readLen, nullptr, FILE_CURRENT);
					readLen = 0; ZeroMemory(readTxt, 2); dwValue = 0;
					res = ReadFile(fileHandle_, readTxt, 1, &readLen, nullptr);
				}
				if (res && 0 == readLen)
				{
					break;
				}
			}
			return strLine;
		}

		protected:

		bool CFileIO::generatorFile(const std::string &path)
		{
			HANDLE fileHandle = CreateFile(CString(path.c_str()),
				GENERIC_READ | GENERIC_READ,
				FILE_SHARE_READ | FILE_SHARE_WRITE,
				nullptr,
				OPEN_ALWAYS,
				FILE_ATTRIBUTE_NORMAL,
				nullptr);
			if (INVALID_HANDLE_VALUE == fileHandle)
			{
				return false;
			}
			return true;
		}

	private:
		HANDLE fileHandle_;
		int filelimitLine_;
		bool isLog_;
	};

	class CLock
	{
	public:
		CLock(){
			m_pCriticalSection = new CRITICAL_SECTION;
			assert(m_pCriticalSection);
			InitializeCriticalSection(m_pCriticalSection);
		}
		~CLock(){
			if (m_pCriticalSection){
				DeleteCriticalSection(m_pCriticalSection);
				delete m_pCriticalSection;
				m_pCriticalSection = nullptr;
			}
		}

		void lock(){
			EnterCriticalSection(m_pCriticalSection);
		}

		void unlock(){
			LeaveCriticalSection(m_pCriticalSection);
		}

	private:
		LPCRITICAL_SECTION m_pCriticalSection;
	};

	class CAutoLock
	{
	public:
		CAutoLock(CLock *pLock) :m_pLock(pLock){
			assert(m_pLock);
			m_pLock->lock();
		}
		~CAutoLock(){
			m_pLock->unlock();
		}

	private:
		CLock *m_pLock;
	};

	class CHighResoluteFrameCtrl
	{//Get frame rate in real time. More accurate.
	public:
		CHighResoluteFrameCtrl(){
			tickInterval_ = 0;
			frameCount_ = 0;
			counterInterval_.QuadPart = 0;
			lfrequency_.QuadPart = 0;
			counterBegin_.QuadPart = 0;
			counterPiror_.QuadPart = 0;
			counterNext_.QuadPart = 0;
		}

		~CHighResoluteFrameCtrl(){
			tickInterval_ = 0;
			frameCount_ = 0;
			counterInterval_.QuadPart = 0;
			lfrequency_.QuadPart = 0;
			counterBegin_.QuadPart = 0;
			counterPiror_.QuadPart = 0;
			counterNext_.QuadPart = 0;
		}

		void setInterval(unsigned int interval){

			assert(interval && 0 < interval);
			if (tickInterval_ == interval)
			{
				return;
			}
			if (tickInterval_)
			{
				tickInterval_ = 0;
				frameCount_ = 0;
				counterInterval_.QuadPart = 0;
				lfrequency_.QuadPart = 0;
				counterBegin_.QuadPart = 0;
				counterPiror_.QuadPart = 0;
				counterNext_.QuadPart = 0;
			}
			tickInterval_ = interval;

			QueryPerformanceFrequency(&lfrequency_);
			QueryPerformanceCounter(&counterBegin_);
			counterPiror_ = counterBegin_;
			counterInterval_.QuadPart = lfrequency_.QuadPart * tickInterval_ / 1000;
		}

		float rate(){
			LARGE_INTEGER counterCurrent;
			QueryPerformanceCounter(&counterCurrent);
			return float(frameCount_ / ((counterCurrent.QuadPart - counterBegin_.QuadPart) / lfrequency_.QuadPart));
		}

		int wait(){

			if (!counterInterval_.QuadPart)
			{
				printf("please setInterval first!!!!!!\n");
				return false;
			}

			frameCount_++;
			LARGE_INTEGER counterCurrent;
			QueryPerformanceCounter(&counterCurrent);
			LONGLONG counterEscape = counterInterval_.QuadPart * frameCount_ - (counterCurrent.QuadPart - counterBegin_.QuadPart);
			LONGLONG res = counterEscape;

			//TO DO
			while (0 < counterEscape)
			{
				Sleep(1);
				QueryPerformanceCounter(&counterCurrent);
				counterEscape = counterInterval_.QuadPart * frameCount_ - (counterCurrent.QuadPart - counterBegin_.QuadPart);
			}

			return (int)res;
		}

		int getFrameCount(){

			return frameCount_;
		}

	private:
		unsigned int tickInterval_;
		int frameCount_;
		LARGE_INTEGER lfrequency_;
		LARGE_INTEGER counterInterval_;
		LARGE_INTEGER counterBegin_;
		LARGE_INTEGER counterPiror_;
		LARGE_INTEGER counterNext_;
	};

	extern AgoraSdkCWrapperUtilc::CFileIO gFileSDK;

	static void  PASCAL AgoraWriteLog(LPSTR lpFormat, ...){

		char szBuffer[1024] = { '\0' };
		va_list args;
		va_start(args, lpFormat);

		SYSTEMTIME st;
		GetLocalTime(&st);
		sprintf_s(szBuffer, "%04d%02d%02d:%02d%02d%02d [%s] ", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond, gStrInstance.data());

		_vsnprintf(szBuffer + strlen(szBuffer), sizeof(szBuffer) / sizeof(char) - strlen(szBuffer), lpFormat, args);
		va_end(args);

#if 0
		gFileSDK.write(szBuffer);
#endif
	}

	static long long OS_GetTickCount()
	{
#if defined(WIN32)
		return GetTickCount();
#else
		int64_t t0;
		struct timeval tv;
		gettimeofday(&tv, NULL);
		t0 = ((int64_t)tv.tv_sec * 1000) + (tv.tv_usec / 1000);
		return t0;
#endif
	}

}



#endif