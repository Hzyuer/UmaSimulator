#pragma once
#include <random>
#include <array>
#include "../GameDatabase/GameDatabase.h"
#include "Person.h"
#include "Action.h"

struct SearchParam;


enum FarmUpgradeStrategyEnum :int16_t
{
  FUS_default,//Ĭ�ϣ����Գ��������ţ�
  FUS_noGarlicLv3,//������Lv3����
  FUS_garlicLv3,//����Lv3����
  FUS_none,//���Զ�����
};

enum scoringModeEnum :int16_t
{
  SM_normal,//��ͨ(���֡����۵�)ģʽ
  SM_race,//ͨ�ô���ģʽ
  SM_jjc,//������ģʽ
  SM_long,//������ģʽ
  SM_2400m,//2400mģʽ
  SM_2000m,//2000mģʽ
  SM_mile,//Ӣ��ģʽ
  SM_short,//�̾���ģʽ
  SM_debug //debugģʽ
};

enum personIdEnum :int16_t
{
  PSID_none = -1,//δ����
  PSID_noncardYayoi = 6,//�ǿ����³�
  PSID_noncardReporter = 7,//�ǿ�����
  PSID_npc = 8//NPC
};

enum gameStageEnum :int16_t
{
  GameStage_beforeTrain = 0,//ѵ�����������ǰ
  GameStage_afterTrain = 1,//ѵ���󣬴����¼�ǰ
};
struct Game
{
  //��ʾ���
  bool playerPrint;//�������ʱ����ʾ������Ϣ

  //��������

  float ptScoreRate;//ÿpt���ٷ�
  float hintPtRate;//ÿһ��hint�ȼ۶���pt
  int16_t eventStrength;//ÿ�غ��У����⣩���ʼ���ô�����ԣ�ģ��֧Ԯ���¼�
  int16_t farmUpgradeStrategy;//����ũ��Ĳ���
  int16_t scoringMode;//���ַ�ʽ

  //����״̬����������ǰ�غϵ�ѵ����Ϣ
  int32_t umaId;//�����ţ���KnownUmas.cpp
  bool isLinkUma;//�Ƿ�Ϊlink��
  bool isRacingTurn[TOTAL_TURN];//��غ��Ƿ����
  int16_t fiveStatusBonus[5];//�������ά���Եĳɳ���

  int16_t turn;//�غ�������0��ʼ����77����
  int16_t gameStage;//��Ϸ�׶Σ�0��ѵ��ǰ��1��ѵ����
  int16_t vital;//������������vital������Ϊ��Ϸ��������е�
  int16_t maxVital;//��������
  int16_t motivation;//�ɾ�����1��5�ֱ��Ǿ����������õ�

  int16_t fiveStatus[5];//��ά���ԣ�1200���ϲ�����
  int16_t fiveStatusLimit[5];//��ά�������ޣ�1200���ϲ�����
  int16_t skillPt;//���ܵ�
  int16_t skillScore;//�����ܵķ���
  int16_t trainLevelCount[5];//ѵ���ȼ�������ÿ��4�¼�һ��

  int16_t failureRateBias;//ʧ���ʸı�������ϰ����=-2����ϰ����=2
  bool isQieZhe;//���� 
  bool isAiJiao;//����
  bool isPositiveThinking;//�ݥ��ƥ���˼�������˵����γ���ѡ�ϵ�buff�����Է�һ�ε�����
  bool isRefreshMind;//+5 vital every turn

  int16_t zhongMaBlueCount[5];//����������Ӹ���������ֻ��3��
  int16_t zhongMaExtraBonus[6];//����ľ籾�����Լ����ܰ����ӣ���Ч��pt����ÿ�μ̳мӶ��١�ȫ��ʦ�����ӵ���ֵ��Լ��30��30��200pt
  
  bool isRacing;//����غ��Ƿ��ڱ���

  int16_t friendship_noncard_yayoi;//�ǿ����³��
  int16_t friendship_noncard_reporter;//�ǿ������

