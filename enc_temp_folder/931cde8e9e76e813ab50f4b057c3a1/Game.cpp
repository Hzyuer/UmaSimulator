#include <iostream>
#include <cassert>
#include "Game.h"
using namespace std;
static bool randBool(mt19937_64& rand, double p)
{
  return rand() % 65536 < p * 65536;
}

//������Game���˳��һ��
void Game::newGame(mt19937_64& rand, bool enablePlayerPrint, int newUmaId, int umaStars, int newCards[6], int newZhongMaBlueCount[5], int newZhongMaExtraBonus[6])
{
  playerPrint = enablePlayerPrint;
  ptScoreRate = GameConstants::ScorePtRateDefault;
  hintPtRate = GameConstants::HintLevelPtRateDefault;
  eventStrength = GameConstants::EventStrengthDefault;
  scoringMode = SM_normal;

  umaId = newUmaId;
  isLinkUma = GameConstants::isLinkChara(umaId);
  if (!GameDatabase::AllUmas.count(umaId))
  {
    throw "ERROR Unknown character. Updating database is required.";
  }
  for (int i = 0; i < TOTAL_TURN; i++)
    isRacingTurn[i] = GameDatabase::AllUmas[umaId].races[i] == TURN_RACE;
  assert(isRacingTurn[11] == true);//������
  isRacingTurn[TOTAL_TURN - 5] = true;//ura1
  isRacingTurn[TOTAL_TURN - 3] = true;//ura2
  isRacingTurn[TOTAL_TURN - 1] = true;//ura3

  for (int i = 0; i < 5; i++)
    fiveStatusBonus[i] = GameDatabase::AllUmas[umaId].fiveStatusBonus[i];

  turn = 0;
  gameStage = GameStage_beforeTrain;
  vital = 100;
  maxVital = 100;
  motivation = 3;

  for (int i = 0; i < 5; i++)
    fiveStatus[i] = GameDatabase::AllUmas[umaId].fiveStatusInitial[i] - 10 * (5 - umaStars); //�������ʼֵ
  for (int i = 0; i < 5; i++)
    fiveStatusLimit[i] = GameConstants::BasicFiveStatusLimit[i]; //ԭʼ��������

  skillPt = 120;
  skillScore = umaStars >= 3 ? 170 * (umaStars - 2) : 120 * (umaStars);//���м���

  for (int i = 0; i < 5; i++)
  {
    trainLevelCount[i] = 0;
  }

  failureRateBias = 0;
  isQieZhe = false;
  isAiJiao = false;
  isPositiveThinking = false;
  isRefreshMind = false;

  for (int i = 0; i < 5; i++)
    zhongMaBlueCount[i] = newZhongMaBlueCount[i];
  for (int i = 0; i < 6; i++)
    zhongMaExtraBonus[i] = newZhongMaExtraBonus[i];

  for (int i = 0; i < 5; i++)
    fiveStatusLimit[i] += int(zhongMaBlueCount[i] * 5.34 * 2); //��������--�������ֵ
  for (int i = 0; i < 5; i++)
    addStatus(i, zhongMaBlueCount[i] * 7); //����

  isRacing = false;

  friendship_noncard_yayoi = 0;
  friendship_noncard_reporter = 0;

  currentDeyilvBonus = 0;
  currentLianghuaEffectEnable = false;

  for (int i = 0; i < MAX_INFO_PERSON_NUM; i++)
  {
    persons[i] = Person();
  }

  saihou = 0;
  friend_type = 0;
  friend_isSSR = false;
  friend_personId = PSID_none;
  friend_stage = 0;
  friend_outgoingUsed = 0;
  friend_vitalBonus = 1.0;
  friend_statusBonus = 1.0;
  for (int i = 0; i < 6; i++)
  {
    int cardId = newCards[i];
    persons[i].setCard(cardId);
    saihou += persons[i].cardParam.saiHou;

    if (persons[i].personType == PersonType_friendCard)
    {
      friend_personId = i;
      int friendCardId = cardId / 10;
      if (friendCardId == GameConstants::FriendCardLianghuaSSRId)
      {
        friend_type = FriendType_lianghua;
        friend_isSSR = true;
      }
      else if (friendCardId == GameConstants::FriendCardLianghuaRId)
      {
        friend_type = FriendType_lianghua;
        friend_isSSR = false;
      }
      else if (friendCardId == GameConstants::FriendCardYayoiSSRId)
      {
        friend_type = FriendType_yayoi;
        friend_isSSR = true;
      }
      else if (friendCardId == GameConstants::FriendCardYayoiRId)
      {
        friend_type = FriendType_yayoi;
        friend_isSSR = false;
      }
      else
        throw string("��֧�ִ�����/���³���������˻��Ŷӿ�");
      int friendLevel = cardId % 10;
      assert(friendLevel >= 0 && friendLevel <= 4);
      friend_vitalBonus = 1.0 + 0.01 * persons[i].cardParam.eventRecoveryAmountUp;
      friend_statusBonus = 1.0 + 0.01 * persons[i].cardParam.eventEffectUp;
      
      friend_vitalBonus += 1e-10;
      friend_statusBonus += 1e-10;//�Ӹ�С����������Ϊ�����������
    }
  }

  std::vector<int> probs = { 100,100,100,100,100,200 }; //���������Ǹ�
  distribution_noncard = std::discrete_distribution<>(probs.begin(), probs.end());
  probs = { 100,100,100,100,100,100 }; //���������Ǹ�
  distribution_npc = std::discrete_distribution<>(probs.begin(), probs.end());

  for (int i = 0; i < 6; i++)//֧Ԯ����ʼ�ӳ�
  {
    for (int j = 0; j < 5; j++)
      addStatus(j, persons[i].cardParam.initialBonus[j]);
    skillPt += persons[i].cardParam.initialBonus[5];
  }

  mecha_linkeffect_gearProbBonus = 0;
  mecha_linkeffect_lvbonus = false;
  for (int i = 0; i < 5; i++)
    mecha_rivalLv[i] = 0;//һ��link��20��2����40���������ûlink�ٸĳ�1
  mecha_overdrive_energy = 0;
  mecha_overdrive_enabled = false;
  mecha_EN = 5;
  for (int i = 0; i < 3; i++)
    for (int j = 0; j < 3; j++)
      mecha_upgrade[i][j] = 0;
  for (int i = 0; i < 5; i++)
    mecha_hasGear[i] = false;
  for (int i = 0; i < 5; i++)
    mecha_win_history[i] = 0;


  //֧Ԯ��link
  for (int i = 0; i < 7; i++)
  {
    int chara = i < 6 ? persons[i].cardParam.charaId : umaId;
    if (GameConstants::isLinkChara_initialEN(chara))
      mecha_EN += 1;
    if (GameConstants::isLinkChara_moreGear(chara))
      mecha_linkeffect_gearProbBonus += 1;
    if (GameConstants::isLinkChara_initialOverdrive(chara))
      mecha_overdrive_energy += 3;
    if (GameConstants::isLinkChara_lvBonus(chara))
      mecha_linkeffect_lvbonus = true;
    if (GameConstants::isLinkChara_initialLv(chara))
      for (int i = 0; i < 5; i++)
        mecha_rivalLv[i] += 20;//һ��link��20��2����40
  }

  if (mecha_overdrive_energy > 6)mecha_overdrive_energy = 6;
  if (mecha_EN > 7)mecha_EN = 7;
  for (int i = 0; i < 5; i++)
    if (mecha_rivalLv[i] < 1)
      mecha_rivalLv[i] = 1;



  randomDistributeCards(rand); //������俨�飬������������
  
}

