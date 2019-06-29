#ifndef VIGILANTE_SKILL_LIST_VIEW_H_
#define VIGILANTE_SKILL_LIST_VIEW_H_

#include "ui/ListView.h"
#include "skill/Skill.h"

namespace vigilante {

class SkillListView : public ListView<Skill*> {
 public:
  SkillListView(PauseMenu* pauseMenu, int visibleItemCount=5);
  virtual ~SkillListView() = default;

  virtual void confirm() override; // ListView<Skill*>

  void showSkills();
};

} // namespace vigilante

#endif // VIGILANTE_SKILL_LIST_VIEW_H_
