//==============================================================================
// 3D-сцена от третьего лица в стиле GTA — OpenGL 3.3 Core, C++ Builder
// ЛКМ — захват мыши, WASD/стрелки — ходьба, Shift — бег, Пробел — прыжок,
// колесо мыши — зум камеры, Esc — отпустить мышь
//==============================================================================
#define NOMINMAX
#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <vector>

#pragma comment(lib, "opengl32.lib")

//==============================================================================
// 1. МИНИМАЛЬНЫЕ ОПРЕДЕЛЕНИЯ OPENGL (чтобы не тянуть GLAD/GLEW)
//==============================================================================
typedef unsigned int GLenum;  typedef unsigned char GLboolean;
typedef unsigned int GLbitfield; typedef int GLint;   typedef int GLsizei;
typedef unsigned int GLuint;  typedef float GLfloat;  typedef float GLclampf;
typedef double GLclampd;      typedef char GLchar;    typedef void GLvoid;
typedef unsigned char GLubyte; typedef unsigned char GLbyte;
typedef unsigned short GLushort; typedef short GLshort;
typedef ptrdiff_t GLsizeiptr; typedef ptrdiff_t GLintptr;
static struct { GLint sunNDC,size; } UOcc;
static float g_sunVis=1.0f, g_sunVisTarget=1.0f;   // âèäèìîñòü ñîëíöà 0..1
static bool g_occOK=false;
#define GL_FALSE 0
#define GL_TRUE  1

enum : unsigned {
	GL_NONE=0,
	GL_ZERO=0, GL_DST_COLOR=0x0306,
    GL_TRIANGLE_STRIP=0x0008,
	GL_SAMPLES_PASSED=0x8914, GL_QUERY_RESULT=0x8866, GL_QUERY_RESULT_AVAILABLE=0x8867,
	GL_BLEND=0x0BE2, GL_ONE=1, GL_SRC_ALPHA=0x0302, GL_ONE_MINUS_SRC_ALPHA=0x0303,
	GL_POLYGON_OFFSET_FILL=0x8037,
    GL_COLOR_BUFFER_BIT=0x4000, GL_DEPTH_BUFFER_BIT=0x100,
    GL_TRIANGLES=0x0004, GL_CW=0x0900, GL_CCW=0x0901,
    GL_FRONT=0x0404, GL_BACK=0x0405, GL_CULL_FACE=0x0B44, GL_DEPTH_TEST=0x0B71,
    GL_LESS=0x0201, GL_LEQUAL=0x0203,
    GL_TEXTURE_2D=0x0DE1, GL_FLOAT=0x1406, GL_UNSIGNED_INT=0x1405,
    GL_TEXTURE_MAG_FILTER=0x2800, GL_TEXTURE_MIN_FILTER=0x2801,
    GL_TEXTURE_WRAP_S=0x2802, GL_TEXTURE_WRAP_T=0x2803,
    GL_LINEAR=0x2601, GL_CLAMP_TO_BORDER=0x812D, GL_TEXTURE_BORDER_COLOR=0x1004,
    GL_DEPTH_COMPONENT=0x1902, GL_RENDERER=0x1F01,
    GL_MULTISAMPLE=0x809D, GL_TEXTURE0=0x84C0,
    GL_DEPTH_COMPONENT24=0x81A6,
    GL_TEXTURE_COMPARE_MODE=0x884C, GL_TEXTURE_COMPARE_FUNC=0x884D,
    GL_COMPARE_REF_TO_TEXTURE=0x884E,
    GL_ARRAY_BUFFER=0x8892, GL_ELEMENT_ARRAY_BUFFER=0x8893, GL_STATIC_DRAW=0x88E4,
    GL_FRAGMENT_SHADER=0x8B30, GL_VERTEX_SHADER=0x8B31,
    GL_COMPILE_STATUS=0x8B81, GL_LINK_STATUS=0x8B82,
    GL_FRAMEBUFFER=0x8D40, GL_DEPTH_ATTACHMENT=0x8D00,
    GL_FRAMEBUFFER_COMPLETE=0x8CD5
};

// Функции GL 1.1 — импортируются напрямую из opengl32.dll
extern "C" {
__declspec(dllimport) void APIENTRY glColorMask(GLboolean,GLboolean,GLboolean,GLboolean);
__declspec(dllimport) void APIENTRY glBlendFunc(GLenum,GLenum);
__declspec(dllimport) void APIENTRY glPolygonOffset(GLfloat,GLfloat);
__declspec(dllimport) void APIENTRY glClear(GLbitfield);
__declspec(dllimport) void APIENTRY glClearColor(GLclampf,GLclampf,GLclampf,GLclampf);
__declspec(dllimport) void APIENTRY glEnable(GLenum);
__declspec(dllimport) void APIENTRY glDisable(GLenum);
__declspec(dllimport) void APIENTRY glDepthFunc(GLenum);
__declspec(dllimport) void APIENTRY glDepthMask(GLboolean);
__declspec(dllimport) void APIENTRY glCullFace(GLenum);
__declspec(dllimport) void APIENTRY glFrontFace(GLenum);
__declspec(dllimport) void APIENTRY glViewport(GLint,GLint,GLsizei,GLsizei);
__declspec(dllimport) void APIENTRY glDrawArrays(GLenum,GLint,GLsizei);
__declspec(dllimport) void APIENTRY glDrawElements(GLenum,GLsizei,GLenum,const void*);
__declspec(dllimport) void APIENTRY glDrawBuffer(GLenum);
__declspec(dllimport) void APIENTRY glReadBuffer(GLenum);
__declspec(dllimport) const GLubyte* APIENTRY glGetString(GLenum);
__declspec(dllimport) HGLRC APIENTRY wglCreateContext(HDC);
__declspec(dllimport) BOOL  APIENTRY wglDeleteContext(HGLRC);
__declspec(dllimport) BOOL  APIENTRY wglMakeCurrent(HDC,HGLRC);
__declspec(dllimport) PROC  APIENTRY wglGetProcAddress(LPCSTR);
}