void Game::randomDistributeCards(std::mt19937_64& rand)
{
  for (int i = 0; i < 5; i++)
    for (int j = 0; j < 5; j++)
      personDistribution[i][j] = -1;

  if (isRacing)
  {
    return;//�������÷��俨��
  }
  

  int headN[5] = { 0,0,0,0,0 };
  vector<int8_t> buckets[5];
  for (int i = 0; i < 5; i++)
    buckets[i].clear();
  //�ȷ�����/���³�/����
  for (int i = 0; i < 6 + 2; i++)
  {
    int atTrain = 5;
    if (friend_type == FriendType_yayoi && i == friend_personId)
    {
      //���˿�
      atTrain = persons[i].distribution(rand);
    }
    else if (i == PSID_noncardYayoi && friend_type != FriendType_yayoi)//�ǿ����³�
    {
      atTrain = distribution_noncard(rand);
    }
    else if (i == PSID_noncardReporter)//����
    {
      if (turn < 12 || isXiahesu())//���ߵ�13�غ������ĺ���Ҳ����
        continue;
      atTrain = distribution_noncard(rand);
    }

    if (atTrain < 5)
    {
      buckets[atTrain].push_back(i);
    }
    
  }
  for (int i = 0; i < 5; i++)
  {
    if (buckets[i].size() == 1)
    {
      personDistribution[i][0] = buckets[i][0];
      headN[i] += 1;
    }
    else if (buckets[i].size() > 1)//���ѡһ����ͷ
    {
      personDistribution[i][0] = buckets[i][rand() % buckets[i].size()];
      headN[i] += 1;
    }
    buckets[i].clear();
  }

  //Ȼ������֧ͨԮ��
  for (int i = 0; i < 6; i++)
  {
    Person& p = persons[i];
    if (p.personType == PersonType_card)
    {
      int atTrain = p.distribution(rand);
      if (atTrain < 5)
      {
        buckets[atTrain].push_back(i);
      }
    }
  }

  //npc
  int npcCount = 6;
  for (int i = 0; i < npcCount; i++)
  {
    int atTrain = distribution_npc(rand);
    if (atTrain < 5)
    {
      buckets[atTrain].push_back(PSID_npc);
    }
  }

  //ѡ��������5����ͷ
  for (int i = 0; i < 5; i++)
  {
    int maxHead = 5 - headN[i];
    if (buckets[i].size() <= maxHead)
    {
      for (int j = 0; j < buckets[i].size(); j++)
      {
        personDistribution[i][headN[i]] = buckets[i][j];
        headN[i] += 1;
      }
    }
    else//����������5�ˣ����ѡmaxHead��
    {
      for (int j = 0; j < maxHead; j++)
      {
        int idx = rand() % (buckets[i].size() - j);

        int count = 0;
        for (int k = 0; k < buckets[i].size(); k++)
        {
          if (buckets[i][k] != -1)
          {
            if (idx == count)
            {
              personDistribution[i][headN[i]] = buckets[i][k];
              buckets[i][k] = -1;
              headN[i] += 1;
              break;
            }
            else
              count++;
          }
        }
      }
      assert(headN[i] == 5);
    }
  }

  //�Ƿ���hint
  for (int pid = 0; pid < 6; pid++)
  {
    if (persons[pid].personType == PersonType_card)
    {
      double hintProb = 0.06 * (1 + 0.01 * persons[pid].cardParam.hintProbIncrease);
      hintProb *= (1.0 + 0.15 * mecha_upgrade[0][1]);
      persons[pid].isHint = randBool(rand, hintProb);
        
    }
  }

  //��������Ƿ��г��֡�����в�Ȧ������calculateTrainingValue()�����true�����ﲻ�ÿ���
  double gearProb = GameConstants::Mecha_GearProb + GameConstants::Mecha_GearProbLinkBonus * mecha_linkeffect_gearProbBonus;
  for (int i = 0; i < 5; i++)
  {
    mecha_hasGear[i] = randBool(rand, gearProb);
  }

  calculateTrainingValue();
}

//
//�ϲ�=min(����-�²�, 100)
void Game::calculateTrainingValue()
{
  //���¼���ͳ����Ϣ
  mecha_rivalLvTotal = 0;
  for (int i = 0; i < 5; i++)
  {
    mecha_rivalLvTotal += mecha_rivalLv[i];
  }

  mecha_rivalLvLimit = turn < 24 ? 200 : turn < 36 ? 300 : turn < 48 ? 400 : turn < 60 ? 500 : turn < 72 ? 600 : 700;

  for (int i = 0; i < 3; i++)
  {
    mecha_upgradeTotal[i] = 0;
    for (int j = 0; j < 3; j++)
      mecha_upgradeTotal[i] += mecha_upgrade[i][j];
  }

  //���Լӳ�
  for (int i = 0; i < 5; i++)
  {
    double m = 1.0;
    //double rivalLvBonus = 0.06 + 0.0006 * mecha_rivalLv[i];
    //if (mecha_linkeffect_lvbonus)rivalLvBonus * 1.5;
    //m *= (1 + rivalLvBonus);

    if (mecha_overdrive_enabled)
    {
      //m *= 1.25;
      int upgradeGroup =
        (i == 0 || i == 2) ? mecha_upgradeTotal[1] :
        (i == 1 || i == 3) ? mecha_upgradeTotal[2] :
        mecha_upgradeTotal[0];
      if (upgradeGroup >= 9)
      {
        int count = 1 + (mecha_rivalLvTotal - 1) / 200;
        double bonus = 0.03 * count;
        m *= (1 + bonus);
      }
      else if (upgradeGroup >= 6)
      {
        int count = 1 + (mecha_rivalLvTotal - 1) / 300;
        double bonus = 0.03 * count;
        m *= (1 + bonus);
      }
    }

    mecha_trainingStatusMultiplier[i] = m;
  }

  double ptb = 1.0;
  ptb *= 1 + mecha_upgrade[2][2] * 0.12;
  if (mecha_overdrive_enabled && mecha_upgradeTotal[2] >= 15)
  {
    int count = 1 + (mecha_rivalLvTotal - 1) / 150;
    double bonus = 0.03 * count;
    ptb *= (1 + bonus);
  }
  mecha_trainingStatusMultiplier[5] = ptb;

  for (int i = 0; i < 5; i++)
  {
    int upgradeLv =
      i == 0 ? mecha_upgrade[2][0] :
      i == 1 ? mecha_upgrade[1][0] :
      i == 2 ? mecha_upgrade[2][1] :
      i == 3 ? mecha_upgrade[1][1] :
      mecha_upgrade[0][0];
    double lvGainBonus =
      upgradeLv == 5 ? 40 :
      upgradeLv == 4 ? 33 :
      upgradeLv == 3 ? 26 :
      upgradeLv == 2 ? 18 :
      upgradeLv == 1 ? 10 :
      0;
    if (mecha_overdrive_enabled)
    {
      if (mecha_upgradeTotal[0] >= 12)
        lvGainBonus += 25;
      else if (mecha_upgradeTotal[0] >= 9)
        lvGainBonus += 20;
      else if (mecha_upgradeTotal[0] >= 6)
        lvGainBonus += 15;
    }

    mecha_lvGainMultiplier[i] = 1.0 + 0.01 * lvGainBonus;
  }

  for (int i = 0; i < 5; i++)
    calculateTrainingValueSingle(i);
}

