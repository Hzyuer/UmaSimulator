#pragma once
#include <cstdint>
#include <random>
#include <string>

enum TrainActionTypeEnum :int16_t
{
  TRA_speed = 0,
  TRA_stamina,
  TRA_power,
  TRA_guts,
  TRA_wiz,
  TRA_rest,
  TRA_outgoing,
  TRA_race,
  TRA_none = -1, //��Action��ѵ����ֻ����
  TRA_redistributeCardsForTest = -2 //ʹ��������ʱ��˵��ҪrandomDistributeCards�����ڲ���ai��������Search::searchSingleActionThread��ʹ��
};
enum DishTypeEnum :int16_t
{
  DISH_none = 0,
  DISH_sandwich, //speed+power+wiz 25%
  DISH_curry, //speed+stamina+guts 25%
  DISH_speed1,//150+80  60%
  DISH_stamina1,
  DISH_power1,
  DISH_guts1,
  DISH_wiz1,
  DISH_speed2,//250+80  90%~100%
  DISH_stamina2,
  DISH_power2,
  DISH_guts2,
  DISH_wiz2,
  DISH_g1plate //5*80
};
//���ǵ�����籾����������ԣ����ܻ�Ӱ��ϣ��ѡ��ѵ��
//һ���غϿ��Բ�������������ˣ���ѵ����Ҳ���Բ���
//Action���ʾ���ˣ�����ѵ������������+ѵ��
struct Action 
{
  static const std::string trainingName[8];
  static const std::string dishName[14];
  static const Action Action_RedistributeCardsForTest;

  
  int16_t dishType;//���ˣ�0Ϊ������

  int16_t train;//-1��ʱ��ѵ����01234���������ǣ�5�����6��Ϣ��7���� 
  //ע��������������������û������ͨ��������ṩѡ��

  int toInt() const;
  std::string toString() const;
  static Action intToAction(int i);
};