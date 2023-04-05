/*
Group No. 15
ID: 2020A7PS0981P               Name: Utkarsh Darolia
ID: 2020A7PS0084P               Name: Tanveer Singh
ID: 2020A7PS0124P			    Name: Shivam Abhay Pande
ID: 2020A7PS0980P			    Name: Mithil Shah
ID: 2020A7PS0120P			    Name: Kshitij Garg
*/

#include "lexer.h"
int BUFFSIZE;
bool printErrors=true;
char* buff = NULL;
int lineCounter=1;
int lexemeBegin=0;
int forward=0;
bool bufferInit=false;

const char* const tokenToString[]={
    [ID]="ID",
    [NUM]="NUM",
    [RNUM]="RNUM",
    [AND]="AND",
    [OR]="OR",
    [TRUE]="TRUE",
    [FALSE]="FALSE",
    [PLUS]="PLUS",
    [MINUS]="MINUS",
    [MUL]="MUL",
    [DIV]="DIV",
    [LT]="LT",
    [LE]="LE",
    [GE]="GE",
    [GT]="GT",
    [EQ]="EQ",
    [NE]="NE",
    [DEF]="DEF",
    [ENDDEF]="ENDDEF",
    [DRIVERDEF]="DRIVERDEF",
    [DRIVERENDDEF]="DRIVERENDDEF",
    [COLON]="COLON",
    [RANGEOP]="RANGEOP",
    [SEMICOL]="SEMICOL",
    [COMMA]="COMMA",
    [ASSIGNOP]="ASSIGNOP",
    [SQBO]="SQBO",
    [SQBC]="SQBC",
    [BO]="BO",
    [BC]="BC",
    [INTEGER]="INTEGER",
    [REAL]="REAL",
    [BOOLEAN]="BOOLEAN",
    [OF]="OF",
    [ARRAY]="ARRAY",
    [START]="START",
    [END]="END",
    [DECLARE]="DECLARE",
    [MODULE]="MODULE",
    [DRIVER]="DRIVER",
    [PROGRAM]="PROGRAM",
    [GET_VALUE]="GET_VALUE",
    [PRINT]="PRINT",
    [USE]="USE",
    [WITH]="WITH",
    [PARAMETERS]="PARAMETERS",
    [TAKES]="TAKES",
    [INPUT]="INPUT",
    [RETURNS]="RETURNS",
    [FOR]="FOR",
    [IN]="IN",
    [SWITCH]="SWITCH",
    [CASE]="CASE",
    [BREAK]="BREAK",
    [DEFAULT]="DEFAULT",
    [WHILE]="WHILE",
    [ENDOFFILE]="ENDOFFILE",
    [ERROR]="ERROR"
};

void fill_keyword(ht* lookup_table, char* keyword, Token t){
    Token *token = malloc(sizeof(Token));
    *token = t;
    if(ht_store(lookup_table, keyword, token) == NULL) exit_nomem();
}

void fill_lookup_table(ht* lookup_table){
    fill_keyword(lookup_table, "integer", INTEGER);
    fill_keyword(lookup_table, "real", REAL);
    fill_keyword(lookup_table, "boolean", BOOLEAN);
    fill_keyword(lookup_table, "of", OF);
    fill_keyword(lookup_table, "array", ARRAY);
    fill_keyword(lookup_table, "start", START);
    fill_keyword(lookup_table, "end", END);
    fill_keyword(lookup_table, "declare", DECLARE);
    fill_keyword(lookup_table, "module", MODULE);
    fill_keyword(lookup_table, "driver", DRIVER);
    fill_keyword(lookup_table, "program", PROGRAM);
    fill_keyword(lookup_table, "get_value", GET_VALUE);
    fill_keyword(lookup_table, "print", PRINT);
    fill_keyword(lookup_table, "use", USE);
    fill_keyword(lookup_table, "with", WITH);
    fill_keyword(lookup_table, "parameters", PARAMETERS);
    fill_keyword(lookup_table, "takes", TAKES);
    fill_keyword(lookup_table, "input", INPUT);
    fill_keyword(lookup_table, "returns", RETURNS);
    fill_keyword(lookup_table, "for", FOR);
    fill_keyword(lookup_table, "in", IN);
    fill_keyword(lookup_table, "switch", SWITCH);
    fill_keyword(lookup_table, "case", CASE);
    fill_keyword(lookup_table, "break", BREAK);
    fill_keyword(lookup_table, "default", DEFAULT);
    fill_keyword(lookup_table, "while", WHILE);
    fill_keyword(lookup_table, "AND", AND);
    fill_keyword(lookup_table, "OR", OR);
    fill_keyword(lookup_table, "true", TRUE);
    fill_keyword(lookup_table, "false", FALSE);
}