void Game::addTrainingLevelCount(int trainIdx, int n)
{
  trainLevelCount[trainIdx] += n;
  if (trainLevelCount[trainIdx] > 16)trainLevelCount[trainIdx] = 16;
}

void Game::maybeUpdateDeyilv()
{
  int deyilvBonus = 15 * mecha_upgrade[0][0];
  bool lianghuaEffectEnable =
    friend_type == FriendType_lianghua &&
    friend_isSSR &&
    persons[friend_personId].friendship >= 60;
  if (deyilvBonus != currentDeyilvBonus || lianghuaEffectEnable != currentLianghuaEffectEnable)
  {
    currentDeyilvBonus = deyilvBonus;
    currentLianghuaEffectEnable = lianghuaEffectEnable;
    for (int i = 0; i < 6; i++)
    {
      persons[i].setExtraDeyilvBonus(deyilvBonus, lianghuaEffectEnable);
    }
  }
}

bool Game::tryInvitePeople(std::mt19937_64& rand) 
{
  int invitePerson = rand() % 6;
  int inviteTrain = rand() % 5;

  int space = -1;
  for (int idx = 0; idx < 5; idx++)
  {
    int pid = personDistribution[inviteTrain][idx];
    if (pid == -1 && space == -1)
      space = idx;
    if (pid == invitePerson)
      return false;
  }

  if (space == -1)
    return false;

  personDistribution[inviteTrain][space] = invitePerson;
  return true;
  //require recalculate later
}
void Game::mecha_addRivalLv(int idx, int value)
{
  assert(idx >= 0 && idx < 5);
  int t = mecha_rivalLv[idx] + value;

  if (t > mecha_rivalLvLimit)
    t = mecha_rivalLvLimit;
}
void Game::mecha_distributeEN(int head3, int chest3, int foot3)
{
  throw "todo";
}
void Game::mecha_maybeRunUGE()
{
  throw "todo";
}
bool Game::mecha_activate_overdrive()
{
  throw "todo";
  return false;
}
int Game::calculateRealStatusGain(int value, int gain) const//����1200����Ϊ2�ı�����ʵ����������ֵ
{
  int newValue = value + gain;
  if (newValue <= 1200)return gain;
  if (gain == 1)return 2;
  return (newValue / 2) * 2 - value;
}
void Game::addStatus(int idx, int value)
{
  assert(idx >= 0 && idx < 5);
  int t = fiveStatus[idx] + value;
  
  if (t > fiveStatusLimit[idx])
    t = fiveStatusLimit[idx];
  if (t < 1)
    t = 1;
  if (t > 1200)
    t = (t / 2) * 2;
  fiveStatus[idx] = t;
}
void Game::addVital(int value)
{
  vital += value;
  if (vital > maxVital)
    vital = maxVital;
  if (vital < 0)
    vital = 0;
}
void Game::addVitalMax(int value)
{
  maxVital += value;
  if (maxVital > 120)
    maxVital = 120;
}
void Game::addMotivation(int value)
{
  if (value < 0)
  {
    if (isPositiveThinking)
      isPositiveThinking = false;
    else
    {
      motivation += value;
      if (motivation < 1)
        motivation = 1;
    }
  }
  else
  {
    motivation += value;
    if (motivation > 5)
      motivation = 5;
  }
}
void Game::addJiBan(int idx, int value, bool ignoreAijiao)
{
  if(idx == PSID_noncardYayoi)
    friendship_noncard_yayoi += value;
  else if (idx == PSID_noncardReporter)
    friendship_noncard_reporter += value;
  else if (idx < 6)
  {
    auto& p = persons[idx];
    int gain = (isAiJiao && !ignoreAijiao) ? value + 2 : value;
    persons[idx].friendship += gain;
    if (p.friendship > 100)p.friendship = 100;
  }
  else
    throw "ERROR: Game::addJiBan Unknown person id";
}
void Game::addAllStatus(int value)
{
  for (int i = 0; i < 5; i++)addStatus(i, value);
}
int Game::calculateFailureRate(int trainType, double failRateMultiply) const
{
  static const double A = 0.025;
  static const double B = 1.25;
  double x0 = 0.1 * GameConstants::FailRateBasic[trainType][getTrainingLevel(trainType)];
  
  double f = 0;
  if (vital < x0)
  {
    f = (100 - vital) * (x0 - vital) / 40.0;
  }
  if (f < 0)f = 0;
  if (f > 99)f = 99;//����ϰ���֣�ʧ�������99%
  f *= failRateMultiply;//֧Ԯ����ѵ��ʧ�����½�����
  int fr = ceil(f);
  fr += failureRateBias;
  if (fr < 0)fr = 0;
  if (fr > 100)fr = 100;
  return fr;
}
void Game::runRace(int basicFiveStatusBonus, int basicPtBonus)
{
  double raceMultiply = 1 + 0.01 * saihou;

  int fiveStatusBonus = int(raceMultiply * basicFiveStatusBonus);
  int ptBonus = int(raceMultiply * basicPtBonus);
  //cout << fiveStatusBonus << " " << ptBonus << endl;
  addAllStatus(fiveStatusBonus);
  skillPt += ptBonus;
}

void Game::addStatusFriend(int idx, int value)
{
  value = int(value * friend_statusBonus);
  if (idx == 5)skillPt += value;
  else addStatus(idx, value);
}

void Game::addVitalFriend(int value)
{
  value = int(value * friend_vitalBonus);
  addVital(value);
}


