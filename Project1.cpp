//---------------------------------------------------------------------------
// Project1.cpp
// Симуляция реального мира: персонаж, камера 3-го лица, астрономия,
// вода, тени, облака. OpenGL 4.x Core, без внешних библиотек.
// C++ Builder 13, компилятор bcc64x.
//
// ФАЙЛ СОБИРАЕТСЯ ИЗ 4 ЧАСТЕЙ. Это часть 1.
//---------------------------------------------------------------------------

#include <windows.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#pragma hdrstop

#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

// ======================= Типы OpenGL =======================
typedef unsigned int    GLuint;
typedef unsigned int    GLenum;
typedef int             GLint;
typedef int             GLsizei;
typedef unsigned int    GLbitfield;
typedef unsigned char   GLboolean;
typedef char            GLchar;
typedef ptrdiff_t       GLsizeiptr;
typedef float           GLfloat;

#ifndef APIENTRYP
#define APIENTRYP WINAPI *
#endif

// Константы OpenGL
#define GL_ARRAY_BUFFER                  0x8892
#define GL_ELEMENT_ARRAY_BUFFER          0x8893
#define GL_STATIC_DRAW                   0x88E4
#define GL_DYNAMIC_DRAW                  0x88E5
#define GL_FRAGMENT_SHADER               0x8B30
#define GL_VERTEX_SHADER                 0x8B31
#define GL_COMPILE_STATUS                0x8B81
#define GL_LINK_STATUS                   0x8B82
#define GL_DEPTH_TEST                    0x0B71
#define GL_CULL_FACE                     0x0B44
#define GL_COLOR_BUFFER_BIT              0x00004000
#define GL_DEPTH_BUFFER_BIT              0x00000100
#define GL_TRIANGLES                     0x0004
#define GL_TRIANGLE_STRIP                0x0005
#define GL_FLOAT                         0x1406
#define GL_FALSE                         0
#define GL_TRUE                          1
#define GL_BACK                          0x0405
#define GL_FRONT                         0x0404
#define GL_CW                            0x0900
#define GL_CCW                           0x0901
#define GL_TEXTURE_2D                    0x0DE1
#define GL_RGBA                          0x1908
#define GL_UNSIGNED_BYTE                 0x1401
#define GL_NEAREST                       0x2600
#define GL_LINEAR                        0x2601
#define GL_REPEAT                        0x2901
#define GL_CLAMP_TO_EDGE                 0x812F
#define GL_TEXTURE_MIN_FILTER            0x2801
#define GL_TEXTURE_MAG_FILTER            0x2800
#define GL_TEXTURE_WRAP_S                0x2802
#define GL_TEXTURE_WRAP_T                0x2803
#define GL_FRAMEBUFFER                   0x8D40
#define GL_COLOR_ATTACHMENT0             0x8CE0
#define GL_DEPTH_ATTACHMENT              0x8D00
#define GL_DEPTH_COMPONENT               0x1902
#define GL_FRAMEBUFFER_COMPLETE          0x8CD5
#define GL_TEXTURE_COMPARE_MODE          0x884C
#define GL_TEXTURE_COMPARE_FUNC          0x884D
#define GL_COMPARE_REF_TO_TEXTURE        0x884E
#define GL_LESS                          0x0201

// ======================= Указатели на функции OpenGL =======================
typedef GLuint (APIENTRYP PFN_glCreateShaderPROC)(GLenum);
typedef void  (APIENTRYP PFN_glShaderSourcePROC)(GLuint, GLsizei, const GLchar* const*, const GLint*);
typedef void  (APIENTRYP PFN_glCompileShaderPROC)(GLuint);
typedef GLuint(APIENTRYP PFN_glCreateProgramPROC)(void);
typedef void  (APIENTRYP PFN_glAttachShaderPROC)(GLuint, GLuint);
typedef void  (APIENTRYP PFN_glLinkProgramPROC)(GLuint);
typedef void  (APIENTRYP PFN_glUseProgramPROC)(GLuint);
typedef void  (APIENTRYP PFN_glGetShaderivPROC)(GLuint, GLenum, GLint*);
typedef void  (APIENTRYP PFN_glGetProgramivPROC)(GLuint, GLenum, GLint*);
typedef void  (APIENTRYP PFN_glGetShaderInfoLogPROC)(GLuint, GLsizei, GLsizei*, GLchar*);
typedef void  (APIENTRYP PFN_glGenVertexArraysPROC)(GLsizei, GLuint*);
typedef void  (APIENTRYP PFN_glBindVertexArrayPROC)(GLuint);
typedef void  (APIENTRYP PFN_glGenBuffersPROC)(GLsizei, GLuint*);
typedef void  (APIENTRYP PFN_glBindBufferPROC)(GLenum, GLuint);
typedef void  (APIENTRYP PFN_glBufferDataPROC)(GLenum, GLsizeiptr, const void*, GLenum);
typedef void  (APIENTRYP PFN_glVertexAttribPointerPROC)(GLuint, GLint, GLenum, GLboolean, GLsizei, const void*);
typedef void  (APIENTRYP PFN_glEnableVertexAttribArrayPROC)(GLuint);
typedef GLint (APIENTRYP PFN_glGetUniformLocationPROC)(GLuint, const GLchar*);
typedef void  (APIENTRYP PFN_glUniformMatrix4fvPROC)(GLint, GLsizei, GLboolean, const GLfloat*);
typedef void  (APIENTRYP PFN_glUniform3fPROC)(GLint, GLfloat, GLfloat, GLfloat);
typedef void  (APIENTRYP PFN_glUniform1fPROC)(GLint, GLfloat);
typedef void  (APIENTRYP PFN_glUniform1iPROC)(GLint, GLint);
typedef void  (APIENTRYP PFN_glDeleteVertexArraysPROC)(GLsizei, const GLuint*);
typedef void  (APIENTRYP PFN_glDeleteBuffersPROC)(GLsizei, const GLuint*);
typedef void  (APIENTRYP PFN_glViewportPROC)(GLint, GLint, GLsizei, GLsizei);
typedef void  (APIENTRYP PFN_glClearPROC)(GLbitfield);
typedef void  (APIENTRYP PFN_glClearColorPROC)(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void  (APIENTRYP PFN_glEnablePROC)(GLenum);
typedef void  (APIENTRYP PFN_glDisablePROC)(GLenum);
typedef void  (APIENTRYP PFN_glDrawArraysPROC)(GLenum, GLint, GLsizei);
typedef void  (APIENTRYP PFN_glDrawElementsPROC)(GLenum, GLsizei, GLenum, const void*);
typedef void  (APIENTRYP PFN_glDepthMaskPROC)(GLboolean);
typedef void  (APIENTRYP PFN_glBlendFuncPROC)(GLenum, GLenum);
typedef void  (APIENTRYP PFN_glActiveTexturePROC)(GLenum);
typedef void  (APIENTRYP PFN_glBindTexturePROC)(GLenum, GLuint);
typedef void  (APIENTRYP PFN_glGenTexturesPROC)(GLsizei, GLuint*);
typedef void  (APIENTRYP PFN_glTexImage2DPROC)(GLenum, GLint, GLint, GLsizei, GLsizei, GLint, GLenum, GLenum, const void*);
typedef void  (APIENTRYP PFN_glTexParameteriPROC)(GLenum, GLenum, GLint);
typedef void  (APIENTRYP PFN_glGenFramebuffersPROC)(GLsizei, GLuint*);
typedef void  (APIENTRYP PFN_glBindFramebufferPROC)(GLenum, GLuint);
typedef void  (APIENTRYP PFN_glFramebufferTexture2DPROC)(GLenum, GLenum, GLenum, GLuint, GLint);
typedef GLenum(APIENTRYP PFN_glCheckFramebufferStatusPROC)(GLenum);
typedef void  (APIENTRYP PFN_glDeleteProgramPROC)(GLuint);

static PFN_glCreateShaderPROC             glCreateShader;
static PFN_glShaderSourcePROC             glShaderSource;
static PFN_glCompileShaderPROC            glCompileShader;
static PFN_glCreateProgramPROC            glCreateProgram;
static PFN_glAttachShaderPROC             glAttachShader;
static PFN_glLinkProgramPROC              glLinkProgram;
static PFN_glUseProgramPROC               glUseProgram;
static PFN_glGetShaderivPROC              glGetShaderiv;
static PFN_glGetProgramivPROC             glGetProgramiv;
static PFN_glGetShaderInfoLogPROC         glGetShaderInfoLog;
static PFN_glGenVertexArraysPROC          glGenVertexArrays;
static PFN_glBindVertexArrayPROC          glBindVertexArray;
static PFN_glGenBuffersPROC               glGenBuffers;
static PFN_glBindBufferPROC               glBindBuffer;
static PFN_glBufferDataPROC               glBufferData;
static PFN_glVertexAttribPointerPROC      glVertexAttribPointer;
static PFN_glEnableVertexAttribArrayPROC  glEnableVertexAttribArray;
static PFN_glGetUniformLocationPROC       glGetUniformLocation;
static PFN_glUniformMatrix4fvPROC         glUniformMatrix4fv;
static PFN_glUniform3fPROC                glUniform3f;
static PFN_glUniform1fPROC                glUniform1f;
static PFN_glUniform1iPROC                glUniform1i;
static PFN_glDeleteVertexArraysPROC       glDeleteVertexArrays;
static PFN_glDeleteBuffersPROC            glDeleteBuffers;
static PFN_glViewportPROC                 glViewport;
static PFN_glClearPROC                    glClear;
static PFN_glClearColorPROC               glClearColor;
static PFN_glEnablePROC                   glEnable;
static PFN_glDisablePROC                  glDisable;
static PFN_glDrawArraysPROC               glDrawArrays;
static PFN_glDrawElementsPROC             glDrawElements;
static PFN_glDepthMaskPROC                glDepthMask;
static PFN_glBlendFuncPROC                glBlendFunc;
static PFN_glActiveTexturePROC            glActiveTexture;
static PFN_glBindTexturePROC              glBindTexture;
static PFN_glGenTexturesPROC              glGenTextures;
static PFN_glTexImage2DPROC               glTexImage2D;
static PFN_glTexParameteriPROC            glTexParameteri;
static PFN_glGenFramebuffersPROC          glGenFramebuffers;
static PFN_glBindFramebufferPROC          glBindFramebuffer;
static PFN_glFramebufferTexture2DPROC     glFramebufferTexture2D;
static PFN_glCheckFramebufferStatusPROC   glCheckFramebufferStatus;
static PFN_glDeleteProgramPROC            glDeleteProgram;

#define GL_BLEND                         0x0BE2
#define GL_SRC_ALPHA                     0x0302
#define GL_ONE_MINUS_SRC_ALPHA           0x0303
#define GL_TEXTURE0                      0x84C0
#define GL_LEQUAL                        0x0203
#define GL_DEPTH_FUNC                    0x0B74

typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARB)(HDC, HGLRC, const int*);
typedef BOOL  (WINAPI *PFNWGLSWAPINTERVALEXTPROC)(int);

