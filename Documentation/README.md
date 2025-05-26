## Characters
<img src="/Documentation/images/character.jpg">


requiem's **[Character](https://github.com/aesophor/requiem/blob/master/Classes/character/Character.h)** has the following concrete subclasses:
* **[Player](https://github.com/aesophor/requiem/blob/master/Classes/character/Player.h)**
* **[Enemy](https://github.com/aesophor/requiem/blob/master/Classes/character/Enemy.h)**
* **[Npc](https://github.com/aesophor/requiem/blob/master/Classes/character/Npc.h)**

It is probably one of the most complicated class in this game. Let's have a look at some Character's important methods:
```cpp
virtual void moveLeft();
virtual void moveRight();
virtual void jump();
virtual void jumpDown();
virtual void crouch();
virtual void getUp();

virtual void sheathWeapon();
virtual void unsheathWeapon();
virtual void attack();
virtual void activateSkill(Skill* skill);
virtual void knockBack(Character* target, float forceX, float forceY) const;
virtual void inflictDamage(Character* target, int damage);
virtual void receiveDamage(Character* source, int damage);
virtual void lockOn(Character* target);

virtual void addItem(Item* item, int amount=1);
virtual void removeItem(Item* item, int amount=1);
virtual void useItem(Consumable* consumable);
virtual void equip(Equipment* equipment);
virtual void unequip(Equipment::Type equipmentType);
virtual void pickupItem(Item* item);
virtual void discardItem(Item* item, int amount);
virtual void interact(Interactable* target);
```
<br>

## Items and Equipment
<img src="/Documentation/images/item.png">

requiem's **[Item](https://github.com/aesophor/requiem/blob/master/Classes/item/Item.h)** has the following concrete subclasses:
* **[Equipment](https://github.com/aesophor/requiem/blob/master/Classes/item/Equipment.h)** (e.g., leather armor, leather boots, short sword)
* **[Consumable](https://github.com/aesophor/requiem/blob/master/Classes/item/Consumable.h)** (e.g., potions)
* **[Misc Item](https://github.com/aesophor/requiem/blob/master/Classes/item/MiscItem.h)** (e.g., gold coins)

Items can be acquired by opening a chest or looting from enemies. Also, the items equipped **will be reflected on player's appearance**! 

<br>

## Inventory and Equipment
<img src="/Documentation/images/inventory_equipment.gif">

Items and equipment can be accessed via **[PauseMenu](https://github.com/aesophor/requiem/blob/master/Classes/ui/pause_menu/PauseMenu.h)**. Depending on the item's type, there are different actions to perform:
* Equipment - **equip**, discard, cancel
* Consumable - **use**, discard, cancel
* Misc Item - discard, cancel

> Some UI classes worth looking at:
> 
> [1] [ListView](https://github.com/aesophor/requiem/blob/master/Classes/ui/ListView.h) - ItemListView, SkillListView, DialogueListView...    
> [2] [TabView](https://github.com/aesophor/requiem/blob/master/Classes/ui/TabView.h)    
> [3] [PauseMenuDialog](https://github.com/aesophor/requiem/blob/master/Classes/ui/pause_menu/PauseMenuDialog.h)    

<br>

## Combat and Loot
<img src="/Documentation/images/combat.gif">

requiem's **Combat System** mainly consists of the following:
* **melee regular attack** (e.g., sword, axe)
* **distant regular attack**  (e.g., bow, staff) -- not implemented
* **special skills** (e.g., ice spike, forward slash, back dash)

When an enemy dies, it will give **exp point** to the character who kills it (could be the Player or an ally NPC), and then drop some items (defined in enemy's json file). See [`Enemy::receiveDamage`]([https://github.com/aesophor/requiem/blob/master/Classes/character/Enemy.cc#L87](https://github.com/aesophor/requiem/blob/master/Classes/character/Enemy.cc#L87)).

<br>

## Skills and Hotkeys
<img src="/Documentation/images/skill.png">

requiem's **[Skill](https://github.com/aesophor/requiem/blob/master/Classes/item/Item.h)** has the following concrete subclasses:
* **[Magical Missile](https://github.com/aesophor/requiem/blob/master/Classes/skill/MagicalMissile.h)** - e.g., ice spike. Fireball can use this class too
* **[Forward Slash](https://github.com/aesophor/requiem/blob/master/Classes/skill/ForwardSlash.h)** - rushes forward an slash the enemies contacted
* **[Back Dash](https://github.com/aesophor/requiem/blob/master/Classes/skill/BackDash.h)** - castlevania like back dash
* more skills will be added

The **[Hotkey Manager](https://github.com/aesophor/requiem/blob/master/Classes/input/HotkeyManager.h)** allows you to bind **Skills and Consumables** to a certain keystroke so that the gameplay can have less interruption.

<br>

## Quests, Dialogues and Story
<img src="/Documentation/images/dialogue_quest.gif">

#### Quests
In requiem, a [Quest](https://github.com/aesophor/requiem/tree/master/Classes/quest) usually contains multiple [Objectives](https://github.com/aesophor/requiem/blob/master/Classes/quest/Quest.h#L19), where an objective can be one of the following:
* [Kill](https://github.com/aesophor/requiem/blob/master/Classes/quest/KillTargetObjective.h)
* [Collect](https://github.com/aesophor/requiem/blob/master/Classes/quest/CollectItemObjective.h)
* Escort (not added)
* Delivery (not added)
* Talk to (not added)
* ...

By completing the current objective, the quest will move on to the next objective. When all objectives are done, then the entire quest is finished.

#### Dialogues
[DialogueTrees](https://github.com/aesophor/requiem/blob/master/Classes/gameplay/DialogueTree.h) are deserialized from json and then converted into a tree using DFS iteratively (via std::stack). See [`DialogueTree::import`](https://github.com/aesophor/requiem/blob/master/Classes/gameplay/DialogueTree.cc#L39).

Note that **certain lines of Dialogue can invoke special events**, such as starting a quest. This is achieved with the help of  `Console::executeCmd`.

#### Story
Now that we have quests and dialogue system, the story can be created!

<br>

## Game UI
The UI classes can be located in [Classes/ui/](https://github.com/aesophor/requiem/tree/master/Classes/ui/). It consists of the following subsystems:
* HUD
* Dialogue (Subtitles and DialogueMenu)
* Console (Let user or other subsystems execute commands)
* Notifications
* Floating Damages
* Quest Hints
* PauseMenu (Inventory UI, Equipment UI, Skills UI ...)

#### Console
Here I will talk a little bit about Console since it is probably the most important subsystem in requiem. **It acts as the interface between quest and dialogue subsystems.**

For now, there are only a few commands available, but more of them will be added! See [CommandParser.cc](https://github.com/aesophor/requiem/blob/master/Classes/ui/console/CommandParser.cc) for implementation details.
* startquest \<quest_json\>
* additem \<item_json\> [amount]
* removeitem \<item_json\> [amount]
* ...
