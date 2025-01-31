#ifndef WEAPON_H
#define WEAPON_H

#include <stdint.h>

#define WEAPON_NAME_LENGTH 30
#define WEAPON_MAX_DAMAGE 50001

struct Weapon {
  char name[WEAPON_NAME_LENGTH];
  uint32_t price;
  uint32_t upgrade_price;
  uint16_t damage;
};

struct Weapon generate_weapon(void);
void upgrade_weapon(struct Weapon* weapon);

#endif  // !WEAPON_H
