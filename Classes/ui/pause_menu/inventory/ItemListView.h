#ifndef VIGILANTE_ITEM_LIST_VIEW_H_
#define VIGILANTE_ITEM_LIST_VIEW_H_

#include <deque>
#include <vector>
#include <memory>

#include "ui/ListView.h"
#include "item/Item.h"
#include "item/Equipment.h"

namespace vigilante {

class ItemListView : public ListView<Item*> {
 public:
  ItemListView(PauseMenu* pauseMenu, int visibleItemCount=5);
  virtual ~ItemListView() = default;

  virtual void confirm() override; // ListView<Item*>
  virtual void selectUp() override; // ListView<Item*>

  void showItemsByType(Item::Type itemType);
  void showEquipmentByType(Equipment::Type equipmentType);

  void fetchItems(Item::Type itemType);
  void fetchEquipment(Equipment::Type equipmentType);
};

} // namespace vigilante

#endif // VIGILANTE_ITEM_LIST_VIEW_H_
