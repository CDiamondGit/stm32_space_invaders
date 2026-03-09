
#include <stm32f031x6.h>
#include "display.h"
#include <stdint.h>
#include <stdio.h>

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py);
void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode);

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 160


void clearDisplay() {
    fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);  
}

typedef enum  {
        MAINMENU,
        GAMESTART,
        HELP,
        PAUSE,
        GAMEOVER
} PlayingState;

PlayingState currentPlayingState = MAINMENU;

void mainMenu(PlayingState *ps) {

	clearDisplay();

        int selectedOption = 0 ; 
        
        uint16_t normal = RGBToWord(0xff,0xff,0);
        uint16_t highlighted = RGBToWord(211,211,211);
        uint16_t startButton = normal;
        uint16_t helpButton = normal;
        uint16_t scoreButton = normal;
        
        int done = 0;
        
        while (!done) {
                
                if (selectedOption ==0) {
                        startButton = highlighted;
                        helpButton = normal;
                        scoreButton = normal;
                }
                if (selectedOption ==1) {
                        helpButton = highlighted;
                        scoreButton = normal;
                        startButton = normal;
                }
                if (selectedOption ==2) {
                        scoreButton = highlighted;
                        startButton = normal;
                        helpButton = normal;
                }
                printText("Start Game", 64, 60, startButton, 0);
                printText("Help", 64, 80, helpButton, 0);
                printText("High Scores", 64, 100, scoreButton, 0);
                
                
                if ((GPIOA->IDR & (1 << 11)) == 0) {
                        if (selectedOption < 2) {
                                selectedOption++;
                                delay(100);
                        } 
                }
                
                
                if ((GPIOA->IDR & (1 << 8)) == 0) {
                        if (selectedOption > 0) {
                                selectedOption--;
                                delay(100);
                        }
                }
                
                
                if ((GPIOB->IDR & (1 << 4)) == 0) {
                        if (selectedOption == 0) {
                                *ps = GAMESTART;
                                done = 1;
                        }
                        if (selectedOption == 1) {
                                *ps = HELP;   
                                done = 1;
                                
                        }
                        if (selectedOption==2) {
				*ps = RECORD;
				done = 1;
			}
                      
                }
                
        }
}



void help() {
	clearDisplay();
	
	

	 printTextX2("HELP", 40, 0, RGBToWord(255,255,0), 0);

  	 printText("MOVE THE SPACESHIP LEFT AND RIGHT WITH THE LEFT & RIGHT BUTTONS", 64, 20, RGBToWord(255,255,255), 0);

  	 printText("FIRE = UP BUTTON", 64, 60, RGBToWord(255,255,255), 0);
   	 printText("PAUSE = DOWN BUTTON", 64, 80, RGBToWord(255,255,255), 0);

	 printText("EXIT HELP WITH DOWN BUTTON", 64,120, RGBToWOrd(255,255,255),0);

  
    	while ((GPIOB->IDR & (1 << 11)) != 0) {

        
}
void playing() {
       clearDisplay(); 
}

void PauseScreen() {
        
}

void gameOver() {
        
}

volatile uint32_t milliseconds;




int main()
{
        int hinverted = 0;
        int vinverted = 0;
        int toggle = 0;
        int hmoved = 0;
        int vmoved = 0;
        uint16_t x = 50;
        uint16_t y = 50;
        uint16_t oldx = x;
        uint16_t oldy = y;
        initClock();
        initSysTick();
        setupIO();
        
        
        // Game Begins
        while(1)
        {
                // Switch statement for our Game State
                switch (currentPlayingState){
                        
                        case MAINMENU:
                        mainMenu(&currentPlayingState);
                        break;
                        case GAMESTART:
                        //runGameStart();
                        break;
                        case PAUSE:
                        //runPausedScreen();
                        break;
                        case GAMEOVER:
                        //runGameOver();
                        break;
                        default:
                        break;
                }
               
        }
        return 0;
}
void initSysTick(void)
{
        SysTick->LOAD = 48000;
        SysTick->CTRL = 7;
        SysTick->VAL = 10;
        __asm(" cpsie i "); // enable interrupts
}
void SysTick_Handler(void)
{
        milliseconds++;
}
void initClock(void)
{
        // This is potentially a dangerous function as it could
        // result in a system with an invalid clock signal - result: a stuck system
        // Set the PLL up
        // First ensure PLL is disabled
        RCC->CR &= ~(1u<<24);
        while( (RCC->CR & (1 <<25))); // wait for PLL ready to be cleared
        
        // Warning here: if system clock is greater than 24MHz then wait-state(s) need to be
        // inserted into Flash memory interface
        
        FLASH->ACR |= (1 << 0);
        FLASH->ACR &=~((1u << 2) | (1u<<1));
        // Turn on FLASH prefetch buffer
        FLASH->ACR |= (1 << 4);
        // set PLL multiplier to 12 (yielding 48MHz)
        RCC->CFGR &= ~((1u<<21) | (1u<<20) | (1u<<19) | (1u<<18));
        RCC->CFGR |= ((1<<21) | (1<<19) );
        
        // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
        RCC->CFGR |= (1<<14);
        
        // and turn the PLL back on again
        RCC->CR |= (1<<24);
        // set PLL as system clock source
        RCC->CFGR |= (1<<1);
}
void delay(volatile uint32_t dly)
{
        uint32_t end_time = dly + milliseconds;
        while(milliseconds != end_time)
        __asm(" wfi "); // sleep
}

void enablePullUp(GPIO_TypeDef *Port, uint32_t BitNumber)
{
        Port->PUPDR = Port->PUPDR &~(3u << BitNumber*2); // clear pull-up resistor bits
        Port->PUPDR = Port->PUPDR | (1u << BitNumber*2); // set pull-up bit
}
void pinMode(GPIO_TypeDef *Port, uint32_t BitNumber, uint32_t Mode)
{
        /*
        */
        uint32_t mode_value = Port->MODER;
        Mode = Mode << (2 * BitNumber);
        mode_value = mode_value & ~(3u << (BitNumber * 2));
        mode_value = mode_value | Mode;
        Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px, uint16_t py)
{
        // checks to see if point px,py is within the rectange defined by x,y,w,h
        uint16_t x2,y2;
        x2 = x1+w;
        y2 = y1+h;
        int rvalue = 0;
        if ( (px >= x1) && (px <= x2))
        {
                // ok, x constraint met
                if ( (py >= y1) && (py <= y2))
                rvalue = 1;
        }
        return rvalue;
}

void setupIO()
{
        RCC->AHBENR |= (1 << 18) + (1 << 17); // enable Ports A and B
        display_begin();
        pinMode(GPIOB,4,0);
        pinMode(GPIOB,5,0);
        pinMode(GPIOA,8,0);
        pinMode(GPIOA,11,0);
        enablePullUp(GPIOB,4);
        enablePullUp(GPIOB,5);
        enablePullUp(GPIOA,11);
        enablePullUp(GPIOA,8);
}