void print_lookup_table(ht* lookup_table){
    ht_itr itr = ht_iterator(lookup_table);
    while(ht_next_entry(&itr)){
        printf("%s %s\n", itr.key, tokenToString[*(Token*)itr.data]);
    }
}

void getStream(FILE *fp){               //function to populate the twin buffers and modify the lexemeBegin and forward pointers accordingly. 
    int flag=0;                                                 //the flag helps determine if we have reached the end of the file.
    if(lexemeBegin>=BUFFSIZE||forward==2*BUFFSIZE-1){           //signifies that we do not require the OLD buffer/first part of the buffer anymore.
        if(lexemeBegin-BUFFSIZE>0){
            lexemeBegin=lexemeBegin-BUFFSIZE;
        }
        else 
            lexemeBegin=0;
        if(forward-BUFFSIZE>0){
            forward=forward-BUFFSIZE;
        }
        else 
            forward=0;

        for(int i=0;i<BUFFSIZE;i++){                            //moving NEW part of the buffer to the OLD part.
            buff[i]=buff[BUFFSIZE+i];
        }
        flag=fread(buff+BUFFSIZE,1,BUFFSIZE,fp);                //reading data into the NEW part of the buffer.
        if(flag!=BUFFSIZE){
            buff[BUFFSIZE+flag]=EOF;                            //Add EOF character after the read data.
        }
    }
}

char nextChar(FILE* fp){                //function for fetching the nex character from the twin buffer and updating the the twin buffer if needed.
    if(forward==2*BUFFSIZE-1){
        getStream(fp);
    }
    forward++;
    return buff[forward];
}

struct token_info tokenize(enum Token t,int begin,int end,int ln){          //function to tokenize the specified data
    char lexeme[end-begin+2];
    strncpy(lexeme,buff+begin,end-begin+1);
    lexeme[end-begin+1] = '\0';
    struct token_info T;
    T.token=t;
    strcpy(T.lexeme,lexeme);
    T.lineNumber=ln;
    if(t==NUM)
        T.value.num=atoi(lexeme);
    else if(t==RNUM)
        T.value.rnum=atof(lexeme);
    else if(t==TRUE)
        T.value.b=true;
    else if(t==FALSE)
        T.value.b=false;
    return T;
}

struct token_info IdNumTokenize(enum Token t,char* lexeme,int ln){          //function for tokenizing NUMs RNUMs IDs and KEYWORDS
                                                                            // different from tokenizer() as these types of tokens have a length limit   
    struct token_info T;
    T.token=t;
    strcpy(T.lexeme,lexeme);
    T.lineNumber=ln;
    if(t==NUM)
        T.value.num=atoi(lexeme);
    else if(t==RNUM)
        T.value.rnum=atof(lexeme);
    else if(t==TRUE)
        T.value.b=true;
    else if(t==FALSE)
        T.value.b=false;
    return T;
}

void retract(int count){                //function for moving the forward pointer back by the specified spaces over the twin buffer
    forward-=count;
}

void movePointerPair(FILE* fp){         //function to move both lexemeBegin and forward pointer to the start of the next possible token. Also update the twin buffer if needed.
    lexemeBegin++;
    forward++;
    getStream(fp);
}

void moveLexemeBegin(FILE* fp){         //function to move lexemeBegin pointer to the forward pointer after tokenizing current token. Also update the twin buffer if needed.
    lexemeBegin=forward;
    getStream(fp);
}

void throwLE(int begin,int end, int lineNumber){            //function for printing Lexical Errors
    if(printErrors){
        char lexeme[end-begin+2];
        strncpy(lexeme,buff+begin,end-begin+1);
        lexeme[end-begin+1]='\0';
        printf("LEXICAL ERROR: Line Number %d, Lexeme: %s\n",lineNumber,lexeme);
    }
}

void throwLEOverflow(char* lexeme,int lineNumber){          //funcion for printing lexical errors which may be due to the lexeme being too long
    if(printErrors){
        if(strlen(lexeme)>20)
            printf("LEXICAL ERROR (lexeme too long): Line Number %d, Lexeme: %s\n",lineNumber,lexeme);
        else
            printf("LEXICAL ERROR: Line Number %d, Lexeme: %s\n",lineNumber,lexeme);
    }
}

