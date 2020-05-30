#define WIDTH     800
#define HEIGHT    600
#define N_BUTTONS   4
#define NAME_SIZE  32
#define KEY_EXIT   27

#include "graphics.h"
#include <coniow.h>
#include <stdio.h> 
#include <stdlib.h>

struct Button // структура кнопки
{
   int left;
   int top;
   int width;
   int height;
   IMAGE *image;
};

int nplayers = 2;
int iplayer = 1;
int n_numbers = 0;
char numbers[1000][5];
int cur_move = 0;
char bull[2];
char cow[2];

Button buttons[N_BUTTONS];
IMAGE *image_about;
IMAGE *image_game;
IMAGE *image_rull;

void load();
void start();
void menu();
int select(int, int);
void about();
void close();
void game();
void rull();
int file_exists(const char *file_name);
void input_string(int x, int y,char s[], int maxlen);
void show_message(int x, int y, const char fmt[], int i);
void wait_player(int i, const char s[]);
void add_number(const char n[], int player);
void wait_key();

int main()
{
   initwindow(WIDTH, HEIGHT);
   load();
   start();
   menu();
   closegraph();
   return 0;
}

void load() // загрузка файлов игры
{
   FILE *input;
   char name[NAME_SIZE];
   
   input = fopen("buttons.txt", "r");
   for (int i = 0; i < N_BUTTONS; i++)
   {
      fscanf(input, "%d", &buttons[i].left);
      fscanf(input, "%d", &buttons[i].top);
      fscanf(input, "%s", name);
      buttons[i].image  = loadBMP(name);
      buttons[i].width  = imagewidth(buttons[i].image);
      buttons[i].height = imageheight(buttons[i].image);
   }
   fclose(input);
   
   image_game = loadBMP("window_game.bmp");
   image_about = loadBMP("window_about.bmp");
   image_rull = loadBMP("window_rull.bmp");
}

void start()
{
   IMAGE *image;
   image = loadBMP("window_start.bmp");
   putimage(0, 0, image, COPY_PUT);
   freeimage(image);
   getch();
}

void menu()
{
   int i;
   
   while (1)
   {
      putimage(0, 0, image_game, COPY_PUT);
      for (i = 0; i < N_BUTTONS; i++)
      {
         putimage(buttons[i].left, buttons[i].top,
                  buttons[i].image, COPY_PUT);
      }
      
      i = -1;
      while (i < 0)
      {
         while (mousebuttons() != 1);
            i = select(mousex(), mousey());
      }
      
      switch (i)
      {
         case 0: game();  break;
         case 1: rull(); break;
         case 2: about(); break;
         case 3: close(); return;
      }
   }
}

int select(int x, int y)
{
   for (int i = 0; i < N_BUTTONS; i++)
   {
      if (x > buttons[i].left &&
          x < buttons[i].left + buttons[i].width &&
          y > buttons[i].top &&
          y < buttons[i].top + buttons[i].height)
      {
         return i;
      }
   }
   return -1;
}
// вывод правил
void rull()
{
   putimage(0, 0, image_rull, COPY_PUT);
   while (getch() != KEY_EXIT);
}

void about()
{
   putimage(0, 0, image_about, COPY_PUT);
   while (getch() != KEY_EXIT);
}

void close()
{
   for (int i = 0; i < N_BUTTONS; i++)
   {
      freeimage(buttons[i].image);
   }
   freeimage(image_about);
   freeimage(image_game);
   remove("player1.txt");
   remove("player2.txt");
}

int file_exists(const char *file_name)
{ 
   FILE *f = fopen(file_name, "r");
   if (f != NULL)
   {
      fclose(f);
      return 1;
   } 
   else
      return 0;
}

void schet(char n[],int len) // подсчет коров и быков
{
   bull[0] = '0';
   cow[0] = '0';
   if(cur_move >1)
   {
      for (int i = 0; i<len; i++)
      {
         for (int j = 0; j < len; j++)
         {
            if (numbers[1 * (2 - iplayer)][j] == n[i])
            {
               if (i == j)
                  bull[0]++;
               else 
                  cow[0]++;
            }
         }
      }
   }
}

int proverka(char n[],int len) // проверка числа на повторяющиеся цифры
{
   
   for (int i = 0; i < len; i++)
      for (int j = i+1; j < len; j++)
      {
         if (n[i] == n[j])
            return 0;
      }
   return 1;
}

void wait_player(int i,const char s[])
{
   FILE *f;
   char file_name[15];
   setfillstyle(SOLID_FILL,LIGHTGRAY);
   bar(10,10,500,70);
   sprintf(file_name,"player%d.txt",i);
   show_message(10, 10, "Ожидание хода участника %d",i);
   while(1) {
      if(file_exists(file_name)) {
         f=fopen(file_name,"r");
         if(f!=NULL) {
            int m;
            fscanf(f,"%d",&m);
            if(m<cur_move)
            {
               fclose(f);
            }
            else {
               fscanf(f,"%s%*c%c%*c%c",s, &bull[0], &cow[0]);
               fclose(f);
               // удалить файл после получения хода
               remove(file_name);
               return;
            }
         }
      }
      delay(500);
   }
}

