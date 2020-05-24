/*
********************************************************
            TRABALHO DE COMPUTAÇÃO GRÁFICA

                  Steve Minecraft

            DANIELA KUINCHTNER, 152064
            CIÊNCIA DA COMPUTAÇÃO - UPF
            PROF. EVANDRO LUIS VIAPIANA
********************************************************
*/
#include <GL/glut.h>
#include <stdio.h>
#include <math.h>
#include <locale.h>

#define PASSO    5

#define NUM_OBJETOS 10
#define ROBO    0
#define CABECA  1
#define TRONCO  2
#define BRACO   3
#define BRACO2  4
#define QUADRIL 5
#define PERNA   6
#define PERNA2  7
#define PE      8
#define PE2     9

#define NUM_TEX   14
#define TEXTURA1  1000 // cabeça frontal
#define TEXTURA2  1001 // tronco
#define TEXTURA3  1002 // perna
#define TEXTURA4  1003 // braço
#define TEXTURA5  1004 // pé
#define TEXTURA6  1005 // quadril
#define TEXTURA7  1006 //cabeça traseira
#define TEXTURA8  1007 // cabeça esquerda
#define TEXTURA9  1008 // cabeça direita
#define TEXTURA10 1009 // cabeça superior
#define TEXTURA11 1010 // cabeça inferior
#define TEXTURA12 1011 // tronco costas
#define TEXTURA13 1012 // braco superior
#define TEXTURA14 1013 // braco inferior

struct tipo_camera {
   GLfloat posx;               // posição x da camera
   GLfloat posy;               // posição y da camera
   GLfloat posz;               // posição z da camera
   GLfloat alvox;              // alvo x da visualização
   GLfloat alvoy;              // alvo y da visualização
   GLfloat alvoz;              // alvo z da visualização
   GLfloat inicio;             // início da área de visualização em profundidade
   GLfloat fim;                // fim da área de visualização em profundidade
   GLfloat ang;                // abertura da 'lente' - efeito de zoom
};

struct tipo_luz {
   GLfloat posicao[ 4 ];
   GLfloat ambiente[ 4 ];
   GLfloat difusa[ 4  ];
   GLfloat especular[ 4 ];
   GLfloat especularidade[ 4 ];
};

typedef struct tipo_transformacao_{
    GLfloat dx, dy, dz;
    GLfloat sx, sy, sz;
    GLfloat angx, angy, angz, ang;
} tipo_transformacao;

tipo_transformacao transf[ NUM_OBJETOS ];

// camera vai conter as definições da camera sintética
struct tipo_camera camera;
// luz vai conter as informações da iluminação
struct tipo_luz luz;

GLfloat aspecto;
GLfloat rotacao = 0;
GLuint  texture_id[ NUM_TEX ];

enum boolean {
    true = 1, false = 0
};
typedef enum boolean bool;

bool visual_eixo, visual_espada, visual_picareta;
char transformacao, eixo;
GLint  objeto;
GLfloat cab;

int  LoadBMP(char *filename){
    #define SAIR        {fclose(fp_arquivo); return -1;}
    #define CTOI(C)     (*(int*)&C)

    GLubyte     *image;
    GLubyte     Header[0x54];
    GLuint      DataPos, imageSize;
    GLsizei     Width,Height;

    int nb = 0;

    // Abre o arquivo e efetua a leitura do Header do arquivo BMP
    FILE * fp_arquivo = fopen(filename,"rb");
    if (!fp_arquivo)
        return -1;
    if (fread(Header,1,0x36,fp_arquivo)!=0x36)
        SAIR;
    if (Header[0]!='B' || Header[1]!='M')
        SAIR;
    if (CTOI(Header[0x1E])!=0)
        SAIR;
    if (CTOI(Header[0x1C])!=24)
        SAIR;

    // Recupera a informação dos atributos de
    // altura e largura da imagem

    Width   = CTOI(Header[0x12]);
    Height  = CTOI(Header[0x16]);
    (CTOI(Header[0x0A]) == 0 ) ? ( DataPos=0x36 ) : ( DataPos = CTOI(Header[0x0A]));

    imageSize=Width*Height*3;

    // Efetura a Carga da Imagem
    image = (GLubyte *) malloc ( imageSize );
    int retorno;
    retorno = fread(image,1,imageSize,fp_arquivo);

    if (retorno !=imageSize){
        free (image);
        SAIR;
    }

    // Inverte os valores de R e B
    int t, i;

    for ( i = 0; i < imageSize; i += 3 ){
        t = image[i];
        image[i] = image[i+2];
        image[i+2] = t;
    }

    // Tratamento da textura para o OpenGL
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S    ,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T    ,GL_REPEAT);

    glTexEnvf ( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

    // Faz a geraçao da textura na memória
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, Width, Height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);

    fclose (fp_arquivo);
    free (image);
    return 1;
}

