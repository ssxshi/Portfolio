#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <stdio.h>

extern int balance;
extern int current_bet;
extern int current_value;

int get_balance();
int get_bet();
int get_value();

void set_balance(int new_balance);
void set_bet(int new_bet);
void set_value(int new_value);

bool is_blackjack();
bool is_bust();

void show_data();

#endif