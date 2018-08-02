#ifndef __CBUFFERMGR_H__
#define __CBUFFERMGR_H__

#include "AgoraUtilc.h"
using namespace AgoraSdkCWrapperUtilc;
#include "stdint.h"

class CArrayBuffer{
public:
	CArrayBuffer(const int &nWidth,const int &nHeight);
	~CArrayBuffer();

	bool push(int nWidth, int nHeight, uint8_t* pBufferIn,const int &nBufferLen);
	bool pop(uint8_t* pBufferOut,int &nBufferLen,int &nWidth,int &nHeight);

	inline bool isFull();
	inline bool isEmpty();

private:

	const static int nKbufferCount = 2;

	uint8_t* m_pBuffer;
	int m_nWidth;
	int m_nHeight;
	int m_nLen;
	int m_nCapacity;
	int m_nByteWrited;
	
	int m_nWritePoint;
	int m_nReadPoint;

	CLock m_lock;
};

class CBufferMgr{
public:
	static CBufferMgr* getInstance();
	static void releaseInstance();
	static void threadProc(LPVOID lpParam);

	~CBufferMgr();

	bool pushYUVBuffer(UINT uid, uint8_t* pBufferIn,int nBuffeLenIn,int nWidth,int nHeight);
	bool popYUVBuffer(UINT uid,uint8_t* pBufferOut,int &nBufferLenOut,int &nWidth,int &nHeight);

protected:
	class CGraba{
	public:
		CGraba(){

		}
		~CGraba(){
			if (m_pBufferMgr){
				CAutoLock al(&m_Lock);
				delete m_pBufferMgr;
				m_pBufferMgr = nullptr;
			}
		}
	};

private:
	CBufferMgr();
	inline BOOL checkBufferMgr();

private:
	static CBufferMgr* m_pBufferMgr;
	static CLock m_Lock;
	static CGraba m_graba;

	//FrameCtrl
	BOOL m_bIsTeminated;

	//BufferMgr
	CLock* m_pMemLock;
	std::map<UINT, CArrayBuffer*> m_mapUserBufferMgr;
	std::map<UINT, LONGLONG > m_mapUserTimeStamp;
};

#endif