void Game::handleFriendOutgoing(std::mt19937_64& rand)
{
  assert(friend_type!=0 && friend_stage >= FriendStage_afterUnlockOutgoing && friend_outgoingUsed < 5);
  int pid = friend_personId;
  if (friend_type == FriendType_yayoi)
  {
    if (friend_outgoingUsed == 0)
    {
      addVitalFriend(30);
      addMotivation(1);
      addStatusFriend(3, 20);
      addJiBan(pid, 5, false);
    }
    else if (friend_outgoingUsed == 1)
    {
      addVitalFriend(30);
      addMotivation(1);
      addStatusFriend(0, 10);
      addStatusFriend(3, 10);
      isRefreshMind = true;
      addJiBan(pid, 5, false);
    }
    else if (friend_outgoingUsed == 2)
    {
      int remainVital = maxVital - vital;
      if (remainVital >= 20)//ѡ��
        addVitalFriend(43);
      else//ѡ��
        addStatusFriend(3, 29);
      addMotivation(1);
      addJiBan(pid, 5, false);
    }
    else if (friend_outgoingUsed == 3)
    {
      addVitalFriend(30);
      addMotivation(1);
      addStatusFriend(3, 25);
      addJiBan(pid, 5, false);
    }
    else if (friend_outgoingUsed == 4)
    {
      //�д�ɹ��ͳɹ�
      if (rand() % 4 != 0)//���Թ��ƣ�75%��ɹ�
      {
        addVitalFriend(30);
        addStatusFriend(3, 36);
        skillPt += 72;//���ܵȼ�
      }
      else
      {
        addVitalFriend(26);
        addStatusFriend(3, 24);
        skillPt += 40;//���ܵȼ�
      }
      addMotivation(1);
      addJiBan(pid, 5, false);
      isRefreshMind = true;
    }
    else throw string("δ֪�ĳ���");
  }
  else if (friend_type == FriendType_lianghua)
  {
    throw "todo";
  }
  else throw string("δ֪�ĳ���");


  friend_outgoingUsed += 1;
}
void Game::handleFriendUnlock(std::mt19937_64& rand)
{
  assert(friend_stage == FriendStage_beforeUnlockOutgoing);

  if (friend_type == FriendType_yayoi)
  { 
    if (maxVital - vital >= 15)
    {
      addVitalFriend(25);
      printEvents("�������������ѡ��");
    }
    else
    {
      addStatusFriend(0, 8);
      addStatusFriend(3, 8);
      skillPt += 10;//ֱ������+5
      printEvents("�������������ѡ��");
    }
    addMotivation(1);
    isRefreshMind = true;
    addJiBan(friend_personId, 5, false);
  }
  else if (friend_type == FriendType_lianghua)
  {
    throw "todo";
  }
  else throw string("δ֪�����˽�������");
  friend_stage = FriendStage_afterUnlockOutgoing;
}
void Game::handleFriendClickEvent(std::mt19937_64& rand, int atTrain)
{
  assert(friend_type!=0 && (friend_personId<6&& friend_personId>=0) && persons[friend_personId].personType==PersonType_friendCard);
  if (friend_stage == FriendStage_notClicked)
  {
    printEvents("��һ�ε�����");
    friend_stage = FriendStage_beforeUnlockOutgoing;

    if (friend_type == FriendType_yayoi)
    {
      addStatusFriend(0, 14);
      addJiBan(friend_personId, 10, false);
      addMotivation(1);
    }
    else if (friend_type == FriendType_lianghua)
    {
      throw "todo";
    }
    else throw string("δ֪�ĵ�һ�ε�����");
  }
  else
  {
    if (rand() % 5 < 3)return;//40%���ʳ��¼���60%���ʲ���

    if (rand() % 10 == 0)
    {
      if (motivation != 5)
        printEvents("���˵���¼�:����+1");
      addMotivation(1);//10%���ʼ�����
    }

    if (friend_type == FriendType_yayoi)
    {
      if (turn < 24)
      {
        //�����͵��˼�3�
        int minJiBan = 10000;
        int minJiBanId = -1;
        for (int i = 0; i < 6; i++)
        {
          if (persons[i].personType == PersonType_card)
          {
            if (persons[i].friendship < minJiBan)
            {
              minJiBan = persons[i].friendship;
              minJiBanId = i;
            }
          }
        }
        if (minJiBanId != -1)
        {
          addJiBan(minJiBanId, 3, false);
        }
        addJiBan(friend_personId, 5, false);
        printEvents("���˵���¼�:" + persons[minJiBanId].getPersonName() + " �+3, ���³��+5");

     
      }
      else if (turn < 48)
      {
        addStatusFriend(0, 12);
        addJiBan(friend_personId, 5, false);
      }
      else
      {
        addStatusFriend(3, 12);
        addJiBan(friend_personId, 5, false);
      }
    }
    else if (friend_type == FriendType_lianghua)
    {
      throw "todo";
    }
    else throw string("δ֪�����˵���¼�");
  }

}
void Game::handleFriendFixedEvent()
{
  if (friend_type == 0)return;//û���˿�
  if (friend_stage < FriendStage_beforeUnlockOutgoing)return;//����û������û�¼�
  if (turn == 23)
  {
    if (friend_type == FriendType_yayoi)
    {
      addMotivation(1);
      addStatusFriend(0, 24);
      addJiBan(friend_personId, 5, false);
      skillPt += 40;//�����������ߣ������н�����������hint����Ч��
    }
    else if (friend_type == FriendType_lianghua)
    {
      throw "todo";
    }
    else throw string("δ֪�����˹̶��¼�");
  }
  else if (turn == 77)
  {
    if (friend_type == FriendType_yayoi)
    {
      if (friend_outgoingUsed >= 5)//�������
      {
        addStatusFriend(0, 20);
        addStatusFriend(3, 20);
        addStatusFriend(5, 56);
      }
      else
      {
        //just guess, to be filled
        addStatusFriend(0, 16);
        addStatusFriend(3, 16);
        addStatusFriend(5, 43);
      }
    }
    else if (friend_type == FriendType_lianghua)
    {
      throw "todo";
    }
    else throw string("δ֪�����˹̶��¼�");
  }
  else
  {
    assert(false && "�����غ�û�����˹̶��¼�");
  }
}
bool Game::applyTraining(std::mt19937_64& rand, int train)
{
  if (isRacing)
  {
    //�̶�������������checkEventAfterTrain()�ﴦ��
    assert(train == TRA_race);

    mecha_overdrive_energy += 1;
    if (mecha_overdrive_energy > 6)mecha_overdrive_energy = 6;
    for (int i = 0; i < 5; i++)
    {
      mecha_addRivalLv(i, 7);
    }

  }
  else
  {
    if (train == TRA_rest)//��Ϣ
    {
      if (isXiahesu())//����ֻ�����
      {
        return false;
      }
      else
      {
        int r = rand() % 100;
        if (r < 25)
          addVital(70);
        else if (r < 82)
          addVital(50);
        else
          addVital(30);
      }
      mecha_overdrive_energy += 1;
      if (mecha_overdrive_energy > 6)mecha_overdrive_energy = 6;
    }
    else if (train == TRA_race)//����
    {
      if (turn <= 12 || turn >= 72)
      {
        printEvents("Cannot race now.");
        return false;
      }
      addAllStatus(1);//������
      runRace(2, 40);//���ԵĽ���

      //����̶�15
      addVital(-15);
      if (rand() % 10 == 0)
        addMotivation(1);

      mecha_overdrive_energy += 1;
      if (mecha_overdrive_energy > 6)mecha_overdrive_energy = 6;
      for (int i = 0; i < 5; i++)
      {
        mecha_addRivalLv(i, 7);
      }
    }
    else if (train == TRA_outgoing)//���
    {
      if (isXiahesu())
      {
        addVital(40);
        addMotivation(1);
      }
      else if (friend_type != 0 &&  //�������˿�
        friend_stage == FriendStage_afterUnlockOutgoing &&  //�ѽ������
        friend_outgoingUsed < 5  //���û����
        )
      {
        //���˳���
        handleFriendOutgoing(rand);
      }
      else //��ͨ����
      {
        //���ò�����ˣ���50%��2���飬50%��1����10����
        if (rand() % 2)
          addMotivation(2);
        else
        {
          addMotivation(1);
          addVital(10);
        }
      }
      mecha_overdrive_energy += 1;
      if (mecha_overdrive_energy > 6)mecha_overdrive_energy = 6;
    }
    else if (train <= 4 && train >= 0)//����ѵ��
    {
      if (rand() % 100 < failRate[train])//ѵ��ʧ��
      {
        if (failRate[train] >= 20 && (rand() % 100 < failRate[train]))//ѵ����ʧ�ܣ�������Ϲ�µ�
        {
          printEvents("ѵ����ʧ�ܣ�");
          addStatus(train, -10);
          if (fiveStatus[train] > 1200)
            addStatus(train, -10);//��Ϸ��1200���Ͽ����Բ��۰룬�ڴ�ģ�������Ӧ1200���Ϸ���
          //�����2��10�������ĳ�ȫ����-4���������
          for (int i = 0; i < 5; i++)
          {
            addStatus(i, -4);
            if (fiveStatus[i] > 1200)
              addStatus(i, -4);//��Ϸ��1200���Ͽ����Բ��۰룬�ڴ�ģ�������Ӧ1200���Ϸ���
          }
          addMotivation(-3);
          addVital(10);
        }
        else//Сʧ��
        {
          printEvents("ѵ��Сʧ�ܣ�");
          addStatus(train, -5);
          if (fiveStatus[train] > 1200)
            addStatus(train, -5);//��Ϸ��1200���Ͽ����Բ��۰룬�ڴ�ģ�������Ӧ1200���Ϸ���
          addMotivation(-1);
        }
      }
      else
      {
        //�ȼ���ѵ��ֵ
        for (int i = 0; i < 5; i++)
          addStatus(i, trainValue[train][i]);
        skillPt += trainValue[train][5];
        addVital(trainVitalChange[train]);

        int friendshipExtra = 0;//�������SSR���˿���+1��������˿������ѵ������+2�������������ﴦ��
        bool isSSRYayoi = friend_type == PersonType_yayoi && friend_isSSR;
        if (isSSRYayoi)
          friendshipExtra += 1;

        vector<int> hintCards;//���ļ����������̾����
        bool clickFriend = false;//���ѵ����û������
        //���SSR�����ڲ�������
        for (int i = 0; i < 5; i++)
        {
          int p = personDistribution[train][i];
          if (p == PSID_none)break;//û��
          if (isSSRYayoi && p == friend_personId)
          {
            friendshipExtra += 2;
            break;
          }
        }
        for (int i = 0; i < 5; i++)
        {
          int p = personDistribution[train][i];
          if (p < 0)break;//û��

          if (p == friend_personId && friend_type != 0)//���˿�
          {
            assert(persons[p].personType == PersonType_friendCard);
            addJiBan(p, 4 + friendshipExtra, false);
            clickFriend = true;
          }
          else if (p < 6)//��ͨ��
          {
            addJiBan(p, 7 + friendshipExtra, false);
            if (persons[p].isHint)
              hintCards.push_back(p);
          }
          else if (p == PSID_npc)//npc
          {
            //nothing
          }
          else if (p == PSID_noncardYayoi)//�ǿ����³�
          {
            int jiban = friendship_noncard_yayoi;
            int g = jiban < 40 ? 2 : jiban < 60 ? 3 : jiban < 80 ? 4 : 5;
            skillPt += g;
            addJiBan(PSID_noncardYayoi, 7, false);
          }
          else if (p == PSID_noncardReporter)//����
          {
            int jiban = friendship_noncard_reporter;
            int g = jiban < 40 ? 2 : jiban < 60 ? 3 : jiban < 80 ? 4 : 5;
            addStatus(train, g);
            addJiBan(PSID_noncardReporter, 7, false);
          }
          else
          {
            //��������/�ſ��ݲ�֧��
            assert(false);
          }
        }

        if (hintCards.size() > 0)
        {
          if (!(mecha_overdrive_enabled && mecha_upgradeTotal[0] >= 15))//���һ�ſ���hint
          {
            int hintCard = hintCards[rand() % hintCards.size()];
            hintCards.clear();
            hintCards.push_back(hintCard);
          }

          for (int p = 0; p < hintCards.size(); p++)
          {
            int hintCard = hintCards[p];
            addJiBan(hintCard, 5, false);
            int hintLevel = persons[hintCard].cardParam.hintLevel;
            if (hintLevel > 0)
            {
              skillPt += int(hintLevel * hintPtRate);
            }
            else //�����������֣�ֻ������
            {
              if (train == 0)
              {
                addStatus(0, 6);
                addStatus(2, 2);
              }
              else if (train == 1)
              {
                addStatus(1, 6);
                addStatus(3, 2);
              }
              else if (train == 2)
              {
                addStatus(2, 6);
                addStatus(1, 2);
              }
              else if (train == 3)
              {
                addStatus(3, 6);
                addStatus(0, 1);
                addStatus(2, 1);
              }
              else if (train == 4)
              {
                addStatus(4, 6);
                skillPt += 5;
              }
            }
          }
        }

        if (clickFriend)
          handleFriendClickEvent(rand, train);


        //ѵ���ȼ�����
        addTrainingLevelCount(train, 1);

        if (mecha_hasGear[train])
        {
          mecha_overdrive_energy += 1;
          if (mecha_overdrive_energy > 6)mecha_overdrive_energy = 6;
        }

        for (int i = 0; i < 5; i++)
        {
          mecha_addRivalLv(i, mecha_lvGain[train][i]);
        }
      }

    }
    else
    {
      printEvents("δ֪��ѵ����Ŀ");
      return false;
    }
  }

  return true;
}