void Texturizacao(){
   glEnable(GL_TEXTURE_2D);
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );//Como armazena o pixel
   glGenTextures ( NUM_TEX , texture_id );//armazena q qtidade de textura

   texture_id[ 0 ] = TEXTURA1; // define um numero (identificacao) para a textura
   glBindTexture ( GL_TEXTURE_2D, texture_id[0] );//armazena na posição 0 do vetor
   LoadBMP ( "textures/cabecafront.bmp" ); // lê a textura

   texture_id[ 1 ] = TEXTURA2;
   glBindTexture ( GL_TEXTURE_2D, texture_id[1] );
   LoadBMP ( "textures/tronco.bmp" );

   texture_id[ 2 ] = TEXTURA3;
   glBindTexture ( GL_TEXTURE_2D, texture_id[2] );
   LoadBMP ( "textures/perna.bmp" );

   texture_id[ 3 ] = TEXTURA4;
   glBindTexture ( GL_TEXTURE_2D, texture_id[3] );
   LoadBMP ( "textures/braco.bmp" );

   texture_id[ 4 ] = TEXTURA5;
   glBindTexture ( GL_TEXTURE_2D, texture_id[4] );
   LoadBMP ( "textures/pe.bmp" );

   texture_id[ 5 ] = TEXTURA6;
   glBindTexture ( GL_TEXTURE_2D, texture_id[5] );
   LoadBMP ( "textures/quadril.bmp" );

   texture_id[ 6 ] = TEXTURA7;
   glBindTexture ( GL_TEXTURE_2D, texture_id[6] );
   LoadBMP ( "textures/cabecatras.bmp" );

   texture_id[ 7 ] = TEXTURA8;
   glBindTexture ( GL_TEXTURE_2D, texture_id[7] );
   LoadBMP ( "textures/cabecaesq.bmp" );

   texture_id[ 8 ] = TEXTURA9;
   glBindTexture ( GL_TEXTURE_2D, texture_id[8] );
   LoadBMP ( "textures/cabecadir.bmp" );

   texture_id[ 9 ] = TEXTURA10;
   glBindTexture ( GL_TEXTURE_2D, texture_id[9] );
   LoadBMP ( "textures/cabecasup.bmp" );

   texture_id[ 10 ] = TEXTURA11;
   glBindTexture ( GL_TEXTURE_2D, texture_id[10] );
   LoadBMP ( "textures/cabecainf.bmp" );

   texture_id[ 11 ] = TEXTURA12;
   glBindTexture ( GL_TEXTURE_2D, texture_id[11] );
   LoadBMP ( "textures/troncocostas.bmp" );

   texture_id[ 12 ] = TEXTURA13;
   glBindTexture ( GL_TEXTURE_2D, texture_id[12] );
   LoadBMP ( "textures/bracosup.bmp" );

   texture_id[ 13 ] = TEXTURA14;
   glBindTexture ( GL_TEXTURE_2D, texture_id[13] );
   LoadBMP ( "textures/bracoinf.bmp" );

   glTexGeni( GL_S , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
   glTexGeni( GL_T , GL_TEXTURE_GEN_MODE , GL_SPHERE_MAP );
}

// Funcão que define a iluminação da cena
void Define_Iluminacao( void ){
    // modelo de preenchimento dos objetos
    glShadeModel( GL_SMOOTH );
    //glShadeModel( GL_FLAT );

    // habilita iluminação
    glEnable( GL_LIGHTING );

    // Ativa o uso da luz ambiente
    glLightModelfv( GL_LIGHT_MODEL_AMBIENT , luz.ambiente );

    // poisção da luz no universo
    glLightfv( GL_LIGHT0 , GL_POSITION , luz.posicao );

    // configura a luz ambiente
    glLightfv( GL_LIGHT0 , GL_AMBIENT  , luz.ambiente  );
    // configura a luz difusa
    glLightfv( GL_LIGHT0 , GL_DIFFUSE  , luz.difusa );
    // configura a luz especular
    glLightfv( GL_LIGHT0 , GL_SPECULAR , luz.especular );

    //habilita a luz 0
    glEnable ( GL_LIGHT0 );

    // Define a refletância do material
    glMaterialfv( GL_FRONT , GL_SPECULAR  , luz.especularidade );

    // define o brilho do material
    glMateriali ( GL_FRONT , GL_SHININESS , 10 );

    glEnable(GL_COLOR_MATERIAL);
}

// Inicializa parâmetros de rendering
void Inicializa (void){
    // cor de fundo da janela (RGBA)
    glClearColor( 0.0 , 0.0 , 0.0 , 1.0 );
    int i;
    for( i = 0 ; i < NUM_OBJETOS ; i++ ){
        transf[ i ].dx  = 0.0;
        transf[ i ].dy  = 0.0;
        transf[ i ].dz  = 0.0;
        transf[i].angx=0.0;
        transf[i].angy=0.0;
        transf[i].angz=0.0;
        transf[i].ang=0.0;
    }

    //inicialização
    objeto = ROBO;
    transformacao = 'R';
    eixo = 'Y';
    visual_eixo = true;

    // posição x da câmera no universo
    camera.posx   = 0;
    // posição y da câmera no universo
    camera.posy   = 0;
    // posição z da câmera no universo
    camera.posz   = 400;
    // posição x do alvo da câmera no universo - para onde a cÂmera 'olha'
    camera.alvox  = 0;
    // posição y do alvo da câmera no universo - para onde a cÂmera 'olha'
    camera.alvoy  = 0;
    // posição z do alvo da câmera no universo - para onde a cÂmera 'olha'
    camera.alvoz  = 0;
    // a partir de que distância da câmera ela começa a 'enxergar' os objetos
    camera.inicio = 0.1;
    // até que distância da câmera ela consegue 'enxergar' os objetos
    camera.fim    = 5000.0;
    // ângulo da câmera - define o zoom
    camera.ang    = 45;

    // posição da fonte de luz
    luz.posicao[ 0 ] = 100.0;
    luz.posicao[ 1 ] = 100.0;
    luz.posicao[ 2 ] = 100.0;
    luz.posicao[ 3 ] = 1.0;

    // cor e intensidade da luz ambiente
    luz.ambiente[ 0 ] = 0.2;
    luz.ambiente[ 1 ] = 0.2;
    luz.ambiente[ 2 ] = 0.2;
    luz.ambiente[ 3 ] = 1.0;

    // cor e intensidade da luz difusa
    luz.difusa[ 0 ] = 0.5;
    luz.difusa[ 1 ] = 0.5;
    luz.difusa[ 2 ] = 0.5;
    luz.difusa[ 3 ] = 1.0;

    // cor e intensidade da luz especular
    luz.especular[ 0 ] = 0.8;
    luz.especular[ 1 ] = 0.8;
    luz.especular[ 2 ] = 0.8;
    luz.especular[ 3 ] = 1.0;

    // cor e intensidade da especularidade
    luz.especularidade[ 0 ] = 0.8;
    luz.especularidade[ 1 ] = 0.8;
    luz.especularidade[ 2 ] = 0.8;
    luz.especularidade[ 3 ] = 1.0;

    // ativa a possibilidade de transparência dos objetos - canal alfa
    glEnable( GL_BLEND );

    // define a forma de cálculo da transparência
    glBlendFunc( GL_SRC_ALPHA , GL_ONE_MINUS_SRC_ALPHA );

    // ativa a remoçào das faces ocultas
    glEnable( GL_CULL_FACE );

    // ativa o cálculo de profundidade z-buffer
    glEnable( GL_DEPTH_TEST );

    Define_Iluminacao();

    Texturizacao();
}