// Современные функции — загружаются через wglGetProcAddress
static GLuint (APIENTRY *glCreateShader)(GLenum);
static void   (APIENTRY *glDeleteShader)(GLuint);
static void   (APIENTRY *glShaderSource)(GLuint,GLsizei,const GLchar* const*,const GLint*);
static void   (APIENTRY *glCompileShader)(GLuint);
static void   (APIENTRY *glGetShaderiv)(GLuint,GLenum,GLint*);
static void   (APIENTRY *glGetShaderInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
static GLuint (APIENTRY *glCreateProgram)(void);
static void   (APIENTRY *glAttachShader)(GLuint,GLuint);
static void   (APIENTRY *glLinkProgram)(GLuint);
static void   (APIENTRY *glGetProgramiv)(GLuint,GLenum,GLint*);
static void   (APIENTRY *glGetProgramInfoLog)(GLuint,GLsizei,GLsizei*,GLchar*);
static void   (APIENTRY *glUseProgram)(GLuint);
static GLint  (APIENTRY *glGetUniformLocation)(GLuint,const GLchar*);
static void   (APIENTRY *glGenBuffers)(GLsizei,GLuint*);
static void   (APIENTRY *glBindBuffer)(GLenum,GLuint);
static void   (APIENTRY *glBufferData)(GLenum,GLsizeiptr,const void*,GLenum);
static void   (APIENTRY *glGenVertexArrays)(GLsizei,GLuint*);
static void   (APIENTRY *glBindVertexArray)(GLuint);
static void   (APIENTRY *glVertexAttribPointer)(GLuint,GLint,GLenum,GLboolean,GLsizei,const void*);
static void   (APIENTRY *glEnableVertexAttribArray)(GLuint);
static void   (APIENTRY *glGenTextures)(GLsizei,GLuint*);
static void   (APIENTRY *glBindTexture)(GLenum,GLuint);
static void   (APIENTRY *glTexImage2D)(GLenum,GLint,GLint,GLsizei,GLsizei,GLint,GLenum,GLenum,const void*);
static void   (APIENTRY *glTexParameteri)(GLenum,GLenum,GLint);
static void   (APIENTRY *glTexParameterfv)(GLenum,GLenum,const GLfloat*);
static void   (APIENTRY *glActiveTexture)(GLenum);
static void   (APIENTRY *glGenFramebuffers)(GLsizei,GLuint*);
static void   (APIENTRY *glBindFramebuffer)(GLenum,GLuint);
static void   (APIENTRY *glFramebufferTexture2D)(GLenum,GLenum,GLenum,GLuint,GLint);
static GLenum (APIENTRY *glCheckFramebufferStatus)(GLenum);
static void   (APIENTRY *glUniform1i)(GLint,GLint);
static void   (APIENTRY *glUniform3f)(GLint,GLfloat,GLfloat,GLfloat);
static void   (APIENTRY *glUniform1f)(GLint,GLfloat);
static void   (APIENTRY *glUniform2f)(GLint,GLfloat,GLfloat);
static void   (APIENTRY *glUniformMatrix4fv)(GLint,GLsizei,GLboolean,const GLfloat*);
static void   (APIENTRY *glGenQueries)(GLsizei,GLuint*);
static void   (APIENTRY *glBeginQuery)(GLenum,GLuint);
static void   (APIENTRY *glEndQuery)(GLenum);
static void   (APIENTRY *glGetQueryObjectuiv)(GLuint,GLenum,GLuint*);

static void* GetGLProc(const char* n)
{
    void* p = (void*)wglGetProcAddress(n);
    if (!p || p==(void*)1 || p==(void*)2 || p==(void*)3)
        p = (void*)GetProcAddress(GetModuleHandleA("opengl32.dll"), n);
    return p;
}
#define LOADGL(f) *(void**)&(f) = GetGLProc(#f)
static void LoadGL()
{
	LOADGL(glBeginQuery); LOADGL(glEndQuery); LOADGL(glGetQueryObjectuiv);
	LOADGL(glCreateShader); LOADGL(glDeleteShader); LOADGL(glShaderSource);
    LOADGL(glCompileShader); LOADGL(glGetShaderiv); LOADGL(glGetShaderInfoLog);
	LOADGL(glCreateProgram); LOADGL(glAttachShader); LOADGL(glLinkProgram);
    LOADGL(glGetProgramiv); LOADGL(glGetProgramInfoLog); LOADGL(glUseProgram);
	LOADGL(glGetUniformLocation);
    LOADGL(glGenBuffers); LOADGL(glBindBuffer); LOADGL(glBufferData);
    LOADGL(glGenVertexArrays); LOADGL(glBindVertexArray);
    LOADGL(glVertexAttribPointer); LOADGL(glEnableVertexAttribArray);
    LOADGL(glGenTextures); LOADGL(glBindTexture); LOADGL(glTexImage2D);
    LOADGL(glTexParameteri); LOADGL(glTexParameterfv); LOADGL(glActiveTexture);
    LOADGL(glGenFramebuffers); LOADGL(glBindFramebuffer);
	LOADGL(glFramebufferTexture2D); LOADGL(glCheckFramebufferStatus);
	LOADGL(glUniform1f);
	LOADGL(glUniform2f);
	LOADGL(glUniform1i); LOADGL(glUniform3f); LOADGL(glUniformMatrix4fv);
}

//==============================================================================
// 2. ÌÀÒÅÌÀÒÈÊÀ
//==============================================================================
const float PI = 3.14159265358979f;

struct Vec3 {
    float x,y,z;
    Vec3(){}
    Vec3(float x_,float y_,float z_):x(x_),y(y_),z(z_){}
    Vec3 operator+(const Vec3& o) const { return Vec3(x+o.x,y+o.y,z+o.z); }
    Vec3 operator-(const Vec3& o) const { return Vec3(x-o.x,y-o.y,z-o.z); }
    Vec3 operator*(float s)       const { return Vec3(x*s,y*s,z*s); }
};
static float  dot(const Vec3&a,const Vec3&b){ return a.x*b.x+a.y*b.y+a.z*b.z; }
static Vec3   cross(const Vec3&a,const Vec3&b){
    return Vec3(a.y*b.z-a.z*b.y, a.z*b.x-a.x*b.z, a.x*b.y-a.y*b.x);
}
static float  len(const Vec3&a){ return sqrtf(dot(a,a)); }
static Vec3   norm(const Vec3&a){ float l=len(a); return l>1e-8f ? a*(1.0f/l) : Vec3(0,0,0); }
static Vec3   lerpv(const Vec3&a,const Vec3&b,float t){ return a+((b-a)*t); }

static float smoothstepf(float a,float b,float x){
    float t=(x-a)/(b-a); t=t<0?0:(t>1?1:t); return t*t*(3-2*t);
}

// Ìàòðèöà 4x4, column-major (êàê â OpenGL)
struct Mat4 {
    float m[16];
    Mat4(){ memset(m,0,sizeof(m)); }
    static Mat4 identity(){ Mat4 r; r.m[0]=r.m[5]=r.m[10]=r.m[15]=1.0f; return r; }
    Mat4 operator*(const Mat4& b) const {
        Mat4 r;
        for (int c=0;c<4;c++) for (int rr=0;rr<4;rr++){
            float s=0;
            for (int k=0;k<4;k++) s += m[k*4+rr]*b.m[c*4+k];
            r.m[c*4+rr]=s;
        }
        return r;
    }
};
static Mat4 translate(const Vec3&v){ Mat4 r=Mat4::identity(); r.m[12]=v.x; r.m[13]=v.y; r.m[14]=v.z; return r; }
static Mat4 scaleM(const Vec3&v){ Mat4 r=Mat4::identity(); r.m[0]=v.x; r.m[5]=v.y; r.m[10]=v.z; return r; }
static Mat4 rotX(float a){ float c=cosf(a),s=sinf(a); Mat4 r=Mat4::identity();
    r.m[5]=c; r.m[6]=s; r.m[9]=-s; r.m[10]=c; return r; }
static Mat4 rotY(float a){ float c=cosf(a),s=sinf(a); Mat4 r=Mat4::identity();
    r.m[0]=c; r.m[2]=-s; r.m[8]=s; r.m[10]=c; return r; }
static Mat4 perspective(float fovY,float aspect,float zn,float zf){
    Mat4 r; float f=1.0f/tanf(fovY*0.5f);
    r.m[0]=f/aspect; r.m[5]=f; r.m[10]=(zf+zn)/(zn-zf);
    r.m[11]=-1.0f; r.m[14]=2.0f*zf*zn/(zn-zf); return r;
}
static Mat4 orthoP(float l,float r,float b,float t,float zn,float zf){
    Mat4 m=Mat4::identity();
    m.m[0]=2.0f/(r-l); m.m[5]=2.0f/(t-b); m.m[10]=-2.0f/(zf-zn);
    m.m[12]=-(r+l)/(r-l); m.m[13]=-(t+b)/(t-b); m.m[14]=-(zf+zn)/(zf-zn); return m;
}
static Mat4 lookAt(const Vec3&eye,const Vec3&target,const Vec3&up){
    Vec3 f=norm(target-eye), s=norm(cross(f,up)), u=cross(s,f);
    Mat4 r=Mat4::identity();
    r.m[0]=s.x; r.m[4]=s.y; r.m[8]=s.z;
    r.m[1]=u.x; r.m[5]=u.y; r.m[9]=u.z;
    r.m[2]=-f.x; r.m[6]=-f.y; r.m[10]=-f.z;
    r.m[12]=-dot(s,eye); r.m[13]=-dot(u,eye); r.m[14]=dot(f,eye);
    return r;
}
static Mat4 inverse(const Mat4& mm)   // óíèâåðñàëüíàÿ (Mesa-style)
{
    const float* a=mm.m; float inv[16];
    inv[0]= a[5]*(a[10]*a[15]-a[11]*a[14])-a[9]*(a[6]*a[15]-a[7]*a[14])+a[13]*(a[6]*a[11]-a[7]*a[10]);
    inv[4]=-(a[4]*(a[10]*a[15]-a[11]*a[14])-a[8]*(a[6]*a[15]-a[7]*a[14])+a[12]*(a[6]*a[11]-a[7]*a[10]));
    inv[8]= a[4]*(a[9]*a[15]-a[11]*a[13])-a[8]*(a[5]*a[15]-a[7]*a[13])+a[12]*(a[5]*a[11]-a[7]*a[9]);
    inv[12]=-(a[4]*(a[9]*a[14]-a[10]*a[13])-a[8]*(a[5]*a[14]-a[6]*a[13])+a[12]*(a[5]*a[10]-a[6]*a[9]));
    inv[1]=-(a[1]*(a[10]*a[15]-a[11]*a[14])-a[9]*(a[2]*a[15]-a[3]*a[14])+a[13]*(a[2]*a[11]-a[3]*a[10]));
    inv[5]= a[0]*(a[10]*a[15]-a[11]*a[14])-a[8]*(a[2]*a[15]-a[3]*a[14])+a[12]*(a[2]*a[11]-a[3]*a[10]);
    inv[9]=-(a[0]*(a[9]*a[15]-a[11]*a[13])-a[8]*(a[1]*a[15]-a[3]*a[13])+a[12]*(a[1]*a[11]-a[3]*a[9]));
    inv[13]= a[0]*(a[9]*a[14]-a[10]*a[13])-a[8]*(a[1]*a[14]-a[2]*a[13])+a[12]*(a[1]*a[10]-a[2]*a[9]);
    inv[2]= a[1]*(a[6]*a[15]-a[7]*a[14])-a[5]*(a[2]*a[15]-a[3]*a[14])+a[13]*(a[2]*a[7]-a[3]*a[6]);
    inv[6]=-(a[0]*(a[6]*a[15]-a[7]*a[14])-a[4]*(a[2]*a[15]-a[3]*a[14])+a[12]*(a[2]*a[7]-a[3]*a[6]));
    inv[10]= a[0]*(a[5]*a[15]-a[7]*a[13])-a[4]*(a[1]*a[15]-a[3]*a[13])+a[12]*(a[1]*a[7]-a[3]*a[5]);
    inv[14]=-(a[0]*(a[5]*a[14]-a[6]*a[13])-a[4]*(a[1]*a[14]-a[2]*a[13])+a[12]*(a[1]*a[6]-a[2]*a[5]));
    inv[3]=-(a[1]*(a[6]*a[11]-a[7]*a[10])-a[5]*(a[2]*a[11]-a[3]*a[10])+a[9]*(a[2]*a[7]-a[3]*a[6]));
    inv[7]= a[0]*(a[6]*a[11]-a[7]*a[10])-a[4]*(a[2]*a[11]-a[3]*a[10])+a[8]*(a[2]*a[7]-a[3]*a[6]);
    inv[11]=-(a[0]*(a[5]*a[11]-a[7]*a[9])-a[4]*(a[1]*a[11]-a[3]*a[9])+a[8]*(a[1]*a[7]-a[3]*a[5]));
    inv[15]= a[0]*(a[5]*a[10]-a[6]*a[9])-a[4]*(a[1]*a[10]-a[2]*a[9])+a[8]*(a[1]*a[6]-a[2]*a[5]);
    float det=a[0]*inv[0]+a[1]*inv[4]+a[2]*inv[8]+a[3]*inv[12];
    Mat4 r;
    if (fabsf(det)<1e-12f) return Mat4::identity();
    det=1.0f/det;
    for (int i=0;i<16;i++) r.m[i]=inv[i]*det;
    return r;
}

struct Vec4 { float x,y,z,w; };
static Vec4 xform(const Mat4& m,const Vec3& p){
    Vec4 r;
    r.x=m.m[0]*p.x+m.m[4]*p.y+m.m[8]*p.z+m.m[12];
    r.y=m.m[1]*p.x+m.m[5]*p.y+m.m[9]*p.z+m.m[13];
    r.z=m.m[2]*p.x+m.m[6]*p.y+m.m[10]*p.z+m.m[14];
    r.w=m.m[3]*p.x+m.m[7]*p.y+m.m[11]*p.z+m.m[15];
    return r;
}

//==============================================================================
// 3. ÃÅÎÌÅÒÐÈß (ïðîöåäóðíàÿ)
//==============================================================================
struct Mesh {
    std::vector<float> v;      // pos.xyz + normal.xyz
    std::vector<GLuint> idx;
    GLuint vao,vbo,ebo; GLsizei count;
    Mesh():vao(0),vbo(0),ebo(0),count(0){}
};
static void Push(std::vector<float>&v,const Vec3&p,const Vec3&n){
    v.push_back(p.x); v.push_back(p.y); v.push_back(p.z);
    v.push_back(n.x); v.push_back(n.y); v.push_back(n.z);
}
static Mesh MakeCube()
{
    Mesh m;
    struct F { Vec3 n,u,v; };
    F f[6]={ {{0,0,1},{1,0,0},{0,1,0}},  {{0,0,-1},{-1,0,0},{0,1,0}},
             {{1,0,0},{0,0,-1},{0,1,0}}, {{-1,0,0},{0,0,1},{0,1,0}},
             {{0,1,0},{1,0,0},{0,0,-1}}, {{0,-1,0},{1,0,0},{0,0,1}} };
    for (int i=0;i<6;i++){
        GLuint base=(GLuint)(m.v.size()/6);
        for (int a=0;a<4;a++){
            float su=((a==1||a==2)?0.5f:-0.5f), sv=((a>=2)?0.5f:-0.5f);
            Push(m.v, f[i].n*0.5f + f[i].u*su + f[i].v*sv, f[i].n);
        }
        m.idx.push_back(base); m.idx.push_back(base+1); m.idx.push_back(base+2);
        m.idx.push_back(base); m.idx.push_back(base+2); m.idx.push_back(base+3);
    }
    return m;
}
static Mesh MakeSphere(int lat,int lon)
{
    Mesh m;
    for (int i=0;i<=lat;i++){
        float th=PI*i/lat, st=sinf(th), ct=cosf(th);
        for (int j=0;j<=lon;j++){
            float ph=2*PI*j/lon, x=st*cosf(ph), z=st*sinf(ph);
            Push(m.v, Vec3(x,ct,z), Vec3(x,ct,z));
        }
    }
    for (int i=0;i<lat;i++) for (int j=0;j<lon;j++){
        GLuint a=i*(lon+1)+j, b=a+lon+1;
        m.idx.push_back(a); m.idx.push_back(a+1); m.idx.push_back(b);
        m.idx.push_back(a+1); m.idx.push_back(b+1); m.idx.push_back(b);
    }
    return m;
}
static Mesh MakeCylinder(int seg)
{
    Mesh m;
    for (int i=0;i<2;i++){
        float y=i?0.5f:-0.5f;
        for (int j=0;j<=seg;j++){
            float ph=2*PI*j/seg, c=cosf(ph), s=sinf(ph);
            Push(m.v, Vec3(c,y,s), Vec3(c,0,s));
        }
    }
    for (int j=0;j<seg;j++){
        GLuint a=j, b=j+seg+1;
        m.idx.push_back(a); m.idx.push_back(b); m.idx.push_back(a+1);
        m.idx.push_back(a+1); m.idx.push_back(b); m.idx.push_back(b+1);
    }
    for (int i=0;i<2;i++){   // êðûøêè
        float y=i?0.5f:-0.5f, ny=i?1.0f:-1.0f;
        GLuint c0=(GLuint)(m.v.size()/6); Push(m.v,Vec3(0,y,0),Vec3(0,ny,0));
        GLuint r0=(GLuint)(m.v.size()/6);
        for (int j=0;j<=seg;j++){
            float ph=2*PI*j/seg; Push(m.v,Vec3(cosf(ph),y,sinf(ph)),Vec3(0,ny,0));
        }
        for (int j=0;j<seg;j++){
            if (i){ m.idx.push_back(c0); m.idx.push_back(r0+j+1); m.idx.push_back(r0+j); }
            else  { m.idx.push_back(c0); m.idx.push_back(r0+j);   m.idx.push_back(r0+j+1); }
        }
    }
    return m;
}
static Mesh MakeCone(int seg)
{
    Mesh m;
    for (int j=0;j<=seg;j++){
        float ph=2*PI*j/seg, c=cosf(ph), s=sinf(ph);
        Vec3 n=norm(Vec3(c,1.0f,s));
        Push(m.v, Vec3(0,0.5f,0), n);
        Push(m.v, Vec3(c,-0.5f,s), n);
    }
    for (int j=0;j<seg;j++){
        m.idx.push_back(2*j+1); m.idx.push_back(2*(j+1)); m.idx.push_back(2*(j+1)+1);
    }
    GLuint c0=(GLuint)(m.v.size()/6); Push(m.v,Vec3(0,-0.5f,0),Vec3(0,-1,0));
    GLuint r0=(GLuint)(m.v.size()/6);
    for (int j=0;j<=seg;j++){
        float ph=2*PI*j/seg; Push(m.v,Vec3(cosf(ph),-0.5f,sinf(ph)),Vec3(0,-1,0));
    }
    for (int j=0;j<seg;j++){
        m.idx.push_back(c0); m.idx.push_back(r0+j); m.idx.push_back(r0+j+1);
    }
    return m;
}
static Mesh MakePlane()
{
    Mesh m;
    Push(m.v,Vec3(-0.5f,0,-0.5f),Vec3(0,1,0));
    Push(m.v,Vec3( 0.5f,0,-0.5f),Vec3(0,1,0));
    Push(m.v,Vec3( 0.5f,0, 0.5f),Vec3(0,1,0));
    Push(m.v,Vec3(-0.5f,0, 0.5f),Vec3(0,1,0));
    m.idx.push_back(0); m.idx.push_back(2); m.idx.push_back(1);
    m.idx.push_back(0); m.idx.push_back(3); m.idx.push_back(2);
    return m;
}

static Mesh MakeGrid(int n)
{
    Mesh m;
    for (int j=0;j<=n;j++) for (int i=0;i<=n;i++){
        float x=(float)i/n-0.5f, z=(float)j/n-0.5f;
        Push(m.v, Vec3(x,0,z), Vec3(0,1,0));
    }
    for (int j=0;j<n;j++) for (int i=0;i<n;i++){
        GLuint a=j*(n+1)+i, b=a+n+1;
        m.idx.push_back(a); m.idx.push_back(b); m.idx.push_back(a+1);
        m.idx.push_back(a+1); m.idx.push_back(b); m.idx.push_back(b+1);
    }
    return m;
}

static void Upload(Mesh& m)
{
    glGenVertexArrays(1,&m.vao); glBindVertexArray(m.vao);
    glGenBuffers(1,&m.vbo); glBindBuffer(GL_ARRAY_BUFFER,m.vbo);
    glBufferData(GL_ARRAY_BUFFER,(GLsizeiptr)(m.v.size()*sizeof(float)),m.v.data(),GL_STATIC_DRAW);
    glGenBuffers(1,&m.ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,m.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,(GLsizeiptr)(m.idx.size()*sizeof(GLuint)),m.idx.data(),GL_STATIC_DRAW);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,6*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
    m.count=(GLsizei)m.idx.size();
}
static void DrawMesh(const Mesh& m)
{
    glBindVertexArray(m.vao);
    glDrawElements(GL_TRIANGLES,m.count,GL_UNSIGNED_INT,0);
    glBindVertexArray(0);
}

//==============================================================================
// 4. ØÅÉÄÅÐÛ
//==============================================================================
static const char* VS_MAIN = R"glsl(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aNormal;
uniform mat4 uModel, uView, uProj, uLightVP;
out vec3 vNormal; out vec3 vWorld; out vec4 vLightSpace;
void main(){
    vec4 wp = uModel * vec4(aPos, 1.0);
    vWorld = wp.xyz;
    vNormal = mat3(uModel) * aNormal;
	vLightSpace = uLightVP * vec4(wp.xyz + normalize(vNormal) * 0.06, 1.0);
    gl_Position = uProj * uView * wp;
})glsl";