bool Game::isLegal(Action action) const
{
  if (!action.isActionStandard())
    return false;

  //stage�Ƿ�ƥ��
  if (action.type != gameStage)
    return false;

  if (action.type == GameStage_beforeTrain)
  {
    if (isRacing)
    {
      if (action.train == TRA_race)
        return true;
      else
        return false;
    }

    //�Ƿ��ܿ�����
    if (action.overdrive)
    {
      if (mecha_overdrive_energy < 3)
        return false;
      if (mecha_overdrive_enabled)
        return false;
      if (mecha_upgradeTotal[1] >= 15)//ҡ�ˣ�Ӧ���ȿ�overdrive��ѡѵ����������
        return action.train == -1;
      else
        return action.train >= 0 && action.train <= 4;
    }

    if (action.train == TRA_rest)
    {
      if (isXiahesu())
      {
        return false;//���ĺ��޵ġ����&��Ϣ����Ϊ���
      }
      return true;
    }
    else if (action.train == TRA_outgoing)
    {
      return true;
    }
    else if (action.train == TRA_race)
    {
      return isRaceAvailable();
    }
    else if (action.train >= 0 && action.train <= 4)
    {
      return true;
    }
    else
    {
      assert(false && "δ֪��ѵ����Ŀ");
      return false;
    }
    return false;
  }
  else if (action.type == GameStage_beforeMechaUpgrade)
  {
    int total3 = mecha_EN / 3;
    int mechaHeadLimit = turn >= 36 ? 5 : 3;//�ڶ���UGE����ͷ3������
    int mechaChestLimit = turn >= 60 ? 5 : 3;//���Ĵ�UGE������3������
    int mechaFootLimit = turn >= 60 ? 5 : 3;//���Ĵ�UGE������3������
    int mechaFoot = total3 - action.mechaHead - action.mechaChest;
    if (action.mechaHead < 0 || action.mechaHead > mechaHeadLimit)return false;
    if (action.mechaChest < 0 || action.mechaChest > mechaChestLimit)return false;
    if (mechaFoot < 0 || mechaFoot > mechaFootLimit)return false;
    return true;
  }
  else throw "unknown action.type";
}



