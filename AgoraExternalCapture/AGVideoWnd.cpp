// AGVideoWnd.cpp : 实现文件
//

#include "stdafx.h"
#include "AgoraExternalCapture.h"
#include "AGVideoWnd.h"
#include "CBufferMgr.h"
#include "YUVTrans.h"

IMPLEMENT_DYNAMIC(CAGInfoWnd, CWnd)

CAGInfoWnd::CAGInfoWnd()
: m_bShowTip(TRUE)
, m_nWidth(0)
, m_nHeight(0)
, m_nFps(0)
, m_nBitrate(0)
{
	m_brBack.CreateSolidBrush(RGB(0x00, 0xA0, 0xE9));
}

CAGInfoWnd::~CAGInfoWnd()
{
	m_brBack.DeleteObject();
}


BEGIN_MESSAGE_MAP(CAGInfoWnd, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void CAGInfoWnd::ShowTips(BOOL bShow)
{
	m_bShowTip = bShow;

	if (bShow)
		ShowWindow(SW_SHOW);
	else 
		ShowWindow(SW_HIDE);

	Invalidate(FALSE);
}

void CAGInfoWnd::SetVideoResolution(int nWidth, int nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::SetFrameRateInfo(int nFPS)
{
	m_nFps = nFPS;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::SetBitrateInfo(int nBitrate)
{
	m_nBitrate = nBitrate;

	if (m_bShowTip) {
		Invalidate(TRUE);
		UpdateWindow();
	}
}

void CAGInfoWnd::OnPaint()
{
	CPaintDC dc(this);
	CRect	rcClient;
	CString strTip;

	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(RGB(0xFF, 0xFF, 0xFF));
	
	if (m_bShowTip) {
		// 640x480,15fps,400k
		GetClientRect(&rcClient);
		rcClient.top += 4;
		strTip.Format(_T("%dx%d, %dfps, %dK"), m_nWidth, m_nHeight, m_nFps, m_nBitrate);
		dc.DrawText(strTip, &rcClient, DT_VCENTER | DT_CENTER);
	}
}

BOOL CAGInfoWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect rcClient;

	GetClientRect(&rcClient);
	pDC->FillRect(&rcClient, &m_brBack);
	
	return TRUE;
}

// CAGVideoWnd

IMPLEMENT_DYNAMIC(CAGVideoWnd, CWnd)

CAGVideoWnd::CAGVideoWnd()
: m_nUID(0)
, m_crBackColor(RGB(0x58, 0x58, 0x58))
, m_bShowVideoInfo(FALSE)
, m_bBigShow(FALSE)
, m_bBackground(FALSE)
{
	m_hRenderGDIEvent = ::CreateEvent(NULL, NULL, FALSE, NULL);
}

CAGVideoWnd::~CAGVideoWnd()
{
	m_imgBackGround.DeleteImageList();
	::CloseHandle(m_hRenderGDIEvent);
}


BEGIN_MESSAGE_MAP(CAGVideoWnd, CWnd)
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CAGVideoWnd 消息处理程序
BOOL CAGVideoWnd::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CRect		rcClient;
	CPoint		ptDraw;
	IMAGEINFO	imgInfo;

	GetClientRect(&rcClient);

	pDC->FillSolidRect(&rcClient, m_crBackColor);
	if (!m_imgBackGround.GetImageInfo(0, &imgInfo))
		return TRUE;

	ptDraw.SetPoint((rcClient.Width() - imgInfo.rcImage.right) / 2, (rcClient.Height() - imgInfo.rcImage.bottom) / 2);
	if (ptDraw.x < 0)
		ptDraw.x = 0;
	if (ptDraw.y <= 0)
		ptDraw.y = 0;

	m_imgBackGround.Draw(pDC, 0, ptDraw, ILD_NORMAL);
	return TRUE;
}

void CAGVideoWnd::SetUID(UINT nUID)
{
	m_nUID = nUID;

	if (m_nUID == 0)
		m_wndInfo.ShowWindow(SW_HIDE);
	else
		m_wndInfo.ShowWindow(SW_SHOW);
}

UINT CAGVideoWnd::GetUID()
{
	return m_nUID;
}

BOOL CAGVideoWnd::IsWndFree()
{
	return m_nUID == 0 ? TRUE : FALSE;
}

BOOL CAGVideoWnd::SetBackImage(UINT nID, UINT nWidth, UINT nHeight, COLORREF crMask)
{
	CBitmap bmBackImage;

	if (!bmBackImage.LoadBitmap(nID))
		return FALSE;

	m_imgBackGround.DeleteImageList();

	m_imgBackGround.Create(nWidth, nHeight, ILC_COLOR24 | ILC_MASK, 1, 1);
	m_imgBackGround.Add(&bmBackImage, crMask);
	bmBackImage.DeleteObject();

	Invalidate(TRUE);

	return TRUE;
}

void CAGVideoWnd::ShowBackground(BOOL bBackground)
{
    m_bBackground = bBackground;

    Invalidate(TRUE);
}

void CAGVideoWnd::SetFaceColor(COLORREF crBackColor)
{
	m_crBackColor = crBackColor;

	Invalidate(TRUE);
}

void CAGVideoWnd::SetVideoResolution(UINT nWidth, UINT nHeight)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;

	m_wndInfo.SetVideoResolution(nWidth, nHeight);
}