  Person persons[MAX_INFO_PERSON_NUM];//������6�ſ����ǿ����³������ߣ�NPC�ǲ���������person�࣬���һ��8
  int16_t personDistribution[5][5];//ÿ��ѵ������Щ��ͷid��personDistribution[�ĸ�ѵ��][�ڼ�����ͷ]����λ��Ϊ-1��0~5��6�ſ����ǿ����³�6������7��NPC�Ǳ��һ��8
  //int lockedTrainingId;//�Ƿ���ѵ�����Լ��������ĸ�ѵ���������Ȳ��ӣ���ai��������ʱ���ټӡ�

  int16_t saihou;//����ӳ�

  std::discrete_distribution<> distribution_noncard;//�ǿ����³�/���ߵķֲ�
  std::discrete_distribution<> distribution_npc;//npc�ķֲ�

  //�籾���--------------------------------------------------------------------------------------
  
  //״̬���
  int16_t cook_material[5];//���ֲ˸���
  int32_t cook_dish_pt;//����pt
  int32_t cook_dish_pt_turn_begin;//�غϸտ�ʼ���Բ�֮ǰ��������pt�������������pt��ص�����
  int16_t cook_farm_level[5];//����ũ��ĵȼ�
  int16_t cook_farm_pt;//ũ������pt
  bool cook_dish_sure_success;//��ɹ�ȷ��
  int16_t cook_dish;//��ǰ��Ч�Ĳ�
  int16_t cook_win_history[5];//�����ʳ���Ƿ񡰴����㡱���Ǵ�����0��������1��������2

  //�����ջ�ֵ=f(cook_harvest_green_count)*(�����ջ� + cook_harvest_history*׷���ջ� + cook_harvest_extra)
  int16_t cook_harvest_history[4];//��4�غϷֱ�����4�ֲˣ�-1�ǻ�δѡ��
  bool cook_harvest_green_history[4];//��4�غ��ǲ����̲�
  int16_t cook_harvest_extra[5];//ÿ�غ��ջ�=׷���ջ�+��ͷ����cook_harvest_extra����ͷ���ۼƲ���
  


  //������ȡ���
  int16_t cook_train_material_type[8];//ѵ�����������õĲ˵����࣬��Ųο�TrainActionTypeEnum
  bool cook_train_green[8];//ѵ����������Ƿ�Ϊ��Ȧ
  int16_t cook_main_race_material_type;//�����غϵĲ˵�����

  //��������籾���˿�����Ϊ�ӽ��ش������������Ŷӿ����Ժ��ٿ���
  int16_t friend_type;//0û�����˿���1 ssr����2 r��
  int16_t friend_personId;//���˿���persons��ı��
  int16_t friend_stage;//0��δ�����1���ѵ����δ�������У�2���ѽ�������
  int16_t friend_outgoingUsed;//���˵ĳ����Ѿ����˼�����   ��ʱ���������������Ŷӿ��ĳ���
  double friend_vitalBonus;//���˿��Ļظ�������
  double friend_statusBonus;//���˿����¼�Ч������





  //����ͨ���������Ϣ�����õķǶ�������Ϣ��ÿ�غϸ���һ�Σ�����Ҫ¼��
  int16_t trainValue[5][6];//ѵ����ֵ���������²�+�ϲ㣩����һ�����ǵڼ���ѵ�����ڶ���������������������pt
  int16_t trainVitalChange[5];//ѵ����������仯�������������ģ�
  int16_t failRate[5];//ѵ��ʧ����
  bool isTrainShining[5];//ѵ���Ƿ�����

  int16_t cook_dishpt_success_rate;//��ɹ���
  int16_t cook_dishpt_training_bonus;//����ptѵ���ӳ�
  int16_t cook_dishpt_skillpt_bonus;//����pt���ܵ�ӳ�
  int16_t cook_dishpt_deyilv_bonus;//����pt�����ʼӳ�
  int16_t cook_train_material_num_extra[8];//ѵ�����������õĲ˵ĸ������ӣ�cook_harvest_extra����ѵ��=��link����ͷ��+3*link�������������=0


  //ѵ����ֵ������м������������������д�߼����й���
  int16_t trainValueLower[5][6];//ѵ����ֵ���²㣬��һ�����ǵڼ���ѵ�����ڶ���������������������pt����
  //double trainValueCardMultiplier[5];//֧Ԯ������=(1+��ѵ���ӳ�)(1+�ɾ�ϵ��*(1+�ܸɾ��ӳ�))(1+0.05*�ܿ���)(1+����1)(1+����2)...

