
#include "xSprite.h"
//#include "JpegFile.h"	// JPEG ���?
//#include "dither.h"     // �����? ���?

///////////////////////////////////////////////////////////////////////////

/*--------------------------------------------------------------------

		  <  0 �� ���� ���� ����(�����? ������ ����Ÿ �κ�) >

 Byte(s)

 4 * height  - �������� ���� �������� �Ÿ��� ���? �迭
   2         - ����(pattern ) �� ---------------------------+
   2         - ������ �� ---------------+                   | ���α��� ��ŭ
   2         - �׸� ����                | ���ϼ���ŭ �ݺ�   | �ݺ�
  size       - ����Ÿ(����*2)-----------+ ------------------+

  ## ����Ÿ�� ũ���? ����*2�̴�(16��Ʈ �̹Ƿ�) ��, ���� ����Ʈ���� �ǹ��Ѵ�
  ## �������� �Ÿ� : ����Ÿ�� ó���������� �� ������ ���۵Ǵ� ��������
					 �Ÿ��� ��Ÿ����.(�������� ���̰� �ƴ�)
					 (Ŭ���ν� �ӵ��� ���̰� ó���� �����ϰ��ϱ�����)
					 (����: �Ÿ�1�� ���� 2����Ʈ������)
  ## ���������� �׸�����Ÿ���� unsigned short���̴�.
 --------------------------------------------------------------------*/



//#define  TRANS  0

typedef struct {
	unsigned short skipb;
	unsigned short putb;
} BLKPATTERN;

/*---------------------------------------------------------
  ���ۿ� �̹����� �����Ͽ� �����ϴ� �Լ�

  unsigned short *image   : �Էµ� �̹��� ������(16��Ʈ)
  int  xl       : �Է� �̹����� ���α���
  int  yl       : �Է� �̹����� ���α���
  unsigned short **output : ��µ�? ���� �̹����� ���� ������
				  (�ݵ��? NULL �̾���Ѵ�?, �����������ӿ� ����)
  unsigned short TRANS    : Color Key

  ���ϰ�  -->  �����? ������(���н� 0)

  ���� ) ���� �̹����� �����̹����� ������
		 ���� �������Ƿ� �̸� ���� free ���־���Ѵ�?.
-----------------------------------------------------------*/
unsigned int
EncodeSpr0(unsigned short *image, int xl, int yl, unsigned short **output, unsigned short TRANS)
{
	if(!image || xl <= 0 || yl <= 0) return 0;

	unsigned short i,k,cnt;
	short pNum;        // ������ ����
	BLKPATTERN pat[1024] = {0,};

	unsigned int bp=0;         // ���� �ε���
	unsigned int dsize = 0;        // �����? ��ü ũ��

	// �ӽ� ���� �Ҵ�(����ũ���� 4�� + LDT���̺�����) 
	// ���α��̴� �ּ� 8�ȼ� �̻����� ����(8���� ���� ���? 4�踦 �ϴ��� ���۰� ���ڶ� �� ����)
    int xl__ = 8;
    if(xl>8)xl__=xl;

	unsigned short *buf = (unsigned short *)malloc((xl__*4)*yl+(4*yl));
	if(buf==NULL) return 0;

	bp += yl*2;   // �� ������ �Ÿ��� ���? ������ �ǳʶ�(����: buf�� unsigned short�� �������̹Ƿ�... yl*2)
	for(k=0; k<yl; k++)
	{
		for(i=0,pNum=0; i<xl;) {
			// skip bytes
			for(cnt=0; i<xl && *(image+i)==TRANS; cnt++,i++) ;
			pat[pNum].skipb = cnt;

			// put bytes
			for(cnt=0; i<xl && *(image+i)!=TRANS; cnt++,i++) ;
			pat[pNum].putb = cnt;

			pNum++;
		}

		// Line-Distance-Table
		memcpy(buf+k*2, &bp, 4); // ����: buf�� unsigned short�� �������̹Ƿ�... k*2

		// ���δ� ������ ����
		*(buf+bp) = pNum;
		bp++;

		// Copy pattern to temporary buffers
		for(i=0;i<pNum;i++) {
			memcpy(&buf[bp], &pat[i],sizeof(BLKPATTERN)); 
			bp+=2;
			if(pat[i].putb!=0) {
				 // Copy Image
				 memcpy(buf+bp, image+pat[i].skipb, pat[i].putb*2); // for 16bit
				 bp+=pat[i].putb;
			}

			image += pat[i].skipb + pat[i].putb;
		}
	}

	dsize=bp*2; // LDT���̺� + �̹���ũ��
	*output = (unsigned short *)malloc(dsize);
	memcpy(*output, buf, dsize); // �ӽù����� ������ ����
	free(buf);
	return(dsize);
}

