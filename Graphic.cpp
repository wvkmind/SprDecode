
#include "Graphic.h"
//#include "Jpegfile.h"

#pragma comment(lib, "GraphEngine.lib")


CPage::CPage()
{
	lpBmpInfo = NULL;
	lpBit     = NULL;
	lPitch    = 0;
	lPitchHalf= 0;
	nBitNum   = 8;
	nBPD      = 1;
	nColorNum = 0;

	nAllocSize = 0;

	////////////


	////////////

	

	////////////

	
	////////////

}


CPage::~CPage()
{
	Destroy();
}

void CPage::Destroy()
{

	nColorNum = 0;
	nAllocSize = 0;

	if(lpBmpInfo != NULL){ delete lpBmpInfo; lpBmpInfo=NULL;}
	//if(lpBit != NULL) { delete lpBit; lpBit=NULL; }

}


BOOL CPage::Init(LONG xl, LONG yl, int bit, RGBQUAD *lpPal, LPCTSTR palfname)
{

	//if(lpBit != NULL) { delete lpBit; lpBit=NULL; }
	if(lpBmpInfo != NULL){ delete lpBmpInfo; lpBmpInfo=NULL;}
	
	switch(bit)
	{
	case 4  : nBPD=1; nColorNum=16;  break;
	case 8  : nBPD=1; nColorNum=256; break;
	case 16 : nBPD=2; nColorNum=0;   break;
	case 24 : nBPD=3; nColorNum=0;   break;
	default : return 0;
	}

	nBitNum = bit;
	width = xl;
	lPitch = xl*nBPD;
	height = yl;
	while(lPitch%4) lPitch++; // Pitch가 4의 배수가 되게 한다
	//lPitch/=2;                // 16비트 칼라여서 편법을 썼다.
	                          // 여기서의 lPitch란 바이트수이다(점의 개수는 아니다)
	lPitchHalf = lPitch/nBPD; // lPitch는 바이트수이므로 8,16,23비트 모드의 점의 개수를 얻는다.

	// 비트맵 인포 구조체를 위한 메모리 할당
	nAllocSize = sizeof(BITMAPINFO) + sizeof(RGBQUAD) * nColorNum;
	//if(nColorNum != 0) nAllocSize -= sizeof(RGBQUAD);
	nAllocSize -= sizeof(RGBQUAD);
	lpBmpInfo = (LPBITMAPINFO)new char[nAllocSize + sizeof(RGBQUAD)];
	if(lpBmpInfo == NULL) return 0;

	// 가상 페이지 생성(비트맵 이미지 포인터)
	//lpBit = (WORD*)new char[lPitch * yl];
	//if(lpBit == NULL) return FALSE;

	// 비트맵 인포 해더 생성
	lpBmpInfo->bmiHeader.biSize	         = sizeof (BITMAPINFOHEADER);
	lpBmpInfo->bmiHeader.biWidth         = width;      //width;
	lpBmpInfo->bmiHeader.biHeight        = -height;    // 음수면 그림이 뒤집혀 출력된다
	lpBmpInfo->bmiHeader.biPlanes        = 1;          // 항상1
	lpBmpInfo->bmiHeader.biBitCount	     = nBitNum;    // 도트당 비트수(16비트는 5:6:5)
	lpBmpInfo->bmiHeader.biCompression	 = 0;          // BI_RGB와 같다(비압축)
	lpBmpInfo->bmiHeader.biSizeImage     = lPitch*yl;  // lPitch; // 항상 4의배수
	lpBmpInfo->bmiHeader.biXPelsPerMeter = 0;
	lpBmpInfo->bmiHeader.biYPelsPerMeter = 0;
	lpBmpInfo->bmiHeader.biClrUsed	     = 0;          // 사용된 총 칼라수(항상 0으로 고정)
	lpBmpInfo->bmiHeader.biClrImportant	 = 0;          // 중요한 색상수(항상 0으로 고정)


	return 1;
}

BOOL CPage::Attach(LONG xl, LONG yl, void *lpbit, LONG lpitch)
{

	nBPD=2; nColorNum=0;
	nBitNum = 16;
	width = xl;
	lPitch = lpitch;
	height = yl;
	//while(lPitch%4) lPitch++; // Pitch가 4의 배수가 되게 한다
	lPitchHalf = lPitch/nBPD; // lPitch는 바이트수이므로 8,16,23비트 모드의 점의 개수를 얻는다.

	lpBit = (WORD*)lpbit;


	bAttached = 1;
	return 1;
}



/*-----------------------------------------------------------------------------------------
   페이지를 비트맵 파일로 저장한다
------------------------------------------------------------------------------------------*/
BOOL CPage::SaveToBitmap(std::string fname)
{
	if(!fname.data()) return 2;
	if(!lpBit ) return 3;
    if(!lpBmpInfo ) return 6;
	if(nAllocSize <= 0) return 4;
	if(nBitNum != 16) return 5;

	FILE *fp = fopen(fname.data(), "wb");
    if(!fp) return 6;

	int npitch = width * 3;
	while(npitch%4) npitch++;
	int allocsize = npitch * height;
	BYTE *pimage = new BYTE[allocsize];
	memset(pimage, 0, allocsize);
	BYTE *bit = pimage;

	//16비트 이미지를 24비트로 변환(이미지를 뒤집음)
	for(int i=height-1; i>=0; i--) {
		for(int j=0; j<width; j++) {
			WORD color = *((WORD*)lpBit + (i * lPitchHalf + j));
			// 주의) BGR순이다
			*(bit+(j*3)) = ((BYTE)B555(color)) << 3;	// B
			*(bit+(j*3)+1) = ((BYTE)G555(color)) << 3;	// G
			*(bit+(j*3)+2) = ((BYTE)R555(color)) << 3;	// R
		}
		bit += npitch;
	}
    
	BITMAPFILEHEADER	bmfHeader = {0,};
	BITMAPINFOHEADER    bmiHeader = {0,};
	memcpy(&bmiHeader, &lpBmpInfo->bmiHeader, sizeof(BITMAPINFOHEADER));

	bmiHeader.biBitCount = 24;
	bmiHeader.biHeight = height;

	bmfHeader.bfType = ((WORD)('M'<<8 | 'B'));	// bmp포맷 표시자;
	bmfHeader.bfSize = 16 + sizeof(BITMAPINFOHEADER) + allocsize;


    printf("DWORD%lu",sizeof(WORD));
    printf("BITMAPFILEHEADER%lu",sizeof(BITMAPFILEHEADER));

	bmfHeader.bfOffBits = 16 + sizeof(BITMAPINFOHEADER);

    bmfHeader.bfOffBits = bmfHeader.bfOffBits << 16 | bmfHeader.bfOffBits >> 16;

	fwrite(&bmfHeader, 16, 1, fp);
	//fwrite(&bmiHeader, sizeof(BITMAPINFOHEADER), 1, fp);
	//fwrite(pimage, allocsize, 1, fp);

	fclose(fp);

	delete [] pimage;

	return 1;
}