void mensagem( void ){
    printf("\n");
    printf("\nObjeto: %d     Transformação: %c      Eixo: %c", objeto, transformacao, eixo);

}

void cuboFront(void){
    glBegin( GL_QUADS );
        glNormal3f(   0.0 ,   0.0 ,  1.0 );	// normal da face
        glTexCoord2f( 1.0 , 1.0 ); glVertex3f(  120.0 ,  120.0 , 120.0 );
        glTexCoord2f( 0.0 , 1.0 ); glVertex3f( -120.0 ,  120.0 , 120.0 );
        glTexCoord2f( 0.0 , 0.0 ); glVertex3f( -120.0 , -120.0 , 120.0 );
        glTexCoord2f( 1.0 , 0.0 ); glVertex3f(  120.0 , -120.0 , 120.0 );
    glEnd();
}

void cuboTras(void){
    glBegin( GL_QUADS );
        glNormal3f(   0.0 ,   0.0 ,  -1.0 );
        glTexCoord2f(  0.0 , 0.0 ); glVertex3f(  120.0 , -120.0 , -120.0 );
        glTexCoord2f(  1.0 , 0.0 ); glVertex3f( -120.0 , -120.0 , -120.0 );
        glTexCoord2f(  1.0 , 1.0 ); glVertex3f( -120.0 ,  120.0 , -120.0 );
        glTexCoord2f(  0.0 , 1.0 ); glVertex3f(  120.0 ,  120.0 , -120.0 );
    glEnd();
}

void cuboEsq(void){
    glBegin( GL_QUADS );
        glNormal3f(   -1.0 ,   0.0 ,  0.0 );
        glTexCoord2f( 0.0 , 1.0 ); glVertex3f( -120.0 ,   120.0 , -120.0 );
        glTexCoord2f( 0.0 , 0.0 ); glVertex3f( -120.0 ,  -120.0 , -120.0 );
        glTexCoord2f( 1.0 , 0.0 ); glVertex3f( -120.0 ,  -120.0 ,  120.0 );
        glTexCoord2f( 1.0 , 1.0 ); glVertex3f( -120.0 ,   120.0 ,  120.0 );
    glEnd();
}

void cuboDir(void){
    glBegin( GL_QUADS );
        glNormal3f(   1.0 ,   0.0 ,  0.0 );
        glTexCoord2f( 1.0 , 1.0 ); glVertex3f( 120.0 ,   120.0 , -120.0 );
        glTexCoord2f( 0.0 , 1.0 ); glVertex3f( 120.0 ,   120.0 ,  120.0 );
        glTexCoord2f( 0.0 , 0.0 ); glVertex3f( 120.0 ,  -120.0 ,  120.0 );
        glTexCoord2f( 1.0 , 0.0 ); glVertex3f( 120.0 ,  -120.0 , -120.0 );
    glEnd();
}

void cuboSup(void){
    glBegin( GL_QUADS );
        glNormal3f(   0.0 ,   1.0 ,  0.0 );
        glTexCoord2f( 0.0 , 1.0 ); glVertex3f(  120.0 ,  120.0 , -120.0 );
        glTexCoord2f( 1.0 , 1.0 ); glVertex3f( -120.0 ,  120.0 , -120.0 );
        glTexCoord2f( 1.0 , 0.0 ); glVertex3f( -120.0 ,  120.0 ,  120.0 );
        glTexCoord2f( 0.0 , 0.0 ); glVertex3f(  120.0 ,  120.0 ,  120.0 );
    glEnd();
}

void cuboInf(void){
    glBegin( GL_QUADS );
        glNormal3f(   0.0 ,   -1.0 ,  0.0 );
        glTexCoord2f( 1.0 , 1.0 ); glVertex3f(  120.0 ,  -120.0 , -120.0 );
        glTexCoord2f( 0.0 , 1.0 ); glVertex3f(  120.0 ,  -120.0 ,  120.0 );
        glTexCoord2f( 0.0 , 0.0 ); glVertex3f( -120.0 ,  -120.0 ,  120.0 );
        glTexCoord2f( 1.0 , 0.0 ); glVertex3f( -120.0 ,  -120.0 , -120.0 );
    glEnd();
}


void desenha_eixos(){
    glLineWidth(10);

    glBegin( GL_LINES );

    //eixo x
    glColor3f( 1.0 , 0.0 , 0.0 );
    glVertex3i( -150 , 0 , 0 );
    glVertex3i(  150 , 0 , 0 );

    //eixo y
    glColor3f( 0.0 , 1.0 , 0.0 );
    glVertex3i( 0 , -150 , 0 );
    glVertex3i( 0 ,  150 , 0 );

    //eixo z
    glColor3f( 0.0 , 0.0 , 1.0 );
    glVertex3i( 0 , 0 , -150 );
    glVertex3i( 0 , 0 ,  150 );

    glEnd();
}

