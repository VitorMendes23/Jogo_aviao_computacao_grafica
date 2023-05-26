#include <GL/glut.h>
#include <GL/freeglut.h>
#include "SOIL/SOIL.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <string.h>


const int LARGURA_JANELA = 640;    // Largura da janela
const int ALTURA_JANELA = 480;   // Altura da janela

const float  LARGURA_AVIAO = 90;// Tamanho do quadrado do avião
const float ALTURA_AVIAO = 90;


float x_Quadrado = LARGURA_JANELA / 2; // Posição inicial do quadrado na coordenada X
float y_Quadrado = ALTURA_JANELA / 2; // Posição inicial do quadrado na coordenada Y

int listaCenario;
bool botaoPrecionado = false;
int click = 0;
int mouseX, mouseY;

bool key_pressed[256] = {false}; // declara e inicializa um array global para acompanhar quais teclas foram pressionadas simultaneamente


GLuint texturaAviao_ID; 

struct
{
  int largura,altura;
}ajusteJanela;

struct
{
    float coordenada_x, coordenada_y; //coordenadas x e y do aviao
    int sprite_x, sprite_y; // 0=imagem central | +1,+2=imagens da direita | -1,-2 = imagens da esquerda
                          // +1,+2,+3 = imagens de cima | -1,-2,-3 = imagens de baixo  
}aviao;

void criaTexto();//assinatura das funções
void criaAviao();

GLuint carregaTextura(const char* arquivo) {  // função para carregar texturas
    GLuint idTextura = SOIL_load_OGL_texture(
                           arquivo,
                           SOIL_LOAD_AUTO,
                           SOIL_CREATE_NEW_ID,
                           SOIL_FLAG_INVERT_Y
                       );

    if (idTextura == 0) {
        printf("Erro do SOIL: '%s'\n", SOIL_last_result());
    }

    return idTextura;
}
void inicializa(){
    aviao.sprite_x=0;
    aviao.sprite_y=0;
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    texturaAviao_ID = carregaTextura("avioes.png");
}
void redimensiona(int width, int height) {
    glViewport(0,0,width,height);   //define a área da janela de visualização do OpenGL que será usada para renderizar a cena
    glMatrixMode(GL_PROJECTION);    //define que a matriz de projeção será usada para transformar as coordenadas do objeto em coordenadas de visualização
    glLoadIdentity();               //define a matriz de projeção inicial como a matriz identidade, que não realiza nenhuma transformação   

    glOrtho(0, LARGURA_JANELA, 0, ALTURA_JANELA, -1,1); //origem (0, 0) fica no canto inferior esquerdo da janela
    
    //para que o quadrado e circulo permaneçam dentro da limitação da tela mesmo com redimensionamentos
    ajusteJanela.altura = height-ALTURA_JANELA;
    ajusteJanela.largura = width-LARGURA_JANELA;

    float razaoJanela = ((float)width)/height;
    float razaoMundo = ((float)LARGURA_JANELA)/ALTURA_JANELA;

    if(razaoJanela < razaoMundo){
        float hViewport = width / razaoMundo;
        float yViewport = (height - hViewport)/2;
        glViewport(0, yViewport, width, hViewport);
    }
    else if( razaoJanela > razaoMundo){
        float wViewport = ((float)height) * razaoMundo;
        float xViewport = (width - wViewport)/2;
        glViewport(xViewport, 0, wViewport, height);

    }else{
        glViewport(0,0,width,height);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void desenhaMinhaCena() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // limpar o buffer de cor e o buffer de profundidade antes de renderizar uma nova cena

    glMatrixMode(GL_PROJECTION);                            //define a matriz atual que vai ser manipulada 
    glLoadIdentity();                                       //carrega uma matriz identidade na pilha de matrizes 
    glOrtho(0,LARGURA_JANELA,0, ALTURA_JANELA,-1,1);        //define uma matriz de projeção ortogonal para a janela, especificando os limites esquerdo, direito, inferior e superior da janela, bem como as coordenadas de profundidade 
    glMatrixMode(GL_MODELVIEW);                             //define que a matriz atual a ser manipulada
    glLoadIdentity();                                       //carrega uma matriz identidade na pilha de matrizes de modelo-vista, resetando quaisquer transformações anteriores

    glCallList(listaCenario);     // chama a lista  
    criaAviao();                  //chama a função que estrutura o quadrado do aviao
    
   if (!botaoPrecionado) {
        glutSwapBuffers();
        glutPostRedisplay();
        return;
    }
   if(click==2){
    glColor3f(0.5,0.5,0.8); //cor
    glLoadIdentity();
    glTranslatef(mouseX, mouseY, 0);
    glBegin(GL_QUADS);
        glVertex2f(-50, -50);
        glVertex2f(50, -50);
        glVertex2f(50, 50);
        glVertex2f(-50, 50);
    glEnd();
    printf("Desenhando quadrado! \n");

   }
   if(click==1){
    glColor3f(1, 0, 1); //cor
    glLoadIdentity();
    glTranslatef(mouseX, mouseY, 0);
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0, 0);
    for (int i = 0; i <= 360; i++) {
        float rad = i * 3.14 / 180.0;
        glVertex2f(cos(rad) * 100, sin(rad) * 100);
    }
    glEnd();
    printf("Desenhando circulo! \n");

   }
   
  glutSwapBuffers(); //para desenhar a cena (o Swap é para 2 buffers e glFlush para 1)// Diz ao OpenGL para colocar o que desenhamos na tela
}
void desenhaArvoreCima(float x, float y, float raio, float altura) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f); // Translada a árvore para a posição especificada pelos parâmetros x e y

    // Desenha o tronco da árvore
    glColor3f(0.8f, 0.4f, 0.1f); // Cor marrom
    glBegin(GL_POLYGON);
    glVertex2f(-10.0f, 0.0f);
    glVertex2f(-10.0f, altura);
    glVertex2f(10.0f, altura);
    glVertex2f(10.0f, 0.0f);
    glEnd();

    // Desenha as folhas da árvore
    glColor3f(0.0f, 0.6f, 0.0f); // Cor verde
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(0.0f, altura + raio);
    for (int i = 0; i <= 360; i += 10) {
        float angulo = i * 3.14159 / 180.0;
        float x = raio * cos(angulo);
        float y = raio * sin(angulo);
        glVertex2f(x, y + altura);
    }
    glEnd();

    glPopMatrix();
}