/*---------------------------------------------------------
  ���ེ������Ʈ�� ���ۿ� ���ڵ��ϴ� �Լ�
  (������ ũ���? '���α���*���α���' �̾���Ѵ�?)
-----------------------------------------------------------*/
void DecodeSpr0( unsigned short *tar, int xl, int yl, unsigned short *sor, unsigned short TRANS)
{
	int  i,j;
	int  sbyte,pbyte;
	short cnt;

	for(i=0; i<yl; i++) // �켱 ���? �������� �����Ѵ�
	{
		for(j=0; j<xl; j++) *(tar+(xl*i)+j) = TRANS; 
	}

	sor += yl*2; // ���ΰŸ� ����Ÿ�� �ǳʶ�
				 // Ŭ������ �����Ƿ� �ʿ� ����

	//sor += *((int*)sor);    // ���? ������ �������� �̵�

	for(i=0;i<yl;i++,tar+=xl) 
	{
		cnt = *sor;      // ���� ���� ���?
		sor++;
		sbyte=0;
		while(cnt--) {

			sbyte+=(*(unsigned short*)sor); // �������� ���?
			sor++;

			pbyte =(*(unsigned short*)sor); // ���� ���?
			sor++;

			memcpy(tar+sbyte, sor, pbyte<<1); // �̹��� ����ֱ�? (16��Ʈ�̹Ƿ� *2)

			sor+=pbyte;
			sbyte+=pbyte;
		}
	}
}

///////////////////////////////////////////////////////////////////////////

//class xDraw;
//extern static int xDraw::PixelFormat; // ��ũ���� Pixel Format�� �˾ƾ� �ϱ� ����

unsigned short RGB555_TO_RGB565(unsigned short color) // RGB555 --> RGB565
{
	unsigned short r = (color & 0x7C00)<<1;
	unsigned short g = (color & 0x03E0)<<1;
	unsigned short b = (color & 0x001F);
	return (r|g|b);
}

unsigned short RGB565_TO_RGB555(unsigned short color) // RGB565 --> RGB555
{
	unsigned short r = (color & 0xF800)>>1;
	unsigned short g = (color & 0x03E0)>>1;
	unsigned short b = (color & 0x07E0);
	return (r|g|b);
}

unsigned short RGB555_TO_BGR565(unsigned short color) // RGB555 --> BGR565
{
	unsigned short r = (color & 0x7C00)>>10;
	unsigned short g = (color & 0x03E0)<<1;
	unsigned short b = (color & 0x001F)<<11;
	return (b|g|r);
}

unsigned short RGB565_TO_BGR565(unsigned short color) // RGB565 --> BGR565
{
	unsigned short r = (color & 0xF800)>>11;
	unsigned short g = (color & 0x03E0);
	unsigned short b = (color & 0x07E0)<<11;
	return (b|g|r);
}

void ANALYSIS_RGB555(unsigned short color, unsigned short* r, unsigned short* g, unsigned short* b) // 16��Ʈ(555)�� RGB�� �м��Ѵ�
{
	*r = (color & 0x7C00)>>10;
	*g = (color & 0x03E0)>>5;
	*b = (color & 0x001F);
}

void ANALYSIS_RGB565(unsigned short color, unsigned short* r, unsigned short* g, unsigned short* b) // 16��Ʈ(565)�� RGB�� �м��Ѵ�
{
	*r = (color & 0xF800)>>11;
	*g = (color & 0x03E0)>>5; 
	*b = (color & 0x07E0);
	// ����) 565���� Green�� �ι��̹Ƿ� ����Ʈ�� �ѹ� ���ؾ� 
	//       RGB ���սÿ� ��Ȯ�� ���� ���´�
	//       �׷��� ���⼱ �ѹ� �� ���� �ʾҴ�.
}


//================================ ��������Ʈ ���� 3.0 Ŭ���� =================================//


xSprite::xSprite()
{
	memset(&sfhead,0, sizeof(SPRFILEHEAD));
	spr=NULL;
}

xSprite::~xSprite()
{
	Remove();
}