// Função callback chamada para fazer o desenho
void Desenha(void){

    GLUquadricObj *quadObj; // um objeto é criado
    quadObj = gluNewQuadric();
    gluQuadricTexture(quadObj, GL_TRUE);
    gluQuadricDrawStyle(quadObj, GLU_FILL);

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glLineWidth( 2 );

    GLint raio, segmentos;
    GLfloat ang;

    if(visual_eixo){
        glDisable(GL_TEXTURE_2D);
        desenha_eixos();
    }

    //mensagem();

    glEnable(GL_TEXTURE_2D);
    glPushMatrix(); // ROBO
    glTranslatef( transf[ ROBO ].dx , transf[ ROBO ].dy , transf[ ROBO ].dz );
    glRotatef( transf[ROBO].angx, 1,0,0);
    glRotatef( transf[ROBO].angy, 0,1,0);
    glRotatef( transf[ROBO].angz, 0,0,1);

        glPushMatrix(); // CABEÇA
        glTranslatef( transf[ CABECA ].dx , 39+transf[ CABECA ].dy , transf[ CABECA ].dz );
        glRotatef( transf[CABECA].angx, 1,0,0);
        glRotatef( transf[CABECA].angy, 0,1,0);
        glRotatef( transf[CABECA].angz, 0,0,1);
        glScalef( 0.2 , 0.2 , 0.2 );

            glBindTexture ( GL_TEXTURE_2D, TEXTURA1 );
            glPushMatrix(); //frontal
                cuboFront();
            glPopMatrix();

            glBindTexture ( GL_TEXTURE_2D, TEXTURA7 );
            glPushMatrix(); //traseira
                cuboTras();
            glPopMatrix();

            glBindTexture ( GL_TEXTURE_2D, TEXTURA10 );
            glPushMatrix(); //superior
                cuboSup();
            glPopMatrix();

            glBindTexture ( GL_TEXTURE_2D, TEXTURA11 );
            glPushMatrix(); //inferior
                cuboInf();
            glPopMatrix();

            glBindTexture ( GL_TEXTURE_2D, TEXTURA9 );
            glPushMatrix(); //esquerda
                cuboEsq();
            glPopMatrix();

            glBindTexture ( GL_TEXTURE_2D, TEXTURA8 );
            glPushMatrix(); //direita
                cuboDir();
            glPopMatrix();
        glPopMatrix();


        glPushMatrix(); // TRONCO
            glPushMatrix(); // BARRIGA
                glTranslatef( transf[ TRONCO ].dx , -15+transf[ TRONCO ].dy , transf[ TRONCO ].dz );
                glRotatef( transf[TRONCO].angx, 1,0,0);
                glRotatef( transf[TRONCO].angy, 0,1,0);
                glRotatef( transf[TRONCO].angz, 0,0,1);
                glScalef( 0.2 , 0.25 , 0.1 );

                glBindTexture ( GL_TEXTURE_2D, TEXTURA2 );
                glPushMatrix(); //frontal
                    cuboFront();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA12 );
                glPushMatrix(); //traseira
                    cuboTras();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA12 );
                glPushMatrix(); //superior
                    cuboSup();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA12 );
                glPushMatrix(); //inferior
                    cuboInf();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA12 );
                glPushMatrix(); //esquerda
                    cuboEsq();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA12 );
                glPushMatrix(); //direita
                    cuboDir();
                glPopMatrix();
            glPopMatrix();

            glPushMatrix(); // BRACO
            glTranslatef( 33+transf[ BRACO ].dx , -19+transf[ BRACO ].dy , transf[ BRACO ].dz );
            glTranslatef(.0, 25, .0);
            glRotatef( transf[BRACO].angx, 1,0,0);
            glRotatef( transf[BRACO].angy, 0,1,0);
            glRotatef( transf[BRACO].angz, 0,0,1);
            glTranslatef(.0, -25, .0);

            glScalef( 0.08 , 0.28 , 0.1 );

            glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); // frontal
                    cuboFront();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); // traseira
                    cuboTras();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA13 );
                glPushMatrix(); //superior
                    cuboSup();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA11 );
                glPushMatrix(); // inferior
                    cuboInf();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); //esquerda
                    cuboEsq();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); //direita
                    cuboDir();
                glPopMatrix();
            glPopMatrix(); // fecha braço

            glPushMatrix(); // BRAÇO 2
            glTranslatef( -33+transf[ BRACO2 ].dx , -19+transf[ BRACO2 ].dy , transf[ BRACO2 ].dz );
            glTranslatef(.0, 25, .0);
            glRotatef( transf[BRACO2].angx, 1,0,0);
            glRotatef( transf[BRACO2].angy, 0,1,0);
            glRotatef( transf[BRACO2].angz, 0,0,1);
            glTranslatef(.0, -25, .0);
            glScalef( 0.08 , 0.28 , 0.1 );

            glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); // frontal
                    cuboFront();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); // traseira
                    cuboTras();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA13 );
                glPushMatrix(); //superior
                    cuboSup();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA11 );
                glPushMatrix(); // inferior
                    cuboInf();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); // esquerda
                    cuboEsq();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA4 );
                glPushMatrix(); // direita
                    cuboDir();
                glPopMatrix();

            glPopMatrix(); // fecha braço 2
        glPopMatrix(); // fecha tronco


        glPushMatrix(); // QUADRIL
            glTranslatef( transf[ QUADRIL ].dx , -50+transf[ QUADRIL ].dy , transf[ QUADRIL ].dz );
            glRotatef( transf[QUADRIL].angx, 1,0,0);
            glRotatef( transf[QUADRIL].angy, 0,1,0);
            glRotatef( transf[QUADRIL].angz, 0,0,1);
            glScalef( 0.2 , 0.05 , 0.1 );

            glBindTexture ( GL_TEXTURE_2D, TEXTURA6 );
                glPushMatrix();
                    cuboFront();
                    cuboTras();
                    cuboSup();
                    cuboInf();
                    cuboDir();
                    cuboEsq();
                glPopMatrix();

                glBindTexture ( GL_TEXTURE_2D, TEXTURA3 );
                glPushMatrix(); // PERNA
                    glTranslatef( -60+transf[ PERNA ].dx , -600+transf[ PERNA ].dy , transf[ PERNA ].dz );
                    glTranslatef(.0, 600, .0);
                    glRotatef( transf[PERNA].angx, 1,0,0);
                    glRotatef( transf[PERNA].angy, 0,1,0);
                    glRotatef( transf[PERNA].angz, 0,0,1);
                    glTranslatef(.0, -600, .0);
                    glScalef( 0.5 , 4 , 1 );
                        cuboFront();
                        cuboTras();
                        cuboSup();
                        cuboInf();
                        cuboDir();
                        cuboEsq();

                        glBindTexture ( GL_TEXTURE_2D, TEXTURA5 );
                        glPushMatrix(); // PÉ
                            glTranslatef( 0+transf[ PE ].dx , -145+transf[ PE ].dy , transf[ PE ].dz );
                            glRotatef( transf[PE].angx, 1,0,0);
                            glRotatef( transf[PE].angy, 0,1,0);
                            glRotatef( transf[PE].angz, 0,0,1);
                            glScalef( 1 , 0.2 , 1 );
                                cuboFront();
                                cuboTras();
                                cuboSup();
                                cuboInf();
                                cuboDir();
                                cuboEsq();
                        glPopMatrix(); // fecha pé
                glPopMatrix(); // fecha perna

                glBindTexture ( GL_TEXTURE_2D, TEXTURA3 );
                glPushMatrix(); // PERNA 2
                    glTranslatef( 60+transf[ PERNA2 ].dx , -600+transf[ PERNA2 ].dy , transf[ PERNA2 ].dz );
                    glTranslatef(.0, 600, .0);
                    glRotatef( transf[PERNA2].angx, 1,0,0);
                    glRotatef( transf[PERNA2].angy, 0,1,0);
                    glRotatef( transf[PERNA2].angz, 0,0,1);
                    glTranslatef(.0, -600, .0);
                    glScalef( 0.5 , 4 , 1 );
                        cuboFront();
                        cuboTras();
                        cuboSup();
                        cuboInf();
                        cuboDir();
                        cuboEsq();

                        glBindTexture ( GL_TEXTURE_2D, TEXTURA5 );
                        glPushMatrix(); // PE 2
                                glTranslatef( 0+transf[ PE2 ].dx , -145+transf[ PE2 ].dy , transf[ PE2 ].dz );
                                glRotatef( transf[PE2].angx, 1,0,0);
                                glRotatef( transf[PE2].angy, 0,1,0);
                                glRotatef( transf[PE2].angz, 0,0,1);
                                glScalef( 1 , 0.2 , 1 );
                                    cuboFront();
                                    cuboTras();
                                    cuboSup();
                                    cuboInf();
                                    cuboDir();
                                    cuboEsq();
                        glPopMatrix(); // fecha pé 2
                glPopMatrix(); // fecha perna
       glPopMatrix(); // fecha quadril
    glPopMatrix(); // fecha ROBO

    glDisable( GL_TEXTURE_GEN_S );
    glDisable( GL_TEXTURE_GEN_T );

    glutSwapBuffers();

    gluDeleteQuadric( quadObj );
 }