// Загрузка функций (вызывать ПОСЛЕ активации современного контекста!)
static bool LoadOpenGLFunctions()
{
    HMODULE hOgl32 = LoadLibraryW(L"opengl32.dll");
    if (!hOgl32) return false;

#define TRY_GET(x) \
    x = (decltype(x))wglGetProcAddress(#x); \
    if (!x) x = (decltype(x))GetProcAddress(hOgl32, #x);

    TRY_GET(glCreateShader)   TRY_GET(glShaderSource)   TRY_GET(glCompileShader)
    TRY_GET(glCreateProgram)  TRY_GET(glAttachShader)   TRY_GET(glLinkProgram)
    TRY_GET(glUseProgram)     TRY_GET(glGetShaderiv)    TRY_GET(glGetProgramiv)
    TRY_GET(glGetShaderInfoLog) TRY_GET(glGenVertexArrays) TRY_GET(glBindVertexArray)
    TRY_GET(glGenBuffers)     TRY_GET(glBindBuffer)     TRY_GET(glBufferData)
    TRY_GET(glVertexAttribPointer) TRY_GET(glEnableVertexAttribArray)
    TRY_GET(glGetUniformLocation) TRY_GET(glUniformMatrix4fv)
    TRY_GET(glUniform3f)      TRY_GET(glUniform1f)      TRY_GET(glUniform1i)
    TRY_GET(glDeleteVertexArrays) TRY_GET(glDeleteBuffers)
    TRY_GET(glViewport)       TRY_GET(glClear)          TRY_GET(glClearColor)
    TRY_GET(glEnable)         TRY_GET(glDisable)        TRY_GET(glDrawArrays)
    TRY_GET(glDrawElements)   TRY_GET(glDepthMask)      TRY_GET(glBlendFunc)
    TRY_GET(glActiveTexture)  TRY_GET(glBindTexture)    TRY_GET(glGenTextures)
    TRY_GET(glTexImage2D)     TRY_GET(glTexParameteri)
    TRY_GET(glGenFramebuffers) TRY_GET(glBindFramebuffer)
    TRY_GET(glFramebufferTexture2D) TRY_GET(glCheckFramebufferStatus)
    TRY_GET(glDeleteProgram)
#undef TRY_GET

    return !(!glCreateShader || !glCreateProgram || !glBufferData ||
             !glDrawArrays || !glGetUniformLocation ||
             !glTexImage2D || !glBindFramebuffer);
}

// ======================= Математика =======================
struct Vec3 { float x=0, y=0, z=0; };

struct Mat4 {
    float m[16];
    Mat4() { memset(m, 0, sizeof(m)); m[0]=m[5]=m[10]=m[15]=1.0f; }
};

static Mat4 operator*(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int c = 0; c < 4; c++)
        for (int rw = 0; rw < 4; rw++) {
            float s = 0;
            for (int k = 0; k < 4; k++)
                s += a.m[k*4+rw] * b.m[c*4+k];
            r.m[c*4+rw] = s;
        }
    return r;
}

static Mat4 Perspective(float fovy, float aspect, float nz, float fz)
{
    Mat4 r; float f = 1.0f/tanf(fovy*0.5f);
    r.m[0]=f/aspect; r.m[5]=f;
    r.m[10]=(fz+nz)/(nz-fz); r.m[11]=-1;
    r.m[14]=2*fz*nz/(nz-fz); r.m[15]=0;
    return r;
}

static Mat4 Ortho(float l, float r_, float b, float t, float nz, float fz)
{
    Mat4 m;
    m.m[0]=2/(r_-l);  m.m[5]=2/(t-b);  m.m[10]=-2/(fz-nz);
    m.m[12]=-(r_+l)/(r_-l); m.m[13]=-(t+b)/(t-b); m.m[14]=-(fz+nz)/(fz-nz);
    return m;
}

static Mat4 LookAt(Vec3 eye, Vec3 center, Vec3 up)
{
    Vec3 f = { center.x-eye.x, center.y-eye.y, center.z-eye.z };
    float fl = sqrtf(f.x*f.x+f.y*f.y+f.z*f.z); f.x/=fl; f.y/=fl; f.z/=fl;
    Vec3 s = { f.y*up.z-f.z*up.y, f.z*up.x-f.x*up.z, f.x*up.y-f.y*up.x };
    float sl = sqrtf(s.x*s.x+s.y*s.y+s.z*s.z); s.x/=sl; s.y/=sl; s.z/=sl;
    Vec3 u = { s.y*f.z-s.z*f.y, s.z*f.x-s.x*f.z, s.x*f.y-s.y*f.x };
    Mat4 r;
    r.m[0]=s.x; r.m[4]=s.y; r.m[8] =s.z;
    r.m[1]=u.x; r.m[5]=u.y; r.m[9] =u.z;
    r.m[2]=-f.x;r.m[6]=-f.y;r.m[10]=-f.z;
    r.m[12]=-(s.x*eye.x+s.y*eye.y+s.z*eye.z);
    r.m[13]=-(u.x*eye.x+u.y*eye.y+u.z*eye.z);
    r.m[14]= (f.x*eye.x+f.y*eye.y+f.z*eye.z);
    return r;
}

// Перенос * поворот вокруг Y * поворот вокруг X * масштаб
static Mat4 ModelTRS(float tx,float ty,float tz,
                     float ry, float rx,
                     float sx,float sy,float sz)
{
    float cy=cosf(ry), sy_=sinf(ry), cx=cosf(rx), sx_=sinf(rx);
    Mat4 r;
    // RY * RX
    r.m[0]=cy;      r.m[4]=sy_*sx_; r.m[8]=sy_*cx;
    r.m[1]=0;       r.m[5]=cx;      r.m[9]=-sx_;
    r.m[2]=-sy_;    r.m[6]=cy*sx_;  r.m[10]=cy*cx;
    // масштаб
    for (int i=0;i<3;i++){ r.m[i]*=sx; r.m[4+i]*=sy; r.m[8+i]*=sz; }
    r.m[12]=tx; r.m[13]=ty; r.m[14]=tz;
    return r;
}

// ======================= Глобальное состояние =======================
HWND  g_hwnd  = nullptr;
HDC   g_hdc   = nullptr;
HGLRC g_hglrc = nullptr;
bool  g_running = true;
bool  g_mouseCaptured = false;

float camYaw = M_PI;      // направление взгляда камеры (вокруг Y)
float camPitch = -0.25f;  // наклон камеры
float g_camDist = 7.0f;
Vec3  g_camPos;
POINT g_lastMouse;
bool  g_firstMouse = true;
bool  g_keys[256] = {};

// --- Опции (сохраняются в options.ini) ---
struct Options {
    bool  fullscreen   = false;
    bool  vsync        = true;
    int   fsWidth      = 1920;
    int   fsHeight     = 1080;
    bool  pauseTime    = false;
    float starSize     = 1.0f;
    float drawDistance = 300.0f;
};
Options g_opt;

// --- Время суток ---
float g_timeOfDay = 0.35f;   // 0..1, 0.25 = полдень, 0.75 = полночь
bool  g_timeForward = true;

// --- Персонаж ---
struct Character {
    Vec3 pos = {0, 0, 10};
    float yaw = 0.0f;
    float vy = 0.0f;
    bool onGround = true;
    float walkPhase = 0.0f;
    float speedFactor = 0.0f;
};
Character g_player;

const float GRAVITY    = -22.0f;
const float JUMP_VEL   = 8.5f;
const float WALK_SPEED = 4.0f;
const float RUN_SPEED  = 9.0f;

// --- Астрономия (вычисляется в RenderFrame) ---
Vec3  g_sunDir;      // направление НА солнце
Vec3  g_moonDir;     // направление НА луну
float g_sunHeight;   // sin высоты солнца (-1..1)
float g_moonPhase;   // 0..1, 0.5 = полнолуние
float g_cloudCover = 0.35f;  // облачность 0..1

// ======================= Шейдеры =======================
// Общий вершинный шейдер для всех объектов мира
static const char* VS_OBJ = R"(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
layout(location=2) in vec3 aColor;
uniform mat4 uProj, uView, uModel;
uniform mat4 uLightMat;              // для shadow map
out vec3 vNormal; out vec3 vWorldPos; out vec3 vColor;
out vec4 vShadowCoord;
void main() {
    vec4 wp = uModel * vec4(aPos, 1.0);
    vWorldPos = wp.xyz;
    vNormal = mat3(uModel) * aNormal;
    vColor = aColor;
    vShadowCoord = uLightMat * wp;
    gl_Position = uProj * uView * wp;
}
)";

