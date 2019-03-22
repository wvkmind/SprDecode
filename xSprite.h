
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <string>
#include <fstream>
#include <ios>

extern unsigned short RGB555_TO_RGB565(unsigned short color); // RGB555 --> RGB565
extern unsigned short RGB565_TO_RGB555(unsigned short color); // RGB565 --> RGB555
extern unsigned short RGB555_TO_BGR565(unsigned short color); // RGB555 --> BGR565
extern unsigned short RGB565_TO_BGR565(unsigned short color); // RGB565 --> BGR565
extern void ANALYSIS_RGB555(unsigned short color, unsigned short* r, unsigned short* g, unsigned short* b); 
extern void ANALYSIS_RGB565(unsigned short color, unsigned short* r, unsigned short* g, unsigned short* b); 
#define R555(r)			(((r)&0x7c00)>>10)
#define G555(g)			(((g)&0x03e0)>>5)
#define B555(b)			((b)&0x001f)

typedef int BOOL;
typedef char * LPSTR;
typedef char * LPCTSTR;
typedef unsigned char BYTE;
typedef struct        
{
    char  Identifier[16];     
    short Version;             
	short BPD;                
	short FixelFormat;        
    short TotalSprNum;       
	int  CharacterNum;      
} SPRFILEHEAD;
typedef SPRFILEHEAD* LPSPRFILEHEAD;

typedef struct       
{
	short attrib;            
    short x1,y1;              
	short x2,y2;              
	char reserved[2];
} SPRPOS;
typedef SPRPOS* LPSPRPOS;

typedef struct      
{
    unsigned char  compress;          
	unsigned short  colorkey;           
	short rx, ry;            
    short xl, yl;              
    short mx, my;             
	short hitNum;             
	short atckNum;            
    unsigned int dsize;      
	short PixelFormat;        
	char  SprFormat;		 
	char  Dithering;		 
	unsigned char  byUserVal;		
} SPRHEAD30;
typedef SPRHEAD30* LPSPRHEAD30;

typedef struct      
{
    unsigned char  id;              
	unsigned char  attrib;             
	short frameNum;          
} ACTIONHEAD;
typedef ACTIONHEAD* LPACTIONHEAD;

typedef struct        
{
    short frame;              
	int  delay;               
	short rx, ry;             
	short flag;               
} ACTIONFRAME;
typedef ACTIONFRAME* LPACTIONFRAME;

//------------------------------------------------------------------------------

typedef struct          // ��������Ʈ ����Ÿ ����ü(�޸𸮿�)
{
    int compress;            // ���࿩��
	unsigned short colorkey;           // �÷�Ű
	int xl, yl;              // X, Y�� ����
    int rx, ry;              // �����ǥ
    int mx, my;              // �߽���ǥ
	int hitNum;              // �浹��ǥ�� ����
	int atckNum;             // ������ǥ�� ����
	unsigned int  dsize;    // ��������Ʈ ����Ÿ�� ������(����� �ʿ�)
	short PixelFormat;       // �ȼ� ����(��= 555, 565, 4444)
	char SprFormat;			 // ���� ����(0:��������Ʈ 1:JPG)
	char Dithering;			 // JPG�����ΰ�� ����� ����(0:��������� 1:�����)
	unsigned char  byUserVal;		 // ����� ���� ����
	SPRPOS* hit;             // �浹��ǥ ����ü�� ������
	SPRPOS* atck;            // ������ǥ ����ü�� ������
    unsigned short*   data;            // ��������Ʈ ����Ÿ ������
	//////////////////////
	unsigned int  jpgsize;  // JPG�����ΰ�� �̹��� ������
	unsigned char*	pJpgData;		 // JPG�����ΰ�� ������ ������
	//////////////////////
} SPRITE30;
typedef unsigned short WORD;
typedef unsigned int DWORD;
typedef int LONG;
typedef struct tagBITMAPFILEHEADER {
 WORD    bfType;
 DWORD   bfSize;
 WORD    bfReserved1;
 WORD    bfReserved2;
 DWORD   bfOffBits;
} BITMAPFILEHEADER;
typedef struct tagBITMAPINFOHEADER{
 DWORD      biSize;
 LONG        biWidth;
 LONG        biHeight;
 WORD       biPlanes;
 WORD       biBitCount;
 DWORD      biCompression;
 DWORD      biSizeImage;
 LONG        biXPelsPerMeter;
 LONG        biYPelsPerMeter;
 DWORD      biClrUsed;
 DWORD      biClrImportant;
} BITMAPINFOHEADER;
typedef struct tagRGBQUAD {
  unsigned char    rgbBlue;
  unsigned char    rgbGreen;
  unsigned char    rgbRed;
  unsigned char    rgbReserved;
} RGBQUAD;