static const char* FS_MAIN = R"glsl(
#version 330 core
in vec3 vNormal; in vec3 vWorld; in vec4 vLightSpace;
uniform vec3 uColor, uKeyDir, uKeyCol, uFillDir, uFillCol, uCamPos, uFogCol;
uniform float uDayF, uDuskF;
uniform sampler2DShadow uShadowMap;
uniform int uPattern;
uniform vec2 uLakeC;
uniform float uLakeR;
uniform vec2 uCloudOff;
uniform vec2 uSunXZY;
uniform float uCloudSh, uOvercast;
out vec4 outColor;
float hash21(vec2 p){
    p = fract(p * vec2(123.34, 456.21));
    p += dot(p, p + 45.32);
    return fract(p.x * p.y);
}
float vnoise(vec2 p){
    vec2 i=floor(p), f=fract(p);
    f=f*f*(3.0-2.0*f);
    float a=hash21(i), b=hash21(i+vec2(1.0,0.0));
    float c=hash21(i+vec2(0.0,1.0)), d=hash21(i+vec2(1.0,1.0));
    return mix(mix(a,b,f.x), mix(c,d,f.x), f.y);
}
float fbm(vec2 p){
    float v=0.0; float a=0.5;
    for (int i=0;i<5;i++){ v+=a*vnoise(p); p=p*2.03+vec2(1.7,9.2); a*=0.5; }
    return v;
}
float ShadowFactor(){
    vec3 p = vLightSpace.xyz / vLightSpace.w * 0.5 + 0.5;
    if (p.z >= 1.0) return 1.0;
    float ndl  = max(dot(normalize(vNormal), uKeyDir), 0.0);
    float bias = max(0.0022 * (1.0 - ndl), 0.0005);
    float texel = 1.0 / 4096.0;
    float s = 0.0;
    for (int x=-1; x<=1; x++)
        for (int y=-1; y<=1; y++)
            s += texture(uShadowMap, vec3(p.xy + vec2(x,y)*texel*1.5, p.z - bias));
    vec2 edgeDist = abs(p.xy - 0.5);
    float edge = max(edgeDist.x, edgeDist.y);
    float fade = 1.0 - smoothstep(0.48, 0.50, edge);
    return mix(1.0, s / 9.0, fade);
}
void main(){
    vec3 N = normalize(vNormal);
    vec3 base = uColor;
    if (uPattern == 1){
        vec2 g = vWorld.xz;
        float n1 = hash21(floor(g*0.6));
        float n2 = hash21(floor(g*3.1)+17.0);
        base = mix(vec3(0.22,0.42,0.17), vec3(0.30,0.52,0.20), n1);
        base = mix(base, vec3(0.26,0.47,0.22), n2*0.5);
        base = mix(vec3(0.46,0.39,0.27), base, smoothstep(2.5, 5.0, length(g)));
        float dl = length(g - uLakeC);
        float sand = 1.0 - smoothstep(uLakeR-1.5, uLakeR+3.0, dl);
        base = mix(base, vec3(0.55,0.47,0.33), sand*0.9);
        base *= 1.0 - 0.18*(1.0-smoothstep(uLakeR-2.0, uLakeR, dl));
    }
    float ndlK = max(dot(N, uKeyDir), 0.0);
    float ndlF = max(dot(N, uFillDir), 0.0);
    float sh = ShadowFactor();
    // ---- движущиеся тени облаков ----
    float cloudSh = 1.0;
    if (uCloudSh > 0.001){
        vec2 cp = vWorld.xz + uSunXZY*(90.0 - vWorld.y) + uCloudOff;
        float cc = fbm(cp*0.006) + 0.18*fbm(cp*0.006*2.7 + vec2(5.2,1.3));
        cloudSh = 1.0 - smoothstep(0.52,0.74,cc)*uCloudSh;
    }
    float hemi = 0.5 + 0.5*N.y;
    vec3 dayAmb   = mix(vec3(0.15,0.18,0.14), vec3(0.33,0.40,0.52), hemi);
    vec3 nightAmb = mix(vec3(0.020,0.028,0.050), vec3(0.045,0.060,0.100), hemi);
    vec3 amb = mix(nightAmb, dayAmb, uDayF);
    amb = mix(amb, vec3(0.30,0.19,0.13), uDuskF*0.45);            // тёплый воздух в сумерки
    vec3 col = base * (amb + uKeyCol*ndlK*sh*cloudSh + uFillCol*ndlF);
    col = mix(col, col*vec3(1.18,0.80,0.58), uDuskF*0.50);        // закатная окраска сцены
    float dist = length(vWorld - uCamPos);
    col = mix(col, uFogCol, clamp(1.0-exp(-dist*dist*0.000020), 0.0, 1.0));
    col *= 1.0 - uOvercast*0.16*uDayF;                            // облачность приглушает
    float luma = dot(col, vec3(0.299,0.587,0.114));
    col = mix(col, vec3(luma), uOvercast*0.22*uDayF);             // снижение контраста/насыщенности
    outColor = vec4(col, 1.0);
})glsl";

static const char* VS_WATER = R"glsl(
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uModel,uView,uProj,uLightVP;
uniform float uTime,uLakeR;
uniform vec2 uLakeC;
out vec3 vWorld; out vec4 vLightSpace;
float WaveH(vec2 p,float t){
    float h=0.0;
    h+=sin(p.x*0.9+t*1.1)*0.05;
    h+=sin(dot(p,vec2(0.7,0.7))*1.3+t*1.7)*0.04;
    h+=sin(dot(p,vec2(-0.4,0.9))*2.1+t*2.3)*0.025;
    h+=sin(dot(p,vec2(0.9,-0.6))*3.7+t*3.1)*0.015;
    h+=sin(p.x*6.0+p.y*5.0+t*4.0)*0.006;
    return h;
}
void main(){
    vec4 wp=uModel*vec4(aPos,1.0);
    float edge=1.0-smoothstep(uLakeR-3.0,uLakeR,length(wp.xz-uLakeC));
    wp.y+=WaveH(wp.xz,uTime)*edge;      // волны затухают у берега
    vWorld=wp.xyz;
    vLightSpace=uLightVP*wp;
    gl_Position=uProj*uView*wp;
})glsl";

static const char* FS_WATER = R"glsl(
#version 330 core
in vec3 vWorld; in vec4 vLightSpace;
uniform vec3 uCamPos,uKeyDir,uKeyCol,uFogCol;
uniform float uTime,uDayF,uLakeR;
uniform vec2 uLakeC;
uniform sampler2DShadow uShadowMap;
out vec4 outColor;
float WaveH(vec2 p,float t){
    float h=0.0;
    h+=sin(p.x*0.9+t*1.1)*0.05;
    h+=sin(dot(p,vec2(0.7,0.7))*1.3+t*1.7)*0.04;
    h+=sin(dot(p,vec2(-0.4,0.9))*2.1+t*2.3)*0.025;
    h+=sin(dot(p,vec2(0.9,-0.6))*3.7+t*3.1)*0.015;
    h+=sin(p.x*6.0+p.y*5.0+t*4.0)*0.006;
    return h;
}
float ShadowFactor(){
    vec3 p=vLightSpace.xyz/vLightSpace.w*0.5+0.5;
    if (p.z>=1.0) return 1.0;
    float ndl=max(dot(vec3(0.0,1.0,0.0),uKeyDir),0.0);
    float bias=max(0.0022*(1.0-ndl),0.0005);
    float texel=1.0/4096.0;
    float s=0.0;
    for (int x=-1;x<=1;x++) for (int y=-1;y<=1;y++)
        s+=texture(uShadowMap,vec3(p.xy+vec2(x,y)*texel*1.5,p.z-bias));
    vec2 ed=abs(p.xy-0.5);
    float fade=1.0-smoothstep(0.48,0.50,max(ed.x,ed.y));
    return mix(1.0,s/9.0,fade);
}
void main(){
    float e=0.22;                                       // нормаль из волн
    float hL=WaveH(vWorld.xz-vec2(e,0.0),uTime);
    float hR=WaveH(vWorld.xz+vec2(e,0.0),uTime);
    float hD=WaveH(vWorld.xz-vec2(0.0,e),uTime);
    float hU=WaveH(vWorld.xz+vec2(0.0,e),uTime);
    vec3 N=normalize(vec3(hL-hR,2.0*e,hD-hU));
    vec3 V=normalize(uCamPos-vWorld);
    vec3 R=reflect(-V,N);
    float fres=0.02+0.98*pow(1.0-max(dot(N,V),0.0),5.0);
    vec3 zen=mix(vec3(0.010,0.015,0.040),vec3(0.11,0.31,0.66),uDayF);
    vec3 sky=(R.y>=0.0)?mix(uFogCol,zen,pow(R.y,0.6)):uFogCol;   // отражение неба
    float sh=ShadowFactor();
    float sd=max(dot(R,uKeyDir),0.0);                   // дорожка солнца/луны
    float spec=pow(sd,200.0)*2.5+pow(sd,24.0)*0.10;
    vec3 deep=mix(vec3(0.008,0.012,0.020),vec3(0.02,0.11,0.14),uDayF);
    float dl=length(vWorld.xz-uLakeC);
    float edge=1.0-smoothstep(uLakeR-2.5,uLakeR,dl);
    float shore=1.0-smoothstep(uLakeR-6.0,uLakeR,dl);
    vec3 waterBase=mix(deep,vec3(0.10,0.20,0.18),(1.0-shore)*0.65); // мелководье
    vec3 col=mix(waterBase,sky,fres)+uKeyCol*spec*sh;
    float alpha=(0.42+0.52*fres)*mix(0.55,1.0,shore)*edge;
    outColor=vec4(col,clamp(alpha,0.0,0.95));
})glsl";

static const char* VS_SHADOW = R"glsl(
#version 330 core
layout(location=0) in vec3 aPos;
uniform mat4 uLightVP, uModel;
void main(){ gl_Position = uLightVP * uModel * vec4(aPos, 1.0); })glsl";