float Game::getSkillScore() const
{
  float rate = isQieZhe ? ptScoreRate * 1.1 : ptScoreRate ;
  return rate * skillPt + skillScore;
}

static double scoringFactorOver1200(double x)//����ʤ������ɫʮ�֣�׷��
{
  if (x <= 1150)return 0;
  return tanh((x - 1150) / 100.0) * sqrt(x - 1150);
}

static double realRacingStatus(double x)
{
  if (x < 1200)return x;
  return 1200 + (x - 1200) / 4;
}

static double smoothUpperBound(double x)
{
  return (x - sqrt(x * x + 1)) / 2;
}

int Game::finalScore_mile() const
{
  double weights[5] = { 400,300,70,70,120 };
  double weights1200[5] = { 0,0,20,10,0 };


  double staminaTarget = 900;
  double staminaBonus = 5 * 100 * (smoothUpperBound((realRacingStatus(fiveStatus[1]) - staminaTarget) / 100.0) - smoothUpperBound((0 - staminaTarget) / 100.0));

  double total = 0;
  total += staminaBonus;
  for (int i = 0; i < 5; i++)
  {
    double realStat = realRacingStatus(min(fiveStatus[i], fiveStatusLimit[i]));
    total += weights[i] * sqrt(realStat);
    total += weights1200[i] * scoringFactorOver1200(realStat);
  }

  total += getSkillScore();
  if (total < 0)total = 0;
  //return uaf_haveLose ? 10000 : 20000;
  return (int)total;
}

int Game::finalScore_sum() const
{
  double weights[5] = { 5,3,3,3,3 };
  double total = 0;
  for (int i = 0; i < 5; i++)
  {
    double realStat = min(fiveStatus[i], fiveStatusLimit[i]);
    if (realStat > 1200)realStat = 1200 + (realStat - 1200) / 2;
    total += weights[i] * realStat;
  }

  total += getSkillScore();
  if (total < 0)total = 0;
  return (int)total;
}

int Game::finalScore_rank() const
{
  int total = 0;
  for (int i = 0; i < 5; i++)
    total += GameConstants::FiveStatusFinalScore[min(fiveStatus[i], fiveStatusLimit[i])];

  total += int(getSkillScore());
  //return uaf_haveLose ? 10000 : 20000;
  return total;
}

int Game::finalScore() const
{
  if (scoringMode == SM_normal)
  {
    return finalScore_rank();
  }
  else if (scoringMode == SM_race)
  {
    return finalScore_sum();
  }
  else if (scoringMode == SM_mile)
  {
    return finalScore_mile();
  }
  else
  {
    throw "�������㷨��δʵ��";
  }
  return 0;
}

bool Game::isEnd() const
{
  return turn >= TOTAL_TURN;
}

int Game::getTrainingLevel(int item) const
{
  if (isXiahesu())return 4;

  return trainLevelCount[item] / 4;
}

void Game::calculateLvGainSingle(int tra, int headNum, bool isShining)
{
  bool xhs = isXiahesu();
  int group = !mecha_hasGear[tra] ? 0 : !isShining ? 1 : 2;
  for (int i = 0; i < 5; i++)
    mecha_lvGain[tra][i] = 0;
  for (int sub = 0; sub < 3; sub++)
  {
    int type = GameConstants::Mecha_LvGainSubTrainIdx[tra][sub];
    int basic = GameConstants::Mecha_LvGainBasic[xhs][group][sub][headNum];
    double multiplier = mecha_lvGainMultiplier[type];
    int gain = int(multiplier * basic);
    if (gain == basic && multiplier > 1)//����+1
      gain += 1;
    mecha_lvGain[tra][type] = gain;
  }
}