void CAGVideoWnd::GetVideoResolution(UINT *nWidth, UINT *nHeight)
{
	*nWidth = m_nWidth;
	*nHeight = m_nHeight;
}

void CAGVideoWnd::SetBitrateInfo(int nReceivedBitrate)
{
	m_nBitRate = nReceivedBitrate;
	m_wndInfo.SetBitrateInfo(nReceivedBitrate);
}

void CAGVideoWnd::SetFrameRateInfo(int nReceiveFrameRate)
{
	m_nFrameRate = nReceiveFrameRate;
	m_wndInfo.SetFrameRateInfo(nReceiveFrameRate);
}

void CAGVideoWnd::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWBIG, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDown(nFlags, point);
}


void CAGVideoWnd::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnRButtonDown(nFlags, point);
}


int CAGVideoWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	m_wndInfo.Create(NULL, NULL, WS_CHILD | WS_VISIBLE, CRect(0, 0, 192, 28), this, IDC_STATIC);
	
	m_renderVideoThreadParam.hExitEvent = m_hRenderGDIEvent;

	return 0;
}

void CAGVideoWnd::StartGDIRendr(BOOL bFlage /*= FALSE*/)
{
	if (bFlage)
		AfxBeginThread(RenderThreadGDI, this);
}

void CAGVideoWnd::ShowVideoInfo(BOOL bShow)
{
	m_bShowVideoInfo = bShow;

	m_wndInfo.ShowTips(bShow);
	Invalidate(TRUE);

/*	if (!bShow) {
		CRect rcTip;
		m_wndInfo.GetWindowRect(&rcTip);
		
	}
	*/

}

void CAGVideoWnd::SetBigShowFlag(BOOL bBigShow)
{
	CRect	rcClient;

	m_bBigShow = bBigShow;
	GetClientRect(&rcClient);

	int x = (rcClient.Width()- 192) / 2;
	int y = rcClient.Height() - 24;
	
	if (m_wndInfo.GetSafeHwnd() != NULL) {
		if (m_bBigShow)
			y -= 4;

		m_wndInfo.MoveWindow(x, y, 192, 24);
	}
};


void CAGVideoWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int x = (cx - 192) / 2;
	int y = cy - 24;
	// TODO:  在此处添加消息处理程序代码
	if (m_wndInfo.GetSafeHwnd() != NULL) {
		if (m_bBigShow)
			y -= 4;

		m_wndInfo.MoveWindow(x, y, 192, 24);
	}
}


void CAGVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	::SendMessage(GetParent()->GetSafeHwnd(), WM_SHOWMODECHANGED, (WPARAM)this, (LPARAM)m_nUID);

	CWnd::OnLButtonDblClk(nFlags, point);
}


void CAGVideoWnd::OnPaint()
{
    CPaintDC dc(this); // device context for painting
    // TODO:  在此处添加消息处理程序代码
    // 不为绘图消息调用 CWnd::OnPaint()

    if (m_bBackground) {
        CRect		rcClient;
        CPoint		ptDraw;
        IMAGEINFO	imgInfo;

        GetClientRect(&rcClient);

        dc.FillSolidRect(&rcClient, m_crBackColor);
        if (!m_imgBackGround.GetImageInfo(0, &imgInfo))
            return;

        ptDraw.SetPoint((rcClient.Width() - imgInfo.rcImage.right) / 2, (rcClient.Height() - imgInfo.rcImage.bottom) / 2);
        if (ptDraw.x < 0)
            ptDraw.x = 0;
        if (ptDraw.y <= 0)
            ptDraw.y = 0;

        m_imgBackGround.Draw(&dc, 0, ptDraw, ILD_NORMAL);
    }
    else
        return CWnd::OnPaint();
}

