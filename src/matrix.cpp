#include "global.h"


/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param matrixName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;
}


/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates matrix
 * statistics.
 *
 * @return true if the matrix has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    
    ifstream fin(this->sourceFileName, ios::in);
    
    char ch;

    string numberString = "";
    int number = 0;

    this->nonZeroCount = 0;
    this->zeroCount = 0;

    this->N = 0;

    while (fin.get(ch))
    {

        if(ch == '\n'){
            (this->N)++;    
        }
        
        if(ch == ',' || ch == '\n'){

            if(numberString == "")
                continue;

            number = atoi(numberString.c_str());

            if(number == 0){
                (this->zeroCount)++;
            }else{
                (this->nonZeroCount)++;
            }

            numberString = "";
        
        }else if(ch >= '0' && ch <= '9'){
            numberString.push_back(ch);
        }       
    }

    this->ratio = ((float)this->zeroCount / (this->nonZeroCount + this->zeroCount));
    logger.log(to_string(this->ratio));


    if(this->ratio > 0.6){
        this->isSparse = true;
    }else{
        this->isSparse = false;
    }

    if(this->isSparse){
        blockifySparseMatrix();
    }else{
        blockifyNonSparseMatrix();
    }

    return true;
}


string Matrix::getPagePathSparse(int pageIndexRowCol, int index){

    return "../data/temp/" + this->matrixName + "_Page_{" + to_string(pageIndexRowCol)+ "_" + to_string(index) +"}";
}


string Matrix::getPagePath(int pageIndexRow, int pageIndexCol, int index = 0){

    // logger.log(to_string(this->isSparse));
    if(this->isSparse == true){
        return "../data/temp/" + this->matrixName + "_Page_{" + to_string(pageIndexRow - pageIndexCol)+ "_" + to_string(index) +"}";
    
    }else{

        return "../data/temp/" + this->matrixName + "_Page_{" + to_string(pageIndexRow) + "_" + to_string(pageIndexCol) +"}";
    }
}