static const char* FS_SHADOW = R"glsl(
#version 330 core
out vec4 outColor;
void main(){ outColor = vec4(0.0); })glsl";

static const char* VS_SKY = R"glsl(
#version 330 core
layout(location=0) in vec2 aPos;
uniform mat4 uInvVP;
uniform vec3 uCamPos;
out vec3 vDir;
void main(){
	gl_Position = vec4(aPos, 1.0, 1.0);
	vec4 p = uInvVP * vec4(aPos, 1.0, 1.0);
	vDir = p.xyz / p.w - uCamPos;
})glsl";

static const char* FS_SKY = R"glsl(
#version 330 core
in vec3 vDir;
uniform vec3 uSunDir, uMoonDir, uCamPos;
uniform float uDayF, uDuskF, uTime;
out vec4 outColor;
float hash13(vec3 p){ p=fract(p*0.1031); p+=dot(p,p.zyx+31.32); return fract((p.x+p.y)*p.z); }
float hash21c(vec2 p){ p=fract(p*vec2(123.34,456.21)); p+=dot(p,p+45.32); return fract(p.x*p.y); }
float vnoise(vec2 p){
    vec2 i=floor(p), f=fract(p);
    f=f*f*(3.0-2.0*f);
    float a=hash21c(i), b=hash21c(i+vec2(1.0,0.0));
    float c=hash21c(i+vec2(0.0,1.0)), d=hash21c(i+vec2(1.0,1.0));
    return mix(mix(a,b,f.x), mix(c,d,f.x), f.y);
}
float fbm(vec2 p){
    float v=0.0; float a=0.5;
    for (int i=0;i<5;i++){ v+=a*vnoise(p); p=p*2.03+vec2(1.7,9.2); a*=0.5; }
    return v;
}
void main(){
    vec3 d=normalize(vDir);
    float h=d.y;
    float dayF=uDayF;
    float dusk=uDuskF;
    vec3 zenith=mix(vec3(0.010,0.015,0.040), vec3(0.11,0.31,0.66), dayF);
    vec3 horizon=mix(vec3(0.030,0.045,0.090), vec3(0.72,0.83,0.96), dayF);
    vec3 dh=normalize(vec3(d.x,0.0,d.z)+vec3(1e-5));
    vec3 shd=normalize(vec3(uSunDir.x,0.0,uSunDir.z)+vec3(1e-5));
    float sunGlow=pow(max(dot(dh,shd),0.0),2.0);
    vec3 duskHorizon=mix(vec3(0.42,0.20,0.42), vec3(1.00,0.40,0.10), sunGlow);
    vec3 duskZenith=vec3(0.22,0.16,0.40);
    horizon=mix(horizon,duskHorizon,dusk);
    zenith=mix(zenith,duskZenith,dusk*0.5);
    vec3 below=mix(vec3(0.010,0.012,0.020), vec3(0.52,0.58,0.56), dayF);
    below=mix(below,vec3(0.40,0.22,0.18),dusk*0.6);
    vec3 col=(h>=0.0)?mix(horizon,zenith,pow(h,0.60)):mix(horizon,below,pow(-h,0.45));
    float band=exp(-h*h*14.0)*dusk;
    col+=vec3(1.00,0.50,0.22)*band*(0.25+0.45*sunGlow);
    vec3 sp=floor(d*220.0);
    float star=step(0.9985,hash13(sp))*(1.0-dayF)*smoothstep(0.0,0.12,h);
    float s=max(dot(d,uSunDir),0.0);
    vec3 sunTint=mix(vec3(1.0,0.93,0.78), vec3(1.0,0.55,0.25), dusk);
    col+=sunTint*pow(s,1500.0)*6.0*dayF;
    col+=sunTint*pow(s,8.0)*0.20*dayF;
    float m=max(dot(d,uMoonDir),0.0);
    col+=vec3(0.85,0.90,1.00)*pow(m,2000.0)*2.0*(1.0-dayF);
    col+=vec3(0.35,0.45,0.70)*pow(m,8.0)*0.10*(1.0-dayF);
    // ==== Облака: два слоя + фейковый объём ====
    float cloudA=0.0;
    if (h>0.015){
        float invY=1.0/max(d.y,0.03);
        // --- слой 1: основной, высота 90 м ---
        vec2 cuv=uCamPos.xz+d.xz*invY*90.0+vec2(uTime*1.6,uTime*0.5);
        float sc=0.006;
        float cov=fbm(cuv*sc)+0.18*fbm(cuv*sc*2.7+vec2(5.2,1.3));
        float cl=smoothstep(0.52,0.74,cov)*smoothstep(0.015,0.14,h);
        vec3 cloudCol=vec3(0.0);
        if (cl>0.001){
            float covS=fbm((cuv+shd.xz*9.0)*sc);          // ïëîòíîñòü â ñòîðîíó ñîëíöà
            float grad=cov-covS;                       // ãðàäèåíò = ïñåâäî-íîðìàëü
            float lit=clamp(0.55+grad*2.2,0.0,1.0)*(0.30+0.70*dayF);
            float edgeGlow=(1.0-smoothstep(0.56,0.74,cov))*0.55;  // ñâåòëûå òîíêèå êðàÿ
            vec3 bright=mix(vec3(0.10,0.11,0.16), vec3(1.02,1.02,1.04), dayF);
            vec3 dark  =mix(vec3(0.030,0.035,0.060), vec3(0.52,0.54,0.60), dayF);
            cloudCol=mix(dark,bright,lit)+bright*edgeGlow*0.6;
            cloudCol=mix(cloudCol, vec3(1.15,0.55,0.30), dusk*(0.30+0.60*sunGlow)*(0.35+0.65*lit));
            cloudCol+=sunTint*pow(s,16.0)*0.8*dayF;    // ñåðåáðÿíàÿ êàéìà
        }
        // --- ñëîé 2: âûñîêèé è ðåäêèé, 150 ì ---
        vec2 cuv2=uCamPos.xz+d.xz*invY*150.0+vec2(uTime*2.3,uTime*0.7);
        float cov2=fbm(cuv2*0.0035+vec2(9.7,3.1));
        float cl2=smoothstep(0.56,0.76,cov2)*smoothstep(0.02,0.18,h)*0.45;
        vec3 col2=mix(vec3(0.05,0.06,0.10), vec3(0.95,0.95,0.99), dayF);
        col2=mix(col2, vec3(1.05,0.60,0.40), dusk*0.5);
        col=mix(col,col2,cl2);
        col=mix(col,cloudCol,cl*0.95);
        cloudA=max(cl,cl2);
    }
    col+=vec3(0.85,0.90,1.00)*star*(1.0-cloudA);
    outColor=vec4(col,1.0);
})glsl";

static const char* VS_GLARE = R"glsl(
#version 330 core
layout(location=0) in vec2 aPos;
out vec2 vUV;
void main(){
    gl_Position = vec4(aPos, 0.9999, 1.0);
    vUV = aPos*0.5 + 0.5;
})glsl";

static const char* FS_GLARE = R"glsl(
#version 330 core
in vec2 vUV;
uniform vec2 uSunUV;
uniform float uIntensity;
uniform float uAspect;
out vec4 outColor;
void main(){
    vec2 d=vUV-uSunUV;
    d.x*=uAspect;
    float dist=length(d);
    float core=exp(-dist*dist*22.0);
    float halo=exp(-dist*dist*3.0)*0.55;
    vec3 col=vec3(1.0,0.95,0.80)*(core+halo)*uIntensity;
    outColor=vec4(col,1.0);
})glsl";

static const char* VS_DIM = R"glsl(
#version 330 core
layout(location=0) in vec2 aPos;
void main(){ gl_Position = vec4(aPos, 0.9999, 1.0); })glsl";

static const char* FS_DIM = R"glsl(
#version 330 core
uniform float uDim;          // 0 = íåò çàòåìíåíèÿ, 1 = ìàêñèìàëüíîå
out vec4 outColor;
void main(){
    float b = 1.0 - uDim*0.65;           // ñöåíà ãàñíåò äî ~35% ÿðêîñòè
    outColor = vec4(b, b, b, 1.0);
})glsl";



static GLuint CompileShader(GLenum type,const char* src)
{
    GLuint s=glCreateShader(type);
    glShaderSource(s,1,&src,NULL);
    glCompileShader(s);
    GLint ok=0; glGetShaderiv(s,GL_COMPILE_STATUS,&ok);
    if (!ok){
        char log[2048]=""; glGetShaderInfoLog(s,sizeof(log),NULL,log);
        char buf[2300]; sprintf(buf,"Îøèáêà øåéäåðà:\n%s",log);
        MessageBoxA(NULL,buf,"GLSL",MB_ICONERROR);
    }
    return s;
}
static GLuint LinkProgram(const char* vs,const char* fs)
{
    GLuint v=CompileShader(GL_VERTEX_SHADER,vs);
    GLuint f=CompileShader(GL_FRAGMENT_SHADER,fs);
    GLuint p=glCreateProgram();
    glAttachShader(p,v); glAttachShader(p,f); glLinkProgram(p);
    GLint ok=0; glGetProgramiv(p,GL_LINK_STATUS,&ok);
    if (!ok){
        char log[2048]=""; glGetProgramInfoLog(p,sizeof(log),NULL,log);
        char buf[2300]; sprintf(buf,"Îøèáêà ëèíêîâêè:\n%s",log);
        MessageBoxA(NULL,buf,"GLSL",MB_ICONERROR);
        return 0;
    }
    glDeleteShader(v); glDeleteShader(f);
    return p;
}

//==============================================================================
// 5. ÑÎÑÒÎßÍÈÅ ÑÖÅÍÛ
//==============================================================================
struct Prop {
    Mesh* mesh; Mat4 model; Vec3 color; bool casts; int pattern;
    Prop(Mesh* m_,const Mat4& t,const Vec3& c,bool cs,int pt)
        :mesh(m_),model(t),color(c),casts(cs),pattern(pt){}
};
struct Part {
    Mat4 m; Vec3 c;
    Part(const Mat4& m_,const Vec3& c_):m(m_),c(c_){}
};
struct Player {
    Vec3 pos; float yaw; Vec3 vel; float vy; bool grounded; float phase;
};

static Mesh g_cube,g_sphere,g_cyl,g_cone,g_plane;
static std::vector<Prop> g_props;
static std::vector<Part> g_charParts;
static Player ch = { Vec3(0,0,0), 0.0f, Vec3(0,0,0), 0.0f, true, 0.0f };
static Vec3 g_sunDir, g_camPos, g_camTarget;

static float g_timeOfDay=0.32f, g_dayLength=240.0f;   // 240 ñåê íà ïîëíûå ñóòêè
static float g_time=0.0f;
static float g_dayF=1.0f, g_duskF=0.0f;
static Vec3 g_moonDir(0,-1,0), g_keyDir(0,1,0), g_keyCol(1,1,1);
static Vec3 g_fillDir(0,1,0), g_fillCol(0,0,0), g_fogCol(0.72f,0.83f,0.96f);

static HWND g_hwnd; static HDC g_hdc; static HGLRC g_hrc;
static int g_w=1280, g_h=720;
static bool g_keys[256]; static bool g_mouseLocked=false; static bool g_msaa=false;
static float g_camYaw=0.0f, g_camPitch=0.35f, g_camDist=6.0f;

static GLuint progMain,progSky,progShadow;
//static GLuint progGlare;
static float g_adapt=0.0f;   // íàêîïëåííàÿ çàñâåòêà (àäàïòàöèÿ ãëàç)
static GLuint progGlare, progDim;
static struct { GLint sunUV,intensity,aspect; } UG;
static struct { GLint dim; } UDim;
static GLuint g_shadowFBO,g_shadowTex,g_skyVAO,g_skyVBO;
static const int SHADOW_RES=4096;
static struct { GLint model,view,proj,lightVP,color,keyDir,keyCol,fillDir,fillCol,cam,shadow,pattern,dayF,fogCol,lakeC,lakeR,duskF,cloudOff,sunXZY,cloudSh,overcast; } U;
static struct { GLint invVP,cam,sun,moon,dayF,duskF,time; } US;
static struct { GLint lightVP,model; } UD;
static char g_renderer[128]="";

static unsigned int g_seed=987654321u;
static float Rnd(){ g_seed=g_seed*1664525u+1013904223u; return (g_seed>>8)/16777216.0f; }

static Mesh g_grid;
static GLuint progWater;
static struct { GLint model,view,proj,lightVP,time,cam,keyDir,keyCol,fogCol,dayF,shadow,lakeC,lakeR; } UW;
static float g_lakeX=40.0f, g_lakeZ=-35.0f, g_lakeR=26.0f, g_waterY=0.07f;

