#ifndef GPU_H_
#define GPU_H_

#define DLLExport __declspec(dllexport)

#include <SDL2/SDL.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <math.h>

typedef signed char      int8_t;
typedef signed short     int16_t;
typedef signed int       int32_t;
typedef signed long long int64_t;

typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned int       uint32_t;
typedef unsigned long long uint64_t;

typedef struct _Font     Font;
typedef struct _Texture  Texture;
typedef struct _Renderer Renderer;


typedef enum _FlipFlag{
    FLIP_NONE       = 0x0000,
    FLIP_HORIZONTAL = 0x0001,
    FLIP_VERTICAL   = 0x0002,
}FlipFlag;

typedef enum _BlendModePreset{
    BLEND_NORMAL,
    BLEND_PREMULTIPLIED_ALPHA,
    BLEND_MULTIPLY,
    BLEND_ADD,
    BLEND_SUBTRACT,
    BLEND_MOD_ALPHA,
    BLEND_SET_ALPHA,
    BLEND_SET,
    BLEND_NORMAL_KEEP_ALPHA,
    BLEND_NORMAL_ADD_ALPHA,
    BLEND_NORMAL_FACTOR_ALPHA,
}BlendModePreset;

typedef enum _BlendFunction{
    FUNC_ZERO                = 0x0000,
    FUNC_ONE                 = 0x0001,
    FUNC_SRC_COLOR           = 0x0300,
    FUNC_ONE_MINUS_SRC_COLOR = 0x0301,
    FUNC_DST_COLOR           = 0x0306,
    FUNC_ONE_MINUS_DST_COLOR = 0x0307,
    FUNC_SRC_ALPHA           = 0x0302,
    FUNC_ONE_MINUS_SRC_ALPHA = 0x0303,
    FUNC_DST_ALPHA           = 0x0304,
    FUNC_ONE_MINUS_DST_ALPHA = 0x0305,
}BlendFunction;

typedef enum _BlendEquation{
    EQ_ADD              = 0x8006,
    EQ_SUBTRACT         = 0x800A,
    EQ_REVERSE_SUBTRACT = 0x800B,
    EQ_MIN              = 0x8007,
    EQ_MAX              = 0x8008,
}BlendEquation;

typedef enum _Filter{
    FILTER_NEAREST                = 0x2600,
    FILTER_LINEAR                 = 0x2601,
    FILTER_NEAREST_MIPMAP_NEAREST = 0x2700,
    FILTER_LINEAR_MIPMAP_NEAREST  = 0x2701,
    FILTER_NEAREST_MIPMAP_LINEAR  = 0x2702,
    FILTER_LINEAR_MIPMAP_LINEAR   = 0x2703,
}Filter;

typedef enum _Wrap{
    WRAP_REPEAT          = 0x2901,
    WRAP_MIRRORED_REPEAT = 0x8370,
    WRAP_CLAMP_TO_EDGE   = 0x812F,
}Wrap;

typedef enum PixelFormat{
    PIXEL_FORMAT_RED             = 0x1903,
    PIXEL_FORMAT_GREEN           = 0x1904,
    PIXEL_FORMAT_BLUE            = 0x1905,
    PIXEL_FORMAT_ALPHA           = 0x1906,
    PIXEL_FORMAT_LUMINANCE       = 0x1909,
    PIXEL_FORMAT_LUMINANCE_ALPHA = 0x190A,
    PIXEL_FORMAT_RG              = 0x8227,
    PIXEL_FORMAT_RGB             = 0x1907,
    PIXEL_FORMAT_RGBA            = 0x1908,
    PIXEL_FORMAT_BGB             = 0x80E0,
    PIXEL_FORMAT_BGBA            = 0x80E1,
}PixelFormat;


typedef struct _Color{
    uint8_t r,g,b,a;
}Color;

typedef struct _Rect{
    float x,y,w,h;
}Rect;

typedef struct _Vector2{
    float x,y;
}Vector2;

typedef struct _Vertex{
    Vector2 position;
    Color color;
    Vector2 texcoord;
}Vertex;

typedef struct _BlendMode{
    uint32_t srcRGB;
    uint32_t dstRGB;

    uint32_t srcAlpha;
    uint32_t dstAlpha;

    uint32_t modeRGB;
    uint32_t modeAlpha;
}BlendMode;


DLLExport BlendMode GetBlendModeByPreset(BlendModePreset preset);

DLLExport void SetBlendFunction(Renderer *renderer,Texture *target,BlendFunction srcRGB,BlendFunction dstRGB,BlendFunction srcAlpha,BlendFunction dstAlpha);

DLLExport void SetBlendEquation(Renderer *renderer,Texture *target,BlendEquation modeRGB,BlendEquation modeAlpha);

DLLExport void SetBlendModeByPreset(Renderer *renderer,Texture *target,BlendModePreset preset);

DLLExport void SetBlendMode(Renderer *renderer,Texture *target,BlendMode blend_mode);

DLLExport void SetBlending(Renderer *renderer,Texture *target,bool blending);


DLLExport void SetClip(Renderer *renderer,Texture *target,Rect *rect);