/*--------------------------------------------------------------------------
  �ε��� ��������Ʈ�� ����
---------------------------------------------------------------------------*/
void xSprite::Remove()
{
	int i;
	if(spr != NULL)  {
		for(i=0; i < sfhead.TotalSprNum; i++) {
			if( spr[i].hit != NULL ) { free(spr[i].hit); spr[i].hit = NULL; }
			if( spr[i].atck != NULL ) { free(spr[i].atck); spr[i].atck = NULL; }
			if( spr[i].data != NULL ) { free(spr[i].data); spr[i].data = NULL; }
		}
		free(spr); spr=NULL;
		memset(&sfhead,0, sizeof(SPRFILEHEAD));
//		ani.Destroy();
	}
}

/*--------------------------------------------------------------------------
  ��������Ʈ ������ �ε�(��ũ���� �ȼ� ���˿� ���� �д´�)
  name: ���ϸ�
  (����: xCanvas�� ���� �ʱ�ȭ �Ǿ� �־��? �Ѵ�)
---------------------------------------------------------------------------*/
int xSprite::Load(char *name)
{
	return(Load(name, 555));
}

/*--------------------------------------------------------------------------
  ��������Ʈ ������ �ε�
  name: ���ϸ�
  ScreenRGBMode: ������ �ȼ� �������� �о���δ�?
---------------------------------------------------------------------------*/
int xSprite::Load(char *name, int ScreenRGBMode)
{
    
	FILE *fp;
    
	SPRFILEHEAD tempsfhead;     // �ӽ� ���� ���?
	SPRHEAD30 shead;            // �� ��������Ʈ�� ���?
    
	if( (fp = fopen( name, "rb")) == NULL ) { 
		//PutErrStr("������ �����ϴ�(%s)", name); 
		return 0;
	}
    
	fread(&tempsfhead, sizeof(SPRFILEHEAD), 1, fp); 
    
	if(strncmp("93XYZ sprite",tempsfhead.Identifier,13) != 0) {

		fclose(fp);
		return 0;
	}
    
	Remove();
    
	memcpy(&sfhead, &tempsfhead, sizeof(SPRFILEHEAD));
    
	spr = (SPRITE30*) malloc(sizeof(SPRITE30) * sfhead.TotalSprNum);
    
	if(spr==NULL) { 
		//PutErrStr("�޸� �Ҵ� ����(���?)"); 
		return 0; 
	}
	memset(spr, 0,sizeof(SPRITE30) * sfhead.TotalSprNum);
    
	for(int i=0; i < sfhead.TotalSprNum; i++)
	{
		fread(&shead, sizeof(SPRHEAD30), 1, fp); // �� ��������Ʈ ������ ���? �ε�
		
		spr[i].compress = shead.compress;
		spr[i].colorkey = shead.colorkey;
        
		spr[i].rx       = shead.rx;
		spr[i].ry       = shead.ry;
		spr[i].xl       = shead.xl;
		spr[i].yl       = shead.yl;
		spr[i].mx       = shead.mx;
		spr[i].my       = shead.my;
		spr[i].hitNum   = shead.hitNum;
		spr[i].atckNum  = shead.atckNum;
		spr[i].hit      = NULL;
		spr[i].atck     = NULL;
		spr[i].dsize    = shead.dsize;
		spr[i].PixelFormat = shead.PixelFormat;
		spr[i].SprFormat = shead.SprFormat;
		spr[i].Dithering = shead.Dithering;
		spr[i].byUserVal = shead.byUserVal;
		//spr[i].pJpgData = NULL;
		//spr[i].jpgsize = 0;
        
		///// �浹 ��ǥ, ���� ��ǥ �б� /////
		if(spr[i].hitNum>0) spr[i].hit = (SPRPOS*)malloc(sizeof(SPRPOS)*spr[i].hitNum);
		if(spr[i].atckNum>0) spr[i].atck = (SPRPOS*)malloc(sizeof(SPRPOS)*spr[i].atckNum);
		fread(spr[i].hit, sizeof(SPRPOS)*spr[i].hitNum, 1, fp);
		fread(spr[i].atck, sizeof(SPRPOS)*spr[i].atckNum, 1, fp);
    
		
		// ��������Ʈ ��������, JPEG �������� Ȯ���ϴ�
		if(spr[i].SprFormat == 1) {
			//
			// JPEG �����̴�
			//

			// �����? �ɼ��� �����Ǿ� �ִٸ� �����? ����
			int bDither = 0;
			if(spr[i].Dithering == 1) bDither = 1;

			//// JPEG ���� �б�
			fpos_t pos;
			fgetpos(fp, &pos);

			int width=0, height=0;/*
			unsigned short *pNewImage = Make16bitFromJpeg(fp, &width, &height, bDither);
			if(!pNewImage) {
				//PutErrStr("JPEG ���� �б� ����");
				fclose(fp);
				return 0;
			}

			if(width != spr[i].xl || height != spr[i].yl) {
				//PutErrStr("JPEG ������ �̹��� ����� ��ġ���� ����");
				fclose(fp);
				return 0;
			}

			spr[i].dsize = width * height * 2;
			spr[i].data = pNewImage;

			// �������? ���� ���·� �ϴ� ��ȯ
			spr[i].compress = 0;

			if(shead.compress == 1) {
				// ���� ��Ŵ
				Compress(i);
			}
*/
			// JPEG �̹��� ������ ����(���ӿ����� �ʿ� ����)
			//fsetpos(fp, &pos);
			//spr[i].jpgsize = shead.dsize;
			//spr[i].pJpgData = new unsigned char[shead.dsize];
			//fread(spr[i].pJpgData, shead.dsize, 1, fp);

			// ���� ��������Ʈ�� ���� ������ �̵�
			pos += shead.dsize;
			fsetpos(fp, &pos);
		}
		else {
			//
			// ��������Ʈ �����̴�
			//

            
			///// ��������Ʈ �̹��� ����Ÿ �б�
			spr[i].data = (unsigned short *)malloc( shead.dsize );
			if(spr[i].data==NULL) { 
				//PutErrStr("�޸� �Ҵ� ����(�̹���)"); 
				return 0; 
			}
			
			
			memset(spr[i].data,0, shead.dsize);
			fread(spr[i].data, shead.dsize, 1, fp);
      DeCompress(i);
			printf("spr[i].dsizee%d\n",spr[i].dsize);
      FILE *fp_ = fopen(std::string("./data/"+std::string(name)+std::to_string(i)+"."+std::to_string(spr[i].PixelFormat)+"."+std::to_string(spr[i].xl)+"x"+std::to_string(spr[i].yl)+".data").data(), "wb");
      for(int _i =0;_i<spr[i].dsize;_i++)
      {
          BYTE r = (BYTE)R555(spr[i].data[_i])<< 3;
          fwrite(&r, 1, 1, fp_);
          BYTE g = (BYTE)G555(spr[i].data[_i])<< 3;
          fwrite(&g, 1, 1, fp_);
          BYTE b = (BYTE)B555(spr[i].data[_i])<< 3;
          fwrite(&b, 1, 1, fp_);				
      }
      fclose(fp_);
			
      
      // SaveToBitmap(spr[i],std::string(name)+std::to_string(i)+".bmp");
		}
			return 1;
	}

	//// ���ϸ��̼� ������ �б�
//	if(sfhead.CharacterNum > 0) ani.Load(fp);

	fclose(fp);

	if(sfhead.FixelFormat != 4444) ChangeFixelFormat(ScreenRGBMode);

	return 1;
}