// Função usada para especificar o volume de visualização
void EspecificaParametrosVisualizacao( void ){
    // seleciona o tipo de matriz para a projeção
    glMatrixMode( GL_PROJECTION );

    // limpa (zera) as matrizes
    glLoadIdentity();

    // Especifica e configura a projeção perspectiva
    gluPerspective( camera.ang , aspecto , camera.inicio , camera.fim );

    // Especifica sistema de coordenadas do modelo
    glMatrixMode( GL_MODELVIEW );

    // Inicializa sistema de coordenadas do modelo
    glLoadIdentity();

    // rotaciona a camera
    glRotatef( rotacao , 0 , 1 , 0 );

    // Especifica posição da camera (o observador) e do alvo
    gluLookAt( camera.posx , camera.posy , camera.posz , camera.alvox , camera.alvoy , camera.alvoz , 0 , 1 , 0 );
}

// Função callback chamada quando o tamanho da janela é alterado
void AlteraTamanhoJanela( GLsizei largura , GLsizei altura ){
    // Para previnir uma divisão por zero
    if ( altura == 0 ) altura = 1;

    // Especifica as dimensões da viewport
    glViewport( 0 , 0 , largura , altura );

    // Calcula a correção de aspecto
    aspecto = ( GLfloat )largura / ( GLfloat )altura;

    printf("\nAspecto (relação entre a altura e a largura) atual : %f", aspecto );

    EspecificaParametrosVisualizacao();
}