DLLExport void UnSetClip(Renderer *renderer,Texture *target);


DLLExport Texture* CreateTexture(Renderer *renderer,int width,int height,PixelFormat format,bool mipmap,bool framebuffer);

DLLExport Texture* LoadTexture(Renderer *renderer,const char *fileName,bool mipmap,bool framebuffer);

DLLExport void TextureFree(Texture *texture);

DLLExport void SetTextureFilter(Texture *texture,Filter min_filter,Filter mag_filter);

DLLExport void SetTextureWrap(Texture *texture,Wrap wrap_x,Wrap wrap_y);

DLLExport void GenerateFrameBuffer(Renderer *renderer,Texture *texture);

DLLExport void GenearateMipmap(Renderer *renderer,Texture *texture);

DLLExport void LockTexture(Texture *texture,Rect *rect,void **pixels,int *pitch);

DLLExport void UnlockTexture(Texture *texture);

DLLExport void TextureSize(Texture *texture,int *width,int *height);

DLLExport void TextureFormat(Texture *texture,PixelFormat *format);

DLLExport bool TextureHasFrameBuffer(Texture *texture);

DLLExport bool TextureHasMipmap(Texture *texture);

DLLExport void TextureWrap(Texture *texture,Wrap *wrap_x,Wrap *wrap_y);

DLLExport void TextureFilter(Texture *texture,Filter *min_filter,Filter *mag_filter);



DLLExport Renderer* CreateRenderer(SDL_Window *window,bool vsync);

DLLExport void RendererFree(Renderer *renderer);

DLLExport void RendererSetTarget(Renderer *renderer,Texture *target);

DLLExport void RendererSetVsync(Renderer *renderer,bool value);

DLLExport void RendererSetViewport(Renderer *renderer,Texture *target,Rect *rect);


DLLExport void FlushBlitBuffer(Renderer *renderer);

DLLExport void ClearColor(Renderer *renderer,Color color);

DLLExport void ClearRGBA(Renderer *renderer,uint8_t r,uint8_t g,uint8_t b,uint8_t a);

DLLExport void Flip(Renderer *renderer);



DLLExport void DrawFilledRectangle(Renderer *renderer,Rect *rect,Color color);

DLLExport void DrawFilledRectangles(Renderer *renderer,Rect *rects,unsigned int count,Color color);

DLLExport void DrawRectangle(Renderer *renderer,Rect *rect,Color color);

DLLExport void DrawRectangles(Renderer *renderer,Rect *rects,unsigned int count,Color color);

DLLExport void DrawFilledTriangle(Renderer *renderer,Vector2 vertex1,Vector2 vertex2,Vector2 vertex3,Color color);

DLLExport void DrawTriangle(Renderer *renderer,Vector2 vertex1,Vector2 vertex2,Vector2 vertex3,Color color);

DLLExport void DrawFilledCircle(Renderer *renderer,Vector2 center,float radius,Color color);

DLLExport void DrawCircle(Renderer *renderer,Vector2 center,float radius,Color color);

DLLExport void DrawFilledArc(Renderer *renderer,Vector2 center,float radius,float startAngle,float endAngle,Color color);

DLLExport void DrawArc(Renderer *renderer,Vector2 center,float radius,float startAngle,float endAngle,Color color);

DLLExport void DrawPixel(Renderer *renderer,Vector2 point,Color color);

DLLExport void DrawLine(Renderer *renderer,Vector2 point1,Vector2 point2,Color color);

DLLExport void DrawLines(Renderer *renderer,Vector2 *points,unsigned int points_count,Color color);

DLLExport void DrawPolygon(Renderer *renderer,Vector2 *vertices,unsigned int vertices_count,Color color);

DLLExport void DrawFilledPolygon(Renderer *renderer,Vector2 *vertices,unsigned int vertices_count,Color color);

DLLExport void Geometry(Renderer *renderer,Texture *texture,Vertex *vertices,unsigned int vertices_count,unsigned *indices,unsigned int indices_count);

DLLExport void Blit(Renderer *renderer,Texture *texture,Rect *src_rect,Rect *dst_rect);

DLLExport void BlitEx(Renderer *renderer,Texture *texture,Rect *src_rect,Rect *dst_rect,float angle,Vector2 *center,FlipFlag flags);


DLLExport bool GlyphIsProvided(Font *font,int char_code);

DLLExport void GlyphMetrics(Font *font,int char_code,int *advance,int *minx,int *maxx,int *miny,int *maxy);

DLLExport void SetFontKerning(Font *font,bool allowed);

DLLExport void SetFontHeight(Font *font,int height);

DLLExport Font* OpenFont(const char *fileName,int height);

DLLExport void CloseFont(Font *font);


DLLExport void SizeText(Font *font,const uint16_t *text,int *width,int *height);

DLLExport Texture* RenderGlyph(Renderer *renderer,Font *font,uint16_t char_code,Color color);

DLLExport Texture* RenderText(Renderer *renderer,Font *font,const uint16_t *text,Color color);

DLLExport Texture* RenderTextMono(Renderer *renderer,Font *font,const uint16_t *text,Color color);

#endif