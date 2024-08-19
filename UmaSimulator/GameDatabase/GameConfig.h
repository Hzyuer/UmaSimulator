#pragma once

#include "iostream"
#include "fstream"
#include "string"
#include "sstream"
#include "filesystem"
#include "../External/json.hpp"

struct GameConfig 
{
  
    static double radicalFactor; // �����ȣ���߻ᵼ�¼������
    static int searchSingleMax;  // ��һѡ��ﵽ���ٴ�������ֹͣ���ؿ���
    static int threadNum;   // �߳������ɸ�������CPU�������������ֱ�����4

    static double scorePtRate;    // ÿpt�ķ���
    static int scoringMode;    // ��ַ�ʽ

    // "localfile": checking ./thisTurn.json
    // "urafile": communicating with URA using file
    // "websocket": communicating with URA using websocket
    static std::string communicationMode;    


    //���²������޸�

    static int eventStrength;  // ģ������ÿ�غ���40%���ʼ���ô�����ԣ�ģ��֧Ԯ���¼���config��������������Ŀ����ĳ�̶ֳ��ϴ��漤���ȣ������˻�������ڿ�����

    static std::string modelPath;    // �������ļ�����Ŀ¼
    static int batchSize;   // �Կ����batchSize�������������������𲻴��������ֱ�����256

    static int searchTotalMax; //����ѡ���ܹ��ﵽ���ٴ�������ֹͣ���ؿ��壬0Ϊ����
    static int searchGroupSize; //����ʱÿ�η�����ټ�����������128����ҪС��16*�߳�����̫С������ܴ���⿪����ÿsearchGroupSize������Ҫ����O(200000)�Σ�
    static double searchCpuct; //cpuct������ԽС����Խ����
    static int maxDepth;  // ���ؿ�����ȣ�������Ĭ����10������������ֱ���ѵ���Ϸ������TOTAL_TURN=67��

    static std::string role;    // ̨�ʷ��

    static bool debugPrint; // ��ʾ������Ϣ�����������'.'��Ĭ��ΪFalse
    static bool noColor;    // ΪTrueʱ����ʾ��ɫ

    static void load(const std::string &path);
};