//Reference��https://github.com/mee1080/umasim/blob/main/core/src/commonMain/kotlin/io/github/mee1080/umasim/scenario/mecha/MechaStore.kt
void Game::calculateTrainingValueSingle(int tra)
{
  //�����²�------------------------------------------------------------------
  int headNum = 0;//���ſ�����npc�����³����߲���
  int shiningNum = 0;//��������
  int linkNum = 0;//����link

  int basicValue[6] = { 0,0,0,0,0,0 };//ѵ���Ļ���ֵ��=ԭ����ֵ+֧Ԯ���ӳ�

  int totalXunlian = 0;//ѵ��1+ѵ��2+...
  int totalGanjing = 0;//�ɾ�1+�ɾ�2+...
  double totalYouqingMultiplier = 1.0;//(1+����1)*(1+����2)*...
  int vitalCostBasic;//�������Ļ�������=ReLU(������������+link������������-�ǲ��������ļ���)
  double vitalCostMultiplier = 1.0;//(1-�������ļ�����1)*(1-�������ļ�����2)*...
  double failRateMultiplier = 1.0;//(1-ʧ�����½���1)*(1-ʧ�����½���2)*...

  int tlevel = getTrainingLevel(tra);


  bool isCardShining_record[6] = { false,false,false,false,false,false };
  for (int h = 0; h < 5; h++)
  {
    int pIdx = personDistribution[tra][h];
    if (pIdx < 0)break;
    if (pIdx == PSID_npc)
    {
      headNum += 1;
      continue;
    }
    if (pIdx >= 6)continue;//����֧Ԯ��

    headNum += 1;
    const Person& p = persons[pIdx];
   
    if (isCardShining(pIdx, tra))
    {
      shiningNum += 1;
      isCardShining_record[pIdx] = true;
    }
    if (p.cardParam.isLink)
    {
      linkNum += 1;
    }
  }
  isTrainShining[tra] = shiningNum > 0;

  //����ֵ
  for (int i = 0; i < 6; i++)
    basicValue[i] = GameConstants::TrainingBasicValue[tra][tlevel][i];
  vitalCostBasic = -GameConstants::TrainingBasicValue[tra][tlevel][6];

  for (int h = 0; h < 5; h++)
  {
    int pid = personDistribution[tra][h];
    if (pid < 0)break;//û��
    if (pid >= 6)continue;//���ǿ�
    const Person& p = persons[pid];
    bool isThisCardShining = isCardShining_record[pid];//���ſ���û��
    bool isThisTrainingShining = shiningNum > 0;//���ѵ����û��
    CardTrainingEffect eff = p.cardParam.getCardEffect(*this, isThisCardShining, tra, p.friendship, p.cardRecord, headNum, shiningNum);
    
    for (int i = 0; i < 6; i++)//����ֵbonus
    {
      if (basicValue[i] > 0)
        basicValue[i] += int(eff.bonus[i]);
    }
    if (isCardShining_record[pid])//���ʣ�����ӳɺ��ǲʻظ�
    {
      totalYouqingMultiplier *= (1 + 0.01 * eff.youQing);
      if (tra == TRA_wiz)
        vitalCostBasic -= eff.vitalBonus;
    }
    totalXunlian += eff.xunLian;
    totalGanjing += eff.ganJing;
    vitalCostMultiplier *= (1 - 0.01 * eff.vitalCostDrop);
    failRateMultiplier *= (1 - 0.01 * eff.failRateDrop);

  }

  //������ʧ����
  if (mecha_overdrive_enabled && mecha_upgradeTotal[0] >= 15)
    vitalCostMultiplier *= 0.5;
  int vitalChangeInt = vitalCostBasic > 0 ? -int(vitalCostBasic * vitalCostMultiplier) : -vitalCostBasic;
  if (vitalChangeInt > maxVital - vital)vitalChangeInt = maxVital - vital;
  if (vitalChangeInt < -vital)vitalChangeInt = -vital;
  trainVitalChange[tra] = vitalChangeInt;
  failRate[tra] = calculateFailureRate(tra, failRateMultiplier);


  //��ͷ * ѵ�� * �ɾ� * ����    //֧Ԯ������
  double cardMultiplier = (1 + 0.05 * headNum) * (1 + 0.01 * totalXunlian) * (1 + 0.1 * (motivation - 3) * (1 + 0.01 * totalGanjing)) * totalYouqingMultiplier;
  //trainValueCardMultiplier[t] = cardMultiplier;

  //�²���Կ�ʼ����
  for (int i = 0; i < 6; i++)
  {
    bool isRelated = basicValue[i] != 0;
    double bvl = basicValue[i];
    double umaBonus = i < 5 ? 1 + 0.01 * fiveStatusBonus[i] : 1;
    trainValueLower[tra][i] = bvl * cardMultiplier * umaBonus;
  }


  //�в�Ȧ�ı��г���-----------------------------------------------------
  if (shiningNum > 0)
    mecha_hasGear[tra] = true;

  //���ϲ�-----------------------------------------------------

  double scenarioTrainMultiplier = 1.0;//�籾��ѵ���ӳ�

  //�о��ȼ��ӳ�
  double lvBonus = mecha_rivalLv[tra] > 1 ? 6 + 0.06 * mecha_rivalLv[tra] : 0;
  if (mecha_linkeffect_lvbonus)lvBonus *= 1.5;
  scenarioTrainMultiplier *= (1 + 0.01 * lvBonus);

  //�г��ֵ�ѵ���мӳ�
  if (mecha_hasGear[tra])
  {
    double gearBonus =
      turn < 12 ? 3 :
      turn < 24 ? 6 :
      turn < 36 ? 10 :
      turn < 48 ? 16 :
      turn < 60 ? 20 :
      turn < 72 ? 25 :
      30;
    scenarioTrainMultiplier *= (1 + 0.01 * gearBonus);
  }

  //�ص�3������������ӳ�
  if(shiningNum > 0) 
  {
    double friendshipBonus = 2 * mecha_upgrade[1][2];
    scenarioTrainMultiplier *= (1 + 0.01 * friendshipBonus);
  }

  //overdrive
  if (mecha_overdrive_enabled)
  {
    //����25%
    scenarioTrainMultiplier *= 1.25;

    //��3����12��
    double headBonus = 
      mecha_upgradeTotal[1] >= 12 ? 3 : 
      mecha_upgradeTotal[1] >= 3 ? 1 : 
      0;

    scenarioTrainMultiplier *= (1 + 0.01 * headNum * headBonus); 
  }



  //�ϲ�=����-�²�

  for (int i = 0; i < 6; i++)
  {
    int lower = trainValueLower[tra][i];
    if (lower > 100) lower = 100;
    trainValueLower[tra][i] = lower;
    int total = int(lower * scenarioTrainMultiplier * mecha_trainingStatusMultiplier[i]);
    int upper = total - lower;
    if (upper > 100)upper = 100;
    if (i < 5)
    {
      lower = calculateRealStatusGain(fiveStatus[i], lower);//consider the integer over 1200
      upper = calculateRealStatusGain(fiveStatus[i] + lower, upper);
    }
    total = upper + lower;
    trainValue[tra][i] = total;
  }

  calculateLvGainSingle(tra, headNum, shiningNum > 0);
}

void Game::addYayoiJiBan(int value)
{
  if (friend_type == FriendType_yayoi)
    addJiBan(friend_personId, value, true);
  else
    addJiBan(PSID_noncardYayoi, value, true);
}

int Game::getYayoiJiBan() const
{
  if (friend_type == FriendType_yayoi)
    return persons[friend_personId].friendship;
  else
    return friendship_noncard_yayoi;
}

void Game::checkEventAfterTrain(std::mt19937_64& rand)
{
  mecha_overdrive_enabled = false;
  checkFixedEvents(rand);
  checkRandomEvents(rand);


  //�غ���+1
  turn++;
  isRacing = isRacingTurn[turn];
  gameStage = GameStage_beforeTrain;
  if (turn >= TOTAL_TURN)
  {
    printEvents("���ɽ���!");
    printEvents("��ĵ÷��ǣ�" + to_string(finalScore()));
  }

}
void Game::checkFixedEvents(std::mt19937_64& rand)
{
  //������̶ֹ��¼�

  if (isRefreshMind)
  {
    addVital(5);
    if (rand() % 4 == 0) //����ÿ�غ���25%����buff��ʧ
      isRefreshMind = false;
  }
  if (isRacing)//���ı���
  {
    if (turn < 72)
    {
      runRace(3, 45);
      addYayoiJiBan(4);
    }
    else if (turn == 73)//ura1
    {
      runRace(10, 40);
    }
    else if (turn == 75)//ura1
    {
      runRace(10, 60);
    }
    else if (turn == 77)//ura3
    {
      runRace(10, 80);
    }

  }

  if (turn == 11)//������
  {
    assert(isRacing);
  }
  else if (turn == 23)//��һ�����
  {
    //����¼���������ѡ������������ѡ����
    {
      int vitalSpace = maxVital - vital;//�������������
      handleFriendFixedEvent();
      if (vitalSpace >= 20)
        addVital(20);
      else
        addAllStatus(5);
    }
    printEvents("��һ�����");
  }
  else if (turn == 29)//�ڶ���̳�
  {

    for (int i = 0; i < 5; i++)
      addStatus(i, zhongMaBlueCount[i] * 6); //�����ӵ���ֵ

    double factor = double(rand() % 65536) / 65536 * 2;//�籾�������0~2��
    for (int i = 0; i < 5; i++)
      addStatus(i, int(factor*zhongMaExtraBonus[i])); //�籾����
    skillPt += int((0.5 + 0.5 * factor) * zhongMaExtraBonus[5]);//���߰��㼼�ܵĵ�Чpt

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += zhongMaBlueCount[i] * 2; //��������--�������ֵ��18�����μ̳й��Ӵ�Լ36���ޣ�ÿ��ÿ��������+1���ޣ�1200�۰��ٳ�2

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += rand() % 8; //��������--�����μ̳��������

    printEvents("�ڶ���̳�");
  }
  else if (turn == 35)
  {
    printEvents("�ڶ�����޿�ʼ");
  }
  else if (turn == 47)//�ڶ������
  {
    //����¼���������ѡ������������ѡ����
    {
      int vitalSpace = maxVital - vital;//�������������
      if (vitalSpace >= 30)
        addVital(30);
      else
        addAllStatus(8);
    }
    printEvents("�ڶ������");
  }
  else if (turn == 48)//�齱
  {
    int rd = rand() % 100;
    if (rd < 16)//��Ȫ��һ�Ƚ�
    {
      addVital(30);
      addAllStatus(10);
      addMotivation(2);

      printEvents("�齱�����������Ȫ/һ�Ƚ�");
    }
    else if (rd < 16 + 27)//���Ƚ�
    {
      addVital(20);
      addAllStatus(5);
      addMotivation(1);
      printEvents("�齱��������˶��Ƚ�");
    }
    else if (rd < 16 + 27 + 46)//���Ƚ�
    {
      addVital(20);
      printEvents("�齱������������Ƚ�");
    }
    else//��ֽ
    {
      addMotivation(-1);
      printEvents("�齱��������˲�ֽ");
    }
  }
  else if (turn == 49)
  {
    skillScore += 170;
    printEvents("���еȼ�+1");
  }
  else if (turn == 53)//������̳�
  {
    for (int i = 0; i < 5; i++)
      addStatus(i, zhongMaBlueCount[i] * 6); //�����ӵ���ֵ

    double factor = double(rand() % 65536) / 65536 * 2;//�籾�������0~2��
    for (int i = 0; i < 5; i++)
      addStatus(i, int(factor * zhongMaExtraBonus[i])); //�籾����
    skillPt += int((0.5 + 0.5 * factor) * zhongMaExtraBonus[5]);//���߰��㼼�ܵĵ�Чpt

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += zhongMaBlueCount[i] * 2; //��������--�������ֵ��18�����μ̳й��Ӵ�Լ36���ޣ�ÿ��ÿ��������+1���ޣ�1200�۰��ٳ�2

    for (int i = 0; i < 5; i++)
      fiveStatusLimit[i] += rand() % 8; //��������--�����μ̳��������

    printEvents("������̳�");

    if (getYayoiJiBan() >= 60)
    {
      skillScore += 170;//���м��ܵȼ�+1
      addMotivation(1);
    }
    else
    {
      addVital(-5);
      skillPt += 25;
    }
  }
  else if (turn == 59)
  {
    printEvents("��������޿�ʼ");
  }
  else if (turn == 70)
  {
    skillScore += 170;//���м��ܵȼ�+1
  }
  else if (turn == 77)//ura3����Ϸ����
  {
    //�����Ѿ���ǰ�洦����
    //����
    if (friendship_noncard_reporter >= 80)
    {
      addAllStatus(5);
      skillPt += 20;
    }
    else if (friendship_noncard_reporter >= 60)
    {
      addAllStatus(3);
      skillPt += 10;
    }
    else if (friendship_noncard_reporter >= 40)
    {
      skillPt += 10;
    }
    else
    {
      skillPt += 5;
    }

    bool allWin = true;

    for (int c = 0; c < 5; c++)
    {
      if (mecha_win_history[c] != 2)
        allWin = false;
    }
    if (allWin)
    {
      skillPt += 40;//�籾��
      addAllStatus(60);
      skillPt += 150;
    }
    else 
    {
      addAllStatus(25);
      //there should be something, but not important
    }


    //���˿��¼�
    handleFriendFixedEvent();

    addAllStatus(5);
    skillPt += 20;

    printEvents("ura3��������Ϸ����");
  }
}