// Скайдом: большой куб вокруг камеры, вся краска неба во фрагментном шейдере
static const char* VS_SKY = R"(
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uProj, uViewRotOnly;
uniform vec3 uCamPos;
out vec3 vDir;
void main() {
    vec3 p = aPos * 400.0 + uCamPos;
    vDir = aPos;
    gl_Position = (uProj * uViewRotOnly) * vec4(p, 1.0);
}
)";

// Фрагментный шейдер неба: градиент, солнце, гало, луна с фазами,
// звёзды, два слоя облаков
static const char* FS_SKY = R"(
#version 330 core
in vec3 vDir;
out vec4 fragColor;

uniform vec3  uSunDir;
uniform vec3  uMoonDir;
uniform float uTimeOfDay;   // 0..1
uniform float uStarSize;
uniform float uCloudCover;
uniform float uTime;        // для анимации облаков
uniform vec3  uCamPos;

// ---------- Хэши/шум ----------
float hash21(vec2 p){ p = fract(p*vec2(123.34, 456.21)); p += dot(p,p+45.32); return fract(p.x*p.y); }
float noise2(vec2 p){
    vec2 i = floor(p), f = fract(p);
    f = f*f*(3.0-2.0*f);
    float a = hash21(i), b = hash21(i+vec2(1,0));
    float c = hash21(i+vec2(0,1)), d = hash21(i+vec2(1,1));
    return mix(mix(a,b,f.x), mix(c,d,f.x), f.y);
}
float fbm(vec2 p){
    float v = 0.0, a = 0.5;
    for(int i=0;i<5;i++){ v += a*noise2(p); p *= 2.03; a *= 0.5; }
    return v;
}

// ---------- Звёзды ----------
// Процедурные звёзды в системе небесной сферы.
vec3 starField(vec3 dir){
    // Небесная сфера вращается со звёздным временем:
    // угол вращения пропорционален времени суток
    float rot = uTimeOfDay * 6.2831853;
    mat3 R = mat3(cos(rot),0,-sin(rot), 0,1,0, sin(rot),0,cos(rot));
    vec3 d = normalize(R * dir);
    // Ячейки на сфере
    vec2 uv = vec2(atan(d.z, d.x)*0.15915494, asin(clamp(d.y,-1.,1.))*0.31830989);
    uv *= 60.0;
    vec2 cell = floor(uv);
    float best = 0.0;
    for(int oy=-1; oy<=1; oy++)
    for(int ox=-1; ox<=1; ox++){
        vec2 c = cell + vec2(ox,oy);
        float h = hash21(c);
        if (h > 0.93) {
            vec2 sp = c + vec2(hash21(c+7.1), hash21(c+3.7));
            float dist = length(uv - sp);
            // Размер звезды зависит от опции starSize
            float sz = (0.05 + h*0.08) * max(uStarSize, 0.01);
            float br = smoothstep(sz, 0.0, dist);
            // Мерцание
            br *= 0.7 + 0.3*sin(uTime*3.0 + h*50.0);
            best = max(best, br);
        }
    }
    // Цвета звёзд слегка разные
    float t = hash21(cell);
    vec3 col = mix(vec3(0.75,0.85,1.0), vec3(1.0,0.9,0.7), t);
    return col * best;
}

void main(){
    vec3 dir = normalize(vDir);
    float sunH = uSunDir.y;                       // -1..1 высота солнца
    float dayF = clamp((sunH + 0.1) / 0.25, 0.0, 1.0);   // плавный день/ночь
    float duskF = clamp(1.0 - abs(sunH)/0.35, 0.0, 1.0); // близость к закату

    // ---------- Градиентное небо ----------
    float h = clamp(dir.y, -0.1, 1.0);
    vec3 dayZenith  = vec3(0.18, 0.42, 0.80);
    vec3 dayHorizon = vec3(0.62, 0.78, 0.92);
    vec3 nightZenith  = vec3(0.008, 0.010, 0.028);
    vec3 nightHorizon = vec3(0.02, 0.03, 0.07);
    vec3 zenith = mix(nightZenith, dayZenith, dayF);
    vec3 horizon= mix(nightHorizon, dayHorizon, dayF);
    vec3 sky = mix(horizon, zenith, pow(max(h,0.0), 0.55));

    // Закатная подсветка горизонта в сторону солнца
    float sunAz = dot(normalize(vec3(uSunDir.x,0,uSunDir.z)+1e-5),
                      normalize(vec3(dir.x,0,dir.z)+1e-5));
    float sunsetGlow = duskF * pow(max(sunAz*0.5+0.5, 0.0), 3.0) * pow(1.0-abs(h), 3.0);
    sky += vec3(0.9, 0.35, 0.12) * sunsetGlow * 0.55;

    // Лёгкая пасмурная десатурация
    float gray = dot(sky, vec3(0.299,0.587,0.114));
    sky = mix(sky, vec3(gray)*1.05 + vec3(0.04), uCloudCover*0.45);

    // ---------- Солнце ----------
    float sd = dot(dir, uSunDir);
    float sunDisc = smoothstep(0.99955, 0.99985, sd);
    // Гало — неярко выраженное
    float halo = pow(max(sd,0.0), 180.0) * 0.35 + pow(max(sd,0.0), 900.0) * 0.5;
    vec3 sunColDay = vec3(1.0, 0.96, 0.86);
    vec3 sunColLow = vec3(1.0, 0.45, 0.15);
    vec3 sunCol = mix(sunColLow, sunColDay, clamp(sunH*4.0+0.5, 0.0, 1.0));
    float sunVis = smoothstep(-0.06, 0.02, sunH);  // солнце садится под горизонт
    // Адаптивное затемнение: чем ярче источник в кадре, тем сильнее тонирование
    float adapt = 1.0 / (1.0 + (sunDisc + halo*0.5) * 2.2);
    sky *= mix(1.0, 0.72, adapt * sunDisc);
    sky += sunCol * (sunDisc*1.2 + halo) * sunVis * (1.0 - uCloudCover*0.85);

    // ---------- Луна ----------
    float md = dot(dir, uMoonDir);
    float moonDisc = smoothstep(0.9993, 0.9996, md);
    if (moonDisc > 0.0) {
        // Локальные координаты диска луны
        vec3 mx = normalize(cross(uMoonDir, vec3(0,1,0)));
        vec3 my = cross(mx, uMoonDir);
        vec2 muv = vec2(dot(dir,mx), dot(dir,my)) * 130.0;   // диск радиуса ~1
        float r2 = dot(muv,muv);
        if (r2 < 1.0) {
            // Кратеры — процедурные пятна
            float crat = fbm(muv*6.0+13.7)*0.5 + fbm(muv*14.0+3.1)*0.5;
            vec3 moonBase = vec3(0.92, 0.87, 0.72);          // желтоватый оттенок
            moonBase -= vec3(0.35,0.33,0.30) * smoothstep(0.35, 0.75, crat);
            // Освещённость фазы: терминатор — плоскость, перпенд. биссектрисе солнце/луна
            vec3 toSun = normalize(uSunDir + uMoonDir*0.001 + vec3(0.0,0.0,0.0));
            // Направление освещения луны солнцем относительно наблюдателя:
            // фазовый угол через разность азимутов уже зашита в uMoonDir/uSunDir.
            // Терминатор: точка диска светла, если её нормаль смотрит на солнце.
            vec3 nrm = normalize(uMoonDir + mx*muv.x*0.999 + my*muv.y*0.999);
            float lit = clamp(dot(nrm, normalize(uSunDir))*1.6 + 0.25, 0.0, 1.0);
            // Днём тёмная часть подкрашивается лазурью неба
            vec3 darkSide = mix(vec3(0.01,0.01,0.02), vec3(0.25,0.45,0.70)*dayF, 0.85);
            vec3 moonCol = mix(darkSide, moonBase*(0.35+0.65*lit), lit*0.92+0.08);
            moonCol = mix(moonBase*lit, moonCol, 1.0);
            // Итоговая видимость луны (видна и днём, слабее)
            float moonVis = 1.0;
            sky = mix(sky, moonCol, moonDisc*moonVis);
            sky += vec3(0.4,0.42,0.5) * pow(max(md,0.0), 400.0) * 0.12; // лунное гало
        }
    }

    // ---------- Звёзды (не перекрывают яркое небо днём) ----------
    float starVis = (1.0 - dayF) * (1.0 - clamp(uCloudCover*1.4-0.2,0.0,1.0));
    if (starVis > 0.002) {
        vec3 st = starField(dir);
        // Не рисуем звёзды поверх диска луны/солнца
        float mask = 1.0 - max(moonDisc, smoothstep(0.999,1.0,sd));
        sky += st * starVis * mask;
    }

    // ---------- Облака: два слоя ----------
    // Проекция направления на слои на высоте
    float cover = uCloudCover;
    for (int layer = 0; layer < 2; layer++) {
        float ly = (layer==0) ? 250.0 : 500.0;
        float sc = (layer==0) ? 0.0016 : 0.0007;
        float spd = (layer==0) ? 0.010 : 0.006;
        if (dir.y > 0.02) {
            vec2 cuv = dir.xz / dir.y * ly * sc;
            cuv += uTime * spd * vec2(1.0, 0.3);
            float cl = fbm(cuv + fbm(cuv*2.0)*0.5);
            float th = (layer==0)? 0.52 : 0.62;
            float cm = smoothstep(th - 0.12*(1.0-cover), th + 0.12, cl + (cover-0.5)*0.5);
            cm *= smoothstep(0.02, 0.12, dir.y);  // нет облаков у самого горизонта
            // Цвет облака: белый днём, закатная подсветка снизу
            vec3 cloudLit = mix(vec3(0.06,0.07,0.10), vec3(1.0), dayF);
            cloudLit += sunCol * duskF * 0.5 * (layer==0 ? 1.0 : 0.4);
            // Тени между слоями: нижний слой затемняется верхним
            if (layer == 0) {
                // грубо: если верхний слой облачен в этой точке, притемнить
                vec2 cuvTop = dir.xz / dir.y * 500.0 * 0.0007 + uTime*0.006*vec2(1.0,0.3);
                float clT = fbm(cuvTop + fbm(cuvTop*2.0)*0.5);
                float topM = smoothstep(0.62-0.12*(1.0-cover), 0.62+0.12, clT+(cover-0.5)*0.5);
                cloudLit *= 1.0 - topM*0.35;
            }
            sky = mix(sky, cloudLit, cm * ((layer==0)?0.95:0.55));
        }
    }

    fragColor = vec4(sky, 1.0);
}
)";