// Função callback chamada para gerenciar eventos do mouse
void GerenciaMouse( GLint button , GLint state , GLint x , GLint y ){
   // botão esquerdo do mouse
    if ( button == GLUT_LEFT_BUTTON )
        if ( state == GLUT_DOWN ) // Zoom-in
            if ( camera.ang >= 6 ) camera.ang -= 5;

    // botão direito do mouse
    if ( button == GLUT_RIGHT_BUTTON )
        if ( state == GLUT_DOWN ) // Zoom-out
            if ( camera.ang <= 174 ) camera.ang += 5;

    EspecificaParametrosVisualizacao();

    // obriga redesenhar
    glutPostRedisplay();
}
//ROTAÇÃO + X
void transfRMaisX(){
    if (transf[CABECA].angx > 20) transf[CABECA].angx = 0;
    if (transf[TRONCO].angx > 0) transf[TRONCO].angx = 0;
    if (transf[BRACO].angx > 80) transf[BRACO].angx = 0;
    if (transf[BRACO2].angx > 80) transf[BRACO2].angx = 0;
    if (transf[QUADRIL].angx > 0) transf[QUADRIL].angx = 0;
    if (transf[PERNA].angx > 20) transf[PERNA].angx = 0;
    if (transf[PERNA2].angx > 20) transf[PERNA2].angx = 0;
    if (transf[PE].angx > 0) transf[PE].angx = 0;
    if (transf[PE2].angx > 0) transf[PE2].angx = 0;
}
//ROTAÇÃO - X
void transfRMenosX(){
    if (transf[CABECA].angx < -25) transf[CABECA].angx = 0;
    if (transf[TRONCO].angx < 0) transf[TRONCO].angx = 0;
    if (transf[BRACO].angx < -200) transf[BRACO].angx = 0;
    if (transf[BRACO2].angx < -200) transf[BRACO2].angx = 0;
    if (transf[QUADRIL].angx < -30) transf[QUADRIL].angx = 0;
    if (transf[PERNA].angx < -50) transf[PERNA].angx = 0;
    if (transf[PERNA2].angx < -50) transf[PERNA2].angx = 0;
    if (transf[PE].angx < 0) transf[PE].angx = 0;
    if (transf[PE2].angx < 0) transf[PE2].angx = 0;
}
//ROTACAÇÃO + Y
void transfRMaisY(){
    if (transf[CABECA].angy > 30) transf[CABECA].angy = 0;
    if (transf[TRONCO].angy > 10) transf[TRONCO].angy = 0;
    if (transf[BRACO].angy > 0) transf[BRACO].angy = 0;
    if (transf[BRACO2].angy > 0) transf[BRACO2].angy = 0;
    if (transf[QUADRIL].angy > 30) transf[QUADRIL].angy = 0;
    if (transf[PERNA].angy > 0) transf[PERNA].angy = 0;
    if (transf[PERNA2].angy > 0) transf[PERNA2].angy = 0;
    if (transf[PE].angy > 0) transf[PE].angy = 0;
    if (transf[PE2].angy > 0) transf[PE2].angy = 0;
}
//ROTAÇÃO - Y
void transfRMenosY(){
    if (transf[CABECA].angy < -30) transf[CABECA].angy = 0;
    if (transf[TRONCO].angy < -10) transf[TRONCO].angy = 0;
    if (transf[BRACO].angy < -0) transf[BRACO].angy = 0;
    if (transf[BRACO2].angy < -0) transf[BRACO2].angy = 0;
    if (transf[QUADRIL].angy < -30) transf[QUADRIL].angy = 0;
    if (transf[PERNA].angy < -0) transf[PERNA].angy = 0;
    if (transf[PERNA2].angy < -0) transf[PERNA2].angy = 0;
    if (transf[PE].angy < 0) transf[PE].angy = 0;
    if (transf[PE2].angy < 0) transf[PE2].angy = 0;
}
//ROTAÇÃO + Z
void transfRMaisZ(){
    if (transf[CABECA].angz > 30) transf[CABECA].angz = 0;
    if (transf[TRONCO].angz > 10) transf[TRONCO].angz = 0;
    if (transf[BRACO].angz > 180) transf[BRACO].angz = 0;
    if (transf[BRACO2].angz > 20) transf[BRACO2].angz = 0;
    if (transf[QUADRIL].angz > 30) transf[QUADRIL].angz = 0;
    if (transf[PERNA].angz > 10) transf[PERNA].angz = 0;
    if (transf[PERNA2].angz > 20) transf[PERNA2].angz = 0;
    if (transf[PE].angz > 0) transf[PE].angz = 0;
    if (transf[PE2].angz > 0) transf[PE2].angz = 0;
}
//ROTAÇÃO - Z
void transfRMenosZ(){
    if (transf[CABECA].angz < -30) transf[CABECA].angz = 0;
    if (transf[TRONCO].angz < -10) transf[TRONCO].angz = 0;
    if (transf[BRACO].angz < -20) transf[BRACO].angz = 0;
    if (transf[BRACO2].angz < -180) transf[BRACO2].angz = 0;
    if (transf[QUADRIL].angz < -30) transf[QUADRIL].angz = 0;
    if (transf[PERNA].angz < -20) transf[PERNA].angz = 0;
    if (transf[PERNA2].angz < -10) transf[PERNA2].angz = 0;
    if (transf[PE].angz < 0) transf[PE].angz = 0;
    if (transf[PE2].angz < 0) transf[PE2].angz = 0;
}

//TRANSLAÇÕES
void transfTMaisX(){
    if (transf[CABECA].dx > 10) transf[CABECA].dx = 0;
    if (transf[TRONCO].dx > 0) transf[TRONCO].dx = 0;
    if (transf[BRACO].dx > 0) transf[BRACO].dx = 0;
    if (transf[BRACO2].dx > 0) transf[BRACO2].dx = 0;
    if (transf[QUADRIL].dx > 5) transf[QUADRIL].dx = 0;
    if (transf[PERNA].dx > 5) transf[PERNA].dx = 0;
    if (transf[PERNA2].dx > 5) transf[PERNA2].dx = 0;
    if (transf[PE].dx > 0) transf[PE].dx = 0;
    if (transf[PE2].dx > 0) transf[PE2].dx = 0;
}

