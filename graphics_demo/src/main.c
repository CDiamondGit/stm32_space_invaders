
#include <stm32f031x6.h>

#include "display.h"

void initClock(void);
void initSysTick(void);
void SysTick_Handler(void);
void delay(volatile uint32_t dly);
void setupIO();
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px,
             uint16_t py);
void enablePullUp(GPIO_TypeDef* Port, uint32_t BitNumber);
void pinMode(GPIO_TypeDef* Port, uint32_t BitNumber, uint32_t Mode);

enum bulletState { FIRE, READY };

int bulletState = READY;

typedef struct {
  uint16_t x;
  uint16_t y;
  uint16_t oldX;
  uint16_t oldY;
} Transform;

typedef struct {
  Transform coords;
  uint16_t shipSpeed;
} Ship;

typedef struct {
  Transform coords;
  uint8_t bulletState;
  uint16_t bulletSpeed;
} Bullet;

typedef struct {
  Ship ship;
  Bullet bullet;
} GameState;

void render_bullet(Bullet*);
// void initGameState(GameState* gs);

volatile uint32_t milliseconds;

const uint16_t spaceShip[] = {
    0,     0,     0,     0,     0,     34882, 34882, 0,     0,     0,     0,
    0,     0,     0,     0,     0,     34882, 37012, 37012, 54701, 0,     0,
    0,     0,     0,     0,     0,     0,     12692, 33585, 33585, 28820, 0,
    0,     0,     0,     0,     0,     0,     9521,  41769, 61181, 61181, 41769,
    42818, 0,     0,     0,     0,     0,     0,     10058, 49961, 18138, 18138,
    41769, 52595, 0,     0,     0,     0,     0,     21933, 52067, 49961, 18138,
    18138, 49961, 28292, 21933, 0,     0,     63414, 28820, 49961, 28027, 41769,
    41769, 41769, 41769, 19299, 41769, 37012, 63670, 35154, 37012, 49961, 28027,
    28820, 37012, 37012, 37012, 19299, 41769, 37012, 35154, 0,     35154, 41769,
    27755, 37012, 35154, 35154, 37012, 28292, 41769, 35154, 0,     0,     0,
    34361, 34361, 34369, 47302, 47302, 34882, 34361, 34361, 0,     0,     0,
    0,     0,     0,     34369, 34882, 33577, 34882, 0,     0,     0,     0,
    0,     0,     0,     0,     0,     45196, 21933, 0,     0,     0,     0,
    0,
};

void initGameState(GameState* gs) {
  gs->ship.coords.x = 58;
  gs->ship.coords.y = 130;
  gs->ship.coords.oldX = 58;
  gs->ship.coords.oldY = 130;
  gs->ship.shipSpeed = 10;

  gs->bullet.coords.x = gs->ship.coords.x + 6;
  gs->bullet.coords.y = gs->ship.coords.y + 6;
  gs->bullet.coords.oldX = gs->ship.coords.x + 6;
  gs->bullet.coords.oldY = gs->ship.coords.y + 6;
  gs->bullet.bulletSpeed = 4;
  gs->bullet.bulletState = READY;
}

GameState gs;
uint32_t lastUpdate = 0;
uint32_t frameDelay = 16;

int main() {
  initClock();
  initSysTick();
  setupIO();
  initGameState(&gs);

  drawLine(0, 144, 128, 144, 57351);

  while (1) {
    uint32_t now = milliseconds;

    if (now - lastUpdate >= frameDelay) {
      lastUpdate = now;

      // if right button is pressed, move ship to right
      if ((GPIOB->IDR & (1 << 4)) == 0) {
        if (gs.ship.coords.x < (128 - 16)) {
          gs.ship.coords.x += gs.ship.shipSpeed;
          printNumber(gs.ship.coords.x, 10, 100, 1, 0);
          delay(20);
        }
      }

      // if left button is pressed move ship to left;
      if ((GPIOB->IDR & (1 << 5)) == 0) {
        if (gs.ship.coords.x > 12) {
          gs.ship.coords.x -= gs.ship.shipSpeed;
          printNumber(gs.ship.coords.x, 10, 100, 1, 0);
          delay(20);
        }
      }

      // if up button is pressed render bullet;
      if ((GPIOA->IDR & (1 << 8)) == 0) {
        if (gs.bullet.coords.y > 2 && gs.bullet.bulletState == READY) {
          gs.bullet.bulletState = FIRE;
          render_bullet(&gs.bullet);
        }
      }

      // if ship position changed, then render ship
      if ((gs.ship.coords.oldX != gs.ship.coords.x) ||
          gs.ship.coords.oldY != gs.ship.coords.y) {
        fillRectangle(gs.ship.coords.oldX, gs.ship.coords.oldY, 12, 12, 0);
        gs.ship.coords.oldX = gs.ship.coords.x;
        gs.ship.coords.oldY = gs.ship.coords.y;
        putImage(gs.ship.coords.x, gs.ship.coords.y, 12, 12, spaceShip, 1, 0);
        if (gs.bullet.bulletState != FIRE) {
        }
      }

      // if bullet fired, render bullet;
      if (gs.bullet.bulletState == FIRE) {
        render_bullet(&gs.bullet);
        gs.bullet.coords.y -= gs.bullet.bulletSpeed;

        if (gs.bullet.coords.y < 5) {
          fillRectangle(gs.bullet.coords.oldX, gs.bullet.coords.oldY, 2, 2, 0);
          gs.bullet.coords.x = gs.bullet.coords.oldX = gs.ship.coords.x;
          gs.bullet.coords.y = gs.bullet.coords.oldY = gs.ship.coords.y + 6;
          gs.bullet.bulletState = READY;
        }
        render_bullet(&gs.bullet);
      } else {
        gs.bullet.coords.x = gs.ship.coords.x + 6;
        gs.bullet.coords.y = gs.ship.coords.y + 6;
      }
      printNumber(gs.bullet.bulletState, 10, 10, 1, 0);
      delay(50);
    }
  }
  return 0;
}