void desenhaRua(float x, float y, float escala) {
    glPushMatrix();
    glTranslatef(x, y, 0.0f); // Translada a árvore para a posição especificada pelos parâmetros x e y
    glScalef(escala, escala, escala); // Aplica uma escala uniforme na árvore, definida pelo parâmetro escala

    // Desenha rua
    glColor3f(0.5f, 0.5, 0.5f); // Cor rua
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(-53, -500);
        glVertex2f(53, -500);
        glVertex2f(53, 500);
        glVertex2f(-53, 500);
    glEnd();

    //faixa
    glColor3f(1.0f, 1.0f, 0.0f); // Cor faixa
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(-2, -500);
        glVertex2f(-1, -500);
        glVertex2f(-1, 500);
        glVertex2f(-2, 500);
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f); // Cor faixa
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(1, -500);
        glVertex2f(2, -500);
        glVertex2f(2, 500);
        glVertex2f(1, 500);
    glEnd();

    glColor3f(0.3f, 0.1f, 0.0f); // Cor da mureta
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(52, -500);
        glVertex2f(55, -500);
        glVertex2f(52, 500);
        glVertex2f(55, 500);
    glEnd();

    glColor3f(0.3f, 0.1f, 0.0f); // Cor da mureta
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(-52, -500);
        glVertex2f(-55, -500);
        glVertex2f(-52, 500);
        glVertex2f(-55, 500);
    glEnd();


    glPopMatrix();
}