/*--------------------------------------------------------------------------
  ��������Ʈ ����� ����ȭ(�ʿ����? ����κ���? ���ش�)
---------------------------------------------------------------------------*/
void xSprite::OptimizeSize(int snum, int fixPos)
{
	if(snum >= sfhead.TotalSprNum || snum < 0) return;
	if(spr[snum].data == NULL) return;

	int compressed=0;
	int i,j;
	int x1=0, y1=0, x2=0, y2=0;
	int fx=0, ex=0, fy=0, ey=0;
	int bline = 1;
	int xl=spr[snum].xl;
	int yl=spr[snum].yl;
	unsigned short colorkey = spr[snum].colorkey;

	if(spr[snum].compress) {compressed=1; DeCompress(snum);}

	// ������ ����ȭ�� ���� ����κ�? ���?
	x1 = spr[snum].xl-1;
	fy = 1;
	for(i=0; i<yl; i++) {
		fx = 0;
		ex = 0;
		bline = 1;		// �������? ����
		for(j=0; j<xl; j++) {
			if(*(spr[snum].data + (i*xl+j)) != colorkey) {
				bline = 0;
				ex = 1;
				ey = 1; y2=i;
				if(fx==0) {
					if(j<x1) x1=j; 
					fx=1;
				}
				if(fy==1) { y1=i; fy=0; }
				if(j>x2) x2=j; 
			}
		}

		if(bline && ey) { y2=i; ey=0; }
	}

	if(x1>=x2 || y1>=y2) {
		if(compressed) Compress(snum);
		return;
	}
	if(x2 >= spr[snum].xl && y2 >= spr[snum].yl) {
		if(compressed) Compress(snum);
		return;
	}

	int newXl = x2-x1+1;
	int newYl = y2-y1+1;
	unsigned short *pNew = (unsigned short*)malloc((newXl*newYl)*2);
	unsigned short *pSor = spr[snum].data + (xl*y1);
	for(i=0; i<newYl; i++) {
		memcpy(pNew+(newXl*i), pSor+x1, newXl*2);
		pSor+=xl;
	}
	free(spr[snum].data);
	spr[snum].data = pNew;
	spr[snum].xl = newXl;
	spr[snum].yl = newYl;

	// ���� ��ǥ���� �Բ� ����
	if(fixPos) {
		spr[snum].rx += x1; spr[snum].ry += y1;
		spr[snum].mx -= x1; spr[snum].my -= y1;

		for(i=0; i<spr[snum].hitNum; i++) {
			spr[snum].hit[i].x1 -= x1; spr[snum].hit[i].y1 -= y1;
			spr[snum].hit[i].x2 -= x1; spr[snum].hit[i].y2 -= y1;
		}

		for(i=0; i<spr[snum].atckNum; i++) {
			spr[snum].atck[i].x1 -= x1; spr[snum].atck[i].y1 -= y1;
			spr[snum].atck[i].x2 -= x1; spr[snum].atck[i].y2 -= y1;
		}
	}

	//MsgBox(NULL,"x1=%d, y1=%d, x2=%d, y2=%d", x1, y1, x2, y2);
	if(compressed) Compress(snum);
}