// Фрагментный шейдер объектов мира (освещение солнцем/луной + PCF-тени + туман)
static const char* FS_OBJ = R"(
#version 330 core
in vec3 vNormal; in vec3 vWorldPos; in vec3 vColor;
in vec4 vShadowCoord;
out vec4 fragColor;

uniform vec3 uSunDir;
uniform vec3 uMoonDir;
uniform vec3 uSunCol;
uniform vec3 uAmbient;
uniform vec3 uCamPos;
uniform int  uUnlit;
uniform sampler2DShadow uShadowMap;
uniform float uDrawDist;
uniform vec3  uFogColor;
uniform float uWaterLevel;

float ShadowPCF(vec4 sc){
    if (sc.w <= 0.0) return 1.0;
    vec3 pc = sc.xyz / sc.w * 0.5 + 0.5;
    if (pc.x<0||pc.x>1||pc.y<0||pc.y>1||pc.z>1) return 1.0;
    float sum = 0.0;
    float texel = 1.0/4096.0;
    for(int y=-1;y<=1;y++)
    for(int x=-1;x<=1;x++){
        sum += texture(uShadowMap, vec3(pc.xy + vec2(x,y)*texel*1.5, pc.z - 0.0025));
    }
    return sum / 9.0;
}

void main(){
    if (uUnlit == 1) { fragColor = vec4(vColor,1.0); return; }
    vec3 N = normalize(vNormal);

    // Солнечный свет
    float sunDiff = max(dot(N, uSunDir), 0.0);
    float moonDiff = max(dot(N, uMoonDir), 0.0);
    float sh = ShadowPCF(vShadowCoord);
    vec3 light = uSunCol * (sunDiff * sh + moonDiff*0.22);
    light += uAmbient;

    vec3 V = normalize(uCamPos - vWorldPos);
    vec3 H = normalize(V + (sunDiff>moonDiff ? uSunDir : uMoonDir));


    float spec = pow(max(dot(N,H),0.0), 32.0) * sunDiff * sh;
    light += uSunCol * spec * 0.3;

    vec3 col = vColor * light;

    // Туман (экспоненциальный)
    float dist = length(vWorldPos - uCamPos);
    float fogF = 1.0 - exp(-dist / max(uDrawDist*0.55, 10.0));
    fragColor = vec4(mix(col, uFogColor, clamp(fogF,0.0,1.0)), 1.0);
}
)";

// Шейдер воды: волны в вершиннике, отражение неба через процедурную функцию
// неба упрощённо + френель
static const char* VS_WATER = R"(
#version 330 core
layout(location=0) in vec2 aXZ;
uniform mat4 uProj, uView;
uniform float uTime;
out vec3 vWorldPos; out vec3 vNormal; out vec4 vShadowCoord;
uniform mat4 uLightMat;
void main(){
    vec3 p = vec3(aXZ.x, 0.0, aXZ.y);
    float t = uTime;
    float w1 = sin(p.x*0.35 + t*1.4)*0.06 + sin(p.z*0.28 - t*1.1)*0.05;
    float w2 = sin((p.x+p.z)*0.12 + t*0.7)*0.09;
    p.y += w1 + w2;
    // Нормаль через конечные разности
    float e = 0.5;
    auto H = [&](float x, float z){
        return sin(x*0.35+t*1.4)*0.06 + sin(z*0.28-t*1.1)*0.05
             + sin((x+z)*0.12+t*0.7)*0.09;
    };
    vec3 dx = vec3(e, H(p.x+e,p.z)-p.y, 0);
    vec3 dz = vec3(0, H(p.x,p.z+e)-p.y, e);
    vNormal = normalize(cross(dz, dx));
    vec4 wp = vec4(p,1.0);
    vWorldPos = wp.xyz;
    vShadowCoord = uLightMat * wp;
    gl_Position = uProj*uView*wp;
}
)";

static const char* FS_WATER = R"(
#version 330 core
in vec3 vWorldPos; in vec3 vNormal; in vec4 vShadowCoord;
out vec4 fragColor;
uniform vec3 uCamPos, uSunDir, uSunCol, uAmbient, uSkyZenith, uSkyHorizon, uFogColor;
uniform float uDrawDist, uDayF;

void main(){
    vec3 N = normalize(vNormal);
    vec3 V = normalize(uCamPos - vWorldPos);

    // Отражённый взгляд → цвет неба
    vec3 R = reflect(-V, N);
    R.y = abs(R.y);
    vec3 skyRef = mix(uSkyHorizon, uSkyZenith, pow(clamp(R.y,0.0,1.0), 0.6));

    // Солнечная дорожка
    float sd = max(dot(R, uSunDir), 0.0);
    skyRef += uSunCol * pow(sd, 250.0) * 2.0;

    // Френель: чем положе смотрим, тем больше отражение
    float fres = pow(1.0 - max(dot(V,N),0.0), 3.0);
    vec3 deep = mix(vec3(0.02,0.07,0.10), vec3(0.03,0.14,0.18), uDayF);
    deep *= (uAmbient.r + uAmbient.g + uAmbient.b) * 2.0 + 0.15;
    vec3 col = mix(deep, skyRef, clamp(fres*1.2 + 0.08, 0.0, 1.0));

    // Лёгкая рябь бликов от луны ночью
    float dist = length(vWorldPos - uCamPos);
    float fogF = 1.0 - exp(-dist/max(uDrawDist*0.55,10.0));
    fragColor = vec4(mix(col, uFogColor, clamp(fogF,0.0,1.0)), 0.92);
}
)";

// ======================= Генерация мира =======================
struct Vertex { Vec3 pos; Vec3 nrm; Vec3 col; };
static std::vector<Vertex> g_worldVerts;   // весь статичный мир одним мешем

static void PushQuad(std::vector<Vertex>& v,
    Vec3 a, Vec3 b, Vec3 c, Vec3 d, Vec3 n, Vec3 col)
{
    // Два треугольника a-b-c и a-c-d, намотка CCW при взгляде со стороны n
    v.push_back({a,n,col}); v.push_back({b,n,col}); v.push_back({c,n,col});
    v.push_back({a,n,col}); v.push_back({c,n,col}); v.push_back({d,n,col});
}

static void PushBox(std::vector<Vertex>& v,
    float x,float y,float z, float sx,float sy,float sz, Vec3 col)
{
    float hx=sx*0.5f, hy=sy*0.5f, hz=sz*0.5f;
    Vec3 lo={x-hx,y-hy,z-hz}, hi={x+hx,y+hy,z+hz};
    PushQuad(v,{lo.x,hi.y,lo.z},{hi.x,hi.y,lo.z},{hi.x,hi.y,hi.z},{lo.x,hi.y,hi.z},{0,1,0},col);   // верх
    PushQuad(v,{lo.x,lo.y,hi.z},{hi.x,lo.y,hi.z},{hi.x,lo.y,lo.z},{lo.x,lo.y,lo.z},{0,-1,0},col*0.6f); // низ
    PushQuad(v,{lo.x,lo.y,hi.z},{hi.x,lo.y,hi.z},{hi.x,hi.y,hi.z},{lo.x,hi.y,hi.z},{0,0,1},col*0.9f);  // юг
    PushQuad(v,{hi.x,lo.y,lo.z},{lo.x,lo.y,lo.z},{lo.x,hi.y,lo.z},{hi.x,hi.y,lo.z},{0,0,-1},col*0.9f); // север
    PushQuad(v,{hi.x,lo.y,hi.z},{hi.x,lo.y,lo.z},{hi.x,hi.y,lo.z},{hi.x,hi.y,hi.z},{1,0,0},col*0.8f);  // восток
    PushQuad(v,{lo.x,lo.y,lo.z},{lo.x,lo.y,hi.z},{lo.x,hi.y,hi.z},{lo.x,hi.y,lo.z},{-1,0,0},col*0.8f); // запад
}