  //bool cardEffectCalculated;//֧Ԯ��Ч���Ƿ��Ѿ�����������޹ز˲���Ҫ���¼��㣬���俨����߶�jsonʱ��Ҫ��Ϊfalse
  //CardTrainingEffect cardEffects[6];




  //��Ϸ�������------------------------------------------------------------------------------------------

public:

  void newGame(std::mt19937_64& rand,
    bool enablePlayerPrint,
    int newUmaId,
    int umaStars,
    int newCards[6],
    int newZhongMaBlueCount[5],
    int newZhongMaExtraBonus[6]);//������Ϸ�����֡�umaId��������


  //��������Ƿ������Һ���
  //������İ����������������Ѿ������ٴ����ˣ�ura���������غ�ѡ��ѵ����Ϊ��
  //������İ��������޷��ų���ĳ���˵���һ��ѵ�����������ܲ����ų�һ���֣�
  bool isLegal(Action action) const;

  //����Action��һֱ������У�ֱ����һ����Ҫ��Ҿ��ߣ����������غϣ�������غ���>=78��ʲô������ֱ��return������Ҫ����������ˣ�
  //ע����Action������ѵ������Բ˵�������ѵ����Ҳ��������һ�غ�
  //URA�ڼ䣬�����غ�Ҳ����ҽ��гԲ˾��ߣ�����˽�����һ�غ�
  void applyAction(
    std::mt19937_64& rand,
    Action action);

  int finalScore() const;//�����ܷ�
  bool isEnd() const;//�Ƿ��Ѿ��վ�



  //ԭ�����⼸��private���У����private��ĳЩ�ط��ǳ��������Ǿ͸ĳ�public

  void autoUpgradeFarm(bool beforeXiahesu);//ũ��������������д�߼������Ͳ���������ˣ�beforeXiahesu���ĺ���ǰ�Ǹ��غ��ղ˺��������
  void randomDistributeCards(std::mt19937_64& rand);//���������ͷ
  void calculateTrainingValue();//��������ѵ���ֱ�Ӷ��٣�������ʧ���ʡ�ѵ���ȼ�������
  bool makeDish(int16_t dishId, std::mt19937_64& rand);//���ˣ�������������棬���������ֵ
  bool applyTraining(std::mt19937_64& rand, int train);//���� ѵ��/����/���� �����������˵���¼������������ˣ��������̶��¼��;籾�¼���������Ϸ����򷵻�false���ұ�֤�����κ��޸�
  void checkEventAfterTrain(std::mt19937_64& rand);//���̶��¼�������¼�����������һ���غ�

  void checkFixedEvents(std::mt19937_64& rand);//ÿ�غϵĹ̶��¼��������籾�¼��͹̶������Ͳ��������¼���
  void checkRandomEvents(std::mt19937_64& rand);//ģ��֧Ԯ���¼�����������¼������������������飬������ȣ�

  //���ýӿ�-----------------------------------------------------------------------------------------------

  bool loadGameFromJson(std::string jsonStr);

  //����������
  void getNNInputV1(float* buf, const SearchParam& param) const;

  void print() const;//�ò�ɫ������ʾ��Ϸ����
  void printFinalStats() const;//��ʾ���ս��




  //���ָ���������ӿڣ����Ը�����Ҫ���ӻ���ɾ��-------------------------------------------------------------------------------

  inline bool isXiahesu() const //�Ƿ�Ϊ�ĺ���
  {
    return (turn >= 36 && turn <= 39) || (turn >= 60 && turn <= 63);
  }
  inline bool isRaceAvailable() const //�Ƿ���Զ������
  {
    return turn >= 13 && turn <= 71;
  }

