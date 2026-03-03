#include <stdint.h>
#include <stm32f031x6.h>
#include "musical_notes.h"

void delay(volatile uint32_t dly);
void playNote(uint32_t freq);

static void stopSound(void)
{
    TIM14->CR1 &= ~1;
}

void twinkle_twinkle(void)
{
    playNote(C4); delay(250);
    playNote(C4); delay(250);
    playNote(G4); delay(250);
    playNote(G4); delay(250);
    playNote(A4); delay(250);
    playNote(A4); delay(250);
    playNote(G4); delay(500);

    playNote(F4); delay(250);
    playNote(F4); delay(250);
    playNote(E4); delay(250);
    playNote(E4); delay(250);
    playNote(D4); delay(250);
    playNote(D4); delay(250);
    playNote(C4); delay(500);

    playNote(G4); delay(250);
    playNote(G4); delay(250);
    playNote(F4); delay(250);
    playNote(F4); delay(250);
    playNote(E4); delay(250);
    playNote(E4); delay(250);
    playNote(D4); delay(500);

    playNote(G4); delay(250);
    playNote(G4); delay(250);
    playNote(F4); delay(250);
    playNote(F4); delay(250);
    playNote(E4); delay(250);
    playNote(E4); delay(250);
    playNote(D4); delay(500);

    playNote(C4); delay(250);
    playNote(C4); delay(250);
    playNote(G4); delay(250);
    playNote(G4); delay(250);
    playNote(A4); delay(250);
    playNote(A4); delay(250);
    playNote(G4); delay(500);

    playNote(F4); delay(250);
    playNote(F4); delay(250);
    playNote(E4); delay(250);
    playNote(E4); delay(250);
    playNote(D4); delay(250);
    playNote(D4); delay(250);
    playNote(C4); delay(600);

    stopSound();
}