static void BuildWorld()
{
    g_worldVerts.clear();
    Vec3 grass={0.25f,0.52f,0.22f};
    // Земля — сетка с лёгкими холмами (вода на y=0 в области озера)
    const int N=48; const float S=200.0f;
    for(int iz=0; iz<N; iz++)
    for(int ix=0; ix<N; ix++){
        float x0=(ix/(float)N-0.5f)*S, z0=(iz/(float)N-0.5f)*S;
        float x1=((ix+1)/(float)N-0.5f)*S, z1=((iz+1)/(float)N-0.5f)*S;
        auto H=[&](float x,float z){
            float lake = smoothstepf(28.0f, 45.0f, sqrtf(x*x+z*z)); // озеро в центре ниже уровня
            float hills = sinf(x*0.04)*cosf(z*0.05)*1.2f;
            return hills*(1.0f-lake) - lake*2.5f;
        };
        float y00=H(x0,z0), y10=H(x1,z0), y11=H(x1,z1), y01=H(x0,z1);
        bool underwater = (y00<0 && y10<0 && y11<0 && y01<0);
        Vec3 c = underwater ? Vec3{0.30f,0.26f,0.18f} : grass;
        if(!underwater){
            // Небольшая вариация цвета травы по шуму координат
            float n = sinf(x0*0.31)*sinf(z0*0.27);
            c = {grass.x+n*0.04f, grass.y+n*0.05f, grass.z+n*0.03f};
        }
        Vec3 nrm = Normalize(Cross({x1-x0,y10-y00,0},{0,y01-y00,z1-z0}));
        PushQuad(g_worldVerts,{x0,y00,z0},{x1,y10,z0},{x1,y11,z1},{x0,y01,z1},nrm,c);
    }
    // Домики, деревья, забор — боксы
    for(int i=-2;i<=2;i++){
        PushBox(g_worldVerts, i*16.0f, 0.9f, -34.0f, 6,3.6,6, {0.78f,0.70f,0.58f});   // стены
        PushBox(g_worldVerts, i*16.0f, 3.8f, -34.0f, 7,0.6,7, {0.45f,0.25f,0.15f});   // крыша
    }
    for(int i=0;i<24;i++){
        float a=i/24.0f*2*M_PI, r=62.0f;
        PushBox(g_worldVerts, cosf(a)*r, 1.6f, sinf(a)*r, 0.7f,3.2f,0.7f, {0.35f,0.24f,0.13f});
        PushBox(g_worldVerts, cosf(a)*r, 3.8f, sinf(a)*r, 2.6f,2.6f,2.6f, {0.16f,0.38f,0.14f});
    }
}

// ======================= Водная поверхность =======================
static GLuint g_waterVAO=0, g_waterVBO=0;
static int   g_waterCount=0;

static void BuildWaterMesh()
{
    std::vector<Vec2> verts;   // struct Vec2{float x,z;} — используем два float
    struct V2{float x,z;};
    std::vector<V2> v;
    const int N=64; const float S=110.0f;
    for(int iz=0;iz<N;iz++)
    for(int ix=0;ix<N;ix++){
        float x0=(ix/(float)N-0.5f)*S, z0=(iz/(float)N-0.5f)*S;
        float x1=((ix+1)/(float)N-0.5f)*S, z1=((iz+1)/(float)N-0.5f)*S;
        v.push_back({x0,z0}); v.push_back({x1,z0}); v.push_back({x1,z1});
        v.push_back({x0,z0}); v.push_back({x1,z1}); v.push_back({x0,z1});
    }
    g_waterCount = (int)v.size()*2;   // 2 компоненты на вершину
    glGenVertexArrays(1,&g_waterVAO);
    glGenBuffers(1,&g_waterVBO);
    glBindVertexArray(g_waterVAO);
    glBindBuffer(GL_ARRAY_BUFFER,g_waterVBO);
    glBufferData(GL_ARRAY_BUFFER,v.size()*sizeof(V2),v.data(),GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,sizeof(V2),(const void*)0);
    glBindVertexArray(0);
}

// ======================= Математика: дополнение =======================
#include <vector>
struct Vec2f { float x=0, z=0; };

static Vec3 Cross(Vec3 a, Vec3 b){
    return { a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x };
}
static float Dot(Vec3 a, Vec3 b){ return a.x*b.x+a.y*b.y+a.z*b.z; }
static float Length(Vec3 a){ return sqrtf(Dot(a,a)); }
static Vec3 Normalize(Vec3 a){ float l=Length(a); return l>1e-6f?Vec3{a.x/l,a.y/l,a.z/l}:a; }
static float smoothstepf(float e0, float e1, float x){
    float t=(x-e0)/(e1-e0); t=t<0?0:t>1?1:t; return t*t*(3-2*t);
}

// ======================= Астрономия =======================
static void UpdateAstronomy()
{
    // Углы солнца и луны. Луна движется чуть медленнее (~13°/сутки отставание)
    float sunAng  = (g_timeOfDay - 0.25f) * 2.0f * M_PI;          // 0.25 = полдень
    float moonAng = sunAng * (1.0f - 1.0f/29.5f) + M_PI*0.9f;     // сидерический сдвиг
    g_sunDir  = { sinf(sunAng)*0.0f, sinf(sunAng), cosf(sunAng) };
    // Наклон орбит для красоты:
    g_sunDir.x = cosf(sunAng);
    g_sunDir.z = 0.35f;
    g_sunDir.y = sinf(sunAng);
    g_sunDir = Normalize(g_sunDir);

    g_moonDir.x = cosf(moonAng);
    g_moonDir.z = -0.30f;
    g_moonDir.y = sinf(moonAng);
    g_moonDir = Normalize(g_moonDir);

    g_sunHeight = g_sunDir.y;
    // Фаза = разность углов: 1 = полнолуние, 0 = новолуние
    float diff = moonAng - sunAng;
    while (diff < -M_PI) diff += 2*M_PI;
    while (diff >  M_PI) diff -= 2*M_PI;
    g_moonPhase = 0.5f - 0.5f*cosf(diff);   // освещённая доля
}

// ======================= Shadow Map =======================
static GLuint g_shadowFBO=0, g_shadowTex=0;
static const int SHADOW_SIZE = 4096;

static bool InitShadowMap()
{
    glGenTextures(1, &g_shadowTex);
    glBindTexture(GL_TEXTURE_2D, g_shadowTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
                 SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glGenFramebuffers(1, &g_shadowFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, g_shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, g_shadowTex, 0);
    GLenum st = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return st == 0x8CD5 /*GL_FRAMEBUFFER_COMPLETE*/;
}

// Матрица света — ортографическая камера из позиции солнца/луны на игрока
static Mat4 g_lightMat;

static void ComputeLightMat(Vec3 lightDir)
{
    Vec3 l = lightDir.y > 0 ? lightDir : Vec3{-lightDir.x, -lightDir.y, -lightDir.z};
    if (l.y < 0.15f) { l.y = 0.15f; l = Normalize(l); }
    Vec3 center = g_player.pos;
    Vec3 eye = { center.x + l.x*80, center.y + l.y*80, center.z + l.z*80 };
    Mat4 V = LookAt(eye, center, {0,1,0});
    Mat4 P = Ortho(-40,40,-40,40,-100,160);
    g_lightMat = P * V;
}

// ======================= Компиляция шейдерных программ =======================
static GLuint CompileShader(GLenum type, const char* src)
{
    GLuint sh = glCreateShader(type);
    glShaderSource(sh, 1, &src, nullptr);
    glCompileShader(sh);
    GLint ok=0; glGetShaderiv(sh, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetShaderInfoLog(sh, sizeof(log), nullptr, log);
        MessageBoxA(nullptr, log, "Ошибка компиляции шейдера", MB_ICONERROR);
    }
    return sh;
}

static GLuint LinkProgramSrc(const char* vs, const char* fs)
{
    GLuint p = glCreateProgram();
    GLuint a = CompileShader(GL_VERTEX_SHADER, vs);
    GLuint b = CompileShader(GL_FRAGMENT_SHADER, fs);
    glAttachShader(p,a); glAttachShader(p,b);
    glLinkProgram(p);
    GLint ok=0; glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[2048]; glGetProgramInfoLogSafe(p, log, sizeof(log));
        MessageBoxA(nullptr, log, "Ошибка линковки", MB_ICONERROR);
    }
    return p;
}
// Хелпер лога программы (glGetProgramInfoLog не объявлен у нас отдельно)
static void glGetProgramInfoLogSafe(GLuint p, char* buf, int sz)
{
    typedef void (APIENTRYP PFN)(GLuint, GLsizei, GLsizei*, GLchar*);
    auto f = (PFN)wglGetProcAddress("glGetProgramInfoLog");
    if (f) f(p, sz, nullptr, buf); else buf[0]=0;
}

static GLuint g_progObj=0, g_progSky=0, g_progWater=0, g_progShadow=0;

// Вершинник теневого прохода
static const char* VS_SHADOW = R"(
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uLightMat;
void main(){ gl_Position = uLightMat * vec4(aPos,1.0); }
)";

static void BuildWorldMeshGL()   // заливка статичного мира в GPU
{
    glGenVertexArrays(1,&g_worldVAO);
    glGenBuffers(1,&g_worldVBO);
    glBindVertexArray(g_worldVAO);
    glBindBuffer(GL_ARRAY_BUFFER,g_worldVBO);
    glBufferData(GL_ARRAY_BUFFER,
                 g_worldVerts.size()*sizeof(Vertex), g_worldVerts.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,pos));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,nrm));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,col));
    glBindVertexArray(0);
}