void criaListaETexto() {
  listaCenario = glGenLists(1);
  glNewList(listaCenario,GL_COMPILE);  

    glColor3f(0.4f, 0.6f, 0.0f); //cor da terra do parque 
    glBegin(GL_TRIANGLE_FAN);
        glVertex3f(0, 480,0);
        glVertex3f(640, 480,0);
        glVertex3f(640, 0,0);
        glVertex3f(0,0,0);
    glEnd();

    glPushMatrix();
      glTranslatef(430, 160, 0);      //fazendo o lago do parque
      glColor3f(0, 0, 0.80);
      glBegin(GL_TRIANGLE_FAN);
        GLfloat x1=70, y1=70, z1=0;
        float angulo1, raio1=50;
          glVertex3f(x1, y1, z1);
            for (int i = 0; i <= 30; ++i) {
              angulo1 = 2 * 3.14 * i / 30;
              glVertex3f(x1 + cos(angulo1) * raio1, y1 + sin(angulo1) * raio1, z1);
            }
      glEnd();
                  
    glPopMatrix();

    desenhaRua(135, 120, 2);

    desenhaArvoreCima(450, 120, 7,2);//desenhaArvoreCima(float x, float y, float raio, float altura) 
    desenhaArvoreCima(350, 220, 7.5,3);
    desenhaArvoreCima(290, 350, 8,2);
    desenhaArvoreCima(480, 310, 10,5);
    desenhaArvoreCima(280, 180, 9,3);
    desenhaArvoreCima(395, 220, 7,4);
    desenhaArvoreCima(395, 250, 7,4);
    desenhaArvoreCima(395, 290, 7,4);



    criaTexto();
    

  glEndList();
}
void criaTexto(){
    glColor3f(0.0f, 0.0f, 0.0f);//definindo cor do texto
     glRasterPos2f(150, glutGet(GLUT_WINDOW_HEIGHT) - 50); //define a posição do texto
    char* text = "Vitor Mendes de Oliveira Abreu"; //texto a ser exibido
    for (int i = 0; i < strlen(text); i++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, text[i]); //exibe letra por letra dentro do loop
    }
}
void criaAviao(){

  float alteraSpriteX = 0.4+(aviao.sprite_x*0.2); //recorta a imagem correta do avião
  float alteraSpritey = 0.4+(aviao.sprite_y*0.2);

  glPushMatrix();//salvar a matriz atual do sistema de coordenadas em uma pilha de matrizes
  glColor3f(0.0f, 0.0f, 1.0f); // Define a cor do quadrado como vermelho
  glTranslatef(x_Quadrado, y_Quadrado, 0.0f); // Move o quadrado para a posição correta aplicando uma translação à matriz atual do sistema de coordenadas.


  glColor3f(1.0,1.0,1.0);
  glEnable(GL_TEXTURE_2D);                          // Habilita o uso de texturas
  glBindTexture(GL_TEXTURE_2D, texturaAviao_ID);    //associa o ID à textura
    glPushMatrix();
    glTranslatef(aviao.coordenada_x, aviao.coordenada_y, 0);

      glBegin(GL_TRIANGLE_FAN); // Começa a desenhar o quadrado
        
        glTexCoord2f(alteraSpriteX,alteraSpritey);
        glVertex2f(-LARGURA_AVIAO / 2, -ALTURA_AVIAO / 2); // Canto inferior esquerdo     

        glTexCoord2f(alteraSpriteX+0.2,alteraSpritey);    
        glVertex2f(LARGURA_AVIAO / 2, -ALTURA_AVIAO / 2);  // Canto inferior direito

        glTexCoord2f(alteraSpriteX+0.2,alteraSpritey+0.2);
        glVertex2f(LARGURA_AVIAO / 2, ALTURA_AVIAO / 2);   // Canto superior direito
        
        glTexCoord2f(alteraSpriteX,alteraSpritey+0.2);
        glVertex2f(-LARGURA_AVIAO / 2, ALTURA_AVIAO / 2);  // Canto superior esquerdo

      glEnd(); // Termina de desenhar o quadrado

    glPopMatrix();

  glDisable(GL_TEXTURE_2D); // fecha a textura

  glutPostRedisplay();


}
void teclaPrecionada(unsigned char key, int x, int y) {
  switch (key) {
    case 'w': // Tecla W pressionada
           //limitando movimentação, 
        if(key_pressed['d'] && x_Quadrado < (LARGURA_JANELA - 40) && y_Quadrado < (ALTURA_JANELA - 40)){   // verifica se a tecla 'd' também foi pressionada
          y_Quadrado += 20.0f; // Move o quadrado para cima]
          x_Quadrado += 20.0f; // Move o quadrado para a direita
          if(aviao.sprite_y>=0 && aviao.sprite_y< 2){   
            aviao.sprite_y++;
          }
          if( aviao.sprite_x >=0 && aviao.sprite_x <2){
            aviao.sprite_x++;
          }
          printf("W e D foram pressionados simultaneamente! \n");

        }else if(key_pressed['a'] && x_Quadrado > 40 && y_Quadrado < (ALTURA_JANELA - 40)){ // verifica se a tecla 'a' também foi pressionada
        
          y_Quadrado += 20.0f; // Move o quadrado para cima]
          x_Quadrado -= 20.0f; // Move o quadrado para a esquerda
          if(aviao.sprite_y>=0 && aviao.sprite_y<2){
            aviao.sprite_y++;
          }
          if(aviao.sprite_x <=0 && aviao.sprite_x >-2){
            aviao.sprite_x--;
          }
          printf("W e A foram pressionados simultaneamente! \n");
        }else if(y_Quadrado < (ALTURA_JANELA - 40) ){
          y_Quadrado += 20.0f; // Move o quadrado para cima] apenas w está precionado
          if(aviao.sprite_y<2){
          aviao.sprite_y ++;
          }
        }

      
      printf("Tamanho do y do quadrado: %f  \n", y_Quadrado);
      break;
    case 'a': // Tecla A pressionada
                       //limitando movimentação
        if(key_pressed['w'] && x_Quadrado > 40 && y_Quadrado <= (ALTURA_JANELA - 40)){   // verifica se a tecla 'w' também foi pressionada
            y_Quadrado += 20.0f; // Move o quadrado para cima]
            x_Quadrado -= 20.0f; // Move o quadrado para a direita
          if(aviao.sprite_y >=0 && aviao.sprite_y< 2){
            aviao.sprite_y++;
          }
          if(aviao.sprite_x <= 0 && aviao.sprite_x >-2){
            aviao.sprite_x--;
          }
            printf("W e A foram pressionados simultaneamente! \n");

          }else if(key_pressed['s'] && x_Quadrado > 40 && y_Quadrado > 40){ // verifica se a tecla 's' também foi pressionada
            
            y_Quadrado -= 20.0f; // Move o quadrado para cima]
            x_Quadrado -= 20.0f; // Move o quadrado para a esquerda
          if(aviao.sprite_y<=0 && aviao.sprite_y>-2){
            aviao.sprite_y--;
          }
          if(aviao.sprite_x <0 && aviao.sprite_x >-2){
            aviao.sprite_x--;
          }
            printf("S e A foram pressionados simultaneamente! \n");
          }else if(x_Quadrado > 40) {
          x_Quadrado -= 20.0f; // Move o quadrado para a esquerda
          if(aviao.sprite_x>-2){
            aviao.sprite_x--;
          }
          }

      
      printf("Tamanho do x do quadrado: %f  \n", x_Quadrado);
      break;
    case 's': // Tecla S pressionada
                          //limitando movimentação
        if(key_pressed['d'] && y_Quadrado > 40 && x_Quadrado < (LARGURA_JANELA - 40)){   // verifica se a tecla 'd' também foi pressionada
            y_Quadrado -= 20.0f; // Move o quadrado para baixo
            x_Quadrado += 20.0f; // Move o quadrado para a direita
            if(aviao.sprite_y<=0 && aviao.sprite_y>-2){
              aviao.sprite_y--;
            }
            if( aviao.sprite_x >=0 && aviao.sprite_x <2){
              aviao.sprite_x++;
            }
            printf("S e D foram pressionados simultaneamente! \n");

          }else if(key_pressed['a'] && y_Quadrado > 40 && x_Quadrado > 40){ // verifica se a tecla 'a' também foi pressionada
          
            y_Quadrado -= 20.0f; // Move o quadrado para baixo
            x_Quadrado -= 20.0f; // Move o quadrado para a esquerda
          if(aviao.sprite_y<=0 && aviao.sprite_y>-2){
            aviao.sprite_y--;
          }
          if( aviao.sprite_x <=0 && aviao.sprite_x >-2){
            aviao.sprite_x--;
          }
            printf("S e A foram pressionados simultaneamente! \n");
          }else if(y_Quadrado > 40 ) {
          y_Quadrado -= 20.0f; // Move o quadrado para baixo
          if(aviao.sprite_y>-2){
            aviao.sprite_y--;
          }
          }

        
      printf("Tamanho do y do quadrado: %f  \n", y_Quadrado);
      break;
    case 'd': // Tecla D pressionada
                //limitando movimentação
          if(key_pressed['w'] && x_Quadrado < (LARGURA_JANELA - 40) && y_Quadrado < (ALTURA_JANELA - 40)){   // verifica se a tecla 'w' também foi pressionada
            y_Quadrado += 20.0f; // Move o quadrado para cima]
            x_Quadrado += 20.0f; // Move o quadrado para a direita
            if(aviao.sprite_y>=0 && aviao.sprite_y< 2){
              aviao.sprite_y++;
            }
            if( aviao.sprite_x >=0 && aviao.sprite_x <2){
              aviao.sprite_x++;
            }
            printf("W e D foram pressionados simultaneamente! \n");

          }else if(key_pressed['s'] && y_Quadrado > 40 && x_Quadrado < (LARGURA_JANELA - 40)){ // verifica se a tecla 's' também foi pressionada
          
            y_Quadrado -= 20.0f; // Move o quadrado para cima]
            x_Quadrado += 20.0f; // Move o quadrado para a esquerda
            if(aviao.sprite_y<=0 && aviao.sprite_y>-2){
              aviao.sprite_y--;
            }
            if( aviao.sprite_x >=0 && aviao.sprite_x <2){
              aviao.sprite_x++;
            }
            printf("D e S foram pressionados simultaneamente! \n");
          }else if(x_Quadrado < (LARGURA_JANELA - 40)){
            x_Quadrado += 20.0f; // Move o quadrado para a direita
            if(aviao.sprite_x<2){
            aviao.sprite_x++;
            }
          }

      
      printf("Tamanho do x do quadrado: %f  \n", x_Quadrado);      
      break;
    case 27:  //tecla esc para fechar o jogo
      exit(0);
      break;
    default:
      aviao.sprite_x=0;
      aviao.sprite_y=0;
      break;
  }
  key_pressed[key] = true; // atualiza o array com a tecla pressionada

  glutPostRedisplay(); // Redesenha a janela
}
void teclaLiberada(unsigned char key, int x, int y) {
  // código a ser executado quando a tecla é liberada
  switch (key) {
    case 'w':
      key_pressed['w'] = false;
      aviao.sprite_x = 0;   //posição (0,0)
      aviao.sprite_y = 0;
      printf("tá funcionando W \n");// teste
    break;
    case 'a':
      key_pressed['a'] = false;
      aviao.sprite_x = 0;
      aviao.sprite_y = 0;
      printf("tá funcionando A \n");
    break;
    case 's':
      key_pressed['s'] = false;
      aviao.sprite_x = 0;
      aviao.sprite_y = 0;
    break;
    case 'd':
      key_pressed['d'] = false;
      aviao.sprite_x = 0;
      aviao.sprite_y = 0;
    break;
  default:
    break;
  }
}
void mouse(int button, int state, int x, int y) {
/*Conversão das coordenadas da tela para as coordenadas do mundo
    A solução abaixo foi encontrada em: https://community.khronos.org/t/converting-window-coordinates-to-world-coordinates/16029/8
    */
    GLint viewport[4];
    GLdouble modelview[16]; //var to hold the modelview info
    GLdouble projection[16]; //var to hold the projection matrix info
    GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
    GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

    glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
    glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
    glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info

    winX = (float)x;
    winY = (float)viewport[3] - (float)y;
    winZ = 0;

    gluUnProject( winX, winY, winZ, modelview, projection, viewport, &worldX, &worldY, &worldZ);
    // fim da conversão

     if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN){
            mouseX = worldX;
            mouseY = worldY;
            botaoPrecionado = true;
            click=2;
            printf("Tecla direita precionada! \n");
        } else if (state == GLUT_UP) {
            botaoPrecionado = false;
        }
    }

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN){
            mouseX = worldX;
            mouseY = worldY;
            botaoPrecionado = true;
            click=1;
            printf("Tecla esquerda precionada! \n");
        }else if (state == GLUT_UP) {
            botaoPrecionado = false;
        }
    }

    glutPostRedisplay();
}

void atualiza() {
    glutPostRedisplay();
}

int main(int argc, char** argv) {
  // Inicializa o GLUT e cria a janela
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(LARGURA_JANELA, ALTURA_JANELA);  // Define o tamanho da janela
  glutCreateWindow("Jogo aviao 2D");
  glutInitWindowPosition(100, 100);  // Define a posição da janela
  gluOrtho2D(0, LARGURA_JANELA, 0, ALTURA_JANELA); // Define o sistema de coordenadas da janela


  glutDisplayFunc(desenhaMinhaCena);  // Define a função de exibição da janela
  glutKeyboardFunc(teclaPrecionada);  // Define a função que monitora o teclado
  glutKeyboardUpFunc(teclaLiberada);  // Quando a tecla for solta
  glutMouseFunc(mouse);               // Define a função que monitora o mouse
  glutIdleFunc(atualiza);
  glutReshapeFunc(redimensiona);// Define a função que será chamada quando a janela for redimensionada



  criaListaETexto();
  inicializa();
  glutMainLoop();  // Entra no loop principal do GLUT

  return 0;


}
