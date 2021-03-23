#ifndef CLOGIC_H
#define CLOGIC_H
//#include "matrix.h"
#include "graph.h"
#include <queue>
#include <map>
#include <stack>
#include <tuple>

class CLogic
{
public:
    CLogic(size_t nColNum, size_t nRowNum, size_t nGridType);
    ~CLogic();
    std::map<std::pair<size_t, size_t>, std::pair<int, short>> checkField(size_t nH, size_t nW, size_t& nCount, bool bFirst=false, int nStep = 2);
    void initGraph();
    void generateGraphForMatrix(std::vector<std::pair<int, std::set<size_t> > > & connections);
    void setStartState(std::vector<std::vector<int>> matrix);
    size_t count = 0;
private:
    CGraph* m_pField;
    std::queue<size_t> vertexesToVisit;
    size_t m_nColNum;
    size_t m_nRowNum;
    size_t m_nGridType;
};


class CLogicEncoder
{
public:
    CLogicEncoder(size_t nColNum, size_t nRowNum);
    ~CLogicEncoder();

    std::vector<std::vector<int>> generateKey();
    void setImage(std::vector<std::vector<int>> matrix);
    std::vector<std::vector<int>> getImage();
    std::vector<std::vector<int>> encode();
    bool getDecodedAndFinished(std::vector<std::vector<int>>& matrix);
private:
    size_t m_nColNum;
    size_t m_nRowNum;
    bool m_bFirstDecodeReq = true;
    size_t m_nCheckedRow = 0;
    std::vector<std::vector<int>> m_arrImage;
    std::vector<std::vector<int>> m_arrMergedKey;
    std::vector<std::vector<int>> m_arrValuesToCheck;
    CLogic* m_pLogicForDec = nullptr;
};
#endif // !CLOGIC_H
