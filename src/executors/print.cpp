#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */
bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (!(tokenizedQuery.size() == 2 || tokenizedQuery.size() == 3))
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = PRINT;

    if(tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
        parsedQuery.printRelationName = tokenizedQuery[2];
    else
        parsedQuery.printRelationName = tokenizedQuery[1];
    
    return true;
}

bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");
    if (
        !(
            tableCatalogue.isTable(parsedQuery.printRelationName) ||
            matrixCatalogue.isMatrix(parsedQuery.printRelationName)
        )    
    )
    {
        if(!tableCatalogue.isTable(parsedQuery.printRelationName)){
            cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        }else{
            cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        }

        return false;
    }
    return true;
}

void executePRINT()
{
    logger.log("executePRINT");

    if (tableCatalogue.isTable(parsedQuery.printRelationName)){
        
        Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
        table->print();
    
    }else{

        Matrix * matrix = matrixCatalogue.getMatrix(parsedQuery.printRelationName);
        matrix->print();
    }
    return;
}