typedef struct tagBITMAPINFO {
  BITMAPINFOHEADER bmiHeader;
  RGBQUAD          bmiColors[1];
} BITMAPINFO, *LPBITMAPINFO, *PBITMAPINFO;
typedef struct tagPALETTEENTRY {
  BYTE peRed;
  BYTE peGreen;
  BYTE peBlue;
  BYTE peFlags;
} PALETTEENTRY;
typedef struct tagLOGPALETTE {
  WORD         palVersion;
  WORD         palNumEntries;
  PALETTEENTRY palPalEntry[1];
} LOGPALETTE, *PLOGPALETTE, *NPLOGPALETTE, *LPLOGPALETTE;
class xSprite
{
public:

	///// ���� ���� ����
	SPRFILEHEAD sfhead;                  // ��������Ʈ ���� ���
	SPRITE30 *spr;                       // ��������Ʈ ���� ����ü
//	xAniFile ani;                        // ���ϸ��̼� ������ Ŭ����

	///// ������, �Ҹ���
	xSprite();
	~xSprite();

	//// ��������Ʈ �ε�,���̺�,�����Լ�
	int Load(char *name);
	int Load(char *name,  int ScreenRGBMode);
	void Remove();

	//// Ư����� �Լ�
	int Compress(int snum);                       // ��������Ʈ ������ ����
	int DeCompress(int snum);                     // ��������Ʈ ������ ���� ����
	int ChangeFixelFormat(int NewForm);            // �ȼ� ������ �ٲ۴�(555,565,-565)
	void OptimizeSize(int snum, int fixPos=1);	// ��������Ʈ ����� ����ȭ�Ѵ�
	int TransToSpr(int xl, int yl, unsigned short *pImage, unsigned short wColorKey);  // Ư�� �̹����� ��������Ʈ�� �����(�������� �ʰ� �����͸� �״�� ������)
	int TransToSprByCopy(int xl, int yl, unsigned short *pImage, unsigned short wColorKey); // Ư�� �̹����� ��������Ʈ�� �����(���������� �Ҵ��Ͽ� ������)
	int NewSpace(int xl, int yl, unsigned short *pImage, unsigned short wColorKey, int bCompress);
	int Insert(int snum, int xl, int yl, unsigned short *pImage, unsigned short wColorKey, int bCompress);  // ��������Ʈ �� ����
	int InsertByCopy(int snum, int xl, int yl, unsigned short *pImage, unsigned short wColorKey, int bCompress);

	/////////// static member

	inline static unsigned short RGB565(int r,int g,int b) {	
		return 	(unsigned short) (  (((unsigned short)(((r)/8)<<11))) | (((unsigned short)(((g)/4)<<5))) | ((unsigned short)(b)/8)  );
	}
	
	inline static unsigned short	RGB555(int r,int g,int b) {	
		//g>>=1;
		return 	(unsigned short) (  (((unsigned short)(((r)/8)<<10))) | (((unsigned short)(((g)/8)<<5))) | ((unsigned short)(b)/8)  );
	}

//	static unsigned short* Make16bitFromJpeg(LPCTSTR fname, int* width, int* height, int bDither);
//	static unsigned short* Make16bitFromJpeg(FILE *fp, int* width, int* height, int bDither);
};