static void BuildWorld()
{
    g_props.clear(); g_props.reserve(256);
    g_props.push_back(Prop(&g_plane, scaleM(Vec3(420,1,420)), Vec3(0.3f,0.5f,0.25f), false, 1));
    for (int i=0;i<46;i++){                                  // äåðåâüÿ
        float a=Rnd()*2*PI, r=12.0f+Rnd()*80.0f;
		float x=cosf(a)*r, z=sinf(a)*r, h=2.4f+Rnd()*1.6f, tr=0.18f+Rnd()*0.08f;

        if (sqrtf((x-g_lakeX)*(x-g_lakeX)+(z-g_lakeZ)*(z-g_lakeZ))<g_lakeR+3.0f) continue;

        Vec3 trunkC(0.36f+Rnd()*0.06f, 0.25f+Rnd()*0.05f, 0.15f+Rnd()*0.04f);
        g_props.push_back(Prop(&g_cyl, translate(Vec3(x,h*0.5f,z))*scaleM(Vec3(tr,h,tr)), trunkC, true, 0));
        if (Rnd()<0.5f){
            float cr=1.2f+Rnd()*0.9f;
            Vec3 leaf(0.12f+Rnd()*0.06f, 0.34f+Rnd()*0.12f, 0.14f+Rnd()*0.06f);
            g_props.push_back(Prop(&g_cone, translate(Vec3(x,h+1.4f,z))*scaleM(Vec3(cr,3.0f,cr)), leaf, true, 0));
        } else {
            float cr=1.3f+Rnd()*1.0f;
            Vec3 leaf(0.15f+Rnd()*0.08f, 0.40f+Rnd()*0.12f, 0.13f+Rnd()*0.06f);
            g_props.push_back(Prop(&g_sphere, translate(Vec3(x,h+cr*0.7f,z))*scaleM(Vec3(cr,cr*0.9f,cr)), leaf, true, 0));
        }
    }
    for (int i=0;i<24;i++){                                  // êàìíè
        float a=Rnd()*2*PI, r=8.0f+Rnd()*88.0f;
        float x=cosf(a)*r, z=sinf(a)*r;
        float sx=0.35f+Rnd()*0.9f, sy=0.25f+Rnd()*0.5f, sz=0.35f+Rnd()*0.9f;
        float g=0.38f+Rnd()*0.15f;
        g_props.push_back(Prop(&g_sphere, translate(Vec3(x,sy*0.8f,z))*rotY(Rnd()*6.28f)*scaleM(Vec3(sx,sy,sz)), Vec3(g,g+0.01f,g+0.02f), true, 0));
    }
    for (int i=0;i<36;i++){                                  // êóñòû
        float a=Rnd()*2*PI, r=6.0f+Rnd()*88.0f;
        float x=cosf(a)*r, z=sinf(a)*r, s=0.35f+Rnd()*0.45f;
        g_props.push_back(Prop(&g_sphere, translate(Vec3(x,s*0.7f,z))*scaleM(Vec3(s,s*0.8f,s)), Vec3(0.13f+Rnd()*0.06f,0.32f+Rnd()*0.1f,0.12f), true, 0));
    }
    for (int i=0;i<6;i++){                                   // ÿùèêè
        float x=2.5f+Rnd()*3.0f, z=-4.0f+Rnd()*3.0f, s=0.7f+Rnd()*0.5f;
        g_props.push_back(Prop(&g_cube, translate(Vec3(x,s*0.5f,z))*rotY(Rnd()*1.5f)*scaleM(Vec3(s,s,s)), Vec3(0.55f,0.40f,0.22f), true, 0));
    }
}

static void BuildCharacterParts()
{
    g_charParts.clear();
    Mat4 base = translate(ch.pos)*rotY(ch.yaw);
    float amp=len(ch.vel)/3.6f; if (amp>1.0f) amp=1.0f;
    float sw=sinf(ch.phase)*0.8f*amp;
    float lsw=sw, rsw=-sw, lasw=-sw, rasw=sw;
    if (!ch.grounded){ lsw=-0.45f; rsw=0.55f; lasw=-2.5f; rasw=-2.3f; } // ïîçà ïðûæêà
    Vec3 shirt(0.72f,0.20f,0.18f), pants(0.16f,0.22f,0.38f), skin(0.93f,0.75f,0.60f);
    g_charParts.push_back(Part(base*translate(Vec3(0,1.07f,0))*scaleM(Vec3(0.55f,0.72f,0.30f)), shirt)); // òîðñ
    g_charParts.push_back(Part(base*translate(Vec3(0,1.66f,0))*scaleM(Vec3(0.30f,0.32f,0.30f)), skin));  // ãîëîâà
    g_charParts.push_back(Part(base*translate(Vec3(-0.15f,0.95f,0))*rotX(lsw)*translate(Vec3(0,-0.45f,0))*scaleM(Vec3(0.20f,0.90f,0.24f)), pants));
    g_charParts.push_back(Part(base*translate(Vec3( 0.15f,0.95f,0))*rotX(rsw)*translate(Vec3(0,-0.45f,0))*scaleM(Vec3(0.20f,0.90f,0.24f)), pants));
    g_charParts.push_back(Part(base*translate(Vec3(-0.37f,1.40f,0))*rotX(lasw)*translate(Vec3(0,-0.28f,0))*scaleM(Vec3(0.16f,0.58f,0.18f)), shirt));
    g_charParts.push_back(Part(base*translate(Vec3( 0.37f,1.40f,0))*rotX(rasw)*translate(Vec3(0,-0.28f,0))*scaleM(Vec3(0.16f,0.58f,0.18f)), shirt));
}

//==============================================================================
// 6. ÈÍÈÖÈÀËÈÇÀÖÈß ÊÎÍÒÅÊÑÒÀ (WGL, core profile 3.3 + MSAA)
//==============================================================================
typedef BOOL  (WINAPI *PFNWGLCHOOSEPIXELFORMATARB)(HDC,const int*,const FLOAT*,UINT,int*,UINT*);
typedef HGLRC (WINAPI *PFNWGLCREATECONTEXTATTRIBSARB)(HDC,HGLRC,const int*);
typedef BOOL  (WINAPI *PFNWGLSWAPINTERVALEXT)(int);
static PFNWGLCHOOSEPIXELFORMATARB  pwglChoosePixelFormatARB;
static PFNWGLCREATECONTEXTATTRIBSARB pwglCreateContextAttribsARB;
static PFNWGLSWAPINTERVALEXT       pwglSwapIntervalEXT;
enum { WGL_DRAW_TO_WINDOW_ARB=0x2001, WGL_SUPPORT_OPENGL_ARB=0x2010,
       WGL_DOUBLE_BUFFER_ARB=0x2011, WGL_PIXEL_TYPE_ARB=0x2013,
       WGL_TYPE_RGBA_ARB=0x202B, WGL_COLOR_BITS_ARB=0x2014,
       WGL_DEPTH_BITS_ARB=0x2022, WGL_STENCIL_BITS_ARB=0x2023,
       WGL_SAMPLE_BUFFERS_ARB=0x2041, WGL_SAMPLES_ARB=0x2042,
       WGL_CONTEXT_MAJOR_VERSION_ARB=0x2091, WGL_CONTEXT_MINOR_VERSION_ARB=0x2092,
       WGL_CONTEXT_PROFILE_MASK_ARB=0x9126, WGL_CONTEXT_CORE_PROFILE_BIT_ARB=0x1 };

static bool InitGL(HWND hwnd)
{
    g_hdc = GetDC(hwnd);
    // âðåìåííîå îêíî è legacy-êîíòåêñò, ÷òîáû çàãðóçèòü wgl-ðàñøèðåíèÿ
    WNDCLASSA wc={}; wc.lpfnWndProc=DefWindowProcA;
    wc.hInstance=GetModuleHandleA(NULL); wc.lpszClassName="GLDummy";
    RegisterClassA(&wc);
    HWND dummy=CreateWindowA("GLDummy","d",0,0,0,8,8,NULL,NULL,wc.hInstance,NULL);
    HDC ddc=GetDC(dummy);
    PIXELFORMATDESCRIPTOR pfd={}; pfd.nSize=sizeof(pfd); pfd.nVersion=1;
    pfd.dwFlags=PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER;
    pfd.iPixelType=PFD_TYPE_RGBA; pfd.cColorBits=32; pfd.cDepthBits=24;
    int dummyFmt=ChoosePixelFormat(ddc,&pfd);
    SetPixelFormat(ddc,dummyFmt,&pfd);
    HGLRC tmpRC=wglCreateContext(ddc);
    wglMakeCurrent(ddc,tmpRC);
    pwglChoosePixelFormatARB=(PFNWGLCHOOSEPIXELFORMATARB)wglGetProcAddress("wglChoosePixelFormatARB");
    pwglCreateContextAttribsARB=(PFNWGLCREATECONTEXTATTRIBSARB)wglGetProcAddress("wglCreateContextAttribsARB");
    pwglSwapIntervalEXT=(PFNWGLSWAPINTERVALEXT)wglGetProcAddress("wglSwapIntervalEXT");

    int chosen=0; bool got=false; g_msaa=false;
    if (pwglChoosePixelFormatARB){
        int base[]={WGL_DRAW_TO_WINDOW_ARB,1,WGL_SUPPORT_OPENGL_ARB,1,WGL_DOUBLE_BUFFER_ARB,1,
                    WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB,WGL_COLOR_BITS_ARB,32,
                    WGL_DEPTH_BITS_ARB,24,WGL_STENCIL_BITS_ARB,8,0,0};
        int ms[]={WGL_DRAW_TO_WINDOW_ARB,1,WGL_SUPPORT_OPENGL_ARB,1,WGL_DOUBLE_BUFFER_ARB,1,
                  WGL_PIXEL_TYPE_ARB,WGL_TYPE_RGBA_ARB,WGL_COLOR_BITS_ARB,32,
                  WGL_DEPTH_BITS_ARB,24,WGL_STENCIL_BITS_ARB,8,
                  WGL_SAMPLE_BUFFERS_ARB,1,WGL_SAMPLES_ARB,4,0,0};
        UINT nf=0;
        if (pwglChoosePixelFormatARB(g_hdc,ms,NULL,1,&chosen,&nf)&&nf>0){ got=true; g_msaa=true; }
        else if (pwglChoosePixelFormatARB(g_hdc,base,NULL,1,&chosen,&nf)&&nf>0){ got=true; }
    }
    if (got){
        PIXELFORMATDESCRIPTOR pfd2={}; pfd2.nSize=sizeof(pfd2);
        DescribePixelFormat(g_hdc,chosen,sizeof(pfd2),&pfd2);
        if (!SetPixelFormat(g_hdc,chosen,&pfd2)) got=false;
    }
    if (!got){
        int f=ChoosePixelFormat(g_hdc,&pfd);
        if (!f || !SetPixelFormat(g_hdc,f,&pfd)){
            wglMakeCurrent(NULL,NULL); wglDeleteContext(tmpRC);
            ReleaseDC(dummy,ddc); DestroyWindow(dummy);
            MessageBoxA(NULL,"Íå íàéäåí ïîäõîäÿùèé ôîðìàò ïèêñåëåé.","Îøèáêà",MB_ICONERROR);
            return false;
        }
    }
    if (pwglCreateContextAttribsARB){
        int attrs[]={WGL_CONTEXT_MAJOR_VERSION_ARB,3,WGL_CONTEXT_MINOR_VERSION_ARB,3,
                     WGL_CONTEXT_PROFILE_MASK_ARB,WGL_CONTEXT_CORE_PROFILE_BIT_ARB,0,0};
        g_hrc=pwglCreateContextAttribsARB(g_hdc,NULL,attrs);
    }
    wglMakeCurrent(NULL,NULL); wglDeleteContext(tmpRC);
    ReleaseDC(dummy,ddc); DestroyWindow(dummy);
    UnregisterClassA("GLDummy",GetModuleHandleA(NULL));
    if (!g_hrc){
        MessageBoxA(NULL,"Äðàéâåð íå ïîääåðæèâàåò OpenGL 3.3 Core. Îáíîâèòå äðàéâåð âèäåîêàðòû.",
                    "Îøèáêà",MB_ICONERROR);
        return false;
    }
    wglMakeCurrent(g_hdc,g_hrc);
    if (pwglSwapIntervalEXT) pwglSwapIntervalEXT(1);   // VSync
	LoadGL();
    const char* r=(const char*)glGetString(GL_RENDERER);
    if (r) strncpy(g_renderer,r,sizeof(g_renderer)-1);
    return true;
}

