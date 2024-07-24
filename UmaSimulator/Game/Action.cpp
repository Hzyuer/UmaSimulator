#include "Action.h"
const Action Action::Action_RedistributeCardsForTest = { 0, TRA_redistributeCardsForTest };

const std::string Action::trainingName[8] =
{
  "��",
  "��",
  "��",
  "��",
  "��",
  "��Ϣ",
  "���",
  "����"
};
const std::string Action::dishName[14] =
{
  "��",
  "������",
  "���",
  "С��",
  "С��",
  "С��",
  "С��",
  "С��",
  "����",
  "����",
  "����",
  "���",
  "����",
  "G1Plate"
};
bool Action::isActionStandard() const
{
  if (train >= 0 && train < 8)
  {
    if (dishType == DISH_none)
      return true;
    else
      return false;
  }
  else if (train == TRA_none)
  {
    if (dishType != DISH_none)
      return true;
    else
      return false;
  }
  return false;
}

int Action::toInt() const
{
  if (train >= 0 && train < 8)
  {
    if (dishType == DISH_none)
      return train;
    else
    {
      throw "Action::toInt(): Not standard Action, both dish and training";
      return -1;
    }
  }
  else if (train == TRA_none)
  {
    if (dishType != DISH_none)
    {
      return 8 + dishType - 1;
    }
    else
    {
      throw "Action::toInt(): Not standard Action, no dish and training";
      return -1;
    }
  }
  throw "Action::toInt(): Not standard Action, special training type";
  return -1;
}

Action Action::intToAction(int i)
{
  Action a;
  a.train = TRA_none;
  a.dishType = DISH_none;
  if (i > 0 && i < 8)
    a.train = i;
  else if (i < 21)
    a.dishType = i - 8 + 1;
  else
    throw "Action::intToAction(): Invalid int";
  return a;
}

std::string Action::toString() const
{
  if (train >= 0 && train < 8)
  {
    if (dishType == DISH_none)
      return "������+" + trainingName[train];
    else
      return dishName[dishType] + "+" + trainingName[train];
  }
  else if (train == TRA_none)
  {
    if (dishType != DISH_none)
      return "������:" + dishName[dishType];
    else
      return "ʲô������";
  }
  throw "Action::toString(): Unknown Action";
  return "";
}