void transfTMenosX(){
    if (transf[CABECA].dx < -10) transf[CABECA].dx = 0;
    if (transf[TRONCO].dx < -0) transf[TRONCO].dx = 0;
    if (transf[BRACO].dx < -0) transf[BRACO].dx = 0;
    if (transf[BRACO2].dx < -0) transf[BRACO2].dx = 0;
    if (transf[QUADRIL].dx < -5) transf[QUADRIL].dx = 0;
    if (transf[PERNA].dx < -5) transf[PERNA].dx = 0;
    if (transf[PERNA2].dx < -5) transf[PERNA2].dx = 0;
    if (transf[PE].dx < 0) transf[PE].dx = 0;
    if (transf[PE2].dx < 0) transf[PE2].dx = 0;
}
void transfTMaisY(){
    if (transf[CABECA].dy > 0) transf[CABECA].dy = 0;
    if (transf[TRONCO].dy > 0) transf[TRONCO].dy = 0;
    if (transf[BRACO].dy > 0) transf[BRACO].dy = 0;
    if (transf[BRACO2].dy > 0) transf[BRACO2].dy = 0;
    if (transf[QUADRIL].dy > 0) transf[QUADRIL].dy = 0;
    if (transf[PERNA].dy > 0) transf[PERNA].dy = 0;
    if (transf[PERNA2].dy > 0) transf[PERNA2].dy = 0;
    if (transf[PE].dy > 0) transf[PE].dy = 0;
    if (transf[PE2].dy > 0) transf[PE2].dy = 0;
}
void transfTMenosY(){
    if (transf[CABECA].dy < -0) transf[CABECA].dy = 0;
    if (transf[CABECA].dy < -0) transf[CABECA].dy = 0;
    if (transf[TRONCO].dy < -0) transf[TRONCO].dy = 0;
    if (transf[BRACO].dy < -0) transf[BRACO].dy = 0;
    if (transf[BRACO2].dy < -0) transf[BRACO2].dy = 0;
    if (transf[QUADRIL].dy < -0) transf[QUADRIL].dy = 0;
    if (transf[PERNA].dy < -0) transf[PERNA].dy = 0;
    if (transf[PERNA2].dy < -0) transf[PERNA2].dy = 0;
    if (transf[PE].dy < 0) transf[PE].dy = 0;
    if (transf[PE2].dy < 0) transf[PE2].dy = 0;
}
void transfTMaisZ(){
    if (transf[CABECA].dz > 0) transf[CABECA].dz = 0;
    if (transf[TRONCO].dz > 0) transf[TRONCO].dz = 0;
    if (transf[BRACO].dz > 0) transf[BRACO].dz = 0;
    if (transf[BRACO2].dz > 0) transf[BRACO2].dz = 0;
    if (transf[QUADRIL].dz > 0) transf[QUADRIL].dz = 0;
    if (transf[PERNA].dz > 0) transf[PERNA].dz = 0;
    if (transf[PERNA2].dz > 0) transf[PERNA2].dz = 0;
    if (transf[PE].dz > 0) transf[PE].dz = 0;
    if (transf[PE2].dz > 0) transf[PE2].dz = 0;
}
void transfTMenosZ(){
    if (transf[CABECA].dz < -0) transf[CABECA].dz = 0;
    if (transf[TRONCO].dz < -0) transf[TRONCO].dz = 0;
    if (transf[BRACO].dz < 0) transf[BRACO].dz = 0;
    if (transf[BRACO2].dz < -0) transf[BRACO2].dz = 0;
    if (transf[QUADRIL].dz < -0) transf[QUADRIL].dz = 0;
    if (transf[PERNA].dz < -0) transf[PERNA].dz = 0;
    if (transf[PERNA2].dz < -0) transf[PERNA2].dz = 0;
    if (transf[PE].dz < 0) transf[PE].dz = 0;
    if (transf[PE2].dz < 0) transf[PE2].dz = 0;
}

// tratamento do teclado
void GerenciaTeclado( GLubyte key , GLint x , GLint y ){
    if ( key-48 >= 0 && key-48 <= NUM_OBJETOS-1 )
        objeto = key - 48;

    if ( key == 27 )
        exit( 0 );

    if ( key == 'w' || key == 'W' ){
        transf[PERNA].angx = 20;
        transf[PERNA2].angx = -20;
        transf[BRACO].angx = -20;
        transf[BRACO2].angx = 20;
        transf[CABECA].angz = 2;
        transf[TRONCO].angz = 2;

    }
    if ( key == 'q' || key == 'Q' ){
        transf[PERNA].angx = -20;
        transf[PERNA2].angx = 20;
        transf[BRACO].angx = 20;
        transf[BRACO2].angx = -20;
        transf[CABECA].angz = -2;
        transf[TRONCO].angz = -2;
    }

    if ( key == 'v' || key == 'V' )
        visual_eixo =! visual_eixo;

    if ( toupper( key ) == 'R' || toupper( key ) == 'T' )
        transformacao = toupper( key );

    if ( toupper( key ) == 'X' || toupper( key ) == 'Y' || toupper(key)=='Z')
        eixo = toupper( key );

    if ( key == '+' || key == '=' ){
        switch( transformacao ){
            case 'R':
                if ( eixo == 'X'){
                    transf[ objeto ].angx += PASSO;
                    transfRMaisX();
                }
                else if ( eixo == 'Y'){
                    transf[ objeto ].angy += PASSO;
                    transfRMaisY();
                }
                else{
                    transf[ objeto ].angz += PASSO;
                    transfRMaisZ();
                }
                break;

            case 'T':
                if ( eixo == 'X'){
                    transf[ objeto ].dx += PASSO;
                    transfTMaisX();
                }
                else if ( eixo == 'Y'){
                    transf[ objeto ].dy += PASSO;
                    transfTMaisY();
                }
                else{
                    transf[ objeto ].dz += PASSO;
                    transfTMaisZ();
                }
                break;
        }
    }
    if ( key == '-' || key == '_' ){
        switch( transformacao ){
            case 'R':
                if ( eixo == 'X'){
                    transf[ objeto ].angx -= PASSO;
                    transfRMenosX();
                }

                else if ( eixo == 'Y'){
                    transf[ objeto ].angy -= PASSO;
                    transfRMenosY();
                }
                else{
                    transf[ objeto ].angz -= PASSO;
                    transfRMenosZ();
                }

                break;

            case 'T':
                if ( eixo == 'X'){
                    transf[ objeto ].dx -= PASSO;
                    transfTMenosX();
                }
                else if(eixo=='Y'){
                    transf[ objeto ].dy -= PASSO;
                    transfTMenosY();
                }
                else{
                    transf[ objeto ].dz -= PASSO;
                    transfTMenosZ();
                }

                break;
        }
    }

    // rotaciona a camera
   if ( key == 'G' )
       rotacao += PASSO;

   if ( key == 'g' )
       rotacao -= PASSO;

   // leva a câmera para a direita
   if ( key == 'D' ){
       camera.posx += PASSO;
       camera.alvox += PASSO; //leva o alvo junto
   }

   // leva a câmera para a esquerda
   if ( key == 'd' ){
       camera.posx -= PASSO;
       camera.alvox -= PASSO;//leva o alvo junto
   }

   // leva a câmera para cima
   if ( key == 'C' ){
       camera.posy += PASSO;
       camera.alvoy += PASSO;//leva o alvo junto
   }

   // leva a câmera para baixo
   if ( key == 'c' ){
       camera.posy -= PASSO;
       camera.alvoy -= PASSO;//leva o alvo junto
   }

    //leva a câmera para trás
   if ( key == 'P' ){
       camera.posz += PASSO;
       camera.alvoz += PASSO;//leva o alvo junto
   }

   // leva a câmera para frente
   if ( key == 'p' ){
       camera.posz -= PASSO;
       camera.alvoz -= PASSO;//leva o alvo junto
   }

   // leva o ponto para onde a câmera 'olha' para a direita
   if ( key == 'E' )
       camera.alvox += PASSO;

   // leva o ponto para onde a câmera 'olha' para a esquerda
   if ( key == 'e' )
       camera.alvox -= PASSO;

   // leva o ponto para onde a câmera 'olha' para cima
   if ( key == 'B' )
       camera.alvoy += PASSO;

   // leva o ponto para onde a câmera 'olha' para baixo
   if ( key == 'b' )
       camera.alvoy -= PASSO;

   // leva o ponto para onde a câmera 'olha' para a frente
   if ( key == 'f' )
       camera.alvoz += PASSO;

   // leva o ponto para onde a câmera 'olha' para trás
   if ( key == 'F' )
       camera.alvoz -= PASSO;

   // aumenta o ponto inicial de visualização do objetos pela câmetra
   if ( key == 'K' && camera.inicio + PASSO < camera.fim )
       camera.inicio += PASSO;

   // diminui o ponto inicial de visualização do objetos pela câmetra
   if ( key == 'k' && camera.inicio - PASSO > 0 )
       camera.inicio -= PASSO;

   // aumenta o ponto final de visualização do objetos pela câmetra
   if ( key == 'L' )
       camera.fim += PASSO;

   // diminiu o ponto final de visualização do objetos pela câmetra
   if ( key == 'l' && camera.fim - PASSO > camera.inicio )
       camera.fim -= PASSO;

   // aumenta o angulo de visão da câmera (zoom-out)
   if ( key == 'A' && camera.ang + PASSO < 180 )
       camera.ang += PASSO;

   // diminui o angulo de visão da câmera (zoom-in)
   if ( key == 'a' && camera.ang - PASSO > 0 )
       camera.ang -= PASSO;

   // inicializa tudo
   if ( key == 'I'  || key == 'i' )
      Inicializa();

   EspecificaParametrosVisualizacao();

   // obriga redesenhar
   glutPostRedisplay();

}