// ======================= Персонаж: обновление =======================
static void UpdatePlayer(float dt)
{
    bool fwd=false, back=false, left=false, right=false;
    if (g_mouseCaptured) {
        fwd=g_keys['W']; back=g_keys['S'];
        left=g_keys['A']; right=g_keys['D'];
    }
    float runMul = g_keys[VK_SHIFT] ? RUN_SPEED/WALK_SPEED : 1.0f;
    Vec3 m={};
    if(fwd){m.x+=sinf(camYaw+M_PI); m.z+=cosf(camYaw+M_PI);}
    if(back){m.x-=sinf(camYaw+M_PI); m.z-=cosf(camYaw+M_PI);}
    if(left){m.x-=cosf(camYaw+M_PI); m.z+=sinf(camYaw+M_PI);}
    if(right){m.x+=cosf(camYaw+M_PI); m.z-=sinf(camYaw+M_PI);}
    float ml=sqrtf(m.x*m.x+m.z*m.z);
    bool moving = ml>0.01f && g_player.onGround!=false || moving_any();
    if(ml>0.01f){
        m.x/=ml; m.z/=ml;
        float sp=WALK_SPEED*runMul;
        g_player.pos.x+=m.x*sp*dt; g_player.pos.z+=m.z*sp*dt;
        float ty=atan2f(m.x,m.z);
        float d=ty-g_player.yaw;
        while(d>M_PI)d-=2*M_PI; while(d<-M_PI)d+=2*M_PI;
        g_player.yaw += d*fminf(1.0f,dt*10);
    }
    static bool anyMove=false;
    anyMove = ml>0.01f;
    g_player.speedFactor += ((anyMove?runMul:0)-g_player.speedFactor)*fminf(1,dt*8);

    if(g_mouseCaptured && g_keys[VK_SPACE] && g_player.onGround){
        g_player.vy=JUMP_VEL; g_player.onGround=false;
    }
    g_player.vy+=GRAVITY*dt;
    g_player.pos.y+=g_player.vy*dt;
    // Высота земли под персонажем
    float gh = GroundHeight(g_player.pos.x, g_player.pos.z);
    if(g_player.pos.y<=gh){ g_player.pos.y=gh; g_player.vy=0; g_player.onGround=true; }

    g_player.walkPhase += dt*g_player.speedFactor*8.0f;
}

static bool moving_any(){ return false; }   // заглушка (используется выше)

// Высота рельефа — та же формула, что в BuildWorld
static float GroundHeight(float x, float z)
{
    float r=sqrtf(x*x+z*z);
    float lake=smoothstepf(28.0f,45.0f,r);
    float hills=sinf(x*0.04f)*cosf(z*0.05f)*1.2f;
    return hills*(1-lake)-lake*2.5f;
}

// ======================= Камера =======================
static void UpdateCamera(float dt)
{
    Vec3 target={g_player.pos.x, g_player.pos.y+1.6f, g_player.pos.z};
    float cy=cosf(camPitch);
    Vec3 off={ sinf(camYaw)*cy*g_camDist,
               -sinf(camPitch)*g_camDist+0.8f,
               cosf(camYaw)*cy*g_camDist };
    g_camPos={target.x+off.x, target.y+off.y, target.z+off.z};
    float gh=GroundHeight(g_camPos.x,g_camPos.z)+0.4f;
    if(g_camPos.y<gh) g_camPos.y=gh;
}

// ======================= Рендер персонажа =======================
static void RenderCharacter(Mat4 proj, Mat4 view, GLuint prog)
{
    struct Part{ float px,py,pz, rx,ry, sx,sy,sz; Vec3 col; };
    float sw=sinf(g_player.walkPhase)*0.7f*g_player.speedFactor;
    float bob=fabsf(cosf(g_player.walkPhase))*0.06f*g_player.speedFactor;
    if(!g_player.onGround){ sw=0.55f; bob=0; }
    float py=g_player.pos.y+bob;

    Part parts[]={
        // торс, голова
        {0,py+1.15f,0, 0,g_player.yaw, 0.42f,0.62f,0.26f,{0.85f,0.25f,0.20f}},
        {0,py+1.72f,0, 0,g_player.yaw, 0.24f,0.24f,0.24f,{0.92f,0.78f,0.65f}},
        // ноги (качаются в противофазе)
        {sinf(g_player.yaw)*0.12f+sw*0.28f, py+0.45f, cosf(g_player.yaw)*0.12f,
         sw,g_player.yaw, 0.15f,0.90f,0.15f,{0.20f,0.25f,0.60f}},
        {sinf(g_player.yaw)*-0.12f-sw*0.28f, py+0.45f, cosf(g_player.yaw)*-0.12f,
         -sw,g_player.yaw, 0.15f,0.90f,0.15f,{0.20f,0.25f,0.60f}},
        // руки
        {sinf(g_player.yaw)*0.30f-sw*0.22f, py+1.18f, cosf(g_player.yaw)*0.30f,
         -sw*1.2f,g_player.yaw, 0.12f,0.62f,0.12f,{0.85f,0.25f,0.20f}},
        {sinf(g_player.yaw)*-0.30f+sw*0.22f, py+1.18f, cosf(g_player.yaw)*-0.30f,
         sw*1.2f,g_player.yaw, 0.12f,0.62f,0.12f,{0.85f,0.25f,0.20f}},
    };

    // Строим временный меш боксов для каждой части (просто, надёжно)
    std::vector<Vertex> tmp;
    for(auto&p:parts){
        // Поворот вокруг Y персонажа + качание rx — упрощённо без наклона конечностей
        PushBox(tmp, p.px,p.py,p.pz, p.sx,p.sy,p.sz, p.col);
    }
    // Заливаем в динамический буфер
    static GLuint vao=0,vbo=0; static bool init=false;
    if(!init){
        glGenVertexArrays(1,&vao); glGenBuffers(1,&vbo); init=true;
    }
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER,vbo);
    glBufferData(GL_ARRAY_BUFFER,tmp.size()*sizeof(Vertex),tmp.data(),GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0); glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,pos));
    glEnableVertexAttribArray(1); glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,nrm));
    glEnableVertexAttribArray(2); glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,sizeof(Vertex),(const void*)offsetof(Vertex,col));

    glUseProgram(prog);
    Mat4 model = ModelTRS(0,0,0,0,0,1,1,1);
    SetObjUniforms(prog, proj, view, model);
    glDrawArrays(GL_TRIANGLES,0,(GLint)tmp.size());
    glBindVertexArray(0);
}

// Установка общих uniform'ов объектного шейдера
static void SetObjUniforms(GLuint prog, Mat4 proj, Mat4 view, Mat4 model)
{
    glUniformMatrix4fv(glGetUniformLocation(prog,"uProj"),1,false,proj.m);
    glUniformMatrix4fv(glGetUniformLocation(prog,"uView"),1,false,view.m);
    glUniformMatrix4fv(glGetUniformLocation(prog,"uModel"),1,false,model.m);
    glUniformMatrix4fv(glGetUniformLocation(prog,"uLightMat"),1,false,g_lightMat.m);
    glUniform3f(glGetUniformLocation(prog,"uSunDir"),g_sunDir.x,g_sunDir.y,g_sunDir.z);
    glUniform3f(glGetUniformLocation(prog,"uMoonDir"),g_moonDir.x,g_moonDir.y,g_moonDir.z);
    Vec3 sc = SunColor(); Vec3 amb = AmbientColor();
    glUniform3f(glGetUniformLocation(prog,"uSunCol"),sc.x,sc.y,sc.z);
    glUniform3f(glGetUniformLocation(prog,"uAmbient"),amb.x,amb.y,amb.z);
    glUniform3f(glGetUniformLocation(prog,"uCamPos"),g_camPos.x,g_camPos.y,g_camPos.z);
    glUniform1i(glGetUniformLocation(prog,"uUnlit"),0);
    glUniform1f(glGetUniformLocation(prog,"uDrawDist"),g_opt.drawDistance);
    Vec3 fog=FogColor();
    glUniform3f(glGetUniformLocation(prog,"uFogColor"),fog.x,fog.y,fog.z);
    // Текстура теней на юнит 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,g_shadowTex);
    glUniform1i(glGetUniformLocation(prog,"uShadowMap"),0);
}

// Цвета освещения по высоте солнца + затенение облаками
static Vec3 SunColor(){
    float h=g_sunHeight;
    float dayF=clampf((h+0.10f)/0.25f,0,1);
    float duskF=clampf(1.0f-fabsf(h)/0.35f,0,1);
    Vec3 low={1.0f,0.45f,0.15f}, day={1.0f,0.96f,0.86f};
    Vec3 c=mixv(low,day,clampf(h*4+0.5f,0,1));
    float cloudShade=1.0f-g_cloudCover*0.75f;
    return scalev(c,(dayF*cloudShade+duskF*0.3f));
}
static Vec3 AmbientColor(){
    float dayF=clampf((g_sunHeight+0.10f)/0.25f,0,1);
    Vec3 night={0.03f,0.035f,0.06f}, day={0.32f,0.36f,0.42f};
    return mixv(night,day,dayF);
}
static Vec3 FogColor(){
    float dayF=clampf((g_sunHeight+0.10f)/0.25f,0,1);
    Vec3 night={0.02f,0.025f,0.05f}, day={0.62f,0.76f,0.90f};
    Vec3 f=mixv(night,day,dayF);
    // Пасмурная десатурация
    float gr=(f.x+f.y+f.z)/3;
    return mixv(f,{gr,gr,gr},g_cloudCover*0.5f);
}
static float clampf(float x,float a,float b){return x<a?a:x>b?b:x;}
static Vec3 mixv(Vec3 a,Vec3 b,float t){return{a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t};}
static Vec3 scalev(Vec3 a,float s){return{a.x*s,a.y*s,a.z*s};}
#include <commctrl.h>
#pragma comment(lib,"comctl32.lib")

