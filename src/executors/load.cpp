#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseLOAD()
{
    logger.log("syntacticParseLOAD");
    if (!(tokenizedQuery.size() == 2 || tokenizedQuery.size() == 3)) 
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOAD;

    if(tokenizedQuery.size() == 3 && tokenizedQuery[1] == "MATRIX")
        parsedQuery.loadRelationName = tokenizedQuery[2];
    else
        parsedQuery.loadRelationName = tokenizedQuery[1];
        
    return true;
}

bool semanticParseLOAD()
{
    logger.log("semanticParseLOAD");
    if (tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Relation already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD()
{
    logger.log("executeLOAD");

    if(tokenizedQuery.size() == 2){
        
        Table *table = new Table(parsedQuery.loadRelationName);
        if (table->load())
        {
            tableCatalogue.insertTable(table);
            cout << "Loaded Table. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
        }

    }else{

        Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
        if (matrix->load())
        {
            matrixCatalogue.insertMatrix(matrix); 
            cout << "Loaded Matrix. Column Count: " << matrix->N << " Row Count: " << matrix->N << endl;
        }

    }

    return;
}