#include <cassert>
#include <iostream>
#include "Evaluator.h"
#include "../Search/Search.h"


const double statusWeights[5] = { 5.0,5.0,5.0,5.0,5.0 };
const double jibanValue = 12;
const double vitalFactorStart = 5;
const double vitalFactorEnd = 8;

const double reserveStatusFactor = 30;//������ʱ��ÿ�غ�Ԥ�����٣���0�����ӵ��������

const double smallFailValue = -150;
const double bigFailValue = -500;
const double outgoingBonusIfNotFullMotivation = 150;//������ʱ����������
const double nonTrainBonus = 100;//��غϲ�ѵ���»غ�+3��bonus���� ʣ��غ���/TOTAL_TURN
const double raceBonus = 200;//�������棬����������


//const double xiangtanExhaustLossMax = 800;//��̸�ľ���û���Ĺ�ֵ�۷�

//һ���ֶκ���������������
inline double statusSoftFunction(double x, double reserve, double reserveInvX2)//reserve�ǿ����Ա����ռ䣨����Ȩ�أ���reserveInvX2��1/(2*reserve)
{
  if (x >= 0)return 0;
  if (x > -reserve)return -x * x * reserveInvX2;
  return x + 0.5 * reserve;
}

static void statusGainEvaluation(const Game& g, double* result) { //result����������ѵ���Ĺ�ֵ
  int remainTurn = TOTAL_TURN - g.turn - 2;//���ѵ�����м���ѵ���غ�
  if (remainTurn == 2)remainTurn = 1;//ura�ڶ��غ�
  else if (remainTurn >= 4)remainTurn -= 2;//ura��һ�غ�

  double reserve = reserveStatusFactor * remainTurn * (1 - double(remainTurn) / (TOTAL_TURN * 2));
  double reserveInvX2 = 1 / (2 * reserve);

  double finalBonus0 = 60;
  assert(false && "not implemented");
  finalBonus0 += 20;//ura3�������¼�
  if (remainTurn >= 1)finalBonus0 += 15;//ura2
  if (remainTurn >= 2)finalBonus0 += 15;//ura1

  double remain[5]; //ÿ�����Ի��ж��ٿռ�

  for (int i = 0; i < 5; i++)
  {
    remain[i] = g.fiveStatusLimit[i] - g.fiveStatus[i] - finalBonus0;
  }

  if (g.friend_type != 0)
  {
    remain[0] -= 36;
    remain[4] -= 36;
  }


  for (int tra = 0; tra < 5; tra++)
  {
    double res = 0;
    for (int sta = 0; sta < 5; sta++)
    {
      double s0 = statusSoftFunction(-remain[sta], reserve, reserveInvX2);
      double s1 = statusSoftFunction(g.trainValue[tra][sta] - remain[sta], reserve, reserveInvX2);
      res += statusWeights[sta] * (s1 - s0);
    }
    res += g.ptScoreRate * g.trainValue[tra][5];
    result[tra] = res;
  }
}




static double calculateMaxVitalEquvalant(const Game& g)
{
  int t = g.turn;
  if (t == TOTAL_TURN - 2)//uaf3
    return 0;
  if (t == TOTAL_TURN - 4)//uaf2
    return 50;
  if (t == TOTAL_TURN - 6)//uaf1
    return 65;

  assert(false && "not implemented");

}

static double vitalEvaluation(int vital, int maxVital)
{
  if (vital <= 50)
    return 2.0 * vital;
  else if (vital <= 70)
    return 1.5 * (vital - 50) + vitalEvaluation(50, maxVital);
  else if (vital <= maxVital)
    return 1.0 * (vital - 70) + vitalEvaluation(70, maxVital);
  else
    return vitalEvaluation(maxVital, maxVital);
}