static HWND g_dlg=nullptr;

static void SaveOptions()
{
    WritePrivateProfileStringW(L"Graphics",L"Fullscreen", g_opt.fullscreen?L"1":L"0", L"options.ini");
    WritePrivateProfileStringW(L"Graphics",L"VSync",      g_opt.vsync?L"1":L"0",      L"options.ini");
    WritePrivateProfileStringW(L"Graphics",L"FsWidth",    std::to_wstring(g_opt.fsWidth).c_str(),  L"options.ini");
    WritePrivateProfileStringW(L"Graphics",L"FsHeight",   std::to_wstring(g_opt.fsHeight).c_str(), L"options.ini");
    WritePrivateProfileStringW(L"Scene",   L"PauseTime",  g_opt.pauseTime?L"1":L"0",  L"options.ini");
    WritePrivateProfileStringW(L"Scene",   L"StarSize",   std::to_wstring(g_opt.starSize).c_str(),     L"options.ini");
    WritePrivateProfileStringW(L"Scene",   L"DrawDist",   std::to_wstring((int)g_opt.drawDistance).c_str(), L"options.ini");
}

static void LoadOptions()
{
    g_opt.fullscreen  = GetPrivateProfileIntW(L"Graphics",L"Fullscreen",0,L"options.ini")!=0;
    g_opt.vsync       = GetPrivateProfileIntW(L"Graphics",L"VSync",1,L"options.ini")!=0;
    g_opt.fsWidth     = GetPrivateProfileIntW(L"Graphics",L"FsWidth",1920,L"options.ini");
    g_opt.fsHeight    = GetPrivateProfileIntW(L"Graphics",L"FsHeight",1080,L"options.ini");
    g_opt.pauseTime   = GetPrivateProfileIntW(L"Scene",L"PauseTime",0,L"options.ini")!=0;
    wchar_t buf[64]={};
    GetPrivateProfileStringW(L"Scene",L"StarSize",L"1.0",buf,64,L"options.ini");
    g_opt.starSize   =(float)_wtof(buf);
    g_opt.drawDistance= (float)GetPrivateProfileIntW(L"Scene",L"DrawDist",300,L"options.ini");
}

// Диалог: вкладки через Tab Control, чекбоксы, слайдеры
static INT_PTR CALLBACK DlgProc(HWND h, UINT m, WPARAM w, LPARAM)
{
    switch(m){
    case WM_INITDIALOG: {
        // Чекбоксы
        CheckDlgButton(h,101, g_opt.fullscreen?BST_CHECKED:BST_UNCHECKED);
        CheckDlgButton(h,102, g_opt.vsync?BST_CHECKED:BST_UNCHECKED);
        CheckDlgButton(h,201, g_opt.pauseTime?BST_CHECKED:BST_UNCHECKED);
        // Слайдер звёзд
        SendDlgItemMessageW(h,202,TBM_SETRANGE,true,MAKELPARAM(0,300));
        SendDlgItemMessageW(h,202,TBM_SETPOS,true,(int)(g_opt.starSize*100));
        // Слайдер дальности
        SendDlgItemMessageW(h,203,TBM_SETRANGE,true,MAKELPARAM(50,600));
        SendDlgItemMessageW(h,203,TBM_SETPOS,true,(int)g_opt.drawDistance);
        return true;
    }
    case WM_COMMAND:
        if(HIWORD(w)==BN_CLICKED){
            g_opt.fullscreen = IsDlgButtonChecked(h,101)==BST_CHECKED;
            g_opt.vsync      = IsDlgButtonChecked(h,102)==BST_CHECKED;
            g_opt.pauseTime  = IsDlgButtonChecked(h,201)==BST_CHECKED;
            ApplyGraphicsSettings();
            SaveOptions();
        }
        if(w==IDOK||w==IDCANCEL){ DestroyWindow(h); g_dlg=nullptr; return true; }
        break;
    case WM_HSCROLL: {
        int pos=(int)SendDlgItemMessageW(h,202,TBM_GETPOS,0,0);
        g_opt.starSize=pos/100.0f;
        pos=(int)SendDlgItemMessageW(h,203,TBM_GETPOS,0,0);
        g_opt.drawDistance=(float)pos;
        SaveOptions();
        break;
    }
    case WM_CLOSE: DestroyWindow(h); g_dlg=nullptr; return true;
    }
    return false;
}

static void ToggleOptionsDialog()
{
    if(g_dlg){ DestroyWindow(g_dlg); g_dlg=nullptr; return; }
    g_dlg=CreateDialogW(GetModuleHandleW(nullptr),
                        MAKEINTRESOURCEW(2000), g_hwnd, DlgProc);
    ShowWindow(g_dlg,SW_SHOW);
}

static WINDOWPLACEMENT g_wpPrev={sizeof(g_wpPrev)};

static void ApplyGraphicsSettings()
{
    typedef BOOL (WINAPI *PFNSWI)(int);
    auto wglSwapIntervalEXT=(PFNSWI)wglGetProcAddress("wglSwapIntervalEXT");
    if(wglSwapIntervalEXT) wglSwapIntervalEXT(g_opt.vsync?1:0);

    DWORD style=GetWindowLongW(g_hwnd,GWL_STYLE);
    if(g_opt.fullscreen && !(style&WS_POPUP)){
        MONITORINFO mi={sizeof(mi)};
        GetMonitorInfoW(MonitorFromWindow(g_hwnd,MONITOR_DEFAULTTOPRIMARY),&mi);
        GetWindowPlacement(g_hwnd,&g_wpPrev);
        SetWindowLongW(g_hwnd,GWL_STYLE,style&~WS_OVERLAPPEDWINDOW|WS_POPUP);
        SetWindowPos(g_hwnd,HWND_TOP,mi.rcMonitor.left,mi.rcMonitor.top,
                     g_opt.fsWidth,g_opt.fsHeight,SWP_FRAMECHANGED|SWP_NOOWNERZORDER);
    } else if(!g_opt.fullscreen && (style&WS_POPUP)){
        SetWindowLongW(g_hwnd,GWL_STYLE,(style&~WS_POPUP)|WS_OVERLAPPEDWINDOW);
        SetWindowPlacement(g_hwnd,&g_wpPrev);
        SetWindowPos(g_hwnd,nullptr,0,0,0,0,
                     SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
    }
}

static GLuint g_skyVAO=0,g_skyVBO=0,g_worldVAO=0,g_worldVBO=0;

static void RenderFrame()
{
    UpdateAstronomy();

    // --- Проход 1: тени ---
    Vec3 lightDir = g_sunHeight>-0.05f ? g_sunDir : g_moonDir;
    ComputeLightMat(lightDir);
    glBindFramebuffer(GL_FRAMEBUFFER,g_shadowFBO);
    glViewport(0,0,SHADOW_SIZE,SHADOW_SIZE);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(g_progShadow);
    glUniformMatrix4fv(glGetUniformLocation(g_progShadow,"uLightMat"),1,false,g_lightMat.m);
    glBindVertexArray(g_worldVAO);
    glDrawArrays(GL_TRIANGLES,0,(GLint)g_worldVerts.size());

    // --- Проход 2: основной ---
    RECT rc; GetClientRect(g_hwnd,&rc);
    int W=rc.right-rc.left,H=rc.bottom-rc.top;
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,W,H);

    float aspect=W/(float)H;
    Mat4 proj=Perspective(70.0f*M_PI/180.0f,aspect,0.1f,g_opt.drawDistance*3);

    UpdateCamera(0.016f);
    Vec3 target={g_player.pos.x,g_player.pos.y+1.6f,g_player.pos.z};
    Mat4 view=LookAt(g_camPos,target,{0,1,0});

    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

    // Небо (без записи глубины)
    glDepthMask(false);
    glUseProgram(g_progSky);
    Mat4 viewRot=view; viewRot.m[12]=viewRot.m[13]=viewRot.m[14]=0;
    glUniformMatrix4fv(glGetUniformLocation(g_progSky,"uProj"),1,false,proj.m);
    glUniformMatrix4fv(glGetUniformLocation(g_progSky,"uViewRotOnly"),1,false,viewRot.m);
    glUniform3f(glGetUniformLocation(g_progSky,"uCamPos"),g_camPos.x,g_camPos.y,g_camPos.z);
    glUniform3f(glGetUniformLocation(g_progSky,"uSunDir"),g_sunDir.x,g_sunDir.y,g_sunDir.z);
    glUniform3f(glGetUniformLocation(g_progSky,"uMoonDir"),g_moonDir.x,g_moonDir.y,g_moonDir.z);
    glUniform1f(glGetUniformLocation(g_progSky,"uTimeOfDay"),g_timeOfDay);
    glUniform1f(glGetUniformLocation(g_progSky,"uStarSize"),g_opt.starSize);
    glUniform1f(glGetUniformLocation(g_progSky,"uCloudCover"),g_cloudCover);
    glUniform1f(glGetUniformLocation(g_progSky,"uTime"),g_animTime);
    glBindVertexArray(g_skyVAO);
    glDrawArrays(GL_TRIANGLES,0,36);
    glDepthMask(true);

    // Мир
    glUseProgram(g_progObj);
    glBindVertexArray(g_worldVAO);
    SetObjUniforms(g_progObj,proj,view,ModelTRS(0,0,0,0,0,1,1,1));
    glDrawArrays(GL_TRIANGLES,0,(GLint)g_worldVerts.size());

    // Персонаж
    RenderCharacter(proj,view,g_progObj);

    // Вода (полупрозрачная, поверх)
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glUseProgram(g_progWater);
    glUniformMatrix4fv(glGetUniformLocation(g_progWater,"uProj"),1,false,proj.m);
    glUniformMatrix4fv(glGetUniformLocation(g_progWater,"uView"),1,false,view.m);
    glUniformMatrix4fv(glGetUniformLocation(g_progWater,"uLightMat"),1,false,g_lightMat.m);
    glUniform1f(glGetUniformLocation(g_progWater,"uTime"),g_animTime);
    glUniform3f(glGetUniformLocation(g_progWater,"uCamPos"),g_camPos.x,g_camPos.y,g_camPos.z);
    glUniform3f(glGetUniformLocation(g_progWater,"uSunDir"),g_sunDir.x,g_sunDir.y,g_sunDir.z);
    Vec3 sc=SunColor(); Vec3 am=AmbientColor();
    glUniform3f(glGetUniformLocation(g_progWater,"uSunCol"),sc.x,sc.y,sc.z);
    glUniform3f(glGetUniformLocation(g_progWater,"uAmbient"),am.x,am.y,am.z);
    float dayF=clampf((g_sunHeight+0.1f)/0.25f,0,1);
    glUniform1f(glGetUniformLocation(g_progWater,"uDayF"),dayF);
    Vec3 zen=mixv({0.008f,0.010f,0.028f},{0.18f,0.42f,0.80f},dayF);
    Vec3 hor=mixv({0.02f,0.03f,0.07f},{0.62f,0.78f,0.92f},dayF);
    glUniform3f(glGetUniformLocation(g_progWater,"uSkyZenith"),zen.x,zen.y,zen.z);
    glUniform3f(glGetUniformLocation(g_progWater,"uSkyHorizon"),hor.x,hor.y,hor.z);
    Vec3 fog=FogColor();
    glUniform3f(glGetUniformLocation(g_progWater,"uFogColor"),fog.x,fog.y,fog.z);
    glUniform1f(glGetUniformLocation(g_progWater,"uDrawDist"),g_opt.drawDistance);
    glBindVertexArray(g_waterVAO);
    glDrawArrays(GL_TRIANGLES,0,g_waterCount);
    glDisable(GL_BLEND);

    SwapBuffers(g_hdc);
}

