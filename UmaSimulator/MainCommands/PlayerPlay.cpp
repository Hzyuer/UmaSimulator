#include <iostream>
#include <iomanip> 
#include <random>
#include <sstream>
#include <cassert>
#include <thread>  // for std::this_thread::sleep_for
#include <chrono>  // for std::chrono::seconds
#include "../External/termcolor.hpp"
#include "../Game/Game.h"
#include "../Search/Search.h"
#include "../NeuralNet/TrainingSample.h"
#include "../Selfplay/GameGenerator.h"
using namespace std;


void main_playerPlay()
{
  GameDatabase::loadUmas("./db/umaDB.json");
  //GameDatabase::loadCards("../db/card");
  GameDatabase::loadDBCards("./db/cardDB.json");

  const int threadNum = 8;
  const int searchN = 8192;
  const double radicalFactor = 5;
  SearchParam param(searchN, radicalFactor);


  cout << termcolor::cyan << "�������ֲ˱��籾����ģ���� v0.1" << termcolor::reset << endl;
  cout << termcolor::cyan << "���� Sigmoid��QQ: 2658628026" << termcolor::reset << endl;
  cout << termcolor::cyan << "���뿪Դ��" << termcolor::yellow << "https://github.com/hzyhhzy/UmaAi" << termcolor::reset << endl;
  cout << termcolor::bright_cyan << "��ģ�����������ơ�С�ڰ塱��Ϊ�˷��㣬��û�����ܵĹ��ܣ��ѹ��м��ܺ͸��ּ���hint�������pt��ÿpt��Ϊ" << GameConstants::ScorePtRateDefault << "�֣�����" << GameConstants::ScorePtRateDefault * 1.1 << "�֣�" << termcolor::reset << endl;
  cout << termcolor::bright_cyan << "����ũ����������Զ�����" << termcolor::reset << endl;
  cout << endl;

  random_device rd;
  auto rand = mt19937_64(rd());

  int umaId = 101101;//���Ϸ�
  int umaStars = 5;
  int cards[6] = { 302074,302064,302084,301874,300194,301724 };//���ˣ���ǿ��������Ұ�������޽�������������������
  int zhongmaBlue[5] = { 18,0,0,0,0 };
  int zhongmaBonus[6] = { 5,15,25,5,5,150 };

  int batchsize = 512;


#if USE_BACKEND == BACKEND_LIBTORCH
  const string modelpath = "../training/example/model_traced.pt";
#elif USE_BACKEND == BACKEND_NONE
  const string modelpath = "";
#else
  const string modelpath = "../training/example/model.txt";
#endif

  Model* modelptr = NULL;
  Model model(modelpath, batchsize);
  if (modelpath != "")
  {
    modelptr = &model;
  }


  for (int gamenum = 0; gamenum < 100000; gamenum++)
  {
    Search search(modelptr, batchsize, threadNum, param);
    Game game;
    game.newGame(rand, true, umaId, umaStars, cards, zhongmaBlue, zhongmaBonus);
    //game = gameGenerator.get();
    //for (int i = 0; i < 36; i++)
    //{
    //  Action act = { 4,false, false, false, false };
    //  if (game.larc_ssPersonsCount >= 5)
    //    act.train = 5;
    //  game.applyTrainingAndNextTurn(rand, act);
    //}


    cout << termcolor::bright_blue << "------------------------------------------------------------------------------------------------" << termcolor::reset << endl;
    cout << termcolor::green << "���������ǣ�" << GameDatabase::AllUmas[umaId].name << termcolor::reset << endl;
    cout << termcolor::green << "����俨�ǣ�";
    for (int i = 0; i < 6; i++)
      cout << GameDatabase::AllCards[cards[i]].cardName << ",";
    cout << termcolor::reset << endl;
    {
      cout << termcolor::bright_cyan << "��Enter����ʼ��Ϸ" << termcolor::reset << endl;
      if (gamenum != 0)std::cin.ignore(1000000, '\n');
      std::cin.get();
    }
    cout << endl;


    while(game.turn < TOTAL_TURN)
    {
      std::this_thread::sleep_for(std::chrono::seconds(1));//�ȼ������˿����
      //assert(turn == game.turn && "�غ�������ȷ");
      game.print();


      /*
      if (game.turn < TOTAL_TURN - 1){

        //  std::cout << "????? -- turn: " << game.turn << "  tot_turn-1 : "<< TOTAL_TURN - 1 << "-----------------\n";
        //assert(true && "todo");
        
        Action handWrittenStrategy = Evaluator::handWrittenStrategy(game);
        string strategyText[10] =
        {
          "��",
          "��",
          "��",
          "��",
          "��",
          "SS",
          "��Ϣ",
          "�������",
          "��ͨ���",
          "����"
        };
        cout << "��д�߼���" << strategyText[handWrittenStrategy.train];
        if (game.larc_isAbroad)
        {
          cout << "   ";
          if (!handWrittenStrategy.buy50p)
            cout << "��";
          cout << "����+50%";
        }
        cout << endl;

        game.playerPrint = false;
        search.runSearch(game, rand);

        game.playerPrint = true;
        for (int i = 0; i < Search::buyBuffChoiceNum(game.turn); i++)
        {
          if (Search::buyBuffChoiceNum(game.turn) > 1 && i == 0)
            cout << "����:              ";
          if (i == 1)
            cout << "��+50%:            ";
          if (i == 2 && game.turn < 50)
            cout << "��pt+10:           ";
          if (i == 2 && game.turn >= 50)
            cout << "������-20%:        ";
          if (i == 3 && game.turn < 50)
            cout << "��+50%��pt+10:     ";
          if (i == 3 && game.turn >= 50)
            cout << "��+50%������-20%:  ";
          cout << "����������: ";
          for (int j = 0; j < 10; j++)
          {
            double score = search.allChoicesValue[i][j].value;
            if (score > -20000)
              cout
              //<< fixed << setprecision(1) << search.allChoicesValue[i][j].winrate * 100 << "%:" 
              << fixed << setprecision(0) << score << " ";
            else
              cout << "-- ";
            if (j == 4)cout << " | SS:";
            if (j == 5)cout << " | ��Ϣ:";
            if (j == 6)cout << " �������:";
            if (j == 7)cout << " ��ͨ���:";
            if (j == 8)cout << " ����:";
          }
          cout << endl;
        }
      
      }*/
      /*

      {
        auto policy = search.extractPolicyFromSearchResults(1);
        cout << fixed << setprecision(1) << policy.useVenusPolicy * 100 << "% ";
        cout << endl;
        for (int i = 0; i < 8; i++)
          cout << fixed << setprecision(1) << policy.trainingPolicy[i] * 100 << "% ";
        cout << endl;
        for (int i = 0; i < 3; i++)
          cout << fixed << setprecision(1) << policy.threeChoicesEventPolicy[i] * 100 << "% ";
        cout << endl;
        for (int i = 0; i < 6; i++)
          cout << fixed << setprecision(1) << policy.outgoingPolicy[i] * 100 << "% ";
        cout << endl;
      }
      */

     // auto tdata = search.exportTrainingSample();

      Action action;
      action.train = -1;
      string dishKeys[14] = { "0","a1","a2","b1","b2","b3","b4","b5","c1","c2","c3","c4","c5","d" };//13�ֲ˶�Ӧ�ļ�������

      string s;
      if (game.isRacing)
      {
        cout << termcolor::green << "�����غϣ�" << termcolor::reset <<
          termcolor::cyan << "0" << termcolor::reset << ":���Բ˲����� " <<
          termcolor::cyan << "remake" << termcolor::reset << ":�ؿ� " <<
          termcolor::cyan << "cheat" << termcolor::reset << ":���ñ������� " <<
          endl;
        
      }
      else
      {
        cout << termcolor::green << "��ѡ��ѵ����" << termcolor::reset <<
          termcolor::cyan << "1" << termcolor::reset << ":�� " <<
          termcolor::cyan << "2" << termcolor::reset << ":�� " <<
          termcolor::cyan << "3" << termcolor::reset << ":�� " <<
          termcolor::cyan << "4" << termcolor::reset << ":�� " <<
          termcolor::cyan << "5" << termcolor::reset << ":�� " <<
          termcolor::cyan << "6" << termcolor::reset << ":��Ϣ " <<
          termcolor::cyan << "7" << termcolor::reset << ":���(��������) " <<
          termcolor::cyan << "8" << termcolor::reset << ":���� " <<
          termcolor::cyan << "remake" << termcolor::reset << ":�ؿ� " <<
          termcolor::cyan << "cheat" << termcolor::reset << ":������ͷ�ֲ� " <<
          endl;
      }

      //��ʾ���ԳԵĲ�
      if (game.cook_dish == DISH_none)
      {
        if (game.isRacing)
          cout << termcolor::green << "�Բ˲�������" << termcolor::reset;
        else
          cout << termcolor::green << "�Բˣ�" << termcolor::reset;
        int legalDishNum = 0;
        for (int i = 1; i < 14; i++)
        {
          if (!game.isDishLegal(i))
            continue;
          legalDishNum += 1;
          cout << termcolor::cyan << dishKeys[i] << termcolor::reset << ":" << Action::dishName[i] << " ";
        }
        if (legalDishNum == 0)
          cout << termcolor::red << "û�п��ԳԵĲ�" << termcolor::reset;
        cout << endl;
      }

      cin >> s;

      //s�ǲ��ǳԲ�
      bool isDish = false;
      for (int i = 1; i < 14; i++)
      {
        if (s == dishKeys[i])
        {
          action.dishType = i;
          action.train = TRA_none;
          isDish = true;
        }
      }
      if (!isDish)
      {
        action.dishType = 0;
        if (game.isRacing && s == "0")
          action.train = TRA_race;
        else if (s == "1")
          action.train = TRA_speed;
        else if (s == "2")
          action.train = TRA_stamina;
        else if (s == "3")
          action.train = TRA_power;
        else if (s == "4")
          action.train = TRA_guts;
        else if (s == "5")
          action.train = TRA_wiz;
        else if (s == "6")
        {
          cout << termcolor::green << "��ȷ��Ҫ��Ϣ������yȷ�ϣ�����n����ѡ��" << termcolor::reset << endl;
          cin >> s;
          if (s != "y")
            continue;
          action.train = TRA_rest;
        }
        else if (s == "7")
        {
          cout << termcolor::green << "��ȷ��Ҫ���������yȷ�ϣ�����n����ѡ��" << termcolor::reset << endl;
          cin >> s;
          if (s != "y")
            continue;
          action.train = TRA_outgoing;
        }
        else if (s == "8")
        {
          cout << termcolor::green << "��ȷ��Ҫ����������yȷ�ϣ�����n����ѡ��" << termcolor::reset << endl;
          cin >> s;
          if (s != "y")
            continue;
          action.train = TRA_race;
        }
        else if (s == "remake")
        {
          cout << termcolor::bright_red << "��ȷ��Ҫ�ؿ�������remakeȷ���ؿ������������������ݼ�����Ϸ" << termcolor::reset << endl;
          cin >> s;
          if (s != "remake")
            continue;
          cout << termcolor::red << "���" << termcolor::green << GameDatabase::AllUmas[umaId].name << termcolor::red << "�������⺺����" << termcolor::reset << endl;
          break;
        }
        else if (s == "cheat")//���ÿ������
        {
          cout << termcolor::bright_cyan << "�������·��䣡" << termcolor::reset << endl;
          game.randomDistributeCards(rand);
          //game.print();
          continue;
        }
        else
        {
          cout << termcolor::red << "������������������" << termcolor::reset << endl;
          continue;
        }
      }
      if (!game.isLegal(action))
      {
        cout << termcolor::red << "����������Ϸ�������������" << termcolor::reset << endl;
        continue;
      }
      game.applyAction(rand, action);
      cout << endl;
      if (game.isEnd())break;
    }
    
    cout << termcolor::red << "���ɽ�����" << termcolor::reset << endl;
    game.printFinalStats();
  }
}