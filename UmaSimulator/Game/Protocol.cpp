#include <iostream>
#include <sstream>
#include <cassert>
#include "vector"
#include "../External/json.hpp"
#include "Protocol.h"
#include "Game.h"
using namespace std;
using json = nlohmann::json;
// �Ƿ�ѵ��ƿ��������ƴ����ᵼ��һ����Ԥ��ƫ�
// ΪTrueʱ�������ID�����λ��Ϊ���ƣ�����5xxx����4xxx��
static bool maskUmaId = true;

int mask_umaId(int umaId)
{
    return umaId % 1000000;
}

bool Game::loadGameFromJson(std::string jsonStr)
{
  if (jsonStr == "[test]" || jsonStr == "{\"Result\":1,\"Reason\":null}")
  {
    std::cout << "�ѳɹ���URA�������ӣ�����δ���յ��غ���Ϣ���ȴ���Ϸ��ʼ" << std::endl;
    return false;
  }
  try
  {
    json j = json::parse(jsonStr, nullptr, true, true);
    auto rand = mt19937_64(114514);
    int newcards[6];
    int newzmbluecount[5];
    for (int i = 0; i < 6; i++) {
      newcards[i] = j["cardId"][i];
      if (i < 5) { newzmbluecount[i] = j["zhongMaBlueCount"][i]; }

    }
    //int zhongmaBlue[5] = { 18,0,0,0,0 };
    int zhongmaBonus[6] = { 10,10,30,0,10,70 };
    newGame(rand, true, j["umaId"], j["umaStar"], newcards, newzmbluecount, zhongmaBonus);

    ptScoreRate = j.contains("ptScoreRate") ? double(j["ptScoreRate"]) : GameConstants::ScorePtRateDefault;

    turn = j["turn"];
    gameStage = j["gameStage"];
    if (gameStage == GameStage_beforeMechaUpgrade)
    {
      if (turn == 2)turn -= 1; //���������׻غ���Ϊ��һ���غϽ�β
      if (turn != 1 && turn != 23 && turn != 35 && turn != 47 && turn != 59 && turn != 71)
        throw "Game::loadGameFromJson �ڵ�" + to_string(turn) + "�غ���������";
    }

    vital = j["vital"];
    maxVital = j["maxVital"];
    motivation = j["motivation"];
    for (int i = 0; i < 5; i++) {
      fiveStatus[i] = j["fiveStatus"][i];
      fiveStatusLimit[i] = j["fiveStatusLimit"][i];
    }

    skillPt = j["skillPt"];
    skillScore = j["skillScore"];
    for (int i = 0; i < 5; i++) {
      trainLevelCount[i] = j["trainLevelCount"][i];
    }

    failureRateBias = j["failureRateBias"];
    isQieZhe = j["isQieZhe"];
    isAiJiao = j["isAiJiao"];
    isPositiveThinking = j["isPositiveThinking"];
    isRefreshMind = j["isRefreshMind"];

    isRacing = j["isRacing"];
    if (isRacing != isRacingTurn[turn])
    {
      cout << "Warning:ʵ�����̺�Ԥ�����̲�һ��" << endl;
      isRacingTurn[turn] = isRacing;
    }
    for (int i = 0; i < 6; i++) {
      persons[i].friendship = j["persons"][i]["friendship"];
      persons[i].isHint = j["persons"][i]["isHint"];
    }
    friendship_noncard_yayoi = j["friendship_noncard_yayoi"];
    friendship_noncard_reporter = j["friendship_noncard_reporter"];

    for (int i = 0; i < 5; i++) {
      for (int p = 0; p < 5; p++) {
        int pid = j["personDistribution"][i][p];
        if (pid == 102) {
          personDistribution[i][p] = PSID_noncardYayoi;
        }
        else if (pid == 103) {
          personDistribution[i][p] = PSID_noncardReporter;
        }
        else if (pid >= 1000) {
          personDistribution[i][p] = PSID_npc;
        }
        else if (pid >= 0 && pid < 9)
        {
          personDistribution[i][p] = pid;
        }
        else if (pid == -1)
        {
          personDistribution[i][p] = -1;
        }
        else
        {
          throw "Game::loadGameFromJson��ȡ��δ֪��personId:" + to_string(pid);
        }
      }
    }


    if (friend_type != 0) {
      friend_outgoingUsed = j["friend_outgoingUsed"];
      friend_stage = j["friend_stage"];
    }

    for (int i = 0; i < 5; i++) {
      mecha_rivalLv[i] = j["mecha_rivalLv"][i];
    }
    mecha_overdrive_energy = j["mecha_overdrive_energy"];
    mecha_overdrive_enabled = j["mecha_overdrive_enabled"];
    mecha_EN = j["mecha_EN"]; //�����Ѿ������˵�EN
    for (int i = 0; i < 3; i++) {
      for (int t = 0; t < 3; t++) {
        mecha_upgrade[i][t] = j["mecha_upgrade"][i][t];
      }
    }
    for (int i = 0; i < 5; i++) {
      mecha_hasGear[i] = j["mecha_hasGear"][i];
    }
    mecha_anyLose = false;
    int UGEcount = turn / 12 - 1;
    for (int i = 0; i < 5; i++) {
      mecha_win_history[i] = j["mecha_win_history"][i];
      if (i < UGEcount && mecha_win_history[i] != 2)
        mecha_anyLose = true;
    }



    maybeUpdateDeyilv();
    calculateTrainingValue();
  //for (int k = 1; k < 5; k++) {
   //     cout << trainValue[1][k] << endl;
   // }
    
  }
  catch (string e)
  {
    cout << "��ȡ��Ϸ��Ϣjson����" << e << endl;
    //cout << "-- json --" << endl << jsonStr << endl;
    return false;
  }
  /*
  catch (std::exception& e)
  {
    cout << "��ȡ��Ϸ��Ϣjson����δ֪����" << endl << e.what() << endl;
    //cout << "-- json --" << endl << jsonStr << endl;
    return false;
  }
  catch (...)
  {
    cout << "��ȡ��Ϸ��Ϣjson����δ֪����"  << endl;
    return false;
  }
  */

  return true;
}