static float g_animTime=0;

static void UpdateSimulation(float dt)
{
    if(!g_opt.pauseTime){
        g_timeOfDay += dt*(g_timeForward?1:-1)/240.0f;   // полные сутки за 4 минуты
        if(g_timeOfDay>=1) g_timeOfDay-=1;
        if(g_timeOfDay<0)  g_timeOfDay+=1;
    }
    g_animTime += dt;
    UpdatePlayer(dt);
    // Перемотка M/N
    static bool mM=false,nM=false;
    if(g_keys['M']&&!mM){ g_timeForward=true;  g_opt.pauseTime=false; g_timeOfDay+=0.02f; }
    if(g_keys['N']&&!nM){ g_timeForward=false; g_opt.pauseTime=false; g_timeOfDay-=0.02f; }
    mM=g_keys['M']; nM=g_keys['N'];
}

// ======================= Инициализация сцены =======================
static bool InitScene()
{
    HMODULE hOgl32 = LoadLibraryW(L"opengl32.dll");
    if (!hOgl32) {
        MessageBoxA(nullptr, "Не удалось загрузить opengl32.dll", "Ошибка", MB_ICONERROR);
        return false;
    }

    // --- Шаг 1: формат пикселей ---
    PIXELFORMATDESCRIPTOR pfd = {};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 32;
    pfd.cDepthBits = 24;
    int pf = ChoosePixelFormat(g_hdc, &pfd);
    if (!pf || !SetPixelFormat(g_hdc, pf, &pfd)) {
        MessageBoxA(nullptr, "Ошибка выбора формата пикселей", "Ошибка", MB_ICONERROR);
        return false;
    }

    // --- Шаг 2: временный legacy-контекст ---
    HGLRC legacy = wglCreateContext(g_hdc);
    if (!legacy || !wglMakeCurrent(g_hdc, legacy)) {
        MessageBoxA(nullptr, "Не удалось создать временный контекст", "Ошибка", MB_ICONERROR);
        return false;
    }

    // --- Шаг 3: современный контекст с откатом версий ---
    typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARB)(HDC, HGLRC, const int*);
    auto wglCreateContextAttribs =
        (PFNWGLCREATECONTEXTATTRIBSARB)wglGetProcAddress("wglCreateContextAttribsARB");
    if (!wglCreateContextAttribs) {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(legacy);
        MessageBoxA(nullptr, "WGL_ARB_create_context не поддерживается",
                    "Ошибка", MB_ICONERROR);
        return false;
    }

    static const int profiles[]  = { 0x0002 /*COMPAT*/, 0x0001 /*CORE*/ };
    static const int versions[][2] = { {4,6},{4,3},{4,1},{3,3} };
    HGLRC modern = nullptr;
    for (int pr = 0; pr < 2 && !modern; pr++) {
        for (int i = 0; i < 4 && !modern; i++) {
            int attribs[] = {
                0x2091, versions[i][0],
                0x2092, versions[i][1],
                0x9126, profiles[pr],
                0
            };
            modern = wglCreateContextAttribs(g_hdc, nullptr, attribs);
        }
    }

    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(legacy);
    if (!modern || !wglMakeCurrent(g_hdc, modern)) {
        MessageBoxA(nullptr, "Не удалось активировать современный контекст OpenGL",
                    "Ошибка", MB_ICONERROR);
        return false;
    }
    g_hglrc = modern;

    // --- Шаг 4: загрузка функций OpenGL ---
    if (!LoadOpenGLFunctions()) {
        MessageBoxA(nullptr, "Не удалось загрузить функции OpenGL", "Ошибка", MB_ICONERROR);
        return false;
    }

    // --- Вертикальная синхронизация ---
    typedef BOOL (WINAPI *PFNSWI)(int);
    auto wglSwapIntervalEXT = (PFNSWI)wglGetProcAddress("wglSwapIntervalEXT");
    if (wglSwapIntervalEXT) wglSwapIntervalEXT(g_opt.vsync ? 1 : 0);

    // --- Шаг 5: геометрия скайдома (куб вокруг камеры) ---
    float sky[36 * 3];
    static const float q[8][3] = {{-1,-1,-1},{1,-1,-1},{1,1,-1},{-1,1,-1},
                                  {-1,-1, 1},{1,-1, 1},{1,1, 1},{-1,1, 1}};
    static const int idx[36] = {0,1,2,0,2,3, 5,4,7,5,7,6, 4,0,3,4,3,7,
                                1,5,6,1,6,2, 4,5,1,4,1,0, 3,2,6,3,6,7};
    for (int i = 0; i < 36; i++)
        memcpy(&sky[i*3], q[idx[i]], sizeof(float)*3);
    glGenVertexArrays(1, &g_skyVAO);
    glGenBuffers(1, &g_skyVBO);
    glBindVertexArray(g_skyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, g_skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sky), sky, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, nullptr);
    glBindVertexArray(0);

    // --- Шаг 6: мир, вода, тени ---
    BuildWorld();
    BuildWorldMeshGL();
    BuildWaterMesh();
    if (!InitShadowMap()) {
        MessageBoxA(nullptr, "Не удалось создать shadow map FBO", "Ошибка", MB_ICONERROR);
        return false;
    }

    // --- Шаг 7: компиляция шейдерных программ ---
    static const char* FS_EMPTY =
        "#version 330 core\nout vec4 o;\nvoid main(){ o = vec4(1.0); }\n";

    g_progShadow = LinkProgramSrc(VS_SHADOW, FS_EMPTY);
    g_progSky    = LinkProgramSrc(VS_SKY,    FS_SKY);
    g_progObj    = LinkProgramSrc(VS_OBJ,    FS_OBJ);
    g_progWater  = LinkProgramSrc(VS_WATER,  FS_WATER);

    if (!g_progShadow || !g_progSky || !g_progObj || !g_progWater) {
        MessageBoxA(nullptr, "Ошибка создания шейдерных программ", "Ошибка", MB_ICONERROR);
        return false;
    }

    // --- Шаг 8: состояние OpenGL ---
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // --- Шаг 9: опции и режим окна ---
    LoadOptions();
    ApplyGraphicsSettings();

    return true;
}

// Замените странную строку выше просто на:
// glEnable(GL_LEQUAL); — нет, верно так:
// glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LEQUAL);
// (добавьте typedef и указатель glDepthFunc по образцу остальных функций)

// WndProc: добавьте WM_MOUSEWHEEL для зума, VK_ESCAPE — отпустить мышь,
// VK_F1 — диалог, 'M'/'N' обрабатываются в UpdateSimulation.
// wWinMain: цикл как раньше: QueryPerformanceCounter → dt →
// UpdateSimulation(dt); RenderFrame(); плюс обработка сообщений диалога
// (IsDialogMessage не обязателен).