struct token_info getNextToken(ht* lookup_table, FILE* fp){         //function for the lexer that returns the next token to the parser.
    int state=0;
    struct token_info curToken;
    while(1){
        char ch=buff[lexemeBegin];
        char lexeme[MAXLEXEMESIZE]="";
        int curLexemeSize=1;
        switch(state){
            case 0:
            /*START STATE*/
                if(ch=='+')
                    state=1;
                else if(ch=='-')
                    state=2;
                else if(ch=='/')
                    state=3;
                else if(ch=='!')
                    state=4;
                else if(ch=='.')
                    state=5;
                else if(ch==';')
                    state=6;
                else if(ch==',')
                    state=7;
                else if(ch=='[')
                    state=8;
                else if(ch==']')
                    state=9;
                else if(ch=='(')
                    state=10;
                else if(ch==')')
                    state=11;
                else if(ch=='=')
                    state=12;
                else if(ch==' '||ch=='\t')
                    state=13;
                else if(isdigit(ch))
                    state=14;
                else if(ch=='<')
                    state=15;
                else if(ch=='>')
                    state=16;
                else if(ch=='*')
                    state=17;
                else if(ch==':')
                    state=18;
                else if(ch=='_'||isalpha(ch))
                    state=19;
                else if(ch=='\n')
                    state=20;
                else if(ch==EOF)
                    state=21;
                else    
                    state=-1;
                break;
            case 1:
            /*PLUS*/
                curToken=tokenize(PLUS,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 2:
            /*MINUS*/
                curToken=tokenize(MINUS,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 3:
            /*DIV*/
                curToken=tokenize(DIV,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 4:
            /*NE*/
                switch(nextChar(fp)){
                    case '=':
                        curToken=tokenize(NE,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                        retract(1);
                        throwLE(lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return tokenize(ERROR,lexemeBegin,forward,lineCounter);
                }
            case 5:
            /*RANGEOP*/    
                switch(nextChar(fp)){
                    case '.':
                        curToken=tokenize(RANGEOP,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                        retract(1);
                        throwLE(lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return tokenize(ERROR,lexemeBegin,forward,lineCounter);
                }
            case 6:
            /*SEMICOL*/
                curToken=tokenize(SEMICOL,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 7:
            /*COMMA*/
                curToken=tokenize(COMMA,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 8:
            /*SQBO*/
                curToken=tokenize(SQBO,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 9:
            /*SQBC*/
                curToken=tokenize(SQBC,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 10:
            /*BO*/
                curToken=tokenize(BO,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 11:
            /*BC*/    
                curToken=tokenize(BC,lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return curToken;
            case 12:
            /*EQ*/    
                switch(nextChar(fp)){
                    case '=':
                        curToken=tokenize(EQ,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                        retract(1);
                        throwLE(lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return tokenize(ERROR,lexemeBegin,forward,lineCounter);
                }
            case 13:
            /*WHITESPACES (TABS AND SPACES)*/
                movePointerPair(fp);
                state=0;
                break;
            case 14:
            /*NUM RNUM*/
                strncat(lexeme,&ch,1);
                while(1){
                    ch=nextChar(fp);
                    if(isdigit(ch)){
                        strncat(lexeme,&ch,1);
                        curLexemeSize++;
                    }
                    else if(ch=='.'){
                        strncat(lexeme,&ch,1);
                        curLexemeSize++;
                        break;
                    }
                    else{
                        moveLexemeBegin(fp);
                        return IdNumTokenize(NUM,lexeme,lineCounter);
                    }
                }

                ch=nextChar(fp);
                if(ch=='.'){
                    retract(1);
                    moveLexemeBegin(fp);
                    char returnLexeme[curLexemeSize];
                    strcpy(returnLexeme,lexeme);
                    returnLexeme[curLexemeSize-1]='\0';
                    return IdNumTokenize(NUM,returnLexeme,lineCounter);
                }
                else if(isdigit(ch)){
                    strncat(lexeme,&ch,1);
                }
                else{
                    moveLexemeBegin(fp);
                    ch='\0'; 
                    strncat(lexeme,&ch,1);
                    throwLEOverflow(lexeme, lineCounter);
                    return IdNumTokenize(ERROR,lexeme,lineCounter);
                }

                while(1){
                    ch=nextChar(fp);
                    if(isdigit(ch)){
                        strncat(lexeme,&ch,1);
                    }
                    else if(ch=='e'||ch=='E'){
                        strncat(lexeme,&ch,1);
                        break;
                    }
                    else{
                        moveLexemeBegin(fp);
                        return IdNumTokenize(RNUM,lexeme,lineCounter);
                    }
                }

                ch=nextChar(fp);
                if(ch=='+'||ch=='-'){
                    strncat(lexeme,&ch,1);
                    ch=nextChar(fp);
                    if(isdigit(ch)){
                        strncat(lexeme,&ch,1);
                    }
                    else{
                        moveLexemeBegin(fp);
                        ch='\0'; 
                        strncat(lexeme,&ch,1);
                        throwLEOverflow(lexeme,lineCounter); 
                        return IdNumTokenize(ERROR,lexeme,lineCounter);
                    }
                }
                else if(isdigit(ch)){
                    strncat(lexeme,&ch,1);
                }
                else{
                    moveLexemeBegin(fp);
                    ch='\0'; 
                    strncat(lexeme,&ch,1);
                    throwLEOverflow(lexeme,lineCounter); 
                    return IdNumTokenize(ERROR,lexeme,lineCounter);
                }

                while(1){
                    ch=nextChar(fp);
                    if(isdigit(ch)){
                        strncat(lexeme,&ch,1);
                    }
                    else{
                        moveLexemeBegin(fp);
                        return IdNumTokenize(RNUM,lexeme,lineCounter);
                    }
                }
                break;
            case 15:
            /*LT LE DEF DRIVERDEF*/
                switch(nextChar(fp)){
                    case '<':
                    /*DEF DRIVERDEF*/
                        state = 22;
                        break;
                    case '=':
                    /*LE*/
                        curToken=tokenize(LE,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                    /*LT*/
                        retract(1);
                        curToken=tokenize(LT,lexemeBegin,forward,lineCounter);
                        movePointerPair(fp);
                        return curToken;
                }
                break;
            case 16:
            /*GT GE ENDDEF DRIVERENDDEF*/
                switch(nextChar(fp)){
                    case '>':
                    /*ENDDEF DRIVERENDDEF*/
                        state = 23;
                        break;
                    case '=':
                    /*GE*/
                        curToken=tokenize(GE,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                    /*GT*/
                        retract(1);
                        curToken=tokenize(GT,lexemeBegin,forward,lineCounter);
                        movePointerPair(fp);
                        return curToken;
                }
                break;
            case 17:
            /*MUL COMMENTMARK*/
                switch(nextChar(fp)){
                    case '*':
                    /*COMMENTMARK*/
                        state=24;
                        break;
                    default:
                    /*MUL*/
                        retract(1);
                        curToken=tokenize(MUL,lexemeBegin,forward,lineCounter);
                        movePointerPair(fp);
                        return curToken;
                }
                break;
            case 18:
            /*COLON ASSIGNOP*/
                switch(nextChar(fp)){
                    case '=':
                    /*ASSIGNOP*/ 
                        curToken=tokenize(ASSIGNOP,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                    /*COLON*/
                        retract(1);
                        curToken=tokenize(COLON,lexemeBegin,forward,lineCounter);
                        movePointerPair(fp);
                        return curToken;
                }
            case 19:
            /*T_ID*/
                strncat(lexeme,&ch,1);
                while(1){
                    ch=nextChar(fp);
                    if(isalnum(ch)||ch=='_'){
                        strncat(lexeme,&ch,1);
                        moveLexemeBegin(fp);
                        curLexemeSize++;
                    }
                    else{
                        moveLexemeBegin(fp);
                        break;
                    }
                }
                if(curLexemeSize>20){
                    ch='\0'; 
                    strncat(lexeme,&ch,1);
                    throwLEOverflow(lexeme,lineCounter);
                    return tokenize(ERROR,lexemeBegin,forward,lineCounter);
                }   
                else{
                    
                    enum Token* t = (Token*)ht_fetch(lookup_table,lexeme);
                    if(t==NULL)
                        return IdNumTokenize(ID,lexeme,lineCounter);
                    else    
                        return IdNumTokenize(*t,lexeme,lineCounter);
                }
                break;
            case 20:
            /*/n*/
                lineCounter++;
                movePointerPair(fp);
                state=0;
                break;
            case 21:
            /*EOF*/
                return tokenize(ENDOFFILE,lexemeBegin,forward,lineCounter);
            case 22:
            /*DEF DRIVERDEF*/
                switch(nextChar(fp)){
                    case '<':
                    /*DRIVERDEF*/ 
                        curToken=tokenize(DRIVERDEF,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                    /*DEF*/
                        retract(1);
                        curToken=tokenize(DEF,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                }
            case 23:
            /*ENDDEF DRIVERENDDEF*/
                switch(nextChar(fp)){
                    case '>': 
                    /*DRIVERENDDEF*/
                        curToken=tokenize(DRIVERENDDEF,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                    default:
                    /*ENDDEF*/
                        retract(1);
                        curToken=tokenize(ENDDEF,lexemeBegin,forward,lineCounter);
                        moveLexemeBegin(fp);
                        movePointerPair(fp);
                        return curToken;
                }
            case 24:
            /*COMMENTMARK*/
                moveLexemeBegin(fp);
                int flag=0;
                while(1){
                    if(flag==1)
                        break;
                    switch(nextChar(fp)){
                        case '*':
                            if(nextChar(fp)=='*'){      //comment ended
                                flag=1;
                                moveLexemeBegin(fp);
                                movePointerPair(fp);
                            }
                            else                        //single * character was read
                                moveLexemeBegin(fp);
                            break;
                        case EOF: 
                            flag=1;
                            state=21;
                            break;
                        case '\n':
                            lineCounter++;              //increment line counter and let it fall through to default case
                        default:            
                            moveLexemeBegin(fp);        //normal caharacter read inside comment
                    }
                }
                state=0;
                break;
            default:
            /*LEXICAL ERROR*/
                throwLE(lexemeBegin,forward,lineCounter);
                movePointerPair(fp);
                return tokenize(ERROR,lexemeBegin,forward,lineCounter);
        }
    }
}

int removeComments(FILE* fp){         //function to generate the clean comment free file. Returns 0 for successful execution and 1 for error.
    bool commentStart=false;                    //flag denoting if the current file contents lie within a comment.
    char internalBuffer[RCBUFFSIZE];
    fseek(fp,0,SEEK_SET);
    if(fp==NULL){
        printf("Error opening test case file!\n");
        return 1;
    }
    while(fgets(internalBuffer,RCBUFFSIZE,fp)!=NULL){
        int i=0;
        while(i<RCBUFFSIZE&&internalBuffer[i]!='\0'){
            // We simply toggle the commentStart flag on encountering a ** and print accordingly.
            if(internalBuffer[i]=='*'&&(i+1)<RCBUFFSIZE&&internalBuffer[i+1]=='*'){
                commentStart=!commentStart;
                printf("  ");
                i++;
            }
            else if((commentStart==false||internalBuffer[i]=='\n')&&internalBuffer[i]!='\0'){
                printf("%c",internalBuffer[i]);
            }
            else if(commentStart==true&&internalBuffer[i]!='\0'){
                printf(" ");
            }
            i++;
        }
    }
    printf("\n");
    return 0;
}

void printLexemes(FILE* fp,int buff_size,ht* lookup_table){
    printErrors=true;
    start_lexer(fp, buff_size,true);
    if(lookup_table == NULL) exit_nomem();
    fill_lookup_table(lookup_table);
    /*Code Runner for testing Lexer*/
    while(1){
        struct token_info t=getNextToken(lookup_table,fp);
        if(t.token==ENDOFFILE)
            break;
        if(t.token!=ERROR)
            printf("%-20d | %-20s | %-20s \n",t.lineNumber,t.lexeme,tokenToString[t.token]);
    }
}

void start_lexer(FILE* fp, int buff_size,bool pe){             //Driver function for lexer testing. Create and initialize the Lookup table and initialize the Twin buffer.
    printErrors=pe;
    lexemeBegin=0;
    forward=0;
    lineCounter=1;
    BUFFSIZE = buff_size;
    if (bufferInit == false)
        buff = (char*)malloc(2*BUFFSIZE*sizeof(char));
    fseek(fp,0,SEEK_SET);
    
    int flag=0;
    flag=fread(buff,1,2*BUFFSIZE,fp);
    if(flag!=2*BUFFSIZE){                           //if number of bytes read is less than specified number, we have reached the end of file.
        buff[flag]=EOF;                             //Add EOF character after the read data.
    }
    bufferInit = true;
}

void deallocateBuff () {
    if (buff)
        free(buff);
}