void Matrix::appendRowToPage(vector<int> row, int pageIndexRow, int pageIndexCol){

    string pagePath = getPagePath(pageIndexRow, pageIndexCol);
    ofstream fout(pagePath, ios::app);

    int loop = row.size();

    fout << row[0];
    for(int i=1; i < loop; i++)
        fout << " " << row[i];

    fout<<endl;
    fout.close();
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockifyNonSparseMatrix()
{
    logger.log("Matrix::blockifyNonSparseMatrix");

    this->maxR = this->maxC = ceil((float)N/M);
    this->blockCount = this->maxR * this->maxC;

    logger.log("Page::initializePages");
    for(int r = 1; r <= maxR; r++){
        
        for(int c = 1; c <= maxC; c++){
            
            string pagePath = getPagePath(r, c);
            
            ofstream fout(pagePath, ios::trunc);
            fout.close();
        }
    }

    ifstream fin(this->sourceFileName, ios::in);
    
    char ch;
    int number = 0;

    string numberString = "";

    float currRow = 1;
    float currCol = 1;

    vector<int> rowNums;

    logger.log("Page::writePages");
    while (fin.get(ch))
    {
        if(fin.fail())
            break;

        if(ch != '\n' && ch != ',' && !(ch >= '0' && ch <= '9'))
            continue;

        if( ch == '\n' || ch == ','){

            if(numberString != ""){
                number = atoi(numberString.c_str());
                numberString = "";

                rowNums.push_back(number);
            }

            if(ch == '\n' || (numberString == "" && rowNums.size() <= 0) || rowNums.size() == this->M){
              
                int R = ceil(currRow / this->M);
                int C = ceil(currCol / this->M);

                appendRowToPage(rowNums, R, C);
                rowNums.clear();
            }

            if(ch == '\n'){
                currRow++;
                currCol = 1;
            }else{
                currCol++;
            }

        }else if(ch >= '0' && ch <= '9'){
            numberString.push_back(ch);
        }
    }

    return true;
}


vector<vector<int>> Matrix::fetchMatrixFromPage(int pageIndexRow, int pageIndexCol, int &inPageRowCount, int &inPageColCount){

    vector<vector<int>> subMat(this->M, vector<int>(this->M));

    string pagePath = getPagePath(pageIndexRow, pageIndexCol);
    ifstream fin(pagePath, ios::in);

    for(int mr = 0; mr < this->M; mr++){
        for(int mc = 0; mc <= this->M; mc++){
            
            char ch;
            string numberString = "";

            while(fin.get(ch)){

                if(ch == ' ' || ch == '\n')
                    break;

                numberString.push_back(ch);
            }

            if(numberString == "")
                return subMat;

            int number = stoi(numberString);
            subMat[mr][mc] = number;
            
            inPageColCount = max(inPageColCount, mc);

            if(ch == '\n')
                break;
        }

        inPageRowCount = max(inPageRowCount, mr);
    }

    return subMat;
}

void Matrix::writePage(vector<vector<int>> subMat, int pageIndexRow, int pageIndexCol, int inPageRowCount, int inPageColCount){

    string pagePath = getPagePath(pageIndexRow, pageIndexCol);
    ofstream fout(pagePath, ios::trunc);

    for(int r=0; r<=inPageRowCount; r++){
                
        fout << subMat[r][0];
        for(int c=1; c <= inPageColCount; c++)
            fout << " " << subMat[r][c];

        fout<<endl;
    }

    fout.close();
}

void Matrix::transposeSparse(){

    logger.log("Matrix::transposeMatrixSparse");

    int strt = this->N * -1;
    int end = this->N;

    for(int i = strt; i <= end; i++){
   
        int fileNumber = 0;

        while(true){

            string pagePath = getPagePathSparse(i, fileNumber);
            ifstream * filePointer = new ifstream(pagePath, ios :: in);
            
            if(filePointer->fail()){
                
                filePointer->close();
                delete filePointer;

                filePointer = NULL;
                break;
            }

            vector<vector<int>> data;

            while(true){

                char ch;
                string numberString = "";

                while(filePointer->get(ch)){
                    if(ch == ' ' || ch == '\n')
                        break;
                    numberString.push_back(ch);
                }

                if(numberString == ""){
                    break;
                }

                int r = stoi(numberString);
                numberString = "";

                while(filePointer->get(ch)){
                    if(ch == ' ' || ch == '\n')
                        break;
                    numberString.push_back(ch);
                }

                int c = stoi(numberString);
                numberString = "";

                while(filePointer->get(ch)){
                    if(ch == ' ' || ch == '\n')
                        break;
                    numberString.push_back(ch);
                }

                int number = stoi(numberString);
                data.push_back({c, r, number});
            }

            if(data.size() == 0){
                
                if(filePointer != NULL){

                    filePointer->close();
                    delete filePointer;

                    filePointer = NULL;
                }
                
                break;
            }            

            filePointer->close();
            delete filePointer;

            filePointer = NULL;
            
            ofstream * filePointerOut = new ofstream(pagePath, ios :: out);

            for(auto v : data){
                (*filePointerOut) << v[0] << " " << v[1] << " " << v[2] << endl;
            }

            filePointerOut->close();
            delete filePointerOut;

            filePointerOut = NULL;
            fileNumber++;
        }
    }


    for(int i = 1; i <= end; i++){
   
        int fileNumber = 0;

        while(true){

            string pagePath1 = getPagePathSparse(i, fileNumber);
            string pagePath2 = getPagePathSparse(i*-1, fileNumber);

            fileNumber++;

            ifstream f1 = ifstream(pagePath1);
            ifstream f2 = ifstream(pagePath2);
            
            if(f1.fail() && f2.fail()){
                break;
            }else if(!f1.fail() && !f2.fail()){

                string temp = "temp";

                rename(pagePath1.c_str(), temp.c_str());
                rename(pagePath2.c_str(), pagePath1.c_str());
                rename(temp.c_str(), pagePath2.c_str());

            }else if(f1.fail()){

                rename(pagePath2.c_str(), pagePath1.c_str());

            }else{

                rename(pagePath1.c_str(), pagePath2.c_str());

            }

        }
    }
}


void Matrix::transposeNonSparse(){


    logger.log("Matrix::transposeMatrixNonSparse");

    int inPageColCount=0, inPageRowCount=0;
    vector<vector<int>> subMat;

    for(int r = 1; r <= this->maxR; r++){
        for(int c = 1; c <= this->maxC; c++){

            inPageColCount = inPageRowCount = 0;
            subMat = fetchMatrixFromPage(r, c, inPageRowCount, inPageColCount);

            for(int i=0; i<this->M; i++){
                for(int j=0; j<i; j++){
                    swap(subMat[i][j], subMat[j][i]);
                }
            }

            writePage(subMat, r, c, inPageColCount, inPageRowCount);
        }
    }
    logger.log("nPlaceTransposeDone");

    int ttt = 0;

    for(int r=1; r<= this->maxR; r++){
        for(int c=1; c<r; c++){

            string pagePath1 = getPagePath(r, c);
            string pagePath2 = getPagePath(c ,r);

            string temp = "temp";

            rename(pagePath1.c_str(), temp.c_str());
            rename(pagePath2.c_str(), pagePath1.c_str());
            rename(temp.c_str(), pagePath2.c_str());
        }
    }
    logger.log("RenameDone");
}

void Matrix::transpose(){

    logger.log("Matrix::transposeMatrix");

    if(this->isSparse == true){
        transposeSparse();
    }else{
        transposeNonSparse();
    }
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockifySparseMatrix()
{
    logger.log("Matrix::blockifySparseMatrix");
    
    unordered_map<int, bool> flag;
    unordered_map<int, ofstream *> filePointer;
    unordered_map<int, int> fileNumber;
    unordered_map<int, int> fileContentCounter;

    ifstream fin(this->sourceFileName, ios::in);
    

    int strt = this->N * -1;
    int end = this->N;
    
    for(int i = strt; i <= end; i++){
        flag[i] = false;
        filePointer[i] = NULL;
        fileNumber[i] = 0;
        fileContentCounter[i] = -1;
    }

    char ch;
    int number = 0;

    string numberString = "";

    float currRow = 1;
    float currCol = 1;

    vector<int> rowNums;

    logger.log("Page::writePages");
    while (fin.get(ch))
    {
        if(fin.fail())
            break;

        if( ch == '\n' || ch == ','){

            number = atoi(numberString.c_str());
            numberString = "";

            if(number != 0){

                int idx = currRow - currCol;

                if(flag[idx] == false){

                    string pagePath = getPagePath(currRow, currCol, fileNumber[idx]);

                    filePointer[idx] = new ofstream(pagePath, ios :: out);

                    flag[idx] = true;
                    fileNumber[idx]++;
                    fileContentCounter[idx] = 0;
                }
                
                (*filePointer[idx]) << currRow << " " << currCol << " "<<number<< endl;
                
                fileContentCounter[idx]++;

                if(fileContentCounter[idx] == this->maxPossible){

                    flag[idx] = false;
                    filePointer[idx]->close();
                    delete filePointer[idx];
                    
                    filePointer[idx] = NULL;

                    fileContentCounter[idx] = -1;
                }
           
            }

            if(ch == '\n'){
                currRow++;
                currCol = 1;
            }else{
                currCol++;
            }

        }else if(ch >= '0' && ch <= '9'){
            numberString.push_back(ch);
        }
    }

    for(int i = strt; i <= end; i++){
        
        if(flag[i] == true){
        
            flag[i] = false;
            filePointer[i]->close();
            delete filePointer[i];
            
            filePointer[i] = NULL;

            fileContentCounter[i] = -1;
        }
    }

    return true;
}

vector<int> Matrix :: getSparseMatrixNumber(int idx, unordered_map<int, bool> &flag, unordered_map<int, ifstream *> &filePointer, unordered_map<int, int> &fileNumber){
    
    char ch;
    string numberString = "";

    while(filePointer[idx]->get(ch)){
        if(ch == ' ' || ch == '\n')
            break;
        numberString.push_back(ch);
    }

    if(numberString == ""){

        filePointer[idx]->close();   
        delete filePointer[idx]; 

        string pagePath = getPagePathSparse(idx, fileNumber[idx]);

        filePointer[idx] = new ifstream(pagePath, ios :: in);
        if(filePointer[idx]->fail()){

            // logger.log("DONE " + to_string(idx));

            flag[idx] = false;
            filePointer[idx] = NULL;
            fileNumber[idx] = 0;

            // continue;
            return {-1, -1, -1};
        }

        fileNumber[idx]++;

        while(filePointer[idx]->get(ch)){
            if(ch == ' ' || ch == '\n')
                break;
            numberString.push_back(ch);
        }
    }

    int r = stoi(numberString);
    numberString = "";

    while(filePointer[idx]->get(ch)){
        if(ch == ' ' || ch == '\n')
            break;
        numberString.push_back(ch);
    }

    int c = stoi(numberString);
    numberString = "";

    while(filePointer[idx]->get(ch)){
        if(ch == ' ' || ch == '\n')
            break;
        numberString.push_back(ch);
    }

    int number = stoi(numberString);
    // logger.log(to_string(r)+ " " + to_string(c)+ " " + to_string(number));

    return {r, c, number};
}

void Matrix::printSparse(){

    logger.log("Matrix::printSparse");
        
    unordered_map<int, bool> flag;
    unordered_map<int, ifstream *> filePointer;
    unordered_map<int, int> fileNumber;
    unordered_map<int, vector<int>> prevContent;


    // logger.log(to_string(this->N) + " : " + to_string(this->N) );

    int pc = this->printConstraints;

    int strt = this->N * -1;
    strt = max(strt, -1 * pc);

    int end = this->N;
    end = min(end, pc);

    for(int i = strt; i <= end; i++){

        flag[i] = true;
        filePointer[i] = NULL;
        fileNumber[i] = 0;
        prevContent[i] = {-1, -1, -1};

        string pagePath = getPagePathSparse(i, fileNumber[i]);
        // logger.log(pagePath);
        
        filePointer[i] = new ifstream(pagePath, ios :: in);
        if(filePointer[i]->fail()){

            // logger.log("DONE " + to_string(i));

            flag[i] = false;
            filePointer[i] = NULL;
            fileNumber[i] = 0;
            prevContent[i] = {-1, -1, -1};

        }

        fileNumber[i]++;
    }

    // logger.log(to_string(this->N) + " : " + to_string(this->N) );
    string space = " ";

    int loop = this->N;
    loop = min(loop, pc);

    for(int r = 1; r <= loop; r++){
        for(int c = 1; c <= loop; c++){

            int idx = r - c;
            // cout<<r<<":"<<c<<":";
            // cout<<prevContent[idx][0]<<":"<<prevContent[idx][1]<<":"<<prevContent[idx][2]<<space;

            if(flag[idx]){

                if(prevContent[idx][0] > r){
                    cout<<"0"<<space;
                }else if(prevContent[idx][0] == r){
                    cout<<prevContent[idx][2]<< space;
                }else{
                    
                    prevContent[idx] = getSparseMatrixNumber(idx, flag, filePointer, fileNumber);

                    // cout<<prevContent[idx][0]<<":"<<prevContent[idx][1]<<":"<<prevContent[idx][2]<<space;

                    if(prevContent[idx][0] > r){
                        cout<<"0"<<space;
                    }else if(prevContent[idx][0] == r){
                        cout<<prevContent[idx][2]<<space;
                    }else{
                        cout<<"0"<<space;
                    }
                }

            }else{
                cout<<"0"<<space;
            }

            // cout<<endl;
        }
        cout<<endl;
    }
    cout<<endl;

    for(int i = strt; i <= end; i++){

        if(flag[i]){
            filePointer[i]->close();
            delete filePointer[i];

            filePointer[i] = NULL;    
        }
    }
}

void Matrix::printNonSparse(){
    
    logger.log("Matrix::printNonSparse");
    ifstream * pagePointers;

    string pagePath = getPagePath(1, 1);
    pagePointers = new ifstream(pagePath, ios::in);

    string space = " ";

    for(int rr = 0; rr < this->printConstraints; rr++){
        for(int cc = 0; cc < this->printConstraints; cc++){
                
            char ch;
            string numberString = "";

            while(pagePointers->get(ch)){

                if(ch == ' ' || ch == '\n')
                    break;

                numberString.push_back(ch);
            }

            if(numberString == "")
                return;

            int number = stoi(numberString);
            cout<<number<< space;

            if(ch == '\n')
                break;

            if(cc + 1 == this->printConstraints){
                while(pagePointers->get(ch)){
                    if(ch == '\n')
                        break;
                }
            }
        }
        cout<<endl;
    }  
}


/**
 * @brief Function prints the first few rows of the matrix. If the matrix contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");

    
    if(this->isSparse == true){
        printSparse();
    }else{
        printNonSparse();
    }
}


/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanentSparse()
{
    logger.log("Matrix::makePermanentSparse");
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    // logger.log("Matrix::printSparse");
        
    unordered_map<int, bool> flag;
    unordered_map<int, ifstream *> filePointer;
    unordered_map<int, int> fileNumber;
    unordered_map<int, vector<int>> prevContent;

    int strt = this->N * -1;
    int end = this->N;
    

    for(int i = strt; i <= end; i++){

        flag[i] = true;
        filePointer[i] = NULL;
        fileNumber[i] = 0;
        prevContent[i] = {-1, -1, -1};

        string pagePath = getPagePathSparse(i, fileNumber[i]);
        
        filePointer[i] = new ifstream(pagePath, ios :: in);
        if(filePointer[i]->fail()){
            flag[i] = false;
            filePointer[i] = NULL;
            fileNumber[i] = 0;
            prevContent[i] = {-1, -1, -1};

        }

        fileNumber[i]++;
    }
    string space = ",";

    for(int r = 1; r <= this->N; r++){
        for(int c = 1; c <= this->N; c++){

            int idx = r - c;

            if(flag[idx]){

                if(prevContent[idx][0] > r){
                    fout<<"0"<<space;
                }else if(prevContent[idx][0] == r){
                    fout<<prevContent[idx][2]<< space;
                }else{
                    
                    prevContent[idx] = getSparseMatrixNumber(idx, flag, filePointer, fileNumber);

                    if(prevContent[idx][0] > r){
                        fout<<"0"<<space;
                    }else if(prevContent[idx][0] == r){
                        fout<<prevContent[idx][2]<<space;
                    }else{
                        fout<<"0"<<space;
                    }
                }

            }else{
                fout<<"0"<<space;
            }
        }
        fout<<"\n";
    }

    fout.close();
}

void Matrix::makePermanentNonSparse()
{
    logger.log("Matrix::makePermanentNonSparse");
    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);
    
    int currRow = 0;

    for(int r=1; r <= this->maxR; r++){

        vector<ifstream *> pagePointers(this->maxC+1, NULL);

        for(int c=1; c <= this->maxC; c++){
            
            string pagePath = getPagePath(r, c);
            pagePointers[c] = new ifstream(pagePath, ios::in);
        }

        for(int rr = 0; rr < this->M; rr++){
            for(int c=1; c <= this->maxC; c++){
                for(int cc = 0; cc < this->M; cc++){
                
                    char ch;
                    string numberString = "";

                    while(pagePointers[c]->get(ch)){

                        if(ch == ' ' || ch == '\n')
                            break;
        
                        numberString.push_back(ch);
                    }

                    int number = stoi(numberString);
                    fout<<number<<",";

                    if(ch == '\n')
                        break;
                }
            }
            fout<<"\n";

            currRow++;
            if(currRow == this->N)
                break;
        }

        for(int c=1; c <= this->maxC; c++){
            pagePointers[c]->close();
            delete pagePointers[c];

            pagePointers[c] = NULL;
        }

        if(currRow == this->N)
            break;
    }
    fout.close();
}

void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);

    if(this->isSparse == true){
        makePermanentSparse();
    }else{
        makePermanentNonSparse();
    }
}

/**
 * @brief Function to check if matrix is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the matrix from the database by deleting
 * all temporary files created as part of this matrix
 *
 */
void Matrix::unload(){
    logger.log("Matrix::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->matrixName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}
