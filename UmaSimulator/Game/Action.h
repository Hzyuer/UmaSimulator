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
  TRA_outgoing, //�������޵ġ���Ϣ&�����
  TRA_race,
  TRA_none = -1, //��Action��ѵ����ֻ����
  //TRA_redistributeCardsForTest = -2 //ʹ��������ʱ��˵��ҪrandomDistributeCards�����ڲ���ai��������Search::searchSingleActionThread��ʹ��
};

struct Action 
{
  static const std::string trainingName[8];
  static Action RedistributeCardsForTest();
  static const int MAX_ACTION_TYPE = 14 + 36;//��׼��Action����һһ��Ӧ
  
  //���type��Game::stage��Ӧ
  //0 δ��ʼ��
  //1 ��ͨѵ���������overdrive����Ծ����Ƿ񿪣�Ҳ�����ȿ�overdrive����ʱ��ѡѵ�������޵������°������15���أ�����Ҫ�Ⱦ����Ƿ�overdrive����ҡ�˽����ѡѵ������ѡ�����Ϊ8+5+1
  //2 �����غϡ�ֻ������3����action��ֻ����ͷ���أ���=��-ͷ-�أ���ѡ�����Ϊ6*6=36
  // ����3���Ĳ���ֱ����д�߼�
  //-1 ʹ��������ʱ��˵��ҪrandomDistributeCards�����ڲ���ai��������Search::searchSingleActionThread��ʹ��
  int16_t type;
  
  bool overdrive;//�Ƿ�overdrive�����֣�ѵ��������Ѿ����ˣ������Ϊfalse
  
  int16_t train;//-1��ʱ��ѵ����01234���������ǣ�5�����6��Ϣ��7���� 
  //ע��������������������û������ͨ��������ṩѡ��

  int8_t mechaHead;//ͷ������
  int8_t mechaChest;//�ز�����
  //inline int8_t mechaFoot(int8_t total) {
  //  return total - mechaHead - mechaChest;
  //}

  Action();
  Action(int id);

  bool isActionStandard() const;
  int toInt() const;
  std::string toString() const;
};