void render_bullet(Bullet* b) {
  fillRectangle(b->coords.oldX, b->coords.oldY, 2, 2, 0);
  b->coords.oldY = b->coords.y;
  b->coords.oldY = b->coords.y;

  drawCircle(b->coords.x, b->coords.y, 1, 65287);
}

void initSysTick(void) {
  SysTick->LOAD = 48000;
  SysTick->CTRL = 7;
  SysTick->VAL = 10;
  __asm(" cpsie i ");  // enable interrupts
}
void SysTick_Handler(void) { milliseconds++; }
void initClock(void) {
  // This is potentially a dangerous function as it could
  // result in a system with an invalid clock signal - result: a stuck system
  // Set the PLL up
  // First ensure PLL is disabled
  RCC->CR &= ~(1u << 24);
  while ((RCC->CR & (1 << 25)));  // wait for PLL ready to be cleared

  // Warning here: if system clock is greater than 24MHz then wait-state(s) need
  // to be inserted into Flash memory interface

  FLASH->ACR |= (1 << 0);
  FLASH->ACR &= ~((1u << 2) | (1u << 1));
  // Turn on FLASH prefetch buffer
  FLASH->ACR |= (1 << 4);
  // set PLL multiplier to 12 (yielding 48MHz)
  RCC->CFGR &= ~((1u << 21) | (1u << 20) | (1u << 19) | (1u << 18));
  RCC->CFGR |= ((1 << 21) | (1 << 19));

  // Need to limit ADC clock to below 14MHz so will change ADC prescaler to 4
  RCC->CFGR |= (1 << 14);

  // and turn the PLL back on again
  RCC->CR |= (1 << 24);
  // set PLL as system clock source
  RCC->CFGR |= (1 << 1);
}
void delay(volatile uint32_t dly) {
  uint32_t end_time = dly + milliseconds;
  while (milliseconds != end_time) __asm(" wfi ");  // sleep
}

void enablePullUp(GPIO_TypeDef* Port, uint32_t BitNumber) {
  Port->PUPDR =
      Port->PUPDR & ~(3u << BitNumber * 2);  // clear pull-up resistor bits
  Port->PUPDR = Port->PUPDR | (1u << BitNumber * 2);  // set pull-up bit
}
void pinMode(GPIO_TypeDef* Port, uint32_t BitNumber, uint32_t Mode) {
  /*
   */
  uint32_t mode_value = Port->MODER;
  Mode = Mode << (2 * BitNumber);
  mode_value = mode_value & ~(3u << (BitNumber * 2));
  mode_value = mode_value | Mode;
  Port->MODER = mode_value;
}
int isInside(uint16_t x1, uint16_t y1, uint16_t w, uint16_t h, uint16_t px,
             uint16_t py) {
  // checks to see if point px,py is within the rectange defined by x,y,w,h
  uint16_t x2, y2;
  x2 = x1 + w;
  y2 = y1 + h;
  int rvalue = 0;
  if ((px >= x1) && (px <= x2)) {
    // ok, x constraint met
    if ((py >= y1) && (py <= y2)) rvalue = 1;
  }
  return rvalue;
}

void setupIO() {
  RCC->AHBENR |= (1 << 18) + (1 << 17);  // enable Ports A and B
  display_begin();
  pinMode(GPIOB, 4, 0);   // right button -> blue wire-> PB4
  pinMode(GPIOB, 5, 0);   // left button -> green wire-> PB5
  pinMode(GPIOA, 8, 0);   // up button -> yellow wire -> PA8
  pinMode(GPIOA, 11, 0);  // down button -> purple wire => Pa8
  enablePullUp(GPIOB, 4);
  enablePullUp(GPIOB, 5);
  enablePullUp(GPIOA, 11);
  enablePullUp(GPIOA, 8);
}
