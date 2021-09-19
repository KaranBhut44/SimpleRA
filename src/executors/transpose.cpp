#include "global.h"
/**
 * @brief 
 * SYNTAX: TRANSPOSE TABLES
 */
bool syntacticParseTRANSPOSE()
{
    logger.log("syntacticParseTRANSPOSE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = TRANSPOSE;
    parsedQuery.tranposeMatrixName = tokenizedQuery[1];

    return true;
}

bool semanticParseTRANSPOSE()
{
    logger.log("semanticParseTRANSPOSE");

    if(!matrixCatalogue.isMatrix(parsedQuery.tranposeMatrixName)){
        cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        return false;
    }

    return true;
}

void executeTRANSPOSE()
{
    logger.log("executeTRANSPOSE");
    Matrix * matrix = matrixCatalogue.getMatrix(parsedQuery.tranposeMatrixName);
    matrix->transpose();

}