// tratamento das teclas especiais (teclas de função e de navegação). os parâmetros que recebe são a tecla pressionada e a posição x e y
void TeclasEspeciais( GLint key , GLint x , GLint y ){
}

// Programa Principal
int main( int argc , char *argv[] ){
   setlocale(LC_ALL, "Portuguese");

   glutInit( &argc , argv );

   // especifica a utilização dos buffers duplos e indica o sistema de cores
   glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH  );

   // especifica as dimensões da janela na tela, em pixels
   glutInitWindowSize( 500 , 500 );

   // especifica a coordenada superior esquerda da janela na tela de vídeo
   glutInitWindowPosition( 0 , 0 );

   // cria a janela com o titulo especificado
   glutCreateWindow( "Visualizacao 3D com Textura" );

   // determina qual a função do programa que desenhará os objetos
   glutDisplayFunc( Desenha );

   // determina qual a função do programa que será executada a cada alteração do tamanho da janela no video
   glutReshapeFunc( AlteraTamanhoJanela );

   // determina qual a função do programa tratará o uso do mouse
   glutMouseFunc( GerenciaMouse );

   // determina qual a função do programa tratará o teclado
   glutKeyboardFunc( GerenciaTeclado );

   // Determina qual a função do programa que tratará as teclas especiais digitadas ( teclas de função e de navegação)
   glutSpecialFunc( TeclasEspeciais );

   // mostra na console um help sobre as teclas que o programa usa e o que cada uma faz

   printf("\n----------------------BASIC CONTROLS----------------------");
   printf("\nI i => Initialization");
   printf("\nR r => select transformation ROTATION");
   printf("\nT t => select transformation TRANSLATION");
   printf("\nX x => select X axis");
   printf("\nY y => select Y axis");
   printf("\nZ z => select Z axis");
   printf("\n+ - => transformation direction");
   printf("\n\n--------------------OTHER COMMANDS-----------------------");
   printf("\nG g => rotate camera");
   printf("\nD d => changes position of the camera on X axis");
   printf("\nC c => changes position of the camera on Y axis");
   printf("\nP p => changes position of the camera on Z axis");
   printf("\nE e => change target x");
   printf("\nB b => change target y");
   printf("\nF f => change target z");
   printf("\nK k => changes start of viewing depth");
   printf("\nL l => changes end of viewing depth");
   printf("\nA a => changes the lens opening angle");
   printf("\nV v => show / hide axes");
   printf("\nQ q => walking (tightening interspersed with w)");
   printf("\nW w => walking (tightening interspersed with q)");
   printf("\nESC => exit");
   printf("\n\n---------------------OBJECT SELECTION----------------------");
   printf("\n0 : STEVE");
   printf("\n1 : HEAD");
   printf("\n2 : BODY");
   printf("\n3 : RIGHT ARM");
   printf("\n4 : LEFT ARM");
   printf("\n5 : HIP");
   printf("\n6 : RIGHT LEG");
   printf("\n7 : LEFT LEG");
   printf("\n8 : RIGHT FOOT");
   printf("\n9 : LEFT FOOT");
   printf("\n\n--------------------------WINDOW----------------------------");

   // função simples que inicializa os parãmetros da câmera e da projeção a ser utilizada
   Inicializa();
   // executa
   glutMainLoop();
}