/*--------------------------------------------------------------------------
  ��������Ʈ ����
---------------------------------------------------------------------------*/	
int xSprite::Compress(int snum)
{
	unsigned short *buf = NULL;           // �����? �̹����� �����? ����
	int newsize=0;

	if(spr==NULL) return 0;
	if(snum < 0 || snum >= sfhead.TotalSprNum) return 0;
	if(spr[snum].data == NULL) return 0;
	if(spr[snum].compress == 1) return 0;

	newsize = EncodeSpr0(spr[snum].data, spr[snum].xl, spr[snum].yl, &buf, spr[snum].colorkey);
	if(newsize == 0) return 0;
	
	free(spr[snum].data);          // ������ �̹����� ����
	spr[snum].compress = 1;
	spr[snum].data     = buf;      // ���� �����? �̹���
	spr[snum].dsize    = newsize;  // �����? ������

	return 1;
}

/*--------------------------------------------------------------------------
  ��������Ʈ ���� ����
---------------------------------------------------------------------------*/
int xSprite::DeCompress(int snum)
{
	unsigned short *buf = NULL;           // ������ Ǯ�� �̹����� �����? ����
	int newsize=0;

	if(spr==NULL) return 0;
	if(snum < 0 || snum >= sfhead.TotalSprNum) return 0;
	if(spr[snum].data == NULL) return 0;
	if(spr[snum].compress == 0) return 0;

	newsize = spr[snum].xl*2 * spr[snum].yl;
	buf = (unsigned short *)malloc(newsize);
	if(buf==NULL) return 0;

	DecodeSpr0(buf, spr[snum].xl, spr[snum].yl, spr[snum].data, spr[snum].colorkey);

	free(spr[snum].data);          // ������ �̹����� ����
	spr[snum].compress = 0;
	spr[snum].data     = buf;      // ������ Ǯ�� �̹��� ������ ������
	spr[snum].dsize    = newsize;  // ������ Ǯ�� ������
	printf("newsize%d\n",newsize);
	return 1;
}

