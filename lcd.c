#include "DEV_Config.h"
#include "LCD_1in14.h"
#include "ugui.h"

    uint8_t keyA = 15; 
    uint8_t keyB = 17; 

    uint8_t up = 2;
	uint8_t dowm = 18;
	uint8_t left = 16;
	uint8_t right = 20;
	uint8_t ctrl = 3;
   
UG_GUI maingui;
void UserPixelSetFunction(UG_S16 x, UG_S16 y, UG_COLOR c )
{
  LCD_1IN14_DisplayPoint(x,y,c);
}

#define MAX_OBJECTS 10

void window_1_callback(UG_MESSAGE *msg){
  if(msg->type != MSG_TYPE_OBJECT)
    return;
  if(msg->id != OBJ_TYPE_BUTTON)
    return;
  switch(msg->sub_id){
    case BTN_ID_0:
    UG_ConsolePutString("C\n");
    break;
    case BTN_ID_1:
    UG_ConsolePutString("OK\n");
    break;
  }
}


    UG_WINDOW window1;
    UG_BUTTON button1;
    UG_BUTTON button2;
    UG_OBJECT obj [MAX_OBJECTS] ;


int lcd_init()
{

    DEV_Delay_ms(100);
    printf("LCD_1in14_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }
    DEV_SET_PWM(50);
    /* LCD Init */
    printf("1.14inch LCD demo...\r\n");
    LCD_1IN14_Init(HORIZONTAL);
    LCD_1IN14_Clear(C_BLACK);



    UG_Init(&maingui, UserPixelSetFunction,LCD_1IN14_HEIGHT, LCD_1IN14_WIDTH);
    UG_SelectGUI(&maingui);

    UG_FontSelect(&FONT_8X14);
    UG_FillScreen(C_BLACK);
    //UG_DrawCircle(20,20,20, C_AZURE);

   // UG_PutString(20,20, "grzesl 2023");

    UG_WindowCreate(&window1, obj, MAX_OBJECTS, window_1_callback);
    
    UG_WindowSetTitleTextFont(&window1, &FONT_8X14);
    UG_WindowSetTitleText(&window1, "CTRL");
    UG_WindowResize(&window1, 181,1,238,134);

    UG_ButtonCreate(&window1, &button1, BTN_ID_0, 1,1,50,50);
    UG_ButtonSetFont(&window1, BTN_ID_0, &FONT_8X14);
    UG_ButtonSetText(&window1, BTN_ID_0, "C");
    UG_ButtonShow(&window1, BTN_ID_0);

    UG_ButtonCreate(&window1, &button2, BTN_ID_1, 1,60,50,110);
    UG_ButtonSetText(&window1, BTN_ID_1, "OK");
    UG_ButtonSetFont(&window1, BTN_ID_1, &FONT_8X14);
    UG_ButtonShow(&window1, BTN_ID_1);

    UG_WindowShow(&window1);

    
    UG_WindowSetBackColor ( &window1 , C_BLACK ) ;

    UG_ConsoleSetBackcolor(C_BLACK);
    UG_ConsoleSetForecolor(C_LIGHT_CYAN);
    UG_ConsoleSetArea(0,0,180,137);
    UG_ConsolePutString("grzesl 2023 ;)\n");
    
    SET_Infrared_PIN(keyA);    
    SET_Infrared_PIN(keyB);
		 
	  SET_Infrared_PIN(up);
    SET_Infrared_PIN(dowm);
    SET_Infrared_PIN(left);
    SET_Infrared_PIN(right);
    SET_Infrared_PIN(ctrl);
    

    //DOUBLE Imagesize = LCD_1IN14_HEIGHT*LCD_1IN14_WIDTH*2;
    ///UWORD *BlackImage;
    //if((BlackImage = (UWORD *)calloc(1, Imagesize)) == NULL) {
    //    printf("Failed to apply for black memory...\r\n");
   //     exit(0);
    //}


  /*  UWORD* BlackImage = Paint_NewImage(LCD_1IN14.WIDTH,LCD_1IN14.HEIGHT, 0, WHITE);
    Paint_SetScale(65);
    Paint_Clear(WHITE);
    Paint_SetRotate(ROTATE_180);
    Paint_Clear(BLACK);



    Paint_DrawNum (1, 40 ,9.87654321, &Font20,3,  WHITE,  BLACK);
    Paint_DrawString_EN(1, 1, "ABCD", &Font20, 0x000f, 0xfff0);
    Paint_DrawString_EN(1, 20, "Pico-LCD-1.14", &Font16, RED, WHITE); 

  //  DEV_Delay_ms(2000);

    // /*3.Refresh the picture in RAM to LCD*/
   // LCD_1IN14_Display(BlackImage);
  //  DEV_Delay_ms(2000);*/

    DEV_SET_PWM(10);



}

extern void* server_state;

uint8_t read_lcd_keyb(){
      uint8_t key = 0;
        if(DEV_Digital_Read(keyA ) == 0){
            printf("gpio =%d\r\n",keyA);
            key = keyA;
            char c = 'C';
            tcp_server_write(server_state, (char *)&c, 1);
            putchar_raw(c);
            SerialPutchar(0,c);
            SerialPutchar(1,c);
            UG_TouchUpdate ( obj[0].a_abs.xs + 5, obj[0].a_abs.ys + 5, TOUCH_STATE_PRESSED ) ;
            return key;
        }
        else{
          sleep_ms(50);
          UG_TouchUpdate ( -1, -1, TOUCH_STATE_RELEASED ) ;
        }
            
        if(DEV_Digital_Read(keyB ) == 0){
            printf("gpio =%d\r\n",keyB);
            key = keyB;
            char c = 'O';
            tcp_server_write(server_state, (char *)&c, 1);
            putchar_raw(c);
            SerialPutchar(0,c);
            SerialPutchar(1,c);
            UG_TouchUpdate ( obj[1].a_abs.xs + 5, obj[1].a_abs.ys + 5, TOUCH_STATE_PRESSED ) ;
            return key;
            
        }
        else{
            sleep_ms(50);
            UG_TouchUpdate ( -1, -1, TOUCH_STATE_RELEASED ) ;
        }
        
        if(DEV_Digital_Read(up ) == 0){
            printf("gpio =%d\r\n",up);
            key = up;
            sleep_ms(50);
        }
        else{

        }

        if(DEV_Digital_Read(dowm ) == 0){

            printf("gpio =%d\r\n",dowm);
            key = dowm;
            sleep_ms(50);
        }
        else{

        }
        
        if(DEV_Digital_Read(left ) == 0){

            printf("gpio =%d\r\n",left);
            key = left;
            sleep_ms(50);
        }
        else{

        }
            
        if(DEV_Digital_Read(right ) == 0){

            printf("gpio =%d\r\n",right);
            key = right;
            sleep_ms(50);
        }
        else{

        }
        
        if(DEV_Digital_Read(ctrl ) == 0){

            printf("gpio =%d\r\n",ctrl);
            key = ctrl;
            sleep_ms(50);
        }
        else{
            
        }


        return key;
}

int lcd_loop()
{
  read_lcd_keyb();
  UG_Update();
  return 0;
}