UINT CAGVideoWnd::RenderThreadGDI(LPVOID lParam)
{
	//LPRENDERVIDEO_THREAD_PARAM lpRenderVideoThreadParam = (LPRENDERVIDEO_THREAD_PARAM)lParam;
	CAGVideoWnd* pVideoWnd = (CAGVideoWnd*)lParam;

	CBufferMgr* lpBufferMgr = CBufferMgr::getInstance();
	LPBYTE lpVideoData = new BYTE[0x800000];
	int nWidth = 0;
	int nHeight = 0;
	LPBYTE lpVideoImage24 = nullptr;

	do{
		Sleep(10);
		if (pVideoWnd->m_nUID == 0 || 0 == pVideoWnd->m_nWidth || 0 == pVideoWnd->m_nHeight)
			continue;
		else if ( nullptr == lpVideoImage24)
			lpVideoImage24 = new BYTE[pVideoWnd->m_nWidth * pVideoWnd->m_nHeight * 3];

		if (::WaitForSingleObject(pVideoWnd->m_hRenderGDIEvent, 0) == WAIT_OBJECT_0)
			break;
	
		int nVideoDataLen = pVideoWnd->m_nWidth * pVideoWnd->m_nHeight  * 3 / 2;
		if (!lpBufferMgr->popYUVBuffer(pVideoWnd->m_nUID,lpVideoData,nVideoDataLen,nWidth,nHeight))
			continue;
	
		int nRGB24Len = nWidth * nHeight * 3;
		CONVERT_YUV420PtoRGB24(lpVideoData, lpVideoImage24, nWidth, nHeight);
		//Change to Little Endian
		CHANGE_ENDIAN_PIC(lpVideoImage24, nWidth, nHeight, 24);

		BITMAPINFO m_bmphdr={0};
		DWORD dwBmpHdr = sizeof(BITMAPINFO);
		//24bit
		m_bmphdr.bmiHeader.biBitCount = 24;
		m_bmphdr.bmiHeader.biClrImportant = 0;
		m_bmphdr.bmiHeader.biSize = dwBmpHdr;
		m_bmphdr.bmiHeader.biSizeImage = 0;
		m_bmphdr.bmiHeader.biWidth = nWidth;
		//Notice: BMP storage pixel data in opposite direction of Y-axis (from bottom to top).
		//So we must set reverse biHeight to show image correctly.
		m_bmphdr.bmiHeader.biHeight = nHeight;
		m_bmphdr.bmiHeader.biXPelsPerMeter = 0;
		m_bmphdr.bmiHeader.biYPelsPerMeter = 0;
		m_bmphdr.bmiHeader.biClrUsed = 0;
		m_bmphdr.bmiHeader.biPlanes = 1;
		m_bmphdr.bmiHeader.biCompression = BI_RGB;
		
		HWND wnd = pVideoWnd->m_hWnd;
		HDC hdc = ::GetWindowDC(wnd);
		CRect rt;
		pVideoWnd->GetClientRect(&rt);
		pVideoWnd->ClientToScreen(&rt);
		int nScreenX = rt.right - rt.left;
		int nScreenY = rt.bottom - rt.top;
		
		double d1 = nScreenX * 1.0 / nScreenY;
		double d2 = nWidth * 1.0 / nHeight;
		
		int nDcX = 0; int nDcY = 0; int nDcWdith = 0; int nDcHeight = 0;
		if (1 <= d2) {//x > y
			
			if (d1 >= d2) {
				nDcHeight = nScreenY;
				nDcY = 0;
				nDcWdith = d2 * nDcHeight;
				nDcX = (nScreenX - nDcWdith) / 2;
			}
			else {

				nDcWdith = nScreenX;
				nDcX = 0;
				nDcHeight = d1 * nDcWdith;
				nDcY = (nScreenY - nDcHeight) / 2;
			}
		}
		else {//x < y
			
			if (nScreenY >= nHeight) {

			}
			else if (nScreenY < nHeight) {
				nDcHeight = nScreenY;
				nDcWdith = d2 * nDcHeight;
				nDcY = 0;
				nDcX = (nScreenX - nDcWdith) / 2;
			}
		}

		int nResult = StretchDIBits(hdc,
			nDcX,nDcY,
			nDcWdith,nDcHeight,
			0, 0,
			nWidth, nHeight,
			lpVideoImage24,
			&m_bmphdr,
			DIB_RGB_COLORS,
			SRCCOPY);

		::ReleaseDC(wnd, hdc);
	
#if 0
		FILE *pFile = fopen("../video.yuv", "ab+");
		if (pFile) {
			fwrite(lpVideoData, 1, nVideoDataLen, pFile);
			fclose(pFile);
			pFile = nullptr;
		}
#endif

	} while (TRUE);

	if (lpVideoData)
		delete[] lpVideoData;
	if (lpVideoImage24)
		delete[] lpVideoImage24;
	
	return TRUE;
}