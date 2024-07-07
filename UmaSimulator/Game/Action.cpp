#include "Action.h"

const int Action::XiangtanFromColor[10] = { -1,0,0,1,1,2,2,-1,-1,-1 };
const int Action::XiangtanToColor[10] = { -1,1,2,0,2,0,1,0,1,2 };
const int Action::XiangtanNumCost[10] = { 0,1,1,1,1,1,1,2,2,2 };
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

int Action::toInt() const
{
  if (train < 5)return xiangtanType * 5 + train;
  if (train == TRA_rest)return 50;
  if (train == TRA_outgoing)return 51;
  if (train == TRA_race)return 52;
  return -1;
}

Action Action::intToAction(int i)
{
  Action a = { 0,0 };
  if (i == 50)a.train = TRA_rest;
  else if (i == 51)a.train = TRA_outgoing;
  else if (i == 52)a.train = TRA_race;
  else
  {
    a.train = i % 5;
    a.xiangtanType = i / 5;
  }
  return a;
}

std::string Action::toString() const
{
  return trainingName[train] + " ��̸:" + xiangtanName[xiangtanType];
}
