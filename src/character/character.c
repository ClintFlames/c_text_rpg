#include "character.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../functions.h"
#include "../location/location.h"

void debug_character(struct Character *chr) {
  printf(
      "struct Character {\n"
      "  .name = %s\n"
      "  .gold = %u\n"
      "  .max_health = %u\n"
      "  .health = %u\n"
      "  .weapon = {\n"
      "    .name = %s\n"
      "    .price = %u\n"
      "    .upgrade_price = %u\n"
      "    .damage = %u\n"
      "  }\n"
      "}\n",
      chr->name, chr->gold, chr->max_health, chr->health, chr->weapon.name,
      chr->weapon.price, chr->weapon.upgrade_price, chr->weapon.damage);
}
void print_player_quest(struct Story *story) {
  bool completed = story->quest.target_count == story->quest.progress_count;

  if (story->quest.target_count == 0) return;

  printf(
      "\n-----< QUEST (%s) >-----\n"
      "Kill %s (%u/%u)\n"
      "Reward %ug\n",
      (completed ? "COMPLETED" : "NOT COMPLETED"), story->quest.target_name,
      story->quest.progress_count, story->quest.target_count,
      story->quest.reward_gold);
  if (completed) printf("Head to the Adventurer Guild for reward\n");
}
void print_player(struct Character *chr, struct Story *story) {
  printf(
      "\n-----< PLAYER STATUS >-----\n"
      "Character: %s (%ug %u/%uh)\n"
      "Weapon: %s (%up %uup %ud)\n",
      chr->name, chr->gold, chr->health, chr->max_health, chr->weapon.name,
      chr->weapon.price, chr->weapon.upgrade_price, chr->weapon.damage);

  print_player_quest(story);
}
void print_enemy(struct Character *chr, bool hideGold, bool hideWeaponDamage) {
  printf(
      "\n-----< ENEMY STATUS >-----\n"
      "Character: %s (",
      chr->name);
  if (hideGold)
    printf("?");
  else
    printf("%u", chr->gold);

  printf(
      "g %u/%uh)\n"
      "Weapon: %s (%up %uup ",
      chr->health, chr->max_health, chr->weapon.name, chr->weapon.price,
      chr->weapon.upgrade_price);

  if (hideWeaponDamage)
    printf("?");
  else
    printf("%u", chr->weapon.damage);

  printf("d)\n");
}

void battle_enemy(struct Story *story, struct Character *player,
                  struct Character *enemy) {
  bool hideEnemyGold = true;
  bool hideEnemyWeaponDamage = true;
  while (1) {
    printf(
        "\n-----< BATTLE ACTION >-----\n"
        "s) Player and enemy status (doesn't waste turn)\n"
        "a) Attack enemy\n"
        "r) Run away\n"
        "SELECT: ");

    switch (getchar_clear()) {
      case 's': {
        print_player(player, story);
        print_enemy(enemy, hideEnemyGold, hideEnemyWeaponDamage);
        break;
      }
      case 'a': {
        uint16_t player_new_health = player->health - enemy->weapon.damage;
        uint16_t enemy_new_health = enemy->health - player->weapon.damage;
        uint32_t winner_gold = player->gold + enemy->gold;

        // Prevent underflow and overflow
        if (player_new_health >= player->health) player_new_health = 0;
        if (enemy_new_health >= enemy->health) enemy_new_health = 0;
        if (winner_gold < player->gold || winner_gold > CHARACTER_MAX_GOLD)
          winner_gold = CHARACTER_MAX_GOLD;

        // After enemy attack, there is no sense in hiding enemy damage
        hideEnemyWeaponDamage = false;

        // Enemy turn
        player->health = player_new_health;

        printf(
            "\n-----< BATTLE ENEMY TURN >-----\n"
            "%s deals %u damage to %s using %s\n"
            "%s %u health left\n",
            enemy->name, enemy->weapon.damage, player->name, enemy->weapon.name,
            player->name, player->health);

        if (player->health == 0) {
          printf(
              "\n-----< BATTLE END >-----\n"
              "%s defeats %s using %s\n"
              "%s takes %u gold from %s\n",
              enemy->name, player->name, enemy->weapon.name, enemy->name,
              player->gold, player->name);

          enemy->gold = winner_gold;
          player->gold = 0;

          return;
        }

        // Player turn
        enemy->health = enemy_new_health;

        printf(
            "\n-----< BATTLE PLAYER TURN >-----\n"
            "%s deals %u damage to %s using %s\n"
            "%s %u health left\n",
            player->name, player->weapon.damage, enemy->name,
            player->weapon.name, enemy->name, enemy->health);

        if (enemy->health == 0) {
          printf(
              "\n-----< BATTLE END >-----\n"
              "%s defeats %s using %s\n"
              "%s takes %u gold from %s\n",
              player->name, enemy->name, player->weapon.name, player->name,
              enemy->gold, enemy->name);

          player->gold = winner_gold;
          enemy->gold = 0;

          if (strcmp(enemy->name, story->quest.target_name) == 0) {
            if (story->quest.progress_count < story->quest.target_count)
              story->quest.progress_count++;

            print_player_quest(story);
          }

          return;
        }

        break;
      }
      case 'r': {
        printf(
            "\n-----< BATTLE END >-----\n"
            "%s decided to run away\n",
            player->name);
        return;
        break;
      }
      default:
        printf("\n-----< BATTLE UNKNOWN ACTION >-----\n");
    }
  }
}

