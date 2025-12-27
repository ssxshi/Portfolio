#ifndef BANK_H
#define BANK_H

#include <stdbool.h>

extern int bank_value;

void set_bank_value(int new_value);

int get_bank_value();

bool bank_bust();
bool bank_blackjack();

#endif