  int calculateRealStatusGain(int idx, int value) const;//����1200����Ϊ2�ı�����ʵ����������ֵ
  void addStatus(int idx, int value);//��������ֵ�����������
  void addAllStatus(int value);//ͬʱ�����������ֵ
  void addVital(int value);//���ӻ�������������������
  void addVitalMax(int value);//�����������ޣ�����120
  void addMotivation(int value);//���ӻ�������飬ͬʱ���ǡ�isPositiveThinking��
  void addJiBan(int idx,int value,bool ignoreAijiao);//����������ǰ������������������ignoreAijiao=true
  void addYayoiJiBan(int value);//�������³���籾���������
  int getYayoiJiBan() const;//������³��
  void addStatusFriend(int idx, int value);//���˿��¼�����������ֵ����pt��idx=5���������¼��ӳ�
  void addVitalFriend(int value);//���˿��¼����������������ǻظ����ӳ�
  void runRace(int basicFiveStatusBonus, int basicPtBonus);//�ѱ��������ӵ����Ժ�pt�ϣ������ǲ�������ӳɵĻ���ֵ
  void addTrainingLevelCount(int trainIdx, int n);//Ϊĳ��ѵ������n�μ���
  void checkDishPtUpgrade();//�ڻغϺ󣬼������pt�Ƿ��Σ�����������µ����ʻ�����ѵ���ȼ�
  void updateDeyilv();//��dishPt�����󣬸��µ�����

  int getTrainingLevel(int trainIdx) const;//����ѵ���ȼ�
  int calculateFailureRate(int trainType, double failRateMultiply) const;//����ѵ��ʧ���ʣ�failRateMultiply��ѵ��ʧ���ʳ���=(1-֧Ԯ��1��ʧ�����½�)*(1-֧Ԯ��2��ʧ�����½�)*...

  bool isCardShining(int personIdx, int trainIdx) const;    // �ж�ָ�����Ƿ����ʡ���ͨ�����������ѵ�����Ŷӿ���friendOrGroupCardStage
  //bool trainShiningCount(int trainIdx) const;    // ָ��ѵ����Ȧ�� //uaf��һ������
  void calculateTrainingValueSingle(int tra);//����ÿ��ѵ���Ӷ���   //uaf�籾�������ѵ��һ����ȽϷ���

  //�������
  int maxFarmPtUntilNow() const;//����ȫ����Ȧ����õ�ũ��pt����ȥ�Ѿ�������pt��
  bool upgradeFarm(int item);//�ѵ�item��ũ����1����ʧ�ܷ���false
  void addDishMaterial(int idx, int num);//���Ӳ˲��ϣ����������
  bool isDishLegal(int dishId) const;//�������Ƿ�����
  int getDishTrainingBonus(int trainIdx) const;//���㵱ǰ�����ѵ���ӳ�
  int getDishRaceBonus() const;//���㵱ǰ����ı����ӳ�
  void handleDishBigSuccess(int dishId, std::mt19937_64& rand);//���������ɹ���ص��¼�
  std::vector<int> dishBigSuccess_getBuffs(int dishId, std::mt19937_64& rand);//�����ɹ�-��ȡ����Щbuff
  void dishBigSuccess_hint(std::mt19937_64& rand);//�����ɹ�-����hint
  void dishBigSuccess_invitePeople(int trainIdx, std::mt19937_64& rand);//�����ɹ��ķ���Ч������trainIdx�������һ��֧Ԯ��
  int turnIdxInHarvestLoop() const;//�ջ�������ĵڼ��غ�(turn%4)���ĺ��޺�Ϊ0
  void addFarm(int type, int extra, bool isGreen);//���extra����ͷ���ӣ�isGreen����Ȧ
  std::vector<int> calculateHarvestNum(bool isAfterTrain) const;//�ջ����ֲ˵��������Լ�ũ��pt��
  void maybeHarvest();//ÿ4�غ��ղˣ�����ÿ�غ��ղˣ������ղ˻غϾ�ֱ��return
  void maybeCookingMeeting();//��ʳ��



  //���˿�����¼�
  void handleFriendUnlock(std::mt19937_64& rand);//�����������
  void handleFriendOutgoing(std::mt19937_64& rand);//�������
  void handleFriendClickEvent(std::mt19937_64& rand, int atTrain);//���˵���¼�����ƣ�옔��
  void handleFriendFixedEvent();//���˹̶��¼�������+����
  

  //���
  float getSkillScore() const;//���ܷ֣�����������֮ǰҲ������ǰ��ȥ


  //��ʾ
  void printEvents(std::string s) const;//����ɫ������ʾ�¼�
  std::string getPersonStrColored(int personId, int atTrain) const;//���������������ϳɴ���ɫ���ַ�������С�ڰ�������ʾ
};