void Game::checkRandomEvents(std::mt19937_64& rand)
{
  if (turn >= 72)
    return;//ura�ڼ䲻�ᷢ����������¼�

  //���˻᲻���������
  if (friend_type != 0)
  {
    Person& p = persons[friend_personId];
    assert(p.personType == PersonType_friendCard);
    if (friend_stage==FriendStage_beforeUnlockOutgoing)
    {
      double unlockOutgoingProb = p.friendship >= 60 ?
        GameConstants::FriendUnlockOutgoingProbEveryTurnHighFriendship :
        GameConstants::FriendUnlockOutgoingProbEveryTurnLowFriendship;
      if (randBool(rand, unlockOutgoingProb))//����
      {
        handleFriendUnlock(rand);
      }
    }
  }

  //ģ���������¼�

  //֧Ԯ�������¼��������һ������5�
  if (randBool(rand, GameConstants::EventProb))
  {
    int card = rand() % 6;
    addJiBan(card, 5, false);
    //addAllStatus(4);
    addStatus(rand() % 5, eventStrength);
    skillPt += eventStrength;
    printEvents("ģ��֧Ԯ������¼���" + persons[card].cardParam.cardName + " ���+5��pt���������+" + to_string(eventStrength));

    //֧Ԯ��һ����ǰ�����¼�������
    if (randBool(rand, 0.4 * (1.0 - turn * 1.0 / TOTAL_TURN)))
    {
      addMotivation(1);
      printEvents("ģ��֧Ԯ������¼�������+1");
    }
    if (randBool(rand, 0.5))
    {
      addVital(10);
      printEvents("ģ��֧Ԯ������¼�������+10");
    }
    else if (randBool(rand, 0.03))
    {
      addVital(-10);
      printEvents("ģ��֧Ԯ������¼�������-10");
    }
    if (randBool(rand, 0.03))
    {
      isPositiveThinking = true;
      printEvents("ģ��֧Ԯ������¼�����á�����˼����");
    }
  }

  //ģ����������¼�
  if (randBool(rand, 0.1))
  {
    addAllStatus(3);
    printEvents("ģ����������¼���ȫ����+3");
  }

  //������
  if (randBool(rand, 0.10))
  {
    addVital(5);
    printEvents("ģ������¼�������+5");
  }

  //��30�������Է��¼���
  if (randBool(rand, 0.02))
  {
    addVital(30);
    printEvents("ģ������¼�������+30");
  }

  //������
  if (randBool(rand, 0.02))
  {
    addMotivation(1);
    printEvents("ģ������¼�������+1");
  }

  //������
  if (turn >= 12 && randBool(rand, 0.04))
  {
    addMotivation(-1);
    printEvents("ģ������¼���\033[0m\033[33m����-1\033[0m\033[32m");
  }

}
void Game::applyAction(std::mt19937_64& rand, Action action)
{
  if (isEnd()) return;
  if (action.type == GameStage_beforeMechaUpgrade)
  {
    throw "todo";
  }
  else
  {
    if (action.overdrive)//dish only, not next turn
    {
      bool suc = mecha_activate_overdrive();
      assert(suc && "Game::applyAction �޷�����overdrive");
    }
    if (action.train != TRA_none)
    {
      bool suc = applyTraining(rand, action.train);
      assert(suc && "Game::applyActionѡ���˲��Ϸ���ѵ��");

      checkEventAfterTrain(rand);
      if (isEnd()) return;

      randomDistributeCards(rand);


      //��ura�ı����غ�Ҳ���ܳԲˣ�����ˢpt�����Բ�����

      //if (isRacing && !isUraRace)//��ura�ı����غϣ�ֱ��������һ���غ�
      //{
      //  Action emptyAction;
      //  emptyAction.train = TRA_none;
      //  emptyAction.dishType = DISH_none;
      //  applyAction(rand, emptyAction);
      //}
    }
  }
}

bool Game::isCardShining(int personIdx, int trainIdx) const
{
  const Person& p = persons[personIdx];
  if(p.personType==PersonType_card)
  { 
    return p.friendship >= 80 && trainIdx == p.cardParam.cardType;
  }
  else if (p.personType == PersonType_groupCard)
  {
    throw "other friends or group cards are not supported";
  }
  return false;
}