void show_message(int x, int y, const char fmt[], int i)
{
   char msg[100];
   setfillstyle(SOLID_FILL,COLOR(255, 239, 213));
   bar(x,y,500,70);
   setcolor(BLACK);
   sprintf(msg,fmt,i);
   outtextxy(x,y,msg);
}

void add_number(const char n[], int player)
{
   strcpy(numbers[n_numbers],n);
   setcolor(BLACK);
   if (cur_move > 1)
   {
      if (player == 1)
      {
         outtextxy(10 , 90 + (n_numbers/2*30)%510,n);
         outtextxy(80 , 90 + (n_numbers/2*30)%510,bull);
         outtextxy(140 , 90 + (n_numbers/2*30)%510,cow);
      }
      else
      {      
         outtextxy(200 , 90 + ((n_numbers-1)/2*30)%510,n);
         outtextxy(300 , 90 + ((n_numbers-1)/2*30)%510,bull);
         outtextxy(360 , 90 + ((n_numbers-1)/2*30)%510,cow);
      }
   }
   else if (iplayer == player)
   {
      show_message(500 , 10, "Вы загадали число:", 0);
      outtextxy(640, 10,n);
   }
   n_numbers++;
}

void input_string(int x, int y, char s[], int maxlen)
{
  int len=0;
   s[0]=0;
   show_message(10, 50,"",0); // нет сообщений об ошибках
   while(1)
   {
      setfillstyle(SOLID_FILL,COLOR(255, 239, 213));
      bar(x,y+3,x+textwidth("0")*maxlen,y+12);
      setcolor(BLACK);
      outtextxy(x+2,y-5+5,s);
      int k=getch();
      char c=k; // так как нужно сравнивать с [signed] char
      show_message(10 , 50, "", 0); // очистить сообщения об ошибках
      if(k==0) getch();
      else if(k==KEY_ENTER && len==4)
      {
         if(proverka(s, 4))
         {
            s[len]=0;
            schet(s, 4);
            return;
         }
         else
         {
            show_message(10, 50,"Слово должно содержать четыре неповторяющиеся цифры от 0 до 9",0);
         }
      }
      else if(k==KEY_BACKSPACE && len>0)
      {
         --len;
         s[len]=0;
      }
      else if(k==KEY_ESC) // Сдаться
      { s[0]='-';
         s[1]=0;
         return;
      }
      else if(c >= 48 && c <= 57 && len < maxlen)
      {
         s[len++]=c;
         s[len]=0;
      }
      else
      {
         show_message(10, 50,"Слово должно содержать четыре неповторяющиеся цифры от 0 до 9",0);
      }
      
   }
}

void wait_key()
{
   int k=getch(); 
   if(k==0) getch();
}

void game()
{
   n_numbers = 0;
   cur_move = 0;
   char file_name[15];
   char number[5];
   FILE *f;
   iplayer=1;
   strcpy(file_name, "player1.txt");
   if ( file_exists("player1.txt") ) // файл существует - эта копия ходит 2-й
   { 
      iplayer=2;
      strcpy(file_name, "player2.txt");
   }
   f=fopen(file_name,"w");
   fprintf(f,"0\n");
   fclose(f);
   
   setbkcolor(COLOR(255, 239, 213));
   cleardevice();
   show_message(10, 10,"Загадайте число сопернику",0);
   show_message(10 + ((iplayer-1)*200), 70,"Вы",0);
   show_message(80 , 70,"Быки",0);
   show_message(140, 70,"Коровы",0);
   show_message(200 / (iplayer + ((iplayer -1) * 198)), 70,"Cоперник",0);
   show_message(300 , 70,"Быки",0);
   show_message(360, 70,"Коровы",0);
   wait_key();
   while (1)
   {
      cur_move++;
      for (int i = 1; i <= nplayers; i++)
      {
         if(i==iplayer)
         {
            show_message(10, 10,"Введите число",0);
            input_string(10, 25, number, 4);
            if (number[0] == '-')
            {
               f=fopen(file_name,"w");
               fprintf(f,"%d\n-\n",cur_move);
               fclose(f);
               show_message(10, 10, "Участник %d сдался",i);
               wait_key();
               return;
            }
            else if(bull[0] == '4')
            {
               f=fopen(file_name,"w");
               fprintf(f,"%d\n+\n",cur_move);
               fclose(f);
               show_message(10, 10, "Участник %d выиграл",i);
               wait_key();
               return;
            }
            else
            {
               f = fopen(file_name,"w");
               fprintf(f,"%d\n%s\n%c\n%c\n",cur_move,number, bull[0], cow[0]);
               fclose(f);
               add_number(number, i);
            }
         }
         else
         {
            wait_player(i,number);
            if(number[0]=='-') {
               // сдался
               show_message(10, 10, "Участник %d сдался",i);
               remove(file_name); // удаляем свой файл
               wait_key();
               return;
            }
            if (number[0]=='+')
            {
               show_message(10, 10, "Участник %d выиграл",i);
               remove(file_name); // удаляем свой файл
               wait_key();
               return;
            }
            else {
               add_number(number, i);
            }
         }
      }
   }
}