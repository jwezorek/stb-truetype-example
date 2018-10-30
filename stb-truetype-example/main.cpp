/* Created By: Justin Meiners (2013) */
/* C++-ified jwezorek (2018) */

#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <vector>
#include <string>
#include <fstream>


#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" /* http://nothings.org/stb/stb_image_write.h */

#define STB_TRUETYPE_IMPLEMENTATION 
#include "stb_truetype.h" /* http://nothings.org/stb/stb_truetype.h */

std::vector<unsigned char> readFile(const std::string& filename)
{
	// open the file:
	std::ifstream file(filename, std::ios::binary);

	// read the data:
	return std::vector<unsigned char>(
		std::istreambuf_iterator<char>(file),
		std::istreambuf_iterator<char>()
	);
}

int main(int argc, const char* argv[])
{
    /* load font file */
    std::vector<unsigned char> fontBuffer = readFile("font/cmunrm.ttf");

    /* prepare font */
    stbtt_fontinfo info;
    if (!stbtt_InitFont(&info, &(fontBuffer[0]), 0))
		std::cout << "failed" << std::endl;
    
    int b_w = 512; /* bitmap width */
    int b_h = 128; /* bitmap height */
    int l_h = 64; /* line height */

    /* create a bitmap for the phrase */
	std::vector<unsigned char> bitmap_buffer(b_w * b_h);
	auto bitmap = &(bitmap_buffer[0]);
    
    /* calculate font scaling */
    float scale = stbtt_ScaleForPixelHeight(&info, static_cast<float>(l_h) );

    std::string word = "how are you?";
    
    int x = 0;
       
    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&info, &ascent, &descent, &lineGap);
    
    ascent = static_cast<int>(ascent * scale);
    descent = static_cast<int>(descent * scale);
    
    for (size_t i = 0; i < word.size(); ++i)
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
        x += static_cast<int>(ax * scale);
        
        /* add kerning */
        int kern;
        kern = stbtt_GetCodepointKernAdvance(&info, word[i], word[i + 1]);
        x += static_cast<int>(kern * scale);
    }
    
    /* save out a 1 channel image */
    stbi_write_png("out.png", b_w, b_h, 1, bitmap, b_w);
   
    return 0;
}