/*--------------------------------------------------------------------------
  ��������Ʈ�� Ư�� �ȼ� �������� �ٲ۴�
  (NewForm = 555, 565, -565)
---------------------------------------------------------------------------*/
int xSprite::ChangeFixelFormat(int NewForm)
{
	int i,x,y;
	if(NewForm == sfhead.FixelFormat) return 1;

	if(NewForm!=555 && NewForm!=565 && NewForm!=-565) return 0;

	unsigned short color;
	
	for(i=0; i < sfhead.TotalSprNum; i++) // �� ��������Ʈ ����ŭ
	{
		color = spr[i].colorkey; // �������� ��´�?.

		// �������� �ȼ� ������ ��ȯ�Ѵ�
		if(sfhead.FixelFormat==565 && NewForm==555) // RGB 5:6:5 --> RGB 5:5:5
					spr[i].colorkey = RGB565_TO_RGB555(color);
		if(sfhead.FixelFormat==555 && NewForm==565) // RGB 5:5:5 --> RGB 5:6:5
					spr[i].colorkey = RGB555_TO_RGB565(color);
		if(sfhead.FixelFormat==555 && NewForm==-565) // RGB 5:5:5 --> BGR 5:6:5
					spr[i].colorkey = RGB555_TO_BGR565(color);
		if(sfhead.FixelFormat==565 && NewForm==-565) // RGB 5:6:5 --> BGR 5:6:5
					spr[i].colorkey = RGB565_TO_BGR565(color);
		
		// �̹��� �ȼ� ���� ��ȯ(�����? ��������Ʈ�� ���?)
		if(spr[i].compress) {
			int  m,xl,yl;
			int  sbyte,pbyte;
			short cnt;

			xl = spr[i].xl;
			yl = spr[i].yl;
			unsigned short* tar = spr[i].data + yl*2; // ���ΰŸ� ����Ÿ�� �ǳʶ�

			for(m=0; m<yl; m++) {
				cnt = *tar++;      // ���� ���� ���?
				sbyte=0;
				while(cnt--) {

					sbyte+=(*(unsigned short*)tar); // �������� ���?
					tar++;

					pbyte =(*(unsigned short*)tar); // ���� ���?
					tar++;

					for(int k=0; k<pbyte; k++) {
						color = *tar;
						if(sfhead.FixelFormat==565 && NewForm==555) // RGB 5:6:5 --> RGB 5:5:5
							*tar = RGB565_TO_RGB555(color);
						if(sfhead.FixelFormat==555 && NewForm==565) // RGB 5:5:5 --> RGB 5:6:5
							*tar = RGB555_TO_RGB565(color);
						if(sfhead.FixelFormat==555 && NewForm==-565) // RGB 5:5:5 --> BGR 5:6:5
							*tar = RGB555_TO_BGR565(color);
						if(sfhead.FixelFormat==565 && NewForm==-565) // RGB 5:6:5 --> BGR 5:6:5
							*tar = RGB565_TO_BGR565(color);
						tar++;
					}
					//tar+=pbyte;
					sbyte+=pbyte;
				}
			}
		}
		else // �������? ���� ��������Ʈ�� ���?
		{
			for(y=0; y<spr[i].yl; y++) {
				for(x=0; x<spr[i].xl; x++) {
					unsigned short* tar = (spr[i].data + (spr[i].xl*y + x));
					color = *tar;
					if(sfhead.FixelFormat==565 && NewForm==555) // RGB 5:6:5 --> RGB 5:5:5
						*tar = RGB565_TO_RGB555(color);
					if(sfhead.FixelFormat==555 && NewForm==565) // RGB 5:5:5 --> RGB 5:6:5
						*tar = RGB555_TO_RGB565(color);
					if(sfhead.FixelFormat==555 && NewForm==-565) // RGB 5:5:5 --> BGR 5:6:5
						*tar = RGB555_TO_BGR565(color);
					if(sfhead.FixelFormat==565 && NewForm==-565) // RGB 5:6:5 --> BGR 5:6:5
						*tar = RGB565_TO_BGR565(color);
				}
			}
		}
	}
		
	sfhead.FixelFormat = NewForm;

	return 1;
}

/*--------------------------------------------------------------------------
  Ư�� �̹����� �������� ��������Ʈ�� ����
  (�̹����� ���������� �ʴ´�)
---------------------------------------------------------------------------*/
int xSprite::TransToSpr(int xl, int yl, unsigned short *pImage, unsigned short wColorKey)
{
	if(pImage == NULL) return 0;
	if(xl <= 0 || yl <= 0) return 0;
	Remove();
	
	spr = (SPRITE30*) malloc(sizeof(SPRITE30));
	if(spr==NULL) { 
		//PutErrStr("�޸� �Ҵ� ����(At TransToSpr)"); 
		return 0; 
	}

	sfhead.TotalSprNum = 1;
	strcpy(sfhead.Identifier, "93XYZ sprite");
	sfhead.Version = 30;
	sfhead.BPD = 2;
	sfhead.CharacterNum = 0;

	spr[0].compress = 0;
	spr[0].colorkey = wColorKey;
	spr[0].xl       = xl;
	spr[0].yl       = yl;
	spr[0].rx       = 0;
	spr[0].ry       = 0;
	spr[0].mx       = xl/2;
	spr[0].my       = yl/2;
	spr[0].hitNum   = 0;
	spr[0].atckNum  = 0;
	spr[0].hit      = NULL;
	spr[0].atck     = NULL;
	spr[0].dsize    = xl*2 * yl;
	spr[0].data     = pImage;
	spr[0].PixelFormat = 555;

	return 1;
}

