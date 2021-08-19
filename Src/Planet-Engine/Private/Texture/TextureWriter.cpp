#include "TextureWriter.h"

#include <png.h>
#include <cstdio>

#include "Texture/Texture2D.h"
#include "PlanetLogging.h"

void TextureWriter::writeToFile(const char* filepath, const Texture2D& texture)
{
    FILE* fp = fopen(filepath, "wb");
    P_ASSERT(fp != nullptr, "Failed to open file {} for write", filepath);

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, (png_voidp)nullptr, nullptr, nullptr);
    if (!png_ptr)
        return;
    png_infop info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);
        return;
    }
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        fclose(fp);
        return;
    }

    png_init_io(png_ptr, fp);
    png_set_IHDR(png_ptr,
        info_ptr,
        texture.GetWidth(),
        texture.GetHeight(),
        8,
        PNG_COLOR_TYPE_RGB_ALPHA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png_ptr, info_ptr);

    size_t stride = 4 * texture.GetWidth();
    png_bytep data = (png_bytep) texture.GetData();
    for (int y = 0; y < texture.GetHeight(); ++y)
    {
        png_bytep row_pointer = data + (stride * y);
        png_write_row(png_ptr, row_pointer);
    }

    png_write_end(png_ptr, NULL);
    png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
    png_destroy_write_struct(&png_ptr, (png_infopp)nullptr);

    fclose(fp);
}