//==============================================================================
// 7. ÑÎÇÄÀÍÈÅ ÑÖÅÍÛ
//==============================================================================
static bool InitScene()
{
    progShadow=LinkProgram(VS_SHADOW,FS_SHADOW);
	progMain  =LinkProgram(VS_MAIN,FS_MAIN);
	progSky   =LinkProgram(VS_SKY,FS_SKY);

	progGlare=LinkProgram(VS_GLARE,FS_GLARE);
	UG.sunUV=glGetUniformLocation(progGlare,"uSunUV");
	UG.intensity=glGetUniformLocation(progGlare,"uIntensity");
	UG.aspect=glGetUniformLocation(progGlare,"uAspect");

	progDim=LinkProgram(VS_DIM,FS_DIM);
	UDim.dim=glGetUniformLocation(progDim,"uDim");
    progWater=LinkProgram(VS_WATER,FS_WATER);
	//UG.coreIntensity=glGetUniformLocation(progGlare,"uCoreIntensity");
	//UG.aspect=glGetUniformLocation(progGlare,"uAspect");
	//if (!progShadow||!progMain||!progSky||!progGlare) return false;
	if (!progShadow||!progMain||!progSky||!progGlare||!progDim||!progWater) return false;
	U.model=glGetUniformLocation(progMain,"uModel");
	U.view=glGetUniformLocation(progMain,"uView");
	U.proj=glGetUniformLocation(progMain,"uProj");
	U.lightVP=glGetUniformLocation(progMain,"uLightVP");
	U.color=glGetUniformLocation(progMain,"uColor");
	U.keyDir=glGetUniformLocation(progMain,"uKeyDir");
	U.keyCol=glGetUniformLocation(progMain,"uKeyCol");
	U.fillDir=glGetUniformLocation(progMain,"uFillDir");
	U.fillCol=glGetUniformLocation(progMain,"uFillCol");
	U.cam=glGetUniformLocation(progMain,"uCamPos");
	U.shadow=glGetUniformLocation(progMain,"uShadowMap");
	U.pattern=glGetUniformLocation(progMain,"uPattern");
	U.dayF=glGetUniformLocation(progMain,"uDayF");
	U.fogCol=glGetUniformLocation(progMain,"uFogCol");
	US.invVP=glGetUniformLocation(progSky,"uInvVP");
	US.cam=glGetUniformLocation(progSky,"uCamPos");
	US.sun=glGetUniformLocation(progSky,"uSunDir");
	US.moon=glGetUniformLocation(progSky,"uMoonDir");
	US.dayF=glGetUniformLocation(progSky,"uDayF");
	US.duskF=glGetUniformLocation(progSky,"uDuskF");
    UD.lightVP=glGetUniformLocation(progShadow,"uLightVP");
	UD.model=glGetUniformLocation(progShadow,"uModel");
	US.time=glGetUniformLocation(progSky,"uTime");

	UW.model=glGetUniformLocation(progWater,"uModel");
	UW.view=glGetUniformLocation(progWater,"uView");
	UW.proj=glGetUniformLocation(progWater,"uProj");
	UW.lightVP=glGetUniformLocation(progWater,"uLightVP");
	UW.time=glGetUniformLocation(progWater,"uTime");
	UW.cam=glGetUniformLocation(progWater,"uCamPos");
	UW.keyDir=glGetUniformLocation(progWater,"uKeyDir");
	UW.keyCol=glGetUniformLocation(progWater,"uKeyCol");
	UW.fogCol=glGetUniformLocation(progWater,"uFogCol");
	UW.dayF=glGetUniformLocation(progWater,"uDayF");
	UW.shadow=glGetUniformLocation(progWater,"uShadowMap");
	UW.lakeC=glGetUniformLocation(progWater,"uLakeC");
	UW.lakeR=glGetUniformLocation(progWater,"uLakeR");
	U.lakeC=glGetUniformLocation(progMain,"uLakeC");
	U.lakeR=glGetUniformLocation(progMain,"uLakeR");

    U.duskF=glGetUniformLocation(progMain,"uDuskF");
	U.cloudOff=glGetUniformLocation(progMain,"uCloudOff");
	U.sunXZY=glGetUniformLocation(progMain,"uSunXZY");
	U.cloudSh=glGetUniformLocation(progMain,"uCloudSh");
	U.overcast=glGetUniformLocation(progMain,"uOvercast");

	g_grid=MakeGrid(96); Upload(g_grid);

    g_cube=MakeCube();        Upload(g_cube);
    g_sphere=MakeSphere(18,28); Upload(g_sphere);
    g_cyl=MakeCylinder(20);   Upload(g_cyl);
    g_cone=MakeCone(20);      Upload(g_cone);
    g_plane=MakePlane();      Upload(g_plane);

    float sv[6]={-1,-1, 3,-1, -1,3};                    // ïîëíîýêðàííûé òðåóãîëüíèê íåáà
    glGenVertexArrays(1,&g_skyVAO); glBindVertexArray(g_skyVAO);
    glGenBuffers(1,&g_skyVBO); glBindBuffer(GL_ARRAY_BUFFER,g_skyVBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(sv),sv,GL_STATIC_DRAW);
    glVertexAttribPointer(0,2,GL_FLOAT,GL_FALSE,8,(void*)0);
    glEnableVertexAttribArray(0); glBindVertexArray(0);

    glGenTextures(1,&g_shadowTex); glBindTexture(GL_TEXTURE_2D,g_shadowTex); // êàðòà òåíåé
    glTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT24,SHADOW_RES,SHADOW_RES,0,
                 GL_DEPTH_COMPONENT,GL_UNSIGNED_INT,NULL);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_BORDER);
    float border[4]={1,1,1,1};
    glTexParameterfv(GL_TEXTURE_2D,GL_TEXTURE_BORDER_COLOR,border);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_MODE,GL_COMPARE_REF_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_COMPARE_FUNC,GL_LEQUAL);
    glGenFramebuffers(1,&g_shadowFBO); glBindFramebuffer(GL_FRAMEBUFFER,g_shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,g_shadowTex,0);
    glDrawBuffer(GL_NONE); glReadBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)!=GL_FRAMEBUFFER_COMPLETE)
        MessageBoxA(NULL,"Shadow FBO íå ñîáðàí","Îøèáêà",MB_ICONERROR);
    glBindFramebuffer(GL_FRAMEBUFFER,0);

	BuildWorld();

    glEnable(GL_DEPTH_TEST); glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE); glCullFace(GL_BACK); glFrontFace(GL_CCW);
    if (g_msaa) glEnable(GL_MULTISAMPLE);
    glClearColor(0.72f,0.83f,0.96f,1.0f);
	return true;

}

//==============================================================================
// 8. ÂÂÎÄ È ËÎÃÈÊÀ (óïðàâëåíèå êàê â GTA)
//==============================================================================
static void LockMouse(){ if (!g_mouseLocked){ g_mouseLocked=true; while (ShowCursor(FALSE)>=0); } }
static void UnlockMouse(){ if (g_mouseLocked){ g_mouseLocked=false; while (ShowCursor(TRUE)<0); } }

	static bool RaySphereHit(const Vec3& ro,const Vec3& rd,const Vec3& c,float r){
		Vec3 oc=ro-c;
		float b=dot(oc,rd);
		float q=dot(oc,oc)-r*r;
		float disc=b*b-q;
		if (disc<0.0f) return false;
		float s=sqrtf(disc);
		float t=-b-s; if (t<0.0f) t=-b+s;
		return t>0.0f;
	}
	// Òî÷íîå ïåðåñå÷åíèå ëó÷à ñ êîíóñîì (åëü): âåðøèíà apex, öåíòð îñíîâàíèÿ baseC, ðàäèóñ r, âûñîòà h
static bool RayConeHit(const Vec3& ro,const Vec3& rd,const Vec3& baseC,float r,float h,const Vec3& apex){
    float apexY=baseC.y+h;                          // èñòèííàÿ âûñîòà âåðøèíû
    float tCap=(apexY-ro.y)/rd.y;
    if (tCap>0.0f){
        float px=ro.x+rd.x*tCap-baseC.x, pz=ro.z+rd.z*tCap-baseC.z;
        if (px*px+pz*pz<=1e-6f) return true;
    }
    float ox=ro.x-baseC.x, oz=ro.z-baseC.z;
    float dx=rd.x, dz=rd.z, dy=rd.y;
    float k=(r/h)*(r/h);
    float hy=apexY-ro.y;
    float A=dx*dx+dz*dz-k*dy*dy;
    float B=2.0f*(ox*dx+oz*dz)+2.0f*k*dy*hy;
    float C=ox*ox+oz*oz-k*hy*hy;
    float disc=B*B-4.0f*A*C;
    if (disc<0.0f) return false;
    float s=sqrtf(disc);
    for (int i=0;i<2;i++){
        float t=(i==0)?(-B-s)/(2.0f*A):(-B+s)/(2.0f*A);
        if (t>0.0f){
            float y=ro.y+dy*t;
            if (y>=baseC.y-0.01f && y<=apexY){
                float px=ro.x+dx*t-baseC.x, pz=ro.z+rd.z*t-baseC.z;
                if (px*px+pz*pz<=r*r*1.02f) return true;
            }
        }
    }
    return false;
}
	// Ëó÷ â AABB (ÿùèêè)
	static bool RayBoxHit(const Vec3& ro,const Vec3& rd,const Vec3& mn,const Vec3& mx){
		float tmin=-1e30f, tmax=1e30f;
		float o[3]={ro.x,ro.y,ro.z}, d[3]={rd.x,rd.y,rd.z};
		float lo[3]={mn.x,mn.y,mn.z}, hi[3]={mx.x,mx.y,mx.z};
		for (int i=0;i<3;i++){
			if (fabsf(d[i])<1e-8f){ if (o[i]<lo[i]||o[i]>hi[i]) return false; }
			else {
				float t1=(lo[i]-o[i])/d[i], t2=(hi[i]-o[i])/d[i];
				if (t1>t2){ float tt=t1; t1=t2; t2=tt; }
				if (t1>tmin) tmin=t1;
				if (t2<tmax) tmax=t2;
				if (tmin>tmax) return false;
			}
		}
		return tmax>0.0f;
	}
	static void PropSphere(const Prop& p,Vec3& c,float& r){
		c=Vec3(p.model.m[12],p.model.m[13],p.model.m[14]);
		if (p.mesh==&g_cyl||p.mesh==&g_plane){ r=0.0f; return; }
		float sx=sqrtf(p.model.m[0]*p.model.m[0]+p.model.m[1]*p.model.m[1]+p.model.m[2]*p.model.m[2]);
		float sy=sqrtf(p.model.m[4]*p.model.m[4]+p.model.m[5]*p.model.m[5]+p.model.m[6]*p.model.m[6]);
		float sz=sqrtf(p.model.m[8]*p.model.m[8]+p.model.m[9]*p.model.m[9]+p.model.m[10]*p.model.m[10]);
		if (p.mesh==&g_cone){
			float base=(sx+sz)*0.5f;
			r=sqrtf(base*base+(sy*0.5f)*(sy*0.5f));
		} else if (p.mesh==&g_cube){
			float s=sx; if(sy>s)s=sy; if(sz>s)s=sz;
			r=s*0.87f;
		} else {
			float s=sx; if(sy>s)s=sy; if(sz>s)s=sz;
			r=s;
		}
	}
	static float SunVisibilityCPU(){
		const Vec3 ro=g_camPos, rd=g_sunDir;
		for (size_t i=0;i<g_props.size();i++){
			Vec3 c; float r;
			PropSphere(g_props[i],c,r);
			if (r<=0.001f) continue;
			if (!RaySphereHit(ro,rd,c,r*1.05f)) continue;   // ãðóáûé îòñåâ ïî ñôåðå
			if (g_props[i].mesh==&g_cone){                   // åëü — òî÷íûé òåñò êîíóñà
				float cr=(sqrtf(g_props[i].model.m[0]*g_props[i].model.m[0]+g_props[i].model.m[1]*g_props[i].model.m[1]+g_props[i].model.m[2]*g_props[i].model.m[2])
						 +sqrtf(g_props[i].model.m[8]*g_props[i].model.m[8]+g_props[i].model.m[9]*g_props[i].model.m[9]+g_props[i].model.m[10]*g_props[i].model.m[10]))*0.5f;
				float hh=sqrtf(g_props[i].model.m[4]*g_props[i].model.m[4]+g_props[i].model.m[5]*g_props[i].model.m[5]+g_props[i].model.m[6]*g_props[i].model.m[6]);
				Vec3 baseC(c.x,c.y-hh*0.5f,c.z);
				Vec3 apex(c.x,c.y+hh*0.5f,c.z);
				if (RayConeHit(ro,rd,baseC,cr*0.92f,hh,apex)) return 0.0f;
			} else if (g_props[i].mesh==&g_cube){
				float sx=sqrtf(g_props[i].model.m[0]*g_props[i].model.m[0]+g_props[i].model.m[1]*g_props[i].model.m[1]+g_props[i].model.m[2]*g_props[i].model.m[2]);
				float sy=sqrtf(g_props[i].model.m[4]*g_props[i].model.m[4]+g_props[i].model.m[5]*g_props[i].model.m[5]+g_props[i].model.m[6]*g_props[i].model.m[6]);
				float sz=sqrtf(g_props[i].model.m[8]*g_props[i].model.m[8]+g_props[i].model.m[9]*g_props[i].model.m[9]+g_props[i].model.m[10]*g_props[i].model.m[10]);
				if (RayBoxHit(ro,rd,c-Vec3(sx,sy,sz)*0.5f,c+Vec3(sx,sy,sz)*0.5f)) return 0.0f;
			} else {
				return 0.0f;                                 // äóáû/êàìíè/êóñòû — ñôåðà è åñòü ôîðìà
			}
		}
		return 1.0f;
	}


	static float g_overcastU=0.0f, g_cloudShAmt=0.0f;
	static float fractf(float x){ return x-floorf(x); }
	static float hash21c(float x,float y){
		x=fractf(x*123.34f); y=fractf(y*456.21f);
		float d=x*(x+45.32f)+y*(y+45.32f);
		return fractf((x+d)*(y+d));
	}
	static float vnoisef(float x,float y){
		float ix=floorf(x), iy=floorf(y);
		float fx=x-ix, fy=y-iy;
		fx=fx*fx*(3.0f-2.0f*fx); fy=fy*fy*(3.0f-2.0f*fy);
		float a=hash21c(ix,iy), b=hash21c(ix+1.0f,iy);
		float c=hash21c(ix,iy+1.0f), d=hash21c(ix+1.0f,iy+1.0f);
		float m1=a+(b-a)*fx, m2=c+(d-c)*fx;
		return m1+(m2-m1)*fy;
	}
	static float fbmf(float x,float y){
		float v=0.0f, a=0.5f;
		for (int i=0;i<5;i++){ v+=a*vnoisef(x,y); x=x*2.03f+1.7f; y=y*2.03f+9.2f; a*=0.5f; }
		return v;
	}
	static float CloudCover(float px,float pz,float elev){
		float wx=px+g_time*1.6f, wz=pz+g_time*0.5f;
		float cov=fbmf(wx*0.006f,wz*0.006f)+0.18f*fbmf(wx*0.0162f+5.2f,wz*0.0162f+1.3f);
		return smoothstepf(0.52f,0.74f,cov)*smoothstepf(0.015f,0.14f,elev);
	}

