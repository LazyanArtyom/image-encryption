#include "logic.h"
#include <QDebug>
#include <assert.h>
#include <iostream>

CLogic::CLogic(size_t nColNum, size_t nRowNum, size_t nGridType)
    : m_nColNum(nColNum), m_nRowNum(nRowNum), m_nGridType(nGridType)
{
    initGraph();
    qDebug() << "Column " << m_nColNum << "Row" << m_nRowNum;
}

CLogic::~CLogic()
{
    if (m_pField)
    {
        delete m_pField;
        m_pField = nullptr;
    }
}

std::map<std::pair<size_t, size_t>, std::pair<int, short>> CLogic::checkField(size_t nH, size_t nW, size_t& nCount, bool bFirst, int nStep)
{
    std::map<std::pair<size_t, size_t>, std::pair<int, short>> changedValues;

    if (bFirst)
    {
        for (size_t i = 0; i < m_nRowNum; ++i)
        {
            for (size_t j = 0; j < m_nColNum; ++j)
            {
                changedValues[std::make_pair(i, j)] = std::make_pair(m_pField->getNodeById(i * m_nColNum + j)->m_tValue,
                                                                     m_pField->getNodeById(i * m_nColNum + j)->bPlvac);
            }
        }

        return changedValues;
    }

    size_t nNodeId = m_nColNum * nH + nW;
    vertexesToVisit.push(nNodeId);
    m_pField->getNodeById(nNodeId)->m_tValue += nStep;
    //changedValues[std::make_pair(nH, nW)] = m_pField->getNodeById(nNodeId)->m_tValue;
    do
    {
        size_t nId = vertexesToVisit.front();
        if(m_nGridType == 2) //open
        {
            if(m_pField->getNodeById(nId)->m_tValue >= 4)
            {
                if(m_pField->getNodeById(nId)->bPlvac == 0)
                {
                    m_pField->getNodeById(nId)->bPlvac = 1;
                    ++count;
                }

                for(const auto& nNeighbourId : m_pField->getNodeById(nId)->m_arrNeighbours)
                {
                    m_pField->getNodeById(nNeighbourId)->m_tValue += m_pField->getNodeById(nId)->m_tValue / 4;
                    vertexesToVisit.push(nNeighbourId);
                }
                m_pField->getNodeById(nId)->m_tValue = m_pField->getNodeById(nId)->m_tValue % 4;
            }
        }
        else
        {
            if(m_pField->getNodeById(nId)->m_tValue >= (int)m_pField->getNodeNeighboursCount(nId))
            {
                if(m_pField->getNodeById(nId)->bPlvac == 0)
                {
                    m_pField->getNodeById(nId)->bPlvac = 1;
                    ++count;
                }

                for(const auto& nNeighbourId : m_pField->getNodeById(nId)->m_arrNeighbours)
                {
                    m_pField->getNodeById(nNeighbourId)->m_tValue += m_pField->getNodeById(nId)->m_tValue / (int)m_pField->getNodeNeighboursCount(nId);
                    vertexesToVisit.push(nNeighbourId);
                }
                m_pField->getNodeById(nId)->m_tValue = m_pField->getNodeById(nId)->m_tValue % (int)m_pField->getNodeNeighboursCount(nId);
            }
        }
        vertexesToVisit.pop();
        changedValues[std::make_pair((size_t)(nId / m_nColNum), (size_t)(nId % m_nColNum))]
                = std::make_pair(m_pField->getNodeById(nId)->m_tValue, m_pField->getNodeById(nId)->bPlvac);

        if(m_nGridType != 2 && count == m_nColNum * m_nRowNum)
        {
            nCount = count;
            break;
        }
    }
    while(!vertexesToVisit.empty());
    nCount = count;
    return changedValues;
}

void CLogic::initGraph()
{
    std::vector<std::pair<int, std::set<size_t> > > arrConnections;
    generateGraphForMatrix(arrConnections);
    m_pField = new CGraph(arrConnections);
}

