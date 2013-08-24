/* By Justin Meiners (2013) */
/* font and image utilties from nothings.org */

#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */

#define STB_TRUETYPE_IMPLEMENTATION 
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

int main(int argc, const char * argv[])
{
    /* load font file */
    long size;
    unsigned char* fontBuffer;
    
    FILE* fontFile = fopen("font.ttf", "rb");
    fseek(fontFile, 0, SEEK_END);
    size = ftell(fontFile); /* how long is the file ? */
    fseek(fontFile, 0, SEEK_SET); /* reset */
    
    fontBuffer = malloc(size);
    
    fread(fontBuffer, size, 1, fontFile);
    fclose(fontFile);

    /* prepare font */
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, fontBuffer, 0))
    {
        printf("failed\n");
    }
    
    int b_w = 512; /* bitmap width */
    int b_h = 128; /* bitmap height */
    int l_h = 64; /* line height */

    /* create a bitmap for the phrase */
    unsigned char* bitmap = malloc(b_w * b_h);
    
    /* calculate font scaling */
    float scale = stbtt_ScaleForPixelHeight(&info, l_h);

    char* word = "how are you?";
    
    int x = 0;
    
    
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    
    ascent *= scale;
    descent *= scale;
    
    int i;
    for (i = 0; i < strlen(word); ++i)
    {
        /* get bounding box for character (may be offset to account for chars that dip above or below the line */
        int c_x1, c_y1, c_x2, c_y2;
        stbtt_GetCodepointBitmapBox(&info, word[i], scale, scale, &c_x1, &c_y1, &c_x2, &c_y2);
        
        /* compute y (different characters have different heights */
        int y = ascent + c_y1;
        
        /* render character (stride and offset is important here) */
        int byteOffset = x + (y  * b_w);
        stbtt_MakeCodepointBitmap(&info, bitmap + byteOffset, c_x2 - c_x1, c_y2 - c_y1, b_w, scale, scale, word[i]);
        
        /* how wide is this character */
        int ax;
        stbtt_GetCodepointHMetrics(&info, word[i], &ax, 0);
        x += ax * scale;
        
        /* add kerning */
        int kern;
        kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
        x += kern * scale;
    }
    
    /* save out a 1 channel image */
    stbi_write_png("out.png", b_w, b_h, 1, bitmap, b_w);
    
    free(fontBuffer);
    free(bitmap);
    
    return 0;
}
