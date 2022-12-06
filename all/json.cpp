/*
 * Copyright 2014 Google Inc. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include "flatbuffers/flatbuffers.h"
#include "flatbuffers/util.cpp"
#include "flatbuffers/idl_parser.cpp"
#include "flatbuffers/idl_gen_text.cpp"


#include <iostream>
#include <fstream>




#include "monster_generated.h"  // Already includes "flatbuffers/flatbuffers.h".

using namespace MyGame::Sample;

// This is an example of parsing text straight into a buffer and then
// generating flatbuffer (JSON) text from the buffer.
int main(int /*argc*/, const char * /*argv*/[]) {
  // load FlatBuffer schema (.fbs) and JSON from disk
  std::string schemafile;
  std::string jsonfile;

  flatbuffers::FlatBufferBuilder builder(1024);
  
  bool ok = flatbuffers::LoadFile("all/monster.fbs", false, &schemafile);
  std::cout<<ok<<std::endl;
    if (!ok) {
    printf("couldn't load files!\n");
    return 1;
  }

   auto weapon_one_name = builder.CreateString("Sword");
    short weapon_one_damage = 3;
    
    auto weapon_two_name = builder.CreateString("Axe");
    short weapon_two_damage = 5;
    
    // Use the `CreateWeapon` shortcut to create Weapons with all the fields set.
    auto sword = CreateWeapon(builder, weapon_one_name, weapon_one_damage);
    auto axe = CreateWeapon(builder, weapon_two_name, weapon_two_damage); 

    // Serialize a name for our monster, called "Orc".
auto name = builder.CreateString("Orc");
 
// Create a `vector` representing the inventory of the Orc. Each number
// could correspond to an item that can be claimed after he is slain.
unsigned char treasure[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
auto inventory = builder.CreateVector(treasure, 10);

// Place the weapons into a `std::vector`, then convert that into a FlatBuffer `vector`.
std::vector<flatbuffers::Offset<Weapon>> weapons_vector;
weapons_vector.push_back(sword);
weapons_vector.push_back(axe);
auto weapons = builder.CreateVector(weapons_vector);

Vec3 points[] = { Vec3(1.0f, 2.0f, 3.0f), Vec3(4.0f, 5.0f, 6.0f) };
auto path = builder.CreateVectorOfStructs(points, 2);


// Create the position struct
auto position = Vec3(1.0f, 2.0f, 3.0f);
 
// Set his hit points to 300 and his mana to 150.
int hp = 320;
int mana = 160;
 
// Finally, create the monster using the `CreateMonster` helper function
// to set all fields.
auto orc = CreateMonster(builder, &position, mana, hp, name, inventory,
                        Color_Red, weapons, Equipment_Weapon, axe.Union(),
                        path);


builder.Finish(orc); // You could also call `FinishMonsterBuffer(builder, orc);`.

char* buffer = (char *)builder.GetBufferPointer();

  flatbuffers::Parser parser;
  bool ok1 = parser.Parse(schemafile.c_str());
  GenerateText(parser, builder.GetBufferPointer(), &jsonfile);
 // printf("%s", jsonfile.c_str());

  std::cout<<ok1<<std::endl;

   std::ofstream myfile;
  myfile.open ("example.json");
  myfile << jsonfile;
  myfile.close();



 MonsterT monsterObj;
 GetMonster(builder.GetBufferPointer())->UnPackTo(&monsterObj);
 std::cout << monsterObj.name<<std::endl;

 monsterObj.name = "bob";
 builder.Finish(Monster::Pack(builder,&monsterObj));

 uint8_t *buffer2 = builder.GetBufferPointer();
 auto monster1 = GetMonster(buffer2);
  std::cout << monster1->name()->c_str()<<std::endl;




  printf("The FlatBuffer has been parsed from JSON successfully.\n");
}