static void Update(float dt)
{
	//g_timeOfDay=0.35f;

	if (g_mouseLocked){                                  // âðàùåíèå êàìåðû ìûøüþ
		RECT rc; GetClientRect(g_hwnd,&rc);
		POINT c; c.x=(rc.right-rc.left)/2; c.y=(rc.bottom-rc.top)/2;
		ClientToScreen(g_hwnd,&c);
		POINT p; GetCursorPos(&p);
		int dx=p.x-c.x, dy=p.y-c.y;
		if (dx||dy) SetCursorPos(c.x,c.y);
		g_camYaw   -= dx*0.0032f;
	g_camPitch += dy*0.0030f;
	if (g_camPitch<-1.30f) g_camPitch=-1.30f;   // ââåðõ — òåïåðü ìîæíî çàäðàòü âûñîêî
	if (g_camPitch> 1.30f) g_camPitch= 1.30f;   // âíèç
	}
    Vec3 fwd=norm(Vec3(sinf(g_camYaw),0,cosf(g_camYaw)));  // äâèæåíèå îòíîñèòåëüíî êàìåðû
    Vec3 rgt=cross(fwd,Vec3(0,1,0));
    Vec3 wish(0,0,0);
    if (g_keys['W']||g_keys[VK_UP])    wish=wish+fwd;
    if (g_keys['S']||g_keys[VK_DOWN])  wish=wish-fwd;
    if (g_keys['D']||g_keys[VK_RIGHT]) wish=wish+rgt;
    if (g_keys['A']||g_keys[VK_LEFT])  wish=wish-rgt;
    Vec3 tv(0,0,0);
    float wl=len(wish);
    if (wl>0.001f){
        wish=wish*(1.0f/wl);
        float spd=g_keys[VK_SHIFT]?7.2f:3.6f;
        tv=wish*spd;
    }
    float k=1.0f-expf(-12.0f*dt);
    ch.vel=lerpv(ch.vel,tv,k);
    ch.pos=ch.pos+ch.vel*dt;
    if (g_keys[VK_SPACE]&&ch.grounded){ ch.vy=6.8f; ch.grounded=false; }
    if (!ch.grounded){
        ch.vy-=18.0f*dt; ch.pos.y+=ch.vy*dt;
        if (ch.pos.y<=0.0f){ ch.pos.y=0.0f; ch.vy=0.0f; ch.grounded=true; }
    }
	// Ïåðñîíàæ ñìîòðèò â ñòîðîíó âçãëÿäà êàìåðû (çà ìûøüþ)
float spd2=len(ch.vel);
float want;
if (spd2>0.4f) want=atan2f(ch.vel.x,ch.vel.z);   // â äâèæåíèè — ïî êóðñó
else           want=g_camYaw;                     // íà ìåñòå — çà ìûøüþ
float d=want-ch.yaw;
while (d>PI) d-=2*PI;
while (d<-PI) d+=2*PI;
float t=dt*12.0f; if (t>1.0f) t=1.0f;
ch.yaw+=d*t;
if (spd2>0.4f) ch.phase+=spd2*dt*2.6f;

	if (ch.pos.x> 190.0f) ch.pos.x= 190.0f; if (ch.pos.x<-190.0f) ch.pos.x=-190.0f;
    if (ch.pos.z> 190.0f) ch.pos.z= 190.0f; if (ch.pos.z<-190.0f) ch.pos.z=-190.0f;

// Äèíàìè÷åñêàÿ äèñòàíöèÿ êàìåðû (êàê â GTA)
	float baseDist = g_camDist;
	float pitchFactor = 1.0f;
	if (g_camPitch > 0.0f) {
		// Ïðè âçãëÿäå ââåðõ óìåíüøàåì äèñòàíöèþ (äî 40% ïðè ìàêñèìàëüíîì íàêëîíå)
		pitchFactor = 1.0f - (g_camPitch / 1.30f) * 0.75f;  // äî 25% äèñòàíöèè
	}
	float effectiveDist = baseDist * pitchFactor;

	// Âû÷èñëÿåì ïîçèöèþ êàìåðû
	// ---- Êàìåðà îò 3-ãî ëèöà (ñòèëü GTA, ñ êîëëèçèåé î çåìëþ) ----
	float cp=cosf(g_camPitch), sp=sinf(g_camPitch);
	Vec3 dir(cp*sinf(g_camYaw), -sp, cp*cosf(g_camYaw));   // íàïðàâëåíèå îò êàìåðû ê öåëè
	g_camTarget=ch.pos+Vec3(0,1.5f,0);

	Vec3 camPos=g_camTarget-dir*g_camDist;

	// Êàìåðà íå äîëæíà óõîäèòü ïîä çåìëþ: óêîðà÷èâàåì ëó÷ äî òî÷êè êàñàíèÿ
	const float minCamHeight=0.3f;
	if (camPos.y < minCamHeight && dir.y>0.0001f){
		float t=(g_camTarget.y-minCamHeight)/dir.y;        // äèñòàíöèÿ äî êàñàíèÿ çåìëè
		if (t>0.5f && t<g_camDist) camPos=g_camTarget-dir*t;
	}
	if (camPos.y<minCamHeight) camPos.y=minCamHeight;      // ñòðàõîâêà
	g_camPos=camPos;

		// ---- Öèêë äíÿ è íî÷è ----
	g_time+=dt;
	g_timeOfDay+=dt/g_dayLength;
	if (g_keys['M']) g_timeOfDay+=dt*0.03f;   // M — ïåðåìîòêà âïåð¸ä
	if (g_keys['N']) g_timeOfDay-=dt*0.03f;   // N — íàçàä
	if (g_timeOfDay>=1.0f) g_timeOfDay-=1.0f;
	if (g_timeOfDay<0.0f)  g_timeOfDay+=1.0f;
	float dayAng=(g_timeOfDay-0.25f)*2.0f*PI;             // 0.25 — ðàññâåò, 0.5 — ïîëäåíü
	g_sunDir=norm(Vec3(cosf(dayAng)*0.75f, sinf(dayAng), 0.35f));
	g_moonDir=norm(Vec3(-g_sunDir.x, -g_sunDir.y*0.9f, -g_sunDir.z));
	g_dayF=smoothstepf(-0.10f,0.20f,g_sunDir.y);
	g_duskF = 1.0f - smoothstepf(0.0f, 0.45f, fabsf(g_sunDir.y));  // øèðå è ïëàâíåå
	float warm=g_sunDir.y*2.0f; warm=warm<0?0:(warm>1?1:warm);
	Vec3 sunCol=lerpv(Vec3(1.00f,0.50f,0.22f), Vec3(1.05f,0.96f,0.84f), warm); // îðàíæåâîå íà ðàññâåòå
	float sunI=1.30f*g_dayF;
	Vec3 moonCol(0.55f,0.65f,0.95f);
	float moonI=0.42f*(1.0f-g_dayF);    // ëóíà ñèëüíåå — íî÷íûå òåíè âèäíû
	if (sunI>moonI){ g_keyDir=g_sunDir; g_keyCol=sunCol*sunI; g_fillDir=g_moonDir; g_fillCol=moonCol*moonI; }
	else           { g_keyDir=g_moonDir; g_keyCol=moonCol*moonI; g_fillDir=g_sunDir; g_fillCol=sunCol*sunI; }
	g_fogCol=lerpv(Vec3(0.030f,0.045f,0.090f), Vec3(0.72f,0.83f,0.96f), g_dayF);
    g_fogCol = lerpv(g_fogCol, Vec3(0.95f,0.50f,0.30f), g_duskF*0.55f);
	g_fogCol=g_fogCol+Vec3(1.0f,0.42f,0.15f)*(g_duskF*0.25f);
														// Àäàïòàöèÿ ãëàç ê ÿðêîìó ñâåòó
	// Ïëàâíàÿ âèäèìîñòü ñîëíöà (ðåçóëüòàò îêêëþçèè ñ ïðîøëîé êàäðà)
    // ---- Îáëàêà: âëèÿíèå íà îñëåïëåíèå, òåíè è êîíòðàñò ----
	float covOver=CloudCover(ch.pos.x,ch.pos.z,1.0f);
	g_overcastU+=(covOver-g_overcastU)*(1.0f-expf(-dt*2.0f));
	float cloudSun=0.0f;
	if (g_sunDir.y>0.03f)
		cloudSun=CloudCover(g_camPos.x+g_sunDir.x/g_sunDir.y*90.0f,
							g_camPos.z+g_sunDir.z/g_sunDir.y*90.0f, g_sunDir.y);
	g_cloudShAmt=(g_sunDir.y>0.05f)?g_dayF*(g_sunDir.y*5.0f>1.0f?1.0f:g_sunDir.y*5.0f)*0.45f:0.0f;

	g_sunVisTarget=SunVisibilityCPU();
	g_sunVis+=(g_sunVisTarget-g_sunVis)*(1.0f-expf(-dt*10.0f));

	Vec3 camFwd=norm(g_camTarget-g_camPos);
	float align=dot(camFwd,g_sunDir);
	float target=0.0f;
	if (align>0.50f) target=(align-0.50f)/0.50f;
	target=target*target*target;
	target*=g_sunVis*g_dayF*(1.0f-cloudSun*0.9f);   // îáëàêà ãàñÿò îñëåïëåíèå    // íî÷üþ ñîëíöå íå ñëåïèò                                  // àäàïòàöèÿ òîëüêî êîãäà ñîëíöå âèäíî
	if (target>g_adapt) g_adapt+=(target-g_adapt)*(1.0f-expf(-dt*6.0f));
	else                g_adapt+=(target-g_adapt)*(1.0f-expf(-dt*0.6f));

	BuildCharacterParts();
}