void fix_character(struct Character *chr) {
  if (chr->gold > CHARACTER_MAX_GOLD) chr->gold = CHARACTER_MAX_GOLD;
  if (chr->max_health > CHARACTER_MAX_HEALTH)
    chr->max_health = CHARACTER_MAX_HEALTH;
  if (chr->health > chr->max_health) chr->health = chr->max_health;
}
struct Character generate_player(char *name) {
  struct Character chr = {.weapon = {"Hands", 0, 5, 5}};

  strcpy(chr.name, name);
  chr.gold = RND_MAX(50);
  chr.max_health = 10;
  chr.health = RND_RANGE(10, 1);

  fix_character(&chr);

  return chr;
}
struct Character generate_enemy(uint8_t enemy_id) {
  struct Character chr = {
      .name = "Glitter Glitch",
      .gold = RND_MAX(UINT16_MAX),
      .max_health = chr.gold,
      .health = chr.gold,
      {.name = "Shining Death Star Glitch ", .damage = UINT16_MAX}};

  // Max 10 enemies per location
  switch (enemy_id) {
    // player_room
    case player_room * 10 + 0: {
      strcpy(chr.name, "Rat");
      chr.gold = RND_RANGE(5, 1);
      chr.max_health = 5;
      chr.health = RND_RANGE(5, 1);
      strcpy(chr.weapon.name, "Sharp Teeth");
      chr.weapon.damage = RND_RANGE(2, 1);
      break;
    }
    case player_room * 10 + 1: {
      strcpy(chr.name, "Spider");
      chr.gold = RND_RANGE(3, 1);
      chr.max_health = 4;
      chr.health = RND_RANGE(4, 2);
      strcpy(chr.weapon.name, "Venomous Fangs");
      chr.weapon.damage = RND_RANGE(2, 1);
      break;
    }
    // throne_room
    // demon_lord_castle

    // dead_forest
    // deep_forest
    // forest
    case forest * 10 + 0: {
      strcpy(chr.name, "Slime");
      chr.gold = RND_RANGE(10, 1);
      chr.max_health = 10;
      chr.health = RND_RANGE(10, 5);
      strcpy(chr.weapon.name, "Slimy Tentacle");
      chr.weapon.damage = RND_RANGE(3, 1);
      break;
    }
    case forest * 10 + 1: {
      strcpy(chr.name, "Wild Boar");
      chr.gold = RND_RANGE(10, 4);
      chr.max_health = 20;
      chr.health = RND_RANGE(20, 12);
      strcpy(chr.weapon.name, "Sharp Tusks");
      chr.weapon.damage = RND_RANGE(8, 6);
      break;
    }
    case forest * 10 + 2: {
      strcpy(chr.name, "Goblin");
      chr.gold = RND_RANGE(12, 6);
      chr.max_health = 18;
      chr.health = RND_RANGE(18, 10);
      strcpy(chr.weapon.name, "Rusty Knife");
      chr.weapon.damage = RND_RANGE(15, 6);
      break;
    }
    case forest * 10 + 3: {
      strcpy(chr.name, "Forest Bandit");
      chr.gold = RND_RANGE(20, 10);
      chr.max_health = 25;
      chr.health = RND_RANGE(25, 10);
      strcpy(chr.weapon.name, "Wooden Bow");
      chr.weapon.damage = RND_RANGE(10, 3);
      break;
    }

    // hidden_garden
    // high_mountain
    // mountain
    case mountain * 10 + 0: {
      strcpy(chr.name, "Mountain Goat");
      chr.gold = RND_RANGE(18, 8);
      chr.max_health = 26;
      chr.health = RND_RANGE(26, 14);
      strcpy(chr.weapon.name, "Horns");
      chr.weapon.damage = RND_RANGE(12, 8);
      break;
    }
    case mountain * 10 + 1: {
      strcpy(chr.name, "Rock Troll");
      chr.gold = RND_RANGE(30, 10);
      chr.max_health = 40;
      chr.health = RND_RANGE(40, 18);
      strcpy(chr.weapon.name, "Stone Club");
      chr.weapon.damage = RND_RANGE(16, 10);
      break;
    }

      // 90 - 99 city
      // tavern
      // blacksmith_shop
      // training_ground
      // adventurer_guild
  }

  fix_character(&chr);

  return chr;
}
