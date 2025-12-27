#include "../include/bank.h"

int bank_value = 0;

void set_bank_value(int new_value){
    bank_value = new_value;
}

int get_bank_value(){
    return bank_value;
}

bool bank_bust(){
    return (bank_value > 21);
}
bool bank_blackjack(){
    return (bank_value == 21);
}