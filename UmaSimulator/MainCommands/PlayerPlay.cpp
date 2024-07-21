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
  GameDatabase::loadUmas("../db/umaDB.json");
  //GameDatabase::loadCards("../db/card");
  GameDatabase::loadDBCards("../db/cardDB.json");

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
  int cards[6] = { 301884,301344,301614,300194,300114,301074 };//���ˣ��߷壬��ӥ��������������˾��
  int zhongmaBlue[5] = { 18,0,0,0,0 };
  int zhongmaBonus[6] = { 20,0,40,0,20,150 };

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

      if (game.turn < TOTAL_TURN - 1){

          std::cout << "????? -- turn: " << game.turn << "  tot_turn-1 : "<< TOTAL_TURN - 1 << "-----------------\n";
        assert(true && "todo");
        /*
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
      */
      }
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

      if (game.isRacing)//�����غ�
      {
        assert(false && "�����غ�Ӧ���Ѿ���game��������");
      }
      else//����ѵ���غ�
      {
        Action action;
        action.train = -1;


        string s;
        assert(false && "todo");  
        cout << termcolor::cyan << "��ѡ��ѵ����1�٣�2�ͣ�3����4����5�ǣ�S��SS��ս��a���˳��У�b��ͨ���У�c��Ϣ��d���������remake�ؿ���cheat����" << termcolor::reset << endl;
       
        cin >> s;


        if (s == "1")
          action.train = 0;
        else if (s == "2")
          action.train = 1;
        else if (s == "3")
          action.train = 2;
        else if (s == "4")
          action.train = 3;
        else if (s == "5")
          action.train = 4;
        else if (s == "s")
          action.train = 5;
        else if (s == "a")
        {
          cout << termcolor::green << "��ȷ��Ҫ����������yȷ�ϣ�����n����ѡ��" << termcolor::reset << endl;
          cin >> s;
          if (s != "y")
            continue;
          action.train = 7;
        }
        else if (s == "b")
        {
          cout << termcolor::green << "��ȷ��Ҫ����������yȷ�ϣ�����n����ѡ��" << termcolor::reset << endl;
          cin >> s;
          if (s != "y")
            continue;
          action.train = 8;
        }
        else if (s == "c")
        {
          cout << termcolor::green << "��ȷ��Ҫ��Ϣ������yȷ�ϣ�����n����ѡ��" << termcolor::reset << endl;
          cin >> s;
          if (s != "y")
            continue;
          action.train = 6;
        }
        else if (s == "d")
        {
          cout << termcolor::green << "��ȷ��Ҫ����������yȷ�ϣ�����n����ѡ��" << termcolor::reset << endl;
          cin >> s;
          if (s != "y")
            continue;
          action.train = 9;
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
        else if (s.size() == 2 && s[0] == 'b' && (s[1] >= '1' && s[1] <= '8'))
        {
          int buy_idx = s[1] == '1' ? 3:
            s[1] == '2' ? 1:
            s[1] == '3' ? 2:
            s[1] == '4' ? 0:
            s[1] == '5' ? 4:
            s[1] == '6' ? 5:
            s[1] == '7' ? 6:
            s[1] == '8' ? 7:
            -1;
          continue;
        }
        else
        {
          cout << termcolor::red << "������������������" << termcolor::reset << endl;
          continue;
        }



















        /*

        if (game.venusAvailableWisdom != 0)
        {
          cout << termcolor::cyan << "�Ƿ���Ů����ǣ�y������n������" << termcolor::reset << endl;
          cin >> s;
          if (s == "y")
          {
            useVenus = true;
          }
          else if (s == "n")
          {
            useVenus = false;
          }
          else
          {
            cout << termcolor::red << "������������������" << termcolor::reset << endl;
            continue;
          }
        }


        if (chosenTrain == 7 && game.turn <= 12)
        {
          cout << termcolor::red << "ǰ13�غ��޷�����" << termcolor::reset << endl;
          continue;
        }




        int chosenOutgoing = 5;
        if (chosenTrain == 6 && game.venusCardUnlockOutgoing)
        {
          cout << termcolor::cyan << "��ѡ�������0Ϊ��ͨ��������Ů������ֱ�Ϊ 1 2 3 4 5" << termcolor::reset << endl;
          cin >> s;
          if (s == "0")
            chosenOutgoing = 5;
          else if (s == "1")
            chosenOutgoing = 0;
          else if (s == "2")
            chosenOutgoing = 1;
          else if (s == "3")
            chosenOutgoing = 2;
          else if (s == "4")
            chosenOutgoing = 3;
          else if (s == "5")
            chosenOutgoing = 4;
          else
          {
            cout << termcolor::red << "������������������" << termcolor::reset << endl;
            continue;
          }

          if (!game.isOutgoingLegal(chosenOutgoing))
          {
            cout << termcolor::red << "���Ϸ������������������" << termcolor::reset << endl;
            continue;
          }
        }


        assert(game.cardData[0]->cardType == 5 && "���ſ����ڵ�һ��λ��");


        if (chosenTrain >= 0 && chosenTrain < 5 && game.cardDistribution[chosenTrain][0])//���ſ���ѡ���ѵ��
        {
          if (chosenSpiritColor == -1)
          {
            cout << termcolor::cyan << "�������Ů����ѡһ�¼���ѡ��ʲô��ɫ����Ƭ��q�죬a����z��" << termcolor::reset << endl;
            cin >> s;
            if (s == "q")
            {
              chosenSpiritColor = 0;
            }
            else if (s == "a")
            {
              chosenSpiritColor = 1;
            }
            else if (s == "z")
            {
              chosenSpiritColor = 2;
            }
            else
            {
              cout << termcolor::red << "������������������" << termcolor::reset << endl;
              continue;
            }
          }
          else//��ǰѡ����Ƭ��
            cout << termcolor::cyan << "����ǰѡ����Ƭ��ɫ" << termcolor::reset << endl;

        }*/

        bool suc = game.applyTraining(rand, action.train);
        assert(suc);
        cout << endl;
        game.checkEventAfterTrain(rand);
        if(!game.isEnd())
          game.randomDistributeCards(rand);
      }
    }
    cout << termcolor::red << "���ɽ�����" << termcolor::reset << endl;
    game.printFinalStats();
  }
}