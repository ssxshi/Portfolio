#include "../include/game.h"
#include "../include/player.h"
#include "../include/bank.h"
#include "../include/utils.h"

typedef struct{
    char key;
    int value;
}Card;

int bet_options[5] = {
    10, 25, 50, 100, 250
};
char face_values[13] = "A23456789TJQK";
Card cards[13] = {};

int get_face_value(char face){
    switch (toupper(face)){
        case 'A':
            return 11;
        case '2'...'9':
            return face - '0';
        case 'T':
        case 'J':
        case 'Q':
        case 'K':
            return 10;
        default:
            return 0;
    }
}

Card* get_card(char key) {
    for (int i = 0; i < 13; i++) {
        if (cards[i].key == key) return &cards[i];
    }
    return NULL;
}

void make_cards(){
    for (int i = 0; i < 13; i++){
        cards[i].key = face_values[i];
        cards[i].value = 0;
    }
}

void reset_deck(){
    for (int i = 0; i < 13; i++){
        cards[i].value = 0;
    }
}

int adjust_for_aces(int total, int ace_count){
    while (total > 21 && ace_count > 0){
        total -= 10;
        ace_count--;
    }
    return total;
}

void start_menu(){
    system("cls");

    printf("***** Welcome to Blackjack! *****\n");
    printf("Press [Enter] to start!\n");
    
    while (1){
        if (!kbhit()){continue;}
        if (getch() == 13){break;}
    }

    system("cls");
}

void show_bet_options(){
    printf("\nSelect your bet amount.\n");
    for (int i = 0; i < 5; i++){
        printf("%d] $%d\n", i + 1, bet_options[i]);
    }
}

void get_bet_option(int* option){
    scanf("%d", option);

    if (*option < 1 || *option > 5){
        printf("Invalid option. Try again!\n");
        get_bet_option(option);
    }
}

Card* get_valid(){
    while (1){
        int idx = rand_range(0, 12);
        if (cards[idx].value < 4){
            cards[idx].value++;
            return &cards[idx];
        }
    }
}

void deal(int* player_aces, int* dealer_aces){
    system("cls");
    printf("Dealing cards!...\n");
    
    set_value(0);
    set_bank_value(0);
    
    *player_aces = 0;
    *dealer_aces = 0;
    char dealer_first_card = '\0';

    get_valid();

    for (int i = 0; i < 2; i++){
        Card* player_card = get_valid();
        int card_value = get_face_value(player_card->key);
        if (player_card->key == 'A') (*player_aces)++;
        set_value(get_value() + card_value);
        
        Card* dealer_card = get_valid();
        card_value = get_face_value(dealer_card->key);
        if (dealer_card->key == 'A') (*dealer_aces)++;
        if (i == 0) dealer_first_card = dealer_card->key;
        set_bank_value(get_bank_value() + card_value);
    }
    
    set_value(adjust_for_aces(get_value(), *player_aces));
    set_bank_value(adjust_for_aces(get_bank_value(), *dealer_aces));

    printf("\nDealer shows: %c\n", dealer_first_card);
    printf("Player Value: %d\n", get_value());
}

void play_round(){
    
    int dealer_aces = 0;
    int player_aces = 0;
    deal(&player_aces, &dealer_aces);
    
    if (get_value() == 21){
        printf("\nBLACKJACK! You win!\n");
        set_balance(get_balance() + (get_bet() * 2) + (get_bet() / 2));
        return;
    }
    
    while (get_value() < 21){
        printf("\n[H]it or [S]tand? ");
        char choice = getch();
        choice = toupper(choice);
        
        if (choice == 'S'){
            break;
        } else if (choice == 'H'){
            Card* new_card = get_valid();
            int card_value = get_face_value(new_card->key);
            
            if (new_card->key == 'A'){
                player_aces++;
            }
            
            set_value(get_value() + card_value);
            
            if (get_value() > 21 && player_aces > 0){
                set_value(adjust_for_aces(get_value(), player_aces));
                player_aces = 0;
            }
            
            printf("\nYou drew: %c\n", new_card->key);
            printf("Player Value: %d\n", get_value());
        }
    }
    
    if (get_value() > 21){
        printf("\nBUST! You lose!\n");
        printf("Dealer had: %d\n", get_bank_value());
        return;
    }
    
    printf("\nDealer reveals: %d\n", get_bank_value());

    while (get_bank_value() < 17){
        Card* dealer_card = get_valid();
        int card_value = get_face_value(dealer_card->key);
        set_bank_value(get_bank_value() + card_value);
        
        if (get_bank_value() > 21 && dealer_aces > 0){
            set_bank_value(adjust_for_aces(get_bank_value(), dealer_aces));
            dealer_aces = 0;
        }
        
        printf("Dealer drew: %c (Total: %d)\n", dealer_card->key, get_bank_value());
        sleep(1);
    }
    
    printf("\nFinal - Player: %d, Dealer: %d\n", get_value(), get_bank_value());
    
    if (get_bank_value() > 21){
        printf("Dealer BUSTS! You win!\n");
        set_balance(get_balance() + (get_bet() * 2));
    } else if (get_value() > get_bank_value()){
        printf("You win!\n");
        set_balance(get_balance() + (get_bet() * 2));
    } else if (get_value() < get_bank_value()){
        printf("Dealer wins!\n");
    } else {
        printf("Push! (Tie)\n");
        set_balance(get_balance() + get_bet());
    }
}

void start(){
    make_cards();
    srand((unsigned int)time(NULL));

    start_menu();
    
    while(get_balance() > 0){
        system("cls");
        show_data();
        show_bet_options();

        int option = 0;
        get_bet_option(&option);
        option -= 1;

        int bet_amount = bet_options[option];
        
        if (bet_amount > get_balance()){
            printf("Insufficient balance! Choose a lower bet.\n");
            sleep(2);
            continue;
        }
        
        set_bet(bet_amount);
        set_balance(get_balance() - get_bet());
        
        printf("\nPlayer bet $%d\n", get_bet());
        show_data();
        
        reset_deck();
        play_round();
        
        printf("\nCurrent Balance: $%d\n", get_balance());
        
        if (get_balance() <= 0){
            printf("\nYou're out of money! Game Over.\n");
            break;
        }
        
        printf("\nPress [Enter] to continue or [Q] to quit...");
        while (1){
            if (!kbhit()){continue;}
            char ch = getch();
            if (ch == 13){break;}
            if (toupper(ch) == 'Q'){
                printf("\n\nThanks for playing!\n");
                return;
            }
        }
    }
    
    printf("\nGame Over! Final Balance: $%d\n", get_balance());
}