
//#define BUTTON_BLINK
#define LIGHT_SCHEDULER
// #define TIME_RAND
// #define KEYPAD
// #define KEYPAD_CONTROL
// #define SEVEN_SEGMENT
// #define KEYPAD_SEVEN_SEGMENT
// #define COLOR_LED
// #define ROTARY_ENCODER
// #define ANALOG
// #define PWM

#include <stdbool.h> // booleans, i.e. true and false
#include <stdio.h>   // sprintf() function
#include <stdlib.h>  // srand() and random() functions
#include<time.h>

#include "ece198.h"

//more off time between go's
//answer is wrong

int main(void)
{
    char questions[4][25]={"...-- -..- ..---f","-.-- . .- .-.f",".---- .---- .-.-. ....-f",".- -... -.-.f"};//3x2, year,11+4,abc
    char answers[4][35]={"-....f","..--------..---.----f",".----.....f","-..f"};

    HAL_Init(); // initialize the Hardware Abstraction Layer

    // Peripherals (including GPIOs) are disabled by default to save power, so we
    // use the Reset and Clock Control registers to enable the GPIO peripherals that we're using.

    __HAL_RCC_GPIOA_CLK_ENABLE(); // enable port A (for the on-board LED, for example)
    __HAL_RCC_GPIOB_CLK_ENABLE(); // enable port B (for the rotary encoder inputs, for example)
    __HAL_RCC_GPIOC_CLK_ENABLE(); // enable port C (for the on-board blue pushbutton, for example)

    // initialize the pins to be input, output, alternate function, etc...

    InitializePin(GPIOA, GPIO_PIN_5, GPIO_MODE_OUTPUT_PP, GPIO_NOPULL, 0);  // on-board LED

    // note: the on-board pushbutton is fine with the default values (no internal pull-up resistor
    // is required, since there's one on the board)

    // set up for serial communication to the host computer
    // (anything we write to the serial port will appear in the terminal (i.e. serial monitor) in VSCode)

    SerialSetup(9600);

    // as mentioned above, only one of the following code sections will be used
    // (depending on which of the #define statements at the top of this file has been uncommented)

    while(true)
    {
        #ifdef LIGHT_SCHEDULER
            // Turn on the LED five seconds after reset, and turn it off again five seconds later.
            
        if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == false)
        {
            uint32_t del=HAL_GetTick();
            while((HAL_GetTick()-del)<1000)
            {
                //delay
            }

            int i=0;
            int q=0;//random 0-3
            srand(HAL_GetTick());
            q=rand()%4;
            
            char b[50];
            sprintf(b,"%d",q);
            SerialPuts(b);
            
            //variables storing light duration for each morse code 
            int mid = 350;    //duration of no light between each code
            int dash = 3500;
            int flash = 700;
            int space = 3500;
            uint32_t now = HAL_GetTick();

            while (questions[q][i]!='f')
            {
                if(questions[q][i]=='.')
                {
                    if (HAL_GetTick()-now<=flash)
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);   // turn on LED
                    else
                    { 
                        int a=HAL_GetTick();
                        while(HAL_GetTick()-a<=mid)
                            {HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); } // turn off LED
                        i++;
                        now=HAL_GetTick();
                    }
                }

                if(questions[q][i]=='-')
                {
                    if (HAL_GetTick()-now<=dash)
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);   // turn on LED
                    else
                    {
                        int a=HAL_GetTick();
                        while(HAL_GetTick()-a<=mid)
                            {HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false); } // turn off LED
                        i++;
                        now = HAL_GetTick();
                    }
                }

                    if(questions[q][i]==' ')
                    {
                        if (HAL_GetTick()-now<=space)
                            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);   // turn on LED
                        else
                        {
                            i++;
                            now = HAL_GetTick();
                        }
                    }
            }

            #endif

            uint32_t n1;
            uint32_t n2;
            char ans[50] = {'n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n','n'};
            int r = 0;  //index for ans array
            int k = 0;  //Loop control variable
            while (k == 0)
            {
                n1=HAL_GetTick();
                while(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)==false)
                {            }
                n2=HAL_GetTick();

                if((n2-n1)<800 && (n2-n1)>100)
                {
                    char b[50];
                    sprintf(b,"%d",1);
                    SerialPuts(b);
                    ans[r]='.';
                    r++;
                }

                else if ((n2-n1)>=800 &&(n2-n1)<2500)
                {
                    char b[50];
                    sprintf(b,"%d",2);
                    SerialPuts(b);
                    ans[r]='-';
                    r++;
                }

                //submitting answer
                else if((n2-n1)>2500)
                { 
                    char b[50];
                    sprintf(b,"%d",3);
                    SerialPuts(b);
                    k=1; //breaks
                }

            }

            int c = 0;
            bool win = true;
            while(answers[q][c]!='f')
            {
                if(answers[q][c] != ans[c])
                    win=false;
                c++;
            }

            if(win)
            {
                //blinking lights
                uint32_t one=HAL_GetTick();
                for(int i=0; i<15 ; i++)
                {
                    one=HAL_GetTick();
                    while((HAL_GetTick()-one)<200)
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);
                    while((HAL_GetTick()-one)<400)
                        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);
                }

            }

            else
            {
                //one long light
                uint32_t one=HAL_GetTick();
                while((HAL_GetTick()-one)<5000)
                    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, true);
            }

            //turning off light
            uint32_t one=HAL_GetTick();
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, false);
            while((HAL_GetTick()-one)<3000)
            {}
        
        }
    }
    
    return 0;
}

// This function is called by the HAL once every millisecond
void SysTick_Handler(void)
{
    HAL_IncTick(); // tell HAL that a new tick has happened
    // we can do other things in here too if we need to, but be careful
}
