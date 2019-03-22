#include "xSprite.h"
#include "stdlib.h"
#include "stdio.h"

int main(int argc, char const *argv[])
{
    xSprite m_spr_wnd;
    
    printf("%d",m_spr_wnd.Load((char *)"./SkillEff.spr",555));
    return 0;
}