/*--------------------------------------------------------------------------
  Ư�� �̹����� �������� ��������Ʈ�� ����
  (�̹��� ���۸� ���������� �Ҵ��Ͽ� ������)
---------------------------------------------------------------------------*/
int xSprite::TransToSprByCopy(int xl, int yl, unsigned short *pImage, unsigned short wColorKey)
{
	if(pImage == NULL) return 0;
	if(xl <= 0 || yl <= 0) return 0;

	unsigned short* pNewImage = new unsigned short[xl * yl];
	memcpy(pNewImage, pImage, sizeof(unsigned short)*(xl*yl));

	if(!TransToSpr(xl, yl, pNewImage, wColorKey)) {
		delete [] pNewImage;
		return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------
  ���ʷ� ��������Ʈ�� �����ϱ� ���� �޸� �Ҵ�(������ �뵵)
---------------------------------------------------------------------------*/
int xSprite::NewSpace(int xl, int yl, unsigned short *pImage, unsigned short wColorKey, int bCompress)
{
	if(pImage == NULL) return 0;
	Remove();
	
	spr = (SPRITE30*) malloc(sizeof(SPRITE30));
	if(spr==NULL) { 
		//PutErrStr("�޸� �Ҵ� ����(At NewSpace)"); 
		return 0; 
	}

	sfhead.TotalSprNum = 1;
	strcpy(sfhead.Identifier, "93XYZ sprite");
	sfhead.Version = 30;
	sfhead.BPD = 2;
	sfhead.CharacterNum = 0;

	memset(&spr[0], 0, sizeof(SPRITE30));

	//spr[0].tot = 1;
	spr[0].compress = 0;
	spr[0].colorkey = wColorKey;
	spr[0].xl       = xl;
	spr[0].yl       = yl;
	spr[0].rx       = 0;
	spr[0].ry       = 0;
	spr[0].mx       = xl/2;
	spr[0].my       = yl/2;
	spr[0].hitNum   = 0;
	spr[0].atckNum  = 0;
	spr[0].hit      = NULL;
	spr[0].atck     = NULL;
	spr[0].dsize    = xl*2 * yl;
	spr[0].data     = pImage;
	spr[0].PixelFormat = 555;
	spr[0].SprFormat = 0;
	spr[0].Dithering = 0;
	spr[0].byUserVal = 0;
	spr[0].pJpgData = NULL;
	spr[0].jpgsize = 0;

	if(bCompress) {
		if(Compress(0)==0) return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------
  ���ο� ��������Ʈ�� ����
---------------------------------------------------------------------------*/
int xSprite::Insert(int snum, int xl, int yl, unsigned short *pImage, unsigned short wColorKey, int bCompress)
{
	if(pImage == NULL) { 
		//PutErrStr("�̹��� �����Ͱ� NULL�Դϴ�(At Insert)"); 
		return 0; 
	}

	if(sfhead.TotalSprNum <= 0) {
		// ���ʷ� ������ ��������Ʈ���?
		if(NewSpace(xl, yl, pImage, wColorKey, 1)==0) return 0;
		return 1;
	}

	if(snum > sfhead.TotalSprNum || snum < 0) { 
		//PutErrStr("��������Ʈ �� ���� ���� �ʰ�(At Insert)"); 
		return 0; 
	}

	SPRITE30 *oldspr = (SPRITE30*) malloc(sizeof(SPRITE30) * sfhead.TotalSprNum);
	if(oldspr==NULL) { 
		//PutErrStr("�޸� �Ҵ� ����(At Insert)"); 
		return 0; 
	}
	memcpy(oldspr, spr, sizeof(SPRITE30) * sfhead.TotalSprNum);
	
	free(spr);
	sfhead.TotalSprNum += 1;
	spr = (SPRITE30*) malloc(sizeof(SPRITE30) * sfhead.TotalSprNum);
	if(spr==NULL) { 
		//PutErrStr("�޸� �Ҵ� ����(At Insert)"); 
		return 0; 
	}

	int i;
	for(i=0; i<snum; i++) memcpy(spr+i, oldspr+i, sizeof(SPRITE30));
	for(i=snum+1; i<sfhead.TotalSprNum; i++) memcpy(spr+i, oldspr+i-1, sizeof(SPRITE30));

	if(oldspr!=NULL) free(oldspr);
	//spr[snum].tot = 1;
	memset(&spr[snum], 0, sizeof(SPRITE30));

	spr[snum].compress = 0;
	spr[snum].colorkey = wColorKey;
	spr[snum].xl       = xl;
	spr[snum].yl       = yl;
	spr[snum].rx       = 0;
	spr[snum].ry       = 0;
	spr[snum].mx       = xl/2;
	spr[snum].my       = yl/2;
	spr[snum].hitNum   = 0;
	spr[snum].atckNum  = 0;
	spr[snum].hit      = NULL;
	spr[snum].atck     = NULL;
	spr[snum].dsize    = xl*2 * yl;
	spr[snum].data     = pImage;
	spr[snum].PixelFormat = 555;
	spr[snum].Dithering = 0;
	spr[snum].byUserVal = 0;
	spr[snum].pJpgData = NULL;
	spr[snum].jpgsize = 0;
	spr[snum].SprFormat = 0;

	if(bCompress) {
		if(Compress(snum)==0) return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------
  ���ο� ��������Ʈ�� ����(���ο� �޸𸮸� �Ҵ��Ͽ� ������)
---------------------------------------------------------------------------*/
int xSprite::InsertByCopy(int snum, int xl, int yl, unsigned short *pImage, unsigned short wColorKey, int bCompress)
{
	if(!pImage) return 0;
	if(xl * yl <= 0) return 0;

	unsigned short* pNewImage = new unsigned short[xl*yl];
	memcpy(pNewImage, pImage, xl*yl*2);

	if(!Insert(snum, xl, yl, pNewImage, wColorKey, bCompress)) {
		delete [] pNewImage;
		return 0;
	}

	return 1;
}

/*--------------------------------------------------------------------------
  JPEG ���� �����ͷ� 16��Ʈ 555 �̹����� ����
---------------------------------------------------------------------------*/
/*
unsigned short* xSprite::Make16bitFromJpeg(LPCTSTR fname, int* width, int* height, int bDither)
{
	FILE *fp = fopen(fname,"rb");
	if(!fp) return NULL;
	unsigned short* pRtn = Make16bitFromJpeg(fp, width, height, bDither);
	fclose(fp);
	return pRtn;
}

unsigned short* xSprite::Make16bitFromJpeg(FILE *fp, int* width, int* height, int bDither)
{
	*width = 0;
	*height = 0;

	unsigned char *pImage24 = JpegFile::JpegFileToRGB(fp, (UINT*)width, (UINT*)height);
	if(!pImage24) return NULL;

	int xsize = *width;
	int ysize = *height;
	if(xsize <= 0 || ysize <= 0) return NULL;

	unsigned short* pNew = NULL;

	if(bDither) {
		// �����? �ɼ��� ������ ���?
		unsigned int rmask = (unsigned int)0x0000001F;
		unsigned int gmask = (unsigned int)0x000003E0;
		unsigned int bmask = (unsigned int)0x00007C00;
		// 24��Ʈ �̹����� ������Ͽ�? 16��Ʈ(5:5:5) �̹����� ��ȯ
		setup_dither_mask(rmask, gmask, bmask, 2);
		dither_rgb24(pImage24, xsize, ysize, (unsigned char**)&pNew);
	}
	else {
		pNew = new unsigned short[xsize * ysize];

		unsigned char* bit = pImage24;
		unsigned short* now = (unsigned short*)pNew;
		unsigned short color16;
		unsigned char r,g,b;

		for(int i=0; i<ysize; i++) {
			// 16��Ʈ�� ��ȯ
			static char k=0;
			k=0;

			for(int j=0; j<xsize; j++) {
				// ����) ���⼱ RGB�������� ��Ʈ���� BGR���̴�
				r = *(bit+(j*3));
				g = *(bit+(j*3)+1);
				b = *(bit+(j*3)+2);

				color16 = RGB555(r,g,b);

				*((unsigned short*)now+j) = color16;
			}
			now += xsize;
			bit += xsize*3;
		}
	}

	delete pImage24;

	return pNew;
}
*/