//==============================================================================
// 9. ÐÅÍÄÅÐÈÍÃ: pass òåíåé -> íåáî -> ñöåíà
//==============================================================================
static void Render()
{
	Vec3 center=ch.pos+Vec3(0,1.2f,0);
	Vec3 eye=center+g_keyDir*55.0f;   // êàìåðà ñâåòà íà ñòîðîíå ñîëíöà
	Mat4 lightVP=orthoP(-26,26,-26,26,10,110)*lookAt(eye,center,Vec3(0,1,0));

    // --- pass 1: êàðòà ãëóáèíû ñî ñòîðîíû ñîëíöà ---
	glBindFramebuffer(GL_FRAMEBUFFER,g_shadowFBO);
    glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,0);   // îòâÿçûâàåì shadow-òåêñòóðó ïåðåä çàïèñüþ â íå¸
    glViewport(0,0,SHADOW_RES,SHADOW_RES);
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(progShadow);
    glUniformMatrix4fv(UD.lightVP,1,GL_FALSE,lightVP.m);
    glCullFace(GL_BACK);
    for (size_t i=0;i<g_props.size();i++){
        if (!g_props[i].casts) continue;
        glUniformMatrix4fv(UD.model,1,GL_FALSE,g_props[i].model.m);
        DrawMesh(*g_props[i].mesh);
    }
    for (size_t i=0;i<g_charParts.size();i++){
        glUniformMatrix4fv(UD.model,1,GL_FALSE,g_charParts[i].m.m);
        DrawMesh(g_cube);
    }
    glCullFace(GL_BACK);

    // --- pass 2: îñíîâíàÿ ñöåíà ---
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0,0,g_w,g_h);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    Mat4 proj=perspective(60.0f*PI/180.0f,(float)g_w/(float)g_h,0.1f,600.0f);
    Mat4 view=lookAt(g_camPos,g_camTarget,Vec3(0,1,0));

	glUseProgram(progSky);                                // ãðàäèåíòíîå íåáî
    glDisable(GL_DEPTH_TEST); glDepthMask(GL_FALSE);
	Mat4 invVP=inverse(proj*view);
    glUniformMatrix4fv(US.invVP,1,GL_FALSE,invVP.m);
	glUniform3f(US.cam,g_camPos.x,g_camPos.y,g_camPos.z);
	glUniform3f(US.sun,g_sunDir.x,g_sunDir.y,g_sunDir.z);
    glUniform3f(US.moon,g_moonDir.x,g_moonDir.y,g_moonDir.z);
	glUniform1f(US.dayF,g_dayF);
	glUniform1f(US.duskF,g_duskF);
	glUniform1f(US.time,g_time);

    glUniform2f(U.lakeC,g_lakeX,g_lakeZ);
	glUniform1f(U.lakeR,g_lakeR);

    glBindVertexArray(g_skyVAO); glDrawArrays(GL_TRIANGLES,0,3); glBindVertexArray(0);
    glDepthMask(GL_TRUE); glEnable(GL_DEPTH_TEST);

	glUseProgram(progMain);                               // îáúåêòû è ïåðñîíàæ
    glUniformMatrix4fv(U.view,1,GL_FALSE,view.m);
    glUniformMatrix4fv(U.proj,1,GL_FALSE,proj.m);
    glUniformMatrix4fv(U.lightVP,1,GL_FALSE,lightVP.m);
    glUniform3f(U.keyDir,g_keyDir.x,g_keyDir.y,g_keyDir.z);
	glUniform3f(U.keyCol,g_keyCol.x,g_keyCol.y,g_keyCol.z);
	glUniform3f(U.fillDir,g_fillDir.x,g_fillDir.y,g_fillDir.z);
	glUniform3f(U.fillCol,g_fillCol.x,g_fillCol.y,g_fillCol.z);
	glUniform1f(U.dayF,g_dayF);
	glUniform3f(U.fogCol,g_fogCol.x,g_fogCol.y,g_fogCol.z);

    glUniform2f(U.lakeC,g_lakeX,g_lakeZ);
	glUniform1f(U.lakeR,g_lakeR);
	glUniform1f(U.duskF,g_duskF);
	glUniform2f(U.cloudOff,g_time*1.6f,g_time*0.5f);
	float sxzyX=0.0f, sxzyZ=0.0f;
	if (g_sunDir.y>0.05f){ sxzyX=g_sunDir.x/g_sunDir.y; sxzyZ=g_sunDir.z/g_sunDir.y; }
	glUniform2f(U.sunXZY,sxzyX,sxzyZ);
	glUniform1f(U.cloudSh,g_cloudShAmt);
	glUniform1f(U.overcast,g_overcastU);

	glUniform3f(U.cam,g_camPos.x,g_camPos.y,g_camPos.z);
	glUniform1i(U.shadow,0);
    glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D,g_shadowTex);
    for (size_t i=0;i<g_props.size();i++){
        glUniformMatrix4fv(U.model,1,GL_FALSE,g_props[i].model.m);
        glUniform3f(U.color,g_props[i].color.x,g_props[i].color.y,g_props[i].color.z);
        glUniform1i(U.pattern,g_props[i].pattern);
        DrawMesh(*g_props[i].mesh);
    }
    for (size_t i=0;i<g_charParts.size();i++){
        glUniformMatrix4fv(U.model,1,GL_FALSE,g_charParts[i].m.m);
        glUniform3f(U.color,g_charParts[i].c.x,g_charParts[i].c.y,g_charParts[i].c.z);
        glUniform1i(U.pattern,0);
        DrawMesh(g_cube);
	}

		// ---- Âîäà ----
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glUseProgram(progWater);
	Mat4 wmodel=translate(Vec3(g_lakeX,g_waterY,g_lakeZ))*scaleM(Vec3(g_lakeR*2.0f,1.0f,g_lakeR*2.0f));
	glUniformMatrix4fv(UW.model,1,GL_FALSE,wmodel.m);
	glUniformMatrix4fv(UW.view,1,GL_FALSE,view.m);
	glUniformMatrix4fv(UW.proj,1,GL_FALSE,proj.m);
	glUniformMatrix4fv(UW.lightVP,1,GL_FALSE,lightVP.m);
	glUniform1f(UW.time,g_time);
	glUniform3f(UW.cam,g_camPos.x,g_camPos.y,g_camPos.z);
	glUniform3f(UW.keyDir,g_keyDir.x,g_keyDir.y,g_keyDir.z);
	glUniform3f(UW.keyCol,g_keyCol.x,g_keyCol.y,g_keyCol.z);
	glUniform3f(UW.fogCol,g_fogCol.x,g_fogCol.y,g_fogCol.z);
	glUniform1f(UW.dayF,g_dayF);
	glUniform1i(UW.shadow,0);
	glUniform2f(UW.lakeC,g_lakeX,g_lakeZ);
	glUniform1f(UW.lakeR,g_lakeR);
	DrawMesh(g_grid);
	glDepthMask(GL_TRUE);
	glDisable(GL_BLEND);

													 // ---- Îñëåïëåíèå ñîëíöåì ----
	// ---- Àäàïòàöèÿ ê ÿðêîìó ñâåòó (çðà÷îê ñóæàåòñÿ => òåìíåå) ----
	if (g_adapt>0.001f){
		glDisable(GL_DEPTH_TEST);
		glDepthMask(GL_FALSE);
		glEnable(GL_BLEND);

		// 1) Çàòåìíåíèå âñåé ñöåíû (óìíîæåíèå)
		glUseProgram(progDim);
		glUniform1f(UDim.dim,g_adapt);
		glBlendFunc(GL_DST_COLOR,GL_ZERO);              // dst = src * dst  => òåìíåå
		glBindVertexArray(g_skyVAO); glDrawArrays(GL_TRIANGLES,0,3); glBindVertexArray(0);

		// 2) ßðêîå ñîëíöå ïîâåðõ (àääèòèâíî)
		Vec4 sc=xform(proj*view, g_camPos+g_sunDir*100.0f);
		if (sc.w>0.001f){
			float sunU=(sc.x/sc.w)*0.5f+0.5f;
			float sunV=(sc.y/sc.w)*0.5f+0.5f;
			glUseProgram(progGlare);
			glUniform2f(UG.sunUV,sunU,sunV);
			glUniform1f(UG.intensity,g_adapt*g_sunVis);   // áëèê ãàñíåò, êîãäà ñîëíöå çà îáúåêòîì
			glUniform1f(UG.aspect,(float)g_w/(float)g_h);
			glBlendFunc(GL_ONE,GL_ONE);                 // dst = dst + src  => ÿð÷å
			glBindVertexArray(g_skyVAO); glDrawArrays(GL_TRIANGLES,0,3); glBindVertexArray(0);
		}

		glDisable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glEnable(GL_DEPTH_TEST);
	}
}

//==============================================================================
// 10. ÎÊÍÎ È ÃËÀÂÍÛÉ ÖÈÊË
//==============================================================================
static LRESULT CALLBACK WndProc(HWND h,UINT m,WPARAM w,LPARAM l)
{
    switch (m){
    case WM_CLOSE: DestroyWindow(h); return 0;
    case WM_DESTROY: PostQuitMessage(0); return 0;
    case WM_SIZE: g_w=LOWORD(l); g_h=HIWORD(l); return 0;
    case WM_KEYDOWN:
        if (w<256) g_keys[w]=true;
        if (w==VK_ESCAPE) UnlockMouse();
        return 0;
    case WM_KEYUP: if (w<256) g_keys[w]=false; return 0;
    case WM_LBUTTONDOWN: LockMouse(); return 0;
    case WM_MOUSEWHEEL:
        g_camDist-=GET_WHEEL_DELTA_WPARAM(w)*0.003f;
        if (g_camDist<2.5f) g_camDist=2.5f;
        if (g_camDist>12.0f) g_camDist=12.0f;
        return 0;
    case WM_KILLFOCUS: memset(g_keys,0,sizeof(g_keys)); UnlockMouse(); return 0;
    }
    return DefWindowProcA(h,m,w,l);
}

int WINAPI wWinMain(HINSTANCE hInst,HINSTANCE,LPWSTR,int nCmdShow)
{
    WNDCLASSA wc={};
    wc.style=CS_HREDRAW|CS_VREDRAW|CS_OWNDC;
    wc.lpfnWndProc=WndProc; wc.hInstance=hInst;
	wc.hCursor=LoadCursorA(NULL,(LPCSTR)IDC_ARROW);
	wc.hIcon=LoadIconA(NULL,(LPCSTR)IDI_APPLICATION);
    wc.hbrBackground=(HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszClassName="GtaGLScene";
    RegisterClassA(&wc);
    RECT rc={0,0,1280,720};
    AdjustWindowRect(&rc,WS_OVERLAPPEDWINDOW,FALSE);
    g_hwnd=CreateWindowA("GtaGLScene","Çàãðóçêà...",WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,CW_USEDEFAULT,rc.right-rc.left,rc.bottom-rc.top,NULL,NULL,hInst,NULL);
    if (!g_hwnd) return 1;
    ShowWindow(g_hwnd,nCmdShow); UpdateWindow(g_hwnd);

    if (!InitGL(g_hwnd))  return 1;
    if (!InitScene())     return 1;

    LARGE_INTEGER freq,prev,now;
    QueryPerformanceFrequency(&freq); QueryPerformanceCounter(&prev);
    char title[400]; float acc=0; int frames=0; bool quit=false;
    while (!quit){
        MSG msg;
        while (PeekMessageA(&msg,NULL,0,0,PM_REMOVE)){
            if (msg.message==WM_QUIT){ quit=true; break; }
            TranslateMessage(&msg); DispatchMessageA(&msg);
        }
        if (quit) break;
        QueryPerformanceCounter(&now);
        float dt=(float)((now.QuadPart-prev.QuadPart)/(double)freq.QuadPart);
        prev=now;
        if (dt>0.05f) dt=0.05f;
        if (g_w>1&&g_h>1){ Update(dt); Render(); SwapBuffers(g_hdc); frames++; }
        else Sleep(16);
        acc+=dt;
        if (acc>=1.0f){
            int hh=(int)(g_timeOfDay*24.0f), mm=(int)((g_timeOfDay*24.0f-hh)*60.0f);
				sprintf(title,"OpenGL 3.3 | %.60s | FPS %d | %02d:%02d | WASD - õîäüáà, Shift - áåã, Ïðîáåë - ïðûæîê | ËÊÌ - ìûøü, Esc - îòïóñòèòü | M/N - âðåìÿ",
		g_renderer,(int)(frames/acc),hh,mm);
            SetWindowTextA(g_hwnd,title);
            frames=0; acc=0;
        }
    }
    wglMakeCurrent(NULL,NULL);
    if (g_hrc) wglDeleteContext(g_hrc);
    return 0;
}
