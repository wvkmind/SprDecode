
#ifndef __GRAPHIC_H__
#define __GRAPHIC_H__
#include <stdio.h>
#include <memory.h>

#include "xSprite.h"	// 16 비트 스프라이트 클래스



// 스프라이트 출력효과 설정 플래그
#define LR		((DWORD)1)		// 0000000000000001(b) (좌우반전)
#define UD		((DWORD)2)		// 0000000000000010(b) (상하반전)
#define HB		((DWORD)4)		// 0000000000000100(b) (반투명효과)
#define GB		((DWORD)8)		// 0000000000001000(b) (다단계 반투명효과)
#define GLOW	((DWORD)16)		// 0000000000010000(b) (글로우필터효과)
#define MONO	((DWORD)32)		// 0000000000100000(b) (모노필터효과)
#define ANTI	((DWORD)64)		// 0000000001000000(b) (안티효과)
#define LE		((DWORD)128)	// 0000000010000000(b) (광원효과)
#define DE		((DWORD)256)	// 0000000100000000(b) (Darken Effect)
#define RXY		((DWORD)512)	// 0000001000000000(b) (상대좌표 적용)
#define CENTER	((DWORD)1024)	// 0000010000000000(b) (확대시 캐릭터를 좌표의 중심에 위치시킨다)
#define PERCENT	((DWORD)2048)	// 0000100000000000(b) (확대시 비율에 따른 확대)
#define MIRROR  ((DWORD)4096)	// 0001000000000000(b) (좌우 뒤집기-LR과 달리 상대좌표도 함께 반전)
#define TEMPCLIP ((DWORD)8192)	// 0010000000000000(b) (임시 클리핑 영역을 사용함)


BOOL PointHitTest(xSprite *pSpr, int sprnum, int x, int y, DWORD opt=0);


class CPage
{
public:
	LPBITMAPINFO lpBmpInfo;     // 비트맵 정보
	LPLOGPALETTE lpLogPal;      // 로그 파레트 포인터
    
	DWORD nAllocSize;

	void* lpBit;                // 페이지 포인터
	LONG  lPitch;               // PITCH(4의배수) - 실제의 페이지의 가로 사이즈
	LONG  lPitchHalf;           // lPitch/2
	LONG  width, height;        // 페이지의 유효 범위 길이(최대 클리핑 지역)
	WORD  nBitNum;              // 비트수
	LONG  nBPD;                 // 도트당 바이트 수
	WORD  nColorNum;            // 칼라수
	
	CPage();
	~CPage();

	// 8비트 알파 테이블을 생성하였는가?
	BOOL bCreateAlphaTable8;

	// Attach, Detach
	BOOL bAttached;
	BOOL Attach(LONG xl, LONG yl, void *lpbit, LONG lpitch);


	BOOL Init(LONG xl, LONG yl, int bit, RGBQUAD *lpPal=NULL, LPCTSTR palfname=NULL);				// 페이지 초기화
	void Destroy();																					// 페이지 파괴
	BOOL SaveToBitmap(std::string fname);																	// 페이지의 내용을 비트맵으로 저장



};


#endif