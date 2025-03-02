#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static const float ALIVE_TIME_CAP = 0.5f;

typedef enum {
  CARD_TODO,
  CARD_DONE,
} CardState;

typedef struct {
  char* text;
  CardState state;
  float alive_time;
} Card;

typedef struct {
  Card* cards;
  uint count;
  uint capacity;
} CardList;

void card_list_init(CardList* list) {
  const uint initial_capacity = 8;
  list->cards = (Card*)malloc(initial_capacity * sizeof(Card));
  list->count = 0;
  list->capacity = initial_capacity;
}

void card_list_free(CardList* list) {
  for(int i = 0; i < list->count; i++) {
    free(list->cards[i].text);
  }  
}

void card_list_add(CardList* list, const char* text, CardState state) {
  if(list->count >= list->capacity) {
    // Reallocate and increase capacity
    list->capacity = list->capacity * 2;
    list->cards = (Card*)realloc(list->cards, list->capacity * sizeof(Card));
  }

  list->cards[list->count].text = strdup(text);
  list->cards[list->count].state = state;
  list->cards[list->count].alive_time = 0.f;
  list->count++;
}

void card_list_remove(CardList* list, int index) {
  if(index < 0 || index >= list->count) {
    return;  
  }

  // Free the object
  free(list->cards[index].text); 

  // Shift elements left
  for(int i = index; i < list->count - 1; i++) {
    list->cards[i] = list->cards[i + 1];
  }

  list->count--;
}

void card_list_move(CardList* list, int index, CardState new_state) {
  if(index >= 0 && index < list->count) {
    list->cards[index].state = new_state;
  }
}

Card* card_list_get(CardList* list, int index) {
  if(index < 0 || index >= list->count) {
    printf("Trying to access CardList out of range, index = %d", index);
    return NULL;
  }

  return list->cards + index;
}

// Call this every frame to update internal state of cards
void card_list_update(CardList* card_list, float delta_time) {
  for(int i = 0; i < card_list->count; i++) 
  {
    Card* card = card_list_get(card_list, i);
    card->alive_time += delta_time;
    if(card->alive_time > ALIVE_TIME_CAP) 
    {
      card->alive_time = ALIVE_TIME_CAP;
    }
  }
}
