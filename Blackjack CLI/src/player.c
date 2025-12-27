#include "../include/player.h"

int balance = 10000;
int current_bet = 0;
int current_value = 0;

int get_balance(){
    return balance;
}
int get_bet(){
    return current_bet;
}
int get_value(){
    return current_value;
}

void set_balance(int new_balance){
    balance = new_balance;
}
void set_bet(int new_bet){
    current_bet = new_bet;
}
void set_value(int new_value){
    current_value = new_value;
}

bool is_blackjack(){
    return (current_value == 21);
}
bool is_bust(){
    return (current_value > 21);
}

void show_data(){
    printf("Current Player Balance is [ $%d ]\n", balance);
}