void CLogic::generateGraphForMatrix(std::vector<std::pair<int, std::set<size_t> > > & connections)
{
    connections.resize(m_nColNum * m_nRowNum);
    switch (m_nGridType)
    {
    case 0:
        {
            for(size_t i = 0; i < m_nRowNum; ++i)
            {
                for(size_t j = 0; j < m_nColNum; ++j)
                {
                    if(i != 0)
                        connections[i * m_nColNum + j].second.insert((i - 1) * m_nColNum + j);
                    else
                        connections[i * m_nColNum + j].second.insert((m_nRowNum - 1) * m_nColNum + j);
                    if(i != m_nRowNum - 1)
                        connections[i * m_nColNum + j].second.insert((i + 1) * m_nColNum + j);
                    else
                        connections[i * m_nColNum + j].second.insert(j);
                    if(j != 0)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j - 1);
                    else
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + m_nColNum - 1);
                    if(j != m_nColNum - 1)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j + 1);
                    else
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum);
                   connections[i * m_nColNum + j].first = 2;
                }
            }
        }
        break;
    case 1:
        {
            for(size_t i = 0; i < m_nRowNum; ++i)
            {
                for(size_t j = 0; j < m_nColNum; ++j)
                {
                    if(i != 0)
                        connections[i * m_nColNum + j].second.insert((i - 1) * m_nColNum + j);
                    if(i != m_nRowNum - 1)
                        connections[i * m_nColNum + j].second.insert((i + 1) * m_nColNum + j);
                    if(j != 0)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j - 1);
                    if(j != m_nColNum  - 1)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j + 1);

                    if(i == 0 || j == 0 || j == m_nColNum - 1 || i == m_nRowNum - 1)
                        connections[i * m_nColNum + j].first = 1;
                    else
                        connections[i * m_nColNum + j].first = 2;
                }
            }
            connections[0].first = 0;
            connections[m_nColNum - 1].first = 0;
            connections[(m_nRowNum - 1) * m_nColNum].first = 0;
            connections[m_nColNum * m_nRowNum - 1].first = 0;
        }
        break;
    case 2:
        {
            for(size_t i = 0; i < m_nRowNum; ++i)
            {
                for(size_t j = 0; j < m_nColNum; ++j)
                {
                    if(i != 0)
                        connections[i * m_nColNum + j].second.insert((i - 1) * m_nColNum + j);
                    if(i != m_nRowNum - 1)
                        connections[i * m_nColNum + j].second.insert((i + 1) * m_nColNum + j);
                    if(j != 0)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j - 1);
                    if(j != m_nColNum  - 1)
                        connections[i * m_nColNum + j].second.insert(i * m_nColNum + j + 1);
                    connections[i * m_nColNum + j].first = 2;
                }
            }
        }
    default:
        break;
    }
}

void CLogic::setStartState(std::vector<std::vector<int> > matrix)
{
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            size_t nNodeId = m_nColNum * i + j;
            m_pField->getNodeById(nNodeId)->m_tValue = matrix[i][j];
        }
    }
}

////////////////////////////////
/// CLogicEncoder
/////////////////////////////////
CLogicEncoder::CLogicEncoder(size_t nColNum, size_t nRowNum)
    : m_nColNum(nColNum), m_nRowNum(nRowNum)
{
    m_pLogicForDec = new CLogic(m_nColNum, m_nRowNum, 2);

    m_arrImage.resize(m_nRowNum);
    for(auto& row : m_arrImage)
        row.resize(m_nColNum);

    m_arrMergedKey.resize(m_nRowNum);
    for(auto& row : m_arrMergedKey)
    {
            row.resize(m_nColNum);
    }
    m_arrValuesToCheck.resize(m_nRowNum);
    for(auto& row : m_arrValuesToCheck)
    {
            row.resize(m_nColNum);
    }

    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            if(i == 0 || j == 0 || j == m_nColNum - 1 || i == m_nRowNum - 1)
                m_arrValuesToCheck[i][j] = 1;
            else
                m_arrValuesToCheck[i][j] = 0;
        }
    }
    m_arrValuesToCheck[0][0] = 2;
    m_arrValuesToCheck[0][m_nColNum - 1] = 2;
    m_arrValuesToCheck[m_nRowNum - 1][0] = 2;
    m_arrValuesToCheck[m_nRowNum - 1][m_nColNum - 1] = 2;

}

CLogicEncoder::~CLogicEncoder()
{
    if(m_pLogicForDec != nullptr)
    {
        delete m_pLogicForDec;
        m_pLogicForDec = nullptr;
    }
}

std::vector<std::vector<int>> CLogicEncoder::generateKey()
{
    std::vector<std::vector<int>> result;
    result.resize(m_nRowNum);
    for(auto& row : result)
    {
        for(size_t i = 0; i < m_nColNum; ++i)
        {
            row.push_back(rand() % 3);
        }
    }
    CLogic oLogic(m_nColNum, m_nRowNum, 2);
    oLogic.setStartState(result);
    size_t nCount = 0;

    while(nCount != m_nColNum * m_nRowNum)
    {
        for(size_t i = 0; i < m_nRowNum; ++i)
        {
            for(size_t j = 0; j < m_nColNum; ++j)
            {
                if(m_arrValuesToCheck[i][j] != 0)
                {
                    oLogic.checkField(i, j, nCount, false, m_arrValuesToCheck[i][j]);
                }
            }
        }
    }

    std::map<std::pair<size_t, size_t>, std::pair<int, short>> mapIndexes;
    size_t unusedVal = 0;
    mapIndexes = oLogic.checkField(0, 0, unusedVal, true);
    for (const auto& oValue : mapIndexes)
    {
        result[oValue.first.first][oValue.first.second] = oValue.second.first;
    }

    ///////////keep key merged
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            oLogic.checkField(i, j, unusedVal, false, m_arrMergedKey[i][j]);
        }
    }

    mapIndexes = oLogic.checkField(0, 0, unusedVal, true);
    for (const auto& oValue : mapIndexes)
    {
        m_arrMergedKey[oValue.first.first][oValue.first.second] = oValue.second.first;
    }
    ///////////////////////////
    return result;
}

void CLogicEncoder::setImage(std::vector<std::vector<int> > matrix)
{
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            m_arrImage[i][j] = matrix[i][j];
        }
    }
}

std::vector<std::vector<int>> CLogicEncoder::getImage()
{
    return m_arrImage;
}

std::vector<std::vector<int>> CLogicEncoder::encode()
{
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            m_arrImage[i][j] += 2;
        }
    }
    CLogic oLogic(m_nColNum, m_nRowNum, 2);
    oLogic.setStartState(m_arrImage);
    size_t unusedVal;
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            oLogic.checkField(i, j, unusedVal, false, m_arrMergedKey[i][j]);
        }
    }
    std::map<std::pair<size_t, size_t>, std::pair<int, short>> mapIndexes;
    mapIndexes = oLogic.checkField(0, 0, unusedVal, true);
    for (const auto& oValue : mapIndexes)
    {
        m_arrImage[oValue.first.first][oValue.first.second] = oValue.second.first;
    }
    std::vector<std::vector<int>> result;
    result.resize(m_nRowNum);
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            if(m_arrImage[i][j] == 2 || m_arrImage[i][j] == 3)
                result[i].push_back(1);
            else
                result[i].push_back(0);
        }
    }
    return result;
}

bool CLogicEncoder::getDecodedAndFinished(std::vector<std::vector<int> > &matrix)
{
    if(m_bFirstDecodeReq)
    {
        for(size_t i = 0; i < m_nRowNum; ++i)
        {
            for(size_t j = 0; j < m_nColNum; ++j)
            {
                m_arrValuesToCheck[i][j] -= m_arrMergedKey[i][j];
            }
        }
        m_bFirstDecodeReq = false;
    }
    m_pLogicForDec->setStartState(m_arrImage);
    size_t unusedVal, nCount = 0;
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            if (0 != m_arrValuesToCheck[i][j])
                m_pLogicForDec->checkField(i, j, nCount, false, m_arrValuesToCheck[i][j]);
        }
    }
    std::map<std::pair<size_t, size_t>, std::pair<int, short>> mapIndexes;
    mapIndexes = m_pLogicForDec->checkField(0, 0, unusedVal, true);
    for (const auto& oValue : mapIndexes)
    {
        m_arrImage[oValue.first.first][oValue.first.second] = oValue.second.first;
    }
    matrix.resize(m_nRowNum);
    for(size_t i = 0; i < m_nRowNum; ++i)
    {
        matrix[i].resize(m_nColNum);
        for(size_t j = 0; j < m_nColNum; ++j)
        {
            matrix[i][j] = m_arrImage[i][j] > 2 ? 1 : 0;
        }
    }

//    m_nCheckedRow++;
//    if(m_nCheckedRow == m_nRowNum)
//    {
        if(nCount == m_nColNum * m_nRowNum)
        {
            // qDebug() << "Plvacneri count : " << nCount;
            //
            // for(size_t i = 0; i < m_nRowNum; ++i)
            // {
            //     for(size_t j = 0; j < m_nColNum; ++j)
            //     {
            //         std::cout << m_arrImage[i][j] << " ";
            //     }
            //     std::cout << std::endl;
            // }
            // qDebug() << "###############################";
            // for(size_t i = 0; i < m_nRowNum; ++i)
            // {
            //     for(size_t j = 0; j < m_nColNum; ++j)
            //     {
            //         std::cout << matrix[i][j] << " ";
            //     }
            //     std::cout << std::endl;
            // }

            return true;
        }
        else
        {
            for(size_t i = 0; i < m_nRowNum; ++i)

            {
                for(size_t j = 0; j < m_nColNum; ++j)
                {
                    if(i == 0 || j == 0 || j == m_nColNum - 1 || i == m_nRowNum - 1)
                        m_arrValuesToCheck[i][j] = 1;
                    else
                        m_arrValuesToCheck[i][j] = 0;
                }
            }
            m_arrValuesToCheck[0][0] = 2;
            m_arrValuesToCheck[0][m_nColNum - 1] = 2;
            m_arrValuesToCheck[m_nRowNum - 1][0] = 2;
            m_arrValuesToCheck[m_nRowNum - 1][m_nColNum - 1] = 2;

//            m_nCheckedRow = 0;
        }
//    }
    return false;
}
