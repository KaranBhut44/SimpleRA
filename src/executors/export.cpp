#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() < 2 || tokenizedQuery.size()>3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = EXPORT;

    if(tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
        parsedQuery.exportRelationName = tokenizedQuery[2];
    else
        parsedQuery.exportRelationName = tokenizedQuery[1];

    return true;
}

bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    if(!(tableCatalogue.isTable(parsedQuery.exportRelationName) || matrixCatalogue.isMatrix(parsedQuery.exportRelationName)))
    {
        if(!tableCatalogue.isTable(parsedQuery.exportRelationName)){
            cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        }else{
            cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
        }
        return false;
    }
    return true;
}

void executeEXPORT()
{
    logger.log("executeEXPORT");
    if (tableCatalogue.isTable(parsedQuery.exportRelationName)){
        Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
        table->makePermanent();
    }else{

        Matrix * matrix = matrixCatalogue.getMatrix(parsedQuery.exportRelationName);
        matrix->makePermanent();
    }
}