Action Evaluator::handWrittenStrategy(const Game& game)
{
  assert(false && "not implemented");
  return Action();
  /*
  Action bestAction = { -1,0 };
  if (game.isEnd())return bestAction;
  double bestValue = -1e4;
  Game g = game;

  double vitalFactor = vitalFactorStart + (game.turn / double(TOTAL_TURN)) * (vitalFactorEnd - vitalFactorStart);

  int maxVitalEquvalant = calculateMaxVitalEquvalant(game);
  double vitalEvalBeforeTrain = vitalEvaluation(std::min(maxVitalEquvalant, int(game.vital)), game.maxVital);

  int turnNumBeforeRefresh = countTurnNumBeforeXiangtanRefresh(game);
  double xiangtanEvalBasic = xiangtanEvalBasicStart + (game.turn / double(TOTAL_TURN)) * (xiangtanEvalBasicEnd - xiangtanEvalBasicStart);
  double xiangtanCost[4];
  xiangtanCost[0] = 0.0;
  for (int i = 1; i <= game.uaf_xiangtanRemain; i++)
  {
    xiangtanCost[i] = xiangtanCost[i - 1] + xiangtanEvalBasic * xiangtanRemainEvalTable[game.uaf_xiangtanRemain - i][turnNumBeforeRefresh - 1];
  }

  double nonTrainE = nonTrainEvaluation(game);

  //���/��Ϣ
  {
    bool isFriendOutgoingAvailable =
      game.lianghua_type != 0 &&
      game.persons[game.lianghua_personId].friendOrGroupCardStage >= 2 &&
      game.lianghua_outgoingUsed < 5 &&
      (!game.isXiahesu());
    Action action = { TRA_rest,0 };
    if (isFriendOutgoingAvailable)action.train = TRA_outgoing;//������������������������Ϣ

    int vitalGain = isFriendOutgoingAvailable ? 50 : game.isXiahesu() ? 40 : 50;
    bool addMotivation = game.motivation < 5 && (isFriendOutgoingAvailable || game.isXiahesu());

    int vitalAfterRest = std::min(maxVitalEquvalant, vitalGain + game.vital);
    double value = vitalFactor * (vitalEvaluation(vitalAfterRest, game.maxVital) - vitalEvalBeforeTrain);
    if (addMotivation)value += outgoingBonusIfNotFullMotivation;

    value += nonTrainE;

    if (PrintHandwrittenLogicValueForDebug)
      std::cout << action.toString() << " " << value << std::endl;
    if (value > bestValue)
    {
      bestValue = value;
      bestAction = action;
    }
  }
  //����
  Action raceAction = { TRA_race,0 };
  if(game.isLegal(raceAction))
  {
    double value = raceBonus;


    int vitalAfterRace = std::min(maxVitalEquvalant, -15 + game.vital);
    value += vitalFactor * (vitalEvaluation(vitalAfterRace, game.maxVital) - vitalEvalBeforeTrain);

    value += nonTrainE;

    if (PrintHandwrittenLogicValueForDebug)
      std::cout << raceAction.toString() << " " << value << std::endl;
    if (value > bestValue)
    {
      bestValue = value;
      bestAction = raceAction;
    }
  }


  //ѵ��
  for (int xt = 0; xt < 10; xt++)
  {
    if (!game.isXiangtanLegal(xt))
      continue;
    g = game;
    if (xt != XT_none)
      g.xiangtanAndRecalculate(xt, true);

    double levelGainE[3], statusGainE[5];
    levelGainEvaluation(g, levelGainE);
    statusGainEvaluation(g, statusGainE);


    for (int tra = 0; tra < 5; tra++)
    {
      Action action = { tra,xt };
      Action actionNoXt = { tra,XT_none };
      if (!g.isLegal(actionNoXt))continue; //��ʱ������һ�η���̸��������䣬���Ǻ�����legal�����б���
      double value = statusGainE[tra]+levelGainE[g.uaf_trainingColor[tra]];


      //����hint���
      int cardHintNum = 0;//����hint���ȡһ�������Դ�ֵ�ʱ��ȡƽ��
      for (int j = 0; j < 5; j++)
      {
        int p = g.personDistribution[tra][j];
        if (p < 0)break;//û��
        if (g.persons[p].isHint)
          cardHintNum += 1;
      }
      double hintProb = 1.0 / cardHintNum;
      double yellowBuffFactor = g.uaf_buffNum[2] > 0 ? 2.0 : 1.0;

      for (int j = 0; j < 5; j++)
      {
        int pi = g.personDistribution[tra][j];
        if (pi < 0)break;//û��
        const Person& p = g.persons[pi];
        if (p.personType == 1)//���˿�
        {
          if (p.friendOrGroupCardStage == 0)
            value += 150;
          else if(p.friendship < 60)
            value += 100;
          else value += 40;
        }
        else if (p.personType == 2)
        {
          if (p.friendship < 80)
          {
            double jibanAdd = 7;
            if (g.isAiJiao)jibanAdd += 2;
            if (p.isHint)
            {
              jibanAdd += 5 * hintProb;
              if (g.isAiJiao)jibanAdd += 2 * hintProb;
            }
            jibanAdd = std::max(7.0, std::min(double(80 - p.friendship), jibanAdd));

            value += jibanAdd * jibanValue;
          }

          if (p.isHint)
          {
            for (int i = 0; i < 5; i++)
              value += p.cardParam.hintBonus[i] * statusWeights[i] * hintProb * yellowBuffFactor;
            value += p.cardParam.hintBonus[5] * g.ptScoreRate * hintProb * yellowBuffFactor;
          }
        }

      }




      int vitalAfterTrain = std::min(maxVitalEquvalant, g.trainVitalChange[tra] + g.vital);
      value += vitalFactor * (vitalEvaluation(vitalAfterTrain, g.maxVital) - vitalEvalBeforeTrain);

      //��ĿǰΪֹ����ѵ���ɹ���value
      double failRate = g.failRate[tra];
      if (failRate > 0)
      {
        double bigFailProb = failRate;
        if (failRate < 20)bigFailProb = 0;
        double failValueAvg = 0.01 * bigFailProb * bigFailValue + (1 - 0.01 * bigFailProb) * smallFailValue;
        
        value = 0.01 * failRate * failValueAvg + (1 - 0.01 * failRate) * value;
      }

      //��������̸����ɫbuff���۵���Ӧ����
      value -= xiangtanCost[Action::XiangtanNumCost[xt]];
      for(int i=0;i<3;i++)
        if (g.uaf_buffNum[i] > 0)
        {
          double buffLoss = colorBuffEvalRaw[i];
          if (i == 1)buffLoss *= (double(g.turn) / TOTAL_TURN);
          value -= buffLoss;
        }

      if (value > bestValue)
      {
        bestValue = value;
        bestAction = action;
      }
      if (PrintHandwrittenLogicValueForDebug)
        std::cout << action.toString() << " " << value << std::endl;
    }


  }
  return bestAction;
  */
}

