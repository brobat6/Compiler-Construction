#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

// Linked List

typedef struct LL_NODE llNode;
struct LL_NODE {
    void* data;
    llNode* next;
    llNode* prev;
};

typedef struct LL_HEAD llHead;
struct LL_HEAD {
    void* data;
    llNode* first_node;
    llNode* last_node;
    int count; // Length of corresponding grammar rule
    unsigned long long first_set[2];
    int grammar_rule;
};

llHead* initializeHead () {
    llHead* cur_head = (llHead*) malloc(sizeof(llHead));
    cur_head->first_node = NULL;
    cur_head->last_node = NULL;
    cur_head->count = 0;
    cur_head->data = NULL;
    cur_head->first_set[0] = 0ull;
    cur_head->first_set[1] = 0ull;
    cur_head->grammar_rule = -1;
    return cur_head;
}

llNode* initializeNode () {
    llNode* cur_node = (llNode*) malloc(sizeof(llNode));
    cur_node->next = NULL;
    cur_node->prev = NULL;
    cur_node->data = NULL;
    return cur_node;
}

llHead* insertAtEnd (llHead* head, llNode* node) {
    if (head->count == 0) {
        head->first_node = node;
        head->last_node = node;
        head->count++;
        return head;
    }
    head->last_node->next = node;
    node->prev = head->last_node;
    head->last_node = node;
    head->count++;
    return head;
}

void removeFromEnd (llHead* head) {
    if (head->count == 0) {
        printf("Nothing to remove!\n");
        return;
    }
    if (head->count == 1) {
        llNode* temp = head->last_node;
        temp->next = NULL;
        temp->prev = NULL;
        head->first_node = NULL;
        head->last_node = NULL;
        head->count--;
        free(temp);
        return;
    }
    llNode* temp = head->last_node;
    head->last_node = head->last_node->prev;
    head->last_node->next = NULL;
    temp->next = NULL;
    temp->prev = NULL;
    head->count--;
    free(temp);
}

// Grammar Symbol

typedef struct GRAMMAR_SYMBOL grammar_symbol;
struct GRAMMAR_SYMBOL {
    int epsilon, terminal, non_terminal;
    char* name;
    int start, end;
    unsigned long long* first_set;
    unsigned long long* follow_set;
    llNode** occurences;  // occurences of grammar symbol on rhs
    int occurence_count;
    int* occurence_rule;  // rule numbers corresponding to above occurences
};

grammar_symbol* initialize_grammar_symbol (grammar_symbol* cur_node, int name_size) {
    cur_node = (grammar_symbol*) malloc(sizeof(grammar_symbol));
    cur_node->epsilon = 0;
    cur_node->terminal = 0;
    cur_node->non_terminal = 0;
    cur_node->name = (char*) calloc(name_size, sizeof(char));
    cur_node->start = -1;
    cur_node->end = -1;
    cur_node->first_set = NULL;
    cur_node->follow_set = NULL;
    cur_node->occurences = NULL;
    cur_node->occurence_count = 0;
    cur_node->occurence_rule = NULL;
    return cur_node;
}

void deallocateGrammarSymbol (grammar_symbol* symbol) {
    if (symbol == NULL) {
        return;
    }
    if (symbol->name != NULL) {
        free(symbol->name);
    }
    if (symbol->first_set != NULL) {
        free(symbol->first_set);
    }
    if (symbol->follow_set != NULL) {
        free(symbol->follow_set);
    }
    if (symbol->occurences != NULL) {
        free(symbol->occurences);
    }
    if (symbol->occurence_rule != NULL) {
        free(symbol->occurence_rule);
    }
    free(symbol);
}

// Hash Table

#define INIT_CAP 16
#define OFFSET 2166136261
#define PRIME 16777619

typedef struct{
    const char* key;
    void* data;
} ht_entry;

typedef struct{
    ht_entry* entry;
    size_t cap;
    size_t size;
} ht;

typedef struct{
    const char* key; 
    void* data;      

    //dont use directly
    ht* _table; 
    size_t _index;    
} ht_itr;

void exit_nomem(void) {
    fprintf(stderr, "Out of memory!\n");
    exit(1);
}

ht* init_ht(void){
    //Allocate memory for the hashtable.
    ht* tab = malloc(sizeof(ht));
    if (tab == NULL) return NULL;

    //Initialize the hashtable.
    tab->cap = INIT_CAP;
    tab->size = 0;

    //Allocate memory for the entry buckets.
    tab->entry = calloc(tab->cap, sizeof(ht_entry));
    if (tab->entry == NULL){
        //If could not allocate, free table before returning.
        free(tab); 
        return NULL;
    }

    return tab;
}

void delete_ht(ht* tab) {
    //Free the keys first.
    if (tab == NULL) return;
    for (size_t i = 0; i < tab->cap; i++) {
        if (tab->entry[i].key) free((void *)tab->entry[i].key);
        if (tab->entry[i].data) free(tab->entry[i].data);
    }

    //Then free the entries.
    free(tab->entry);

    //Finally free the table.
    free(tab);
}

size_t ht_size(ht* tab) {
    //Returns the number of distinct entries in the hastable
    return tab->size;
}

ht_itr ht_iterator(ht* tab) {
    //Initialize an interator.
    ht_itr itr;

    //Reference the table and index to the iterator.
    itr._table = tab;
    itr._index = 0;

    return itr;
}

bool ht_next_entry(ht_itr* itr) {
    //Reference the table to the iterator.
    ht* tab = itr->_table;

    //Loop over all the entries till end of table.
    while (itr->_index < tab->cap) {
        size_t i = itr->_index;
        itr->_index++;
        // printf("%d, ", (int)i);

        //If next non-empty entry is obtained, update iterator and key value.
        if (tab->entry[i].key != NULL) {
            // printf("%d, :", (int)i);
            ht_entry entry = tab->entry[i];

            itr->key = entry.key;
            itr->data = entry.data;

            return true;
        }
    }

    //If there is no next entry return false.
    return false;
}

uint32_t generate_hashkey(const char* key) {
    //Initialize the hash value to the offset
    uint32_t hash = OFFSET;

    //Calculate the hash value for the key
    for (const char* p = key; *p; p++) {
        hash ^= (uint32_t)(unsigned char)(*p);
        hash *= PRIME;
    }

    return hash;
}

void* ht_fetch(ht* tab, const char* key) {
    //We mod the hash wrt hashtable capacity to generate index for the key.
    uint32_t hash = generate_hashkey(key);
    size_t hk = (size_t)(hash % (uint32_t)(tab->cap));

    //We try to find our entry till we encounter an empty entry(linear probing).
    while (tab->entry[hk].key != NULL) {
        //Key is found and we return the data in the entry
        if (strcmp(key, tab->entry[hk].key) == 0) return tab->entry[hk].data;
        
        //Key wasn't in this entry so we move to the next entry using linear probing.
        hk++;

        //If we reach end of table, we jump to first entry using mod function.
        hk %= tab->cap;
    }

    //If entry corresponding to key is not found, we return NULL
    return NULL;
}

const char* ht_store_entry(ht_entry* entry, size_t cap, const char* key, void* data, size_t* cnt) {
    //We mod the hash wrt capacity to generate index for the key.
    uint32_t hash = generate_hashkey(key);
    size_t hk = (size_t)(hash % (uint32_t)(cap));

    //We loop till we find an empty entry.
    while (entry[hk].key != NULL){
        //If the key already exists, we update the value.
        if (strcmp(key, entry[hk].key) == 0) {
            entry[hk].data = data;
            return entry[hk].key;
        }

        //If key is not found we continue searching by linear probing.
        hk++;
        hk %= cap;
    }

    //If we did not find the key, we copy and increase the count.
    if (cnt != NULL){
        key = strdup(key);
        if (key == NULL) return NULL;

        (*cnt)++;
    }

    //Then we allocate the new entry.
    entry[hk].key = (char*)key;
    entry[hk].data = data;

    return key;
}

bool ht_resize(ht* tab) {
    //We double the table size when we resize.
    size_t new_cap = tab->cap * 2;

    //This case handles overflow.
    if (new_cap < tab->cap) return false;

    //We make a new entries record nd allocate memory to it.
    ht_entry* new_entry = calloc(new_cap, sizeof(ht_entry));
    if (new_entry == NULL) return false;

    //We populate the new entries with the old entries suitable to the new size of the hashtable.
    for (size_t i = 0; i < tab->cap; i++) {
        ht_entry entry = tab->entry[i];
        if (entry.key != NULL) ht_store_entry(new_entry, new_cap, entry.key,entry.data, NULL);
    }

    //We free the memory the old entries were holding.
    free(tab->entry);

    //We reference the new entries and capacity to the hashtable.
    tab->entry = new_entry;
    tab->cap = new_cap;

    return true;
}

const char* ht_store(ht* tab, const char* key, void* data) {
    //We assert that if data is NULL we have made an error in code logic and must throw an error + abort.
    assert(data != NULL);
    if (data == NULL) return NULL;

    //If hashtable is filled beyond 50%, we double the size of the hashtable to reduce collisions and accomodate new entries.
    if (tab->size >= tab->cap / 2) {
        //If we fail to resize, return NULL to show we could not store the new entry.
        if (!ht_resize(tab)) return NULL;
    }

    //Once resized, we finally store the entry in the hash table.
    return ht_store_entry(tab->entry, tab->cap, key, data, &tab->size);
}

int ht_fetch_index(ht* tab, const char* key) {
    //We mod the hash wrt hashtable capacity to generate index for the key.
    uint32_t hash = generate_hashkey(key);
    size_t hk = (size_t)(hash % (uint32_t)(tab->cap));

    //We try to find our entry till we encounter an empty entry(linear probing).
    while (tab->entry[hk].key != NULL) {
        //Key is found and we return the data in the entry
        if (strcmp(key, tab->entry[hk].key) == 0) return (int)hk;
        
        //Key wasn't in this entry so we move to the next entry using linear probing.
        hk++;

        //If we reach end of table, we jump to first entry using mod function.
        hk %= tab->cap;
    }

    //If entry corresponding to key is not found, we return NULL
    return -1;
}

// Lexer

#define MAXLEXEMESIZE 50
#define RCBUFFSIZE 200

typedef enum Token{
    ID,
    NUM,
    RNUM,
    AND,
    OR,
    TRUE,
    FALSE,
    PLUS,
    MINUS,
    MUL,
    DIV,
    LT,
    LE,
    GE,
    GT,
    EQ,
    NE,
    DEF,
    ENDDEF,
    DRIVERDEF,
    DRIVERENDDEF,
    COLON,
    RANGEOP,
    SEMICOL,
    COMMA,
    ASSIGNOP,
    SQBO,
    SQBC,
    BO,
    BC,
    INTEGER,
    REAL,
    BOOLEAN,
    OF,
    ARRAY,
    START,
    END,
    DECLARE,
    MODULE,
    DRIVER,
    PROGRAM,
    GET_VALUE,
    PRINT,
    USE,
    WITH,
    PARAMETERS,
    TAKES,
    INPUT,
    RETURNS,
    FOR,
    IN,
    SWITCH,
    CASE,
    BREAK,
    DEFAULT,
    WHILE,
    ENDOFFILE,
    ERROR
} Token;

union Data{
    int num;
    double rnum;
    bool b;
};

struct token_info{
    enum Token token;
    char lexeme[21];
    int lineNumber;
    union Data value;
};
typedef struct token_info Token_Info;

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

void deallocateBuff () {
    if (buff)
        free(buff);
}

// Tree

typedef struct treeNode {
  grammar_symbol* symbol;
  struct token_info token;
  struct treeNode* firstchild;
  struct treeNode* lastchild;
  struct treeNode* next;
  struct treeNode* prev;
  struct treeNode* parent;
  int grammar_rule;
} treeNode;

treeNode* createTreeNode(grammar_symbol * gs, struct token_info token)
{
    treeNode* new = (treeNode*)malloc(sizeof(treeNode));
    new->symbol=gs;
    new->token=token;
    new->firstchild=NULL;
    new->lastchild=NULL;
    new->next=NULL;
    new->prev=NULL;
    new->parent=NULL;
    new->grammar_rule=-1;
    return new;
}

void addChild(treeNode *parent, treeNode *child) {
  if (parent->firstchild == NULL) {
    parent->firstchild = child;
    parent->lastchild = child;
  }
  else{
    treeNode* temp=parent->firstchild;
    child->next=temp;
    temp->prev=child;
    parent->firstchild=child;
  }
  child->parent = parent;
  child->prev = NULL;
}

treeNode* nextLeaf (treeNode* cur_node) {
  if (cur_node == NULL) {
    return NULL;
  }
  if (cur_node->parent == NULL) {
    // printf("Error found in tree\n");
    return NULL;
  }
  if (cur_node->next == NULL) {
    return nextLeaf(cur_node->parent);
  }
  return cur_node->next;
}

void deallocateTree(treeNode* root){
    if(root==NULL)
    {
        return;
    }
    else if(root->firstchild==NULL)
    {
        free(root);
        return;
    }
    treeNode* curNode=root->firstchild;
    while(curNode->next!=NULL)
    {
        curNode=curNode->next;
        free(curNode->prev);
    }
    free(root->lastchild);
    free(root);
}

// Parser

int BUF_LIMIT = 101;
int MAX_RULE_COUNT = 150;
int MAXIMUM_OCCURENCES_ON_RHS = 20;
bool errorPrint = true;

ht* terminal_ht = NULL;
ht* nonterminal_ht = NULL;
llHead** grammar = NULL;
int rule_count = 0;
llHead*** parse_table = NULL;

void readGrammarFile () {
    FILE* grammar_fp = fopen("grammar.txt", "r");
    grammar = (llHead**) calloc(MAX_RULE_COUNT, sizeof(llHead*));
    char buf[BUF_LIMIT];
    terminal_ht = init_ht();
    nonterminal_ht = init_ht();
    grammar_symbol* dollar_terminal = initialize_grammar_symbol(dollar_terminal, 2);
    dollar_terminal->terminal = 1;
    strcpy (dollar_terminal->name, "$");
    ht_store(terminal_ht, "$", dollar_terminal);

    grammar_symbol* syn_terminal = initialize_grammar_symbol(syn_terminal, 6);
    syn_terminal->terminal = 1;
    strcpy (syn_terminal->name, "#syn#");
    ht_store(terminal_ht, "#syn#", syn_terminal);
    
    while(fgets(buf, BUF_LIMIT - 1, grammar_fp) != NULL) {
        char cur_id[BUF_LIMIT];
        int cur_buf_ptr = 0, cur_id_ptr = 0, cur_len = strlen(buf);
        grammar_symbol *cur_head = NULL, *cur_node = NULL;
        llHead *cur_ll_head = NULL;
        llNode *cur_ll_node = NULL, *prev_ll_node = NULL;
        int arrow = 0;
        while (cur_buf_ptr < cur_len) {
            if (buf[cur_buf_ptr] == '@') {
                // Must be epsilon
                cur_node = ht_fetch(terminal_ht, "@");
                if (cur_node == NULL) {
                    cur_node = initialize_grammar_symbol(cur_node, 2);
                    cur_node->epsilon = 1;
                    strcpy (cur_node->name, "@");
                    ht_store(terminal_ht, "@", cur_node);
                }
                cur_ll_node = initializeNode();
                cur_ll_node->data = cur_node;
                cur_ll_head = insertAtEnd(cur_ll_head, cur_ll_node);
                cur_ll_node = NULL;
            }
            else if (buf[cur_buf_ptr] == ' ' || buf[cur_buf_ptr] == '\n') {
                if (cur_id_ptr != 0) {
                    // Must be a terminal
                    cur_id[cur_id_ptr] = '\0';
                    int cur_len = strlen(cur_id);
                    cur_node = ht_fetch(terminal_ht, cur_id);
                    if (cur_node == NULL) {
                        cur_node = initialize_grammar_symbol(cur_node, cur_len + 1);
                        cur_node->terminal = 1;
                        strcpy (cur_node->name, cur_id);
                        ht_store(terminal_ht, cur_id, cur_node);
                    }
                    cur_ll_node = initializeNode();
                    cur_ll_node->data = cur_node;
                    cur_ll_head = insertAtEnd(cur_ll_head, cur_ll_node);
                    cur_ll_node = NULL;
                    cur_id_ptr = 0;
                }
            }
            else if (buf[cur_buf_ptr] == '-') {
                arrow = 1;
            }
            else if (buf[cur_buf_ptr] == '>') {
                if (arrow == 1) {
                    arrow = 0;
                }
                else {
                    // Must be a non terminal
                    cur_id[cur_id_ptr] = buf[cur_buf_ptr];
                    cur_id_ptr++;
                    cur_id[cur_id_ptr] = '\0';
                    int cur_len = strlen(cur_id);
                    cur_node = ht_fetch(nonterminal_ht, cur_id);
                    if (cur_node == NULL) {
                        cur_node = initialize_grammar_symbol(cur_node, cur_len + 1);
                        cur_node->non_terminal = 1;
                        strcpy (cur_node->name, cur_id);
                        ht_store(nonterminal_ht, cur_id, cur_node);
                    }
                    if (cur_ll_head == NULL) {
                        if (cur_node->start == -1) {
                            cur_node->start = rule_count;
                        }
                        cur_node->end = rule_count;
                        cur_ll_head = initializeHead();
                        cur_ll_head->data = cur_node;
                        grammar[rule_count] = cur_ll_head;
                        cur_ll_head->grammar_rule = rule_count;
                    }
                    else {
                        cur_ll_node = initializeNode();
                        cur_ll_node->data = cur_node;
                        cur_ll_head = insertAtEnd(cur_ll_head, cur_ll_node);
                        if (cur_node->occurences == NULL) {
                            cur_node->occurences = calloc(MAXIMUM_OCCURENCES_ON_RHS, sizeof(llNode*));
                            cur_node->occurence_rule = calloc(MAXIMUM_OCCURENCES_ON_RHS, sizeof(int));
                        }
                        int cur_occurence_count = cur_node->occurence_count;
                        cur_node->occurences[cur_occurence_count] = cur_ll_node;
                        cur_node->occurence_rule[cur_occurence_count] = rule_count;
                        cur_node->occurence_count++;
                        cur_ll_node = NULL;
                    }
                    cur_id_ptr = 0;
                }
            }
            else {
                cur_id[cur_id_ptr] = buf[cur_buf_ptr];
                cur_id_ptr++;
            }
            cur_buf_ptr++;
        }
        rule_count++;
    }
    fclose(grammar_fp);

    // For printing the stored grammar
    // for (int i=0;i<rule_count;i++) {
    //     grammar_symbol* cur_data = grammar[i]->data;
    //     printf("%d. %s ->", i, cur_data->name);
    //     llNode* cur_node = grammar[i]->first_node;
    //     while (cur_node != NULL) {
    //         cur_data = cur_node->data;
    //         cur_node = cur_node->next;
    //         printf(" %s", cur_data->name);
    //     }
    //     printf("\n");
    // }
}

void deallocateNode (llNode* node) {
    if (node == NULL) {
        return;
    }
    grammar_symbol* symbol = node->data;
    free(node);
}

void deallocateHead (llHead* head) {
    if (head == NULL) {
        return;
    }
    grammar_symbol* symbol = head->data;
    free(head);
}

void deallocateGrammar () {
    for (int i= 0; i <= rule_count; i++) {
        llHead* cur_head = grammar[i];
        llNode* cur_node = cur_head->first_node;
        while (cur_node != NULL) {
            llNode* temp = cur_node;
            cur_node = cur_node->next;
            deallocateNode(temp);
        }
        deallocateHead(cur_head);
    }
    free(grammar);
}

void findFirstSet (char* cur_id) {
    grammar_symbol* cur_grammar_symbol = ht_fetch(terminal_ht, cur_id);
    if (cur_grammar_symbol != NULL) {
        if (cur_grammar_symbol->first_set != NULL) return;
        int cur_terminal_pos = ht_fetch_index(terminal_ht, cur_id);
        cur_grammar_symbol->first_set = calloc(2, sizeof(unsigned long long));
        cur_grammar_symbol->first_set[cur_terminal_pos/64] = (1ull<<(cur_terminal_pos%64));
        return;
    }
    cur_grammar_symbol = ht_fetch(nonterminal_ht, cur_id);
    if (cur_grammar_symbol->first_set != NULL) return;
    int start_rule = cur_grammar_symbol->start;
    int end_rule = cur_grammar_symbol->end;
    int epsilon_pos = ht_fetch_index(terminal_ht, "@");
    unsigned long long epsilon = (1ull<<(epsilon_pos%64));
    unsigned long long* cur_first_set = calloc(2, sizeof(unsigned long long));
    cur_first_set[0] = 0ull;
    cur_first_set[1] = 0ull;
    for (int i = start_rule; i <= end_rule; i++) {
        unsigned long long cur_rule_first_set[2] = {0ull, 0ull};
        llNode* cur_llNode = grammar[i]->first_node;
        while (cur_llNode != NULL) {
            grammar_symbol* cur_node = cur_llNode->data;
            if (cur_node->first_set == NULL) {
                findFirstSet (cur_node->name);
            }
            int isEpsilon = cur_node->first_set[epsilon_pos/64] & epsilon;
            cur_rule_first_set[0] = cur_rule_first_set[0] | (cur_node->first_set[0]);
            cur_rule_first_set[1] = cur_rule_first_set[1] | (cur_node->first_set[1]);
            if (isEpsilon) {
                cur_rule_first_set[epsilon_pos/64] ^= epsilon;
            }
            if (cur_node->terminal) {
                break;
            }
            if ((cur_node->first_set[epsilon_pos/64] & epsilon) == 0ull) {
                break;
            }
            cur_llNode = cur_llNode->next;
        }
        if (cur_llNode == NULL) {
            cur_rule_first_set[epsilon_pos/64] |= epsilon;
        }
        grammar[i]->first_set[0] = cur_rule_first_set[0];
        grammar[i]->first_set[1] = cur_rule_first_set[1];
        cur_first_set[0] = cur_first_set[0] | cur_rule_first_set[0];
        cur_first_set[1] = cur_first_set[1] | cur_rule_first_set[1];
    }
    cur_grammar_symbol->first_set = cur_first_set;
    return;
}

void findFollowSet (char* cur_id) {
    grammar_symbol* cur_grammar_symbol = ht_fetch(nonterminal_ht, cur_id);
    if (cur_grammar_symbol->follow_set != NULL) return;
    int cur_occurence_count = cur_grammar_symbol->occurence_count;
    int epsilon_pos = ht_fetch_index(terminal_ht, "@");
    unsigned long long epsilon = (1ull<<(epsilon_pos%64));
    unsigned long long* cur_follow_set = calloc(2, sizeof(unsigned long long));
    cur_follow_set[0] = 0ull;
    cur_follow_set[1] = 0ull;
    for (int i = 0; i < cur_occurence_count; i++) {
        llNode* cur_llNode = cur_grammar_symbol->occurences[i];
        int cur_occurence_rule = cur_grammar_symbol->occurence_rule[i];
        if (cur_llNode->next == NULL) {
            grammar_symbol* cur_rule_head = grammar[cur_occurence_rule]->data;
            if (strcmp(cur_rule_head->name, cur_grammar_symbol->name) == 0) {
                continue;
            }
            if (cur_rule_head->follow_set == NULL) {
                findFollowSet (cur_rule_head->name);
            }
            cur_follow_set[0] = cur_follow_set[0] | (cur_rule_head->follow_set[0]);
            cur_follow_set[1] = cur_follow_set[1] | (cur_rule_head->follow_set[1]);
        }
        else {
            cur_llNode = cur_llNode->next;
            grammar_symbol* cur_node = cur_llNode->data;
            if (cur_node->epsilon == 1) {
                continue;
            }
            if (cur_node->first_set == NULL) {
                findFirstSet(cur_node->name);
            }
            cur_follow_set[0] = cur_follow_set[0] | (cur_node->first_set[0]);
            cur_follow_set[1] = cur_follow_set[1] | (cur_node->first_set[1]);
            if ((cur_node->first_set[epsilon_pos/64] & epsilon) > 0) {
                cur_follow_set[epsilon_pos/64] ^= epsilon;
                if (cur_node->follow_set == NULL) {
                    findFollowSet (cur_node->name);
                }
                cur_follow_set[0] = cur_follow_set[0] | (cur_node->follow_set[0]);
                cur_follow_set[1] = cur_follow_set[1] | (cur_node->follow_set[1]);
            }
        }
    }
    cur_grammar_symbol->follow_set = cur_follow_set;
    return;
}

void computeFirstAndFollowSets () {
    // Populate first sets
    for (int i = 0; i < rule_count; i++) {
        grammar_symbol* cur_node = grammar[i]->data;
        findFirstSet (cur_node->name);
        // For printing first sets of every non terminal
        // printf("%s ->", cur_node->name);
        // for (int i=0;i<128;i++) {
        //     if ((cur_node->first_set[i/64] & 1ull<<(i%64)) > 0) {
        //         grammar_symbol* node = terminal_ht->entry[i].data;
        //         printf(" %s", node->name);
        //     }
        // }
        // printf("\n");
        // printf("%llu %llu\n", cur_node->first_set[0], cur_node->first_set[1]);
    }

    // Populate follow sets
    // adding $ to follow set of <program>
    int dollar_index = ht_fetch_index(terminal_ht, "$");
    grammar_symbol* cur_node = grammar[0]->data;
    cur_node->follow_set = calloc(2, sizeof(unsigned long long));
    cur_node->follow_set[dollar_index/64] = 1ull<<(dollar_index%64);
    for (int i = 0; i < rule_count; i++) {
        grammar_symbol* cur_node = grammar[i]->data;
        findFollowSet (cur_node->name);
        // For printing follow sets of every non terminal
        // printf("%s ->", cur_node->name);
        // for (int i=0;i<128;i++) {
        //     if ((cur_node->follow_set[i/64] & 1ull<<(i%64)) > 0) {
        //         grammar_symbol* node = terminal_ht->entry[i].data;
        //         printf(" %s", node->name);
        //     }
        // }
        // printf("\n");
        // printf("%llu %llu\n", cur_node->follow_set[0], cur_node->follow_set[1]);
    }
}

void createParseTable () {
    int row_count = nonterminal_ht->cap;
    int col_count = terminal_ht->cap;
    parse_table = calloc(row_count, sizeof(llHead**));
    int epsilon_pos = ht_fetch_index(terminal_ht, "@");
    grammar_symbol* syn_symbol = ht_fetch(terminal_ht, "#syn#");
    unsigned long long epsilon = (1ull<<(epsilon_pos%64));
    grammar[rule_count] = initializeHead();
    llNode* syn_node = initializeNode();
    syn_node->data = syn_symbol;
    insertAtEnd(grammar[rule_count], syn_node);
    for (int i = 0; i < row_count; i++) {
        parse_table[i] = calloc(col_count, sizeof(llHead*));
        for (int j = 0; j < col_count; j++) {
            parse_table[i][j] = NULL;
        }
    }
    for (int i = 0; i < rule_count; i++) {
        grammar_symbol* cur_node = grammar[i]->data;
        int cur_index = ht_fetch_index(nonterminal_ht, cur_node->name);
        int isEpsilon = 0;
        if ((grammar[i]->first_set[epsilon_pos/64] & epsilon) > 0) {
            isEpsilon = 1;
        }
        for (int j = 0; j < 128; j++) {
            if ((grammar[i]->first_set[j/64] & 1ull<<(j%64)) > 0) {
                if (parse_table[cur_index][j] != NULL) {

                }
                else {
                    parse_table[cur_index][j] = grammar[i];
                }
            }
            if ((cur_node->follow_set[j/64] & 1ull<<(j%64)) > 0) {
                if (isEpsilon == 1) {
                    if (parse_table[cur_index][j] != NULL) {

                    }
                    else {
                        parse_table[cur_index][j] = grammar[i];
                    }
                }
                else if (parse_table[cur_index][j] == NULL && i == cur_node->end) {
                    parse_table[cur_index][j] = grammar[rule_count];
                }
            }
        }
    }
}

void deallocateParseTable () {
    if (parse_table == NULL) return;
    int row_count = nonterminal_ht->cap;
    for (int i = 0; i < row_count; i++) {
        if (parse_table[i])
            free(parse_table[i]);
    }
    free(parse_table);
}

treeNode* parseInputSourceCode (ht* lookup_table, FILE* fp, bool printError) {
    llHead* stack = initializeHead();
    // Inserting $ onto the stack
    grammar_symbol* dollar_terminal = ht_fetch(terminal_ht, "$");
    llNode* dollar_node = initializeNode();
    dollar_node->data = dollar_terminal;
    stack = insertAtEnd(stack, dollar_node);
    // Inserting <program> onto the stack
    grammar_symbol* program_start = grammar[0]->data;
    llNode* program_node = initializeNode();
    struct token_info null_token;
    treeNode* root=createTreeNode(dollar_terminal, null_token);
    treeNode* cur_tree_node=createTreeNode(program_start, null_token);
    addChild(root, cur_tree_node);

    program_node->data = program_start;
    stack = insertAtEnd(stack, program_node);
    int isError = 0;
    errorPrint = printError;
    bool wasSemicol = false;
    while (stack->count > 0) {
        struct token_info cur_token = getNextToken(lookup_table, fp);
        // For printing current input token
        // printf("%s\n", tokenToString[cur_token.token]);
        while (stack->count > 0) {
            // For printing current stack state
            // llNode* current = stack->first_node;
            // for (int i = 0; i < stack->count; i++) {
            //     grammar_symbol* gg = current->data;
            //     printf("%s ", gg->name);
            //     current = current->next;
            // }
            // printf("\n");
            grammar_symbol* current_symbol = stack->last_node->data;
            char cur_token_name[20];
            if (cur_token.token == ERROR) {
                // LEXICAL ERROR REPORTED
                break;
            }
            else if (cur_token.token == ENDOFFILE) {
                strcpy(cur_token_name, "$");
            }
            else {
                strcpy(cur_token_name, tokenToString[cur_token.token]);
            }
            if (current_symbol->terminal == 1) {
                if (strcmp(cur_token_name, current_symbol->name) == 0) {
                    if (cur_tree_node) {
                        cur_tree_node->token = cur_token;
                    }
                    wasSemicol = false;
                    removeFromEnd(stack);
                    if (strcmp(cur_token_name, "$") != 0) {
                        cur_tree_node = nextLeaf(cur_tree_node);
                    }
                    break;
                }
                else {
                    if (cur_token.token == ENDOFFILE || wasSemicol == true) {
                        removeFromEnd(stack);
                        cur_tree_node = nextLeaf(cur_tree_node);
                        isError = 1;
                    }
                    else {
                        if (cur_token.token == SEMICOL || cur_token.token == END || cur_token.token == BC) {
                            wasSemicol = true;
                        }
                        if (printError) {
                            printf("\nError -> Line ");
                            printf("%d | Unexpected token %s\n", cur_token.lineNumber, tokenToString[cur_token.token]);
                        }
                        isError = 1;
                        break;
                    }
                }
            }
            else {
                int parse_table_row = ht_fetch_index(nonterminal_ht, current_symbol->name);
                int parse_table_col = ht_fetch_index(terminal_ht, cur_token_name);
                llHead* cur_rule = parse_table[parse_table_row][parse_table_col];
                grammar_symbol* cur_symbol;
                int isEOF = 0;
                if (cur_rule == NULL) {
                    if (cur_token.token != ENDOFFILE && wasSemicol == false) {
                        if (cur_token.token == SEMICOL || cur_token.token == END || cur_token.token == BC) {
                            wasSemicol = true;
                        }
                        if (printError) {
                            printf("\nError -> Line ");
                            printf("%d | Unexpected token %s\n", cur_token.lineNumber, tokenToString[cur_token.token]);
                        }
                        isError = 1;
                        break;
                    }
                    else if (cur_token.token == ENDOFFILE) {
                        isEOF = 1;
                    }
                }
                else {
                    cur_symbol = cur_rule->first_node->data;
                }
                if (cur_rule == NULL || isEOF == 1 || strcmp(cur_symbol->name, "#syn#") == 0) {
                    removeFromEnd(stack);
                    cur_tree_node = nextLeaf(cur_tree_node);
                    isError = 1;
                }
                else {
                    wasSemicol = false;
                    removeFromEnd(stack);
                    cur_tree_node->grammar_rule = cur_rule->grammar_rule;
                    llNode* cur_node = cur_rule->last_node;
                    grammar_symbol* cur_grammar_symbol;

                    treeNode* firsttreenode;
                    int isEpsilon = 0;
                    while (cur_node != NULL) {
                        cur_grammar_symbol = cur_node->data;

                        firsttreenode=createTreeNode(cur_grammar_symbol, null_token);
                        addChild(cur_tree_node,firsttreenode);

                        if (cur_grammar_symbol->epsilon == 1) {
                            isEpsilon = 1;
                            break;
                        }
                        llNode* push_node = initializeNode();
                        push_node->data = cur_grammar_symbol;
                        insertAtEnd(stack, push_node);
                        cur_node = cur_node->prev;
                    }

                    cur_tree_node=firsttreenode;
                    if (isEpsilon) {
                        cur_tree_node = nextLeaf(cur_tree_node);
                    }

                }
            }
        }
    }
    if (printError) {
        if (isError) {
            printf("Error(s) were found during parsing.\n");
        }
        else {
            printf("No error was found during parsing.\n");
        }
    }
    // Stack is empty coming out of the loop
    free(stack);
    return root;
}

void printNodeInfo (treeNode* treenode, FILE* tree_fp) {
    if (treenode->symbol->terminal == 1) {
        struct token_info cur_token = treenode->token;
        fprintf(tree_fp, "%s %d %s ", treenode->symbol->name, cur_token.lineNumber, tokenToString[cur_token.token]);
        if (cur_token.token==NUM) {
            fprintf(tree_fp, "%d ", cur_token.value);
        }
        else if (cur_token.token==RNUM) {
            fprintf(tree_fp, "%f ", cur_token.value);
        }
        else {
            fprintf(tree_fp, "--- ");
        }
        if (treenode->parent == NULL) {

        }
        else {
            grammar_symbol* parent_symbol = treenode->parent->symbol;
            if (strcmp(parent_symbol->name, "$") == 0) {
                fprintf(tree_fp, "ROOT ");
            }
            else {
                fprintf(tree_fp, "%s ", parent_symbol->name);
            }
        }
        fprintf(tree_fp, "YES ");
        grammar_symbol* cur_symbol = treenode->symbol;
        fprintf(tree_fp, "%s\n", cur_symbol->name);
        fprintf(tree_fp, "%d\n", treenode->grammar_rule);
    }
    else {
        struct token_info cur_token = treenode->token;
        fprintf(tree_fp, "--- --- --- --- ");
        if (treenode->parent == NULL) {

        }
        else {
            grammar_symbol* parent_symbol = treenode->parent->symbol;
            if (strcmp(parent_symbol->name, "$") == 0) {
                fprintf(tree_fp, "ROOT ");
            }
            else {
                fprintf(tree_fp, "%s ", parent_symbol->name);
            }
        }
        fprintf(tree_fp, "NO ");
        grammar_symbol* cur_symbol = treenode->symbol;
        fprintf(tree_fp, "%s\n", cur_symbol->name);
        fprintf(tree_fp, "%d\n", treenode->grammar_rule);
    }
}

void printTreeNode(treeNode* treenode, FILE* tree_fp)
{
    if(treenode==NULL) {
        return;
    }
    if(treenode->firstchild!=NULL) {
        printTreeNode(treenode->firstchild, tree_fp);
    }

    //Print Node details
    printNodeInfo (treenode, tree_fp);

    if(treenode->firstchild!=NULL)
    {
        treeNode* restTreeChild=treenode->firstchild->next;
        while(restTreeChild != NULL)
        {
            printTreeNode(restTreeChild, tree_fp);
            restTreeChild=restTreeChild->next;
        }
    }
}

void printParseTree(treeNode* root, char* outputfile)
{
    FILE* tree_fp = fopen(outputfile, "w+");
    if (tree_fp == NULL) {
        printf("Output file doesn't exist!\n");
        exit(0);
    }
    treeNode * curTreeNode=root;
    fprintf(tree_fp, "Lexeme CurrentNodeLineNo. TokenName ValueIfNumber ParentNodeSymbol IsLeafNode(YES/NO) NodeSymbol\n");
    if (root == NULL) {
        return;
    }
    printTreeNode(root->firstchild, tree_fp);
    fclose(tree_fp);
}

void deallocateFromParser () {
    delete_ht(nonterminal_ht);
    delete_ht(terminal_ht);
    deallocateParseTable();
    deallocateGrammar();
}

// AST

typedef struct AST_NODE Ast_Node;
struct AST_NODE {
    int type;
    Ast_Node* syn_next; // Useful if the node is a part of a linkedlist. If it is not, or it is the end, make it NULL. Used for syn attributes.
    Ast_Node* inh_1; // ^^^^ Above attribute might be useless. Used in Rule 40.
    Token_Info* token_data; // Useful for nodes of the form <non-terminal> --> terminal. Helps with compression.
    Ast_Node* child_1; // In case of linked list, child_1 contains data of the node.
    Ast_Node* child_2;
    Ast_Node* child_3;
    Ast_Node* child_4; // Parameters of the node. Consider renaming them to something better, or adding more parameters to help with readability.
};

Ast_Node* createASTNode() {
    Ast_Node* root = (Ast_Node*)malloc(sizeof(Ast_Node));
    root->type = 0;
    root->syn_next = NULL;
    root->inh_1 = NULL;
    root->token_data = NULL;
    root->child_1 = NULL;
    root->child_2 = NULL;
    root->child_3 = NULL;
    root->child_4 = NULL;
}
// Brains behind brains behind the code : Mithil
// Brains behind the code : Tanveer
// Part time helper boy : Mithil
const char* const ast_node_id[] = {
    "ERROR!", // 0
    "program", // 1
    "moduleDeclarations", // 2
    "moduleDeclaration", // 3
    "otherModules", // 4
    "driverModule", // 5
    "module", // 6
    "ret", // 7
    "input_plist", // 8
    "output_plist", // 9
    "dataType", // 10
    "range_arrays", // 11
    "type", // 12
    "moduleDef", // 13
    "statements",  // 14
    "statement",  // 15
    "io_statement_get",  // 16
    "io_statement_print",  // 17
    "N14",  // 18
    "var_print",  // 19
    "minus_var_print",  // 20
    "P1",  // 21
    "assignment_statement",  // 22
    "lvalue_statement",  // 23
    "lvalueARR_statement",  // 24
    "index_arr",  // 25
    "module_reuse_statement",  // 26
    "actual_para_list",  // 27
    "singular_para_list",  // 28
    "minus_singular_para_list",  // 29
    "N13",  // 30
    "N11",  // 31
    "optional",  // 32
    "id_list",  // 33
    "U",  // 34
    "arithmetic_or_boolean_expression",  // 35
    "any_term",  // 36
    "N8",  // 37
    "arithmetic_expression",  // 38
    "term", // 39
    "factor", // 40
    "element_index_with_expressions", // 41
    "minus_element_index_with_expressions", // 42
    "arr_expression", // 43
    "arr_term", // 44
    "declare_statement", // 45
    "conditional_statement", // 46
    "case_statements", // 47
    "FOR", // 48
    "WHILE", // 49
    "range_for_loop", // 50
    "index_for_loop", // 51
};

Token_Info* createHeapTokenInfo(Token_Info old_token) {
    Token_Info* new_token = (Token_Info*)malloc(sizeof(Token_Info));
    new_token->token = old_token.token;
    strcpy(new_token->lexeme, old_token.lexeme);
    new_token->lineNumber = old_token.lineNumber;
    new_token->value = old_token.value;
    return new_token;
}

void printASTNode(Ast_Node* root, FILE* f) {
    if(root == NULL) {
        fprintf(f, "NULL");
    } else {
        fprintf(f, "%d %s\n", root->type, ast_node_id[root->type]);
    }
}

Ast_Node* generateAST(treeNode* curr, Ast_Node* prev) {
    Ast_Node* root = createASTNode();

    bool return_null = false; // For productions like rule 3.
    
    // Handle case where curr == NULL here.

    int rule_no = curr->grammar_rule;

    rule_no++; // Remember one-based indexing!!!!!!!!

    treeNode* temp;

    printf("%d\n", rule_no);

    switch (rule_no)
    {
    case 0:
        // This is the case for tokens.
        root->token_data = createHeapTokenInfo(curr->token);
        break;
    case 1:
        assert(prev == NULL); // i.e this case is only possible at root of program.
        root->type = 1;
        // post-order traversal
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->child_3 = generateAST(curr->firstchild->next->next, NULL);
        root->child_4 = generateAST(curr->firstchild->next->next->next, NULL);
        break;
    case 2:
        root->type = 2;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->firstchild, NULL);
        break;
    case 3:
        return_null = true; // handle case accordingly in the end.
        break;
    case 4:
    // 4. <moduleDeclaration> -> DECLARE MODULE ID SEMICOL
    // Instead of making token, it is much better to just make an Ast_Node for ID.
    // But we will have to handle a case for it. 
        root->type = 3;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        // Token_Info ID = curr->firstchild->next->next->token;
        // root->token_data = createHeapTokenInfo(ID);
        break;
    case 5:
        root->type = 4;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, NULL);
        break;
    case 6:
    // 6. <otherModules> -> @
        return_null = true;
        break;
    case 7:
    //  <driverModule> -> DRIVERDEF DRIVER PROGRAM DRIVERENDDEF <moduleDef>
        root->type = 5;
        root->child_1 = generateAST(curr->firstchild->next->next->next->next, NULL);
        break;
    case 8:
    // <module> -> DEF MODULE ID ENDDEF TAKES INPUT SQBO <input_plist> SQBC SEMICOL <ret> <moduleDef>
        root->type = 6;
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // ID
        temp = temp->next->next->next->next->next;
        root->child_2 = generateAST(temp, NULL); // input_plist
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // ret
        temp = temp->next;
        root->child_4 = generateAST(temp, NULL); // moduleDef
        break;
    case 9:
    // 9. <ret> -> RETURNS SQBO <output_plist> SQBC SEMICOL
        root->type = 7;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 10:
    // 10. <ret> -> @
        return_null = true;
        break;
    case 11:
    // 11. <input_plist> -> ID COLON <dataType> <N1>
        root->type = 8;
        root->child_1 = generateAST(curr->firstchild, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next, NULL); // dataType
        root->syn_next = generateAST(curr->firstchild->next->next->next, NULL); // N1
        break;
    case 12:
    // 12. <N1_1> -> COMMA ID COLON <dataType> <N1_2>
        root->type = 8; // This is a controversial decision. My thinking is that, for an AST, 
        // N1 and input_plist are exactly the same. 
        root->child_1 = generateAST(curr->firstchild->next, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next->next, NULL); // dataType
        root->syn_next = generateAST(curr->firstchild->next->next->next->next, NULL); // N1 
        break;
    case 13:
    // 13. <N1> -> @
        return_null = true;
        break;
    case 14:
    // 14. <output_plist> -> ID COLON <type> <N2>
        root->type = 9;
        root->child_1 = generateAST(curr->firstchild, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next, NULL); // type
        root->syn_next = generateAST(curr->firstchild->next->next->next, NULL); // N2
        break;
    case 15:
    // 15. <N2_1> -> COMMA ID COLON <type> <N2_2>
        root->type = 9; // See comment on Rule 12.
        root->child_1 = generateAST(curr->firstchild->next, NULL); // ID
        root->child_2 = generateAST(curr->firstchild->next->next->next, NULL); // type
        root->syn_next = generateAST(curr->firstchild->next->next->next->next, NULL); // N2 
        break;
    case 16:
    // 16. <N2> -> @
        return_null = true;
        break;
    case 17:
    // 17. <dataType> -> ARRAY SQBO <range_arrays> SQBC OF <type>
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        root->child_2 = generateAST(curr->firstchild->next->next->next->next->next, NULL);
        break;
    case 18:
    // 18. <range_arrays> -> <index_arr_1> RANGEOP <index_arr_2>
        root->type = 11;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 19:
    // 19. <dataType> -> INTEGER
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 20:
    // 20. <dataType> -> REAL
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 21:
    // 21. <dataType> -> BOOLEAN
        root->type = 10;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 22:
    // 22. <type> -> INTEGER
        root->type = 12;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 23:
    // 23. <type> -> REAL
        root->type = 12;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 24:
    // 24. <type> -> BOOLEAN
        root->type = 12;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 25:
    // 25. <moduleDef> -> START <statements> END
        root->type = 13;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 26:
    // 26. <statements> -> <statement><statements>
        root->type = 14;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, NULL);
        break;
    case 27:
    // 27. <statements> -> @
        return_null = true;
        break;
    case 28:
    // 28. <statement> -> <ioStmt>
        root->type = 15;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 29:
    // 29. <statement> -> <simpleStmt>
        root->type = 15;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 30:
    // 30. <statement> -> <declareStmt>
        root->type = 15;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 31:
    // 31. <statement> -> <conditionalStmt>
        root->type = 15;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 32:
    // 32. <statement> -> <iterativeStmt>
        root->type = 15;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 33:
    // 33. <ioStmt> -> GET_VALUE BO ID BC SEMICOL
        root->type = 16;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 34:
    // 34. <ioStmt> -> PRINT BO <var_print> BC SEMICOL
        root->type = 17;
        root->child_1 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 35:
    // 35. <N14> -> NUM
        root->type = 18;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 36:
    // 36. <N14> -> RNUM
        root->type = 18;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 37:
    // 37. <var_print> -> PLUS <N14>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 38:
    // 38. <var_print> -> MINUS <N14>
        root->type = 20;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 39:
    // 39. <var_print> -> <N14>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 40:
    // 40. <var_print> -> ID <P1>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1); // Inherited attibute
        break;
    case 41:
    // 41. <var_print> -> <boolConstt>
        root->type = 19;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 42:
    // 42. <P1> -> SQBO <index_arr> SQBC
        root->type = 21;
        assert(prev != NULL); // Rule 40.
        root->inh_1 = prev->child_1;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 43:
    // 43. <P1> -> @
        return_null = true;
        break;
    case 44:
    // 44. <simpleStmt> -> <assignmentStmt>
        free(root); // Assure that this works correctly.
        root = generateAST(curr->firstchild, NULL);
        break;
    case 45:
    // 45. <simpleStmt> -> <moduleReuseStmt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 46:
    // 46. <assignmentStmt> -> ID <whichStmt>
        root->type = 22;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1); // Pass information about ID
        break;
    case 47:
    // 47. <whichStmt> -> <lvalueIDStmt>
        free(root);
        assert(prev != NULL);
        root = generateAST(curr->firstchild, prev);
        break;
    case 48:
    // 48. <whichStmt> -> <lvalueARRStmt>
        free(root);
        root = generateAST(curr->firstchild, prev); // ID keeps passing downward.
        break;
    case 49:
    // 49. <lvalueIDStmt> -> ASSIGNOP <expression> SEMICOL
        root->type = 23;
        root->child_1 = generateAST(curr->firstchild->next, prev); 
        root->inh_1 = prev;
        break;
    case 50:
    // 50. <lvalueARRStmt> -> SQBO <element_index_with_expressions> SQBC ASSIGNOP <expression> SEMICOL
        root->type = 24;
        temp = curr->firstchild->next; 
        root->child_1 = generateAST(temp, NULL);
        root->inh_1 = prev;
        temp = temp->next->next->next;
        root->child_2 = generateAST(temp, prev);
        break;
    case 51:
    // 51. <index_arr> -> <sign> <new_index>
        // Sadly, we'll have to create a node for sign here. I am not capable enough to 
        // interpret token_info and make this similar to Rule 37. Basically, just forget
        // that rule 37 and 51 COULD HAVE been similar, and keep solving them independently.
        root->type = 25;
        root->child_1 = generateAST(curr->firstchild, NULL); 
        root->child_2 = generateAST(curr->firstchild->next, NULL); 
        break;
    case 52:
    // 52. <new_index> -> NUM
        // Compress the tree! Just return NUM itself. No need for a seperate token for <new_index>.
        // Also optimize this in previous rules!!!!
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 53:
    // 53. <new_index> -> ID
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 54:
    // 54. <sign> -> PLUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 55:
    // 55. <sign> -> MINUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 56:
    // 56. <sign> -> @
        // FIX!!!! BEST THING TO DO IS RETURN TOKEN_INFO OF FIRST. 
        // Currently : if NULL is returned, it means PLUS.
        return_null = true;
        break;
    case 57:
    // 57. <moduleReuseStmt> -> <optional> USE MODULE ID WITH PARAMETERS <actual_para_list> SEMICOL
        root->type = 26;
        temp = curr->firstchild; 
        root->child_1 = generateAST(temp, NULL); // optional
        temp = temp->next->next->next;
        root->child_2 = generateAST(temp, NULL); // ID
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // actual_para_list
        break;
    case 58:
    // 58. <actual_para_list> -> <singular_para_list> <N12>
        root->type = 27;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, NULL);
        break;
    case 59:
    // 59. <N12> -> COMMA <singular_para_list> <N12>
        root->type = 27;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 60:
    // 60. <N12> -> @
        return_null = true;
        break;
    case 61:
    // 61. <singular_para_list> -> <boolConstt>
        root->type = 28;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 62:
    // 62. <singular_para_list> -> PLUS <N13>
        root->type = 28;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 63:
    // 63. <singular_para_list> -> MINUS <N13>
        root->type = 29;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 64:
    // 64. <singular_para_list> -> <N13>
        root->type = 28;
        root->child_1 = generateAST(curr->firstchild, NULL);
        break;
    case 65:
    // 65. <N13> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 66:
    // 66. <N13> -> RNUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 67:
    // 67. <N13> -> ID <N11>
        root->type = 30;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1);
        // if child_2 comes out to be NULL, it means rule 69 was applied.
        break;
    case 68:
    // 68. <N11> -> SQBO <element_index_with_expressions> SQBC
        // Bound checking happens HERE.
        root->type = 31;
        assert(prev != NULL);
        root->inh_1 = prev;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 69:
    // 69. <N11> -> @
        return_null = true;
        break;
    case 70:
    // 70. <optional> -> SQBO <idList> SQBC ASSIGNOP
        root->type = 32;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 71:
    // 71. <optional> -> @
        return_null = true;
        break;
    case 72:
    // 72. <idList> -> ID <N3>
        root->type = 33;
        root->child_1 = generateAST(curr->firstchild, NULL);
        // Also see Rule. 128!!!
        root->inh_1 = prev;
        root->syn_next = generateAST(curr->firstchild->next, prev);
        break;
    case 73:
    // 73. <N3> -> COMMA ID <N3>
        root->type = 33;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        root->inh_1 = prev;
        root->syn_next = generateAST(curr->firstchild->next->next, prev);
        break;
    case 74:
    // 74. <N3> -> @
        return_null = true;
        break;
    case 75:
    // 75. <expression> -> <arithmeticOrBooleanExp>
        free(root);
        assert(prev != NULL);
        root = generateAST(curr->firstchild, prev);
        break;
    case 76:
    // 76. <expression> -> <U>
        free(root);
        assert(prev != NULL);
        root = generateAST(curr->firstchild, prev);
        break;
    case 77:
    // 77. <U> -> <op1> <new_NT>
        root->type = 34;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, prev);
        break;
    case 78:
    // 78. <new_NT> -> BO <arithmeticExpr> BC
        free(root);
        root = generateAST(curr->firstchild, prev);
        break;
    case 79:
    // 79. <new_NT> -> <var_id_num>
        free(root);
        root = generateAST(curr->firstchild, prev);
        break;
    case 80:
    // 80. <arithmeticOrBooleanExp> -> <AnyTerm> <N7>
        root->type = 35;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 81:
    // 81. <N7> -> <logicalOp> <AnyTerm> <N7>
        root->type = 35;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL); // Type checking for this term and prev term can happen here???
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 82:
    // 82. <N7> -> @
        return_null = true;
        break;
    case 83:
    // 83. <AnyTerm> -> <arithmeticExpr> <N8>
        root->type = 36;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 84:
    // 84. <boolConstt> -> TRUE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 85:
    // 85. <boolConstt> -> FALSE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 86:
    // 86. <N8> -> <relationalOp> <arithmeticExpr>
        root->type = 37;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        // Type checking here.
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        break;
    case 87:
    // 87. <N8> -> @
        return_null = true;
        break;
    case 88:
    // 88. <arithmeticExpr> -> <term> <N4>
        root->type = 38;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 89:
    // 89. <N4> -> <op1> <term> <N4>
        /*
        How to distinguish between rules 88 and 89?
        Ans. Using inh_1. It will be NULL for rule 88, but not for 89.
        Also, number of children are different.
        */
        root->type = 38;
        root->child_1 = generateAST(curr->firstchild, NULL);
        // Type checking happens here??
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 90:
    // 90. <N4> -> @
        return_null = true;
        break;
    case 91:
    // 91. <term> -> <factor> <N5>
        root->type = 39;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 92:
    // 92. <N5> -> <op2> <factor> <N5>
        root->type = 39;
        root->child_1 = generateAST(curr->firstchild, NULL);
        // Type checking happens here??
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 93:
    // 93. <N5> -> @
        return_null = true;
        break;
    case 94:
    // 94. <factor> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 95:
    // 95. <factor> -> <boolConstt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 96:
    // 96. <factor> -> ID <N11>
        root->type = 40;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 97:
    // 97. <factor> -> RNUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 98:
    // 98. <factor> -> BO <arithmeticOrBooleanExp> BC
        free(root);
        root = generateAST(curr->firstchild->next, NULL);
        break;
    case 99:
    // 99. <array_element> -> ID SQBO <element_index_with_expressions> SQBC
        // Rule is useless. Never comes on right side. Ignore it.
        assert(false);
        return_null = true;
        break;
    case 100:
    // 100. <element_index_with_expressions> -> <arrExpr>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 101:
    // 101. <element_index_with_expressions> -> PLUS <new_index>
        root->type = 41;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 102:
    // 102. <element_index_with_expressions> -> MINUS <new_index>
        root->type = 42;
        root->child_1 = generateAST(curr->firstchild->next, NULL);
        break;
    case 103:
    // 103. <arrExpr> -> <arrTerm> <arr_N4> 
        // Same as rule 91
        root->type = 43;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 104:
    // 104. <arr_N4> -> <op1> <arrTerm> <arr_N4>
        // Same as rule 92
        root->type = 43;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 105:
    // 105. <arr_N4> -> @
        return_null = true;
        break;
    case 106:
    // 106. <arrTerm> -> <arrFactor> <arr_N5>
        // Same as rule 103
        root->type = 44;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->syn_next = generateAST(curr->firstchild->next, root->child_1);
        break;
    case 107:
    // 107. <arr_N5> -> <op2> <arrFactor> <arr_N5>
        // Same as rule 104
        root->type = 44;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->inh_1 = prev;
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        root->syn_next = generateAST(curr->firstchild->next->next, root->child_2);
        break;
    case 108:
    // 108. <arr_N5> -> @
        return_null = true;
        break;
    case 109:
    // 109. <arrFactor> -> ID
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 110:
    // 110. <arrFactor> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 111:
    // 111. <arrFactor> -> <boolConstt>
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 112:
    // 112. <arrFactor> -> BO <arrExpr> BC
        free(root);
        root = generateAST(curr->firstchild->next, NULL);
        break;
    case 113:
    // 113. <var_id_num> -> ID
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 114:
    // 114. <var_id_num> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 115:
    // 115. <var_id_num> -> RNUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 116:
    // 116. <op1> -> PLUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 117:
    // 117. <op1> -> MINUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 118:
    // 118. <op2> -> MUL
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 119:
    // 119. <op2> -> DIV
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 120:
    // 120. <logicalOp> -> AND
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 121:
    // 121. <logicalOp> -> OR
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 122:
    // 122. <relationalOp> -> LT
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 123:
    // 123. <relationalOp> -> LE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 124:
    // 124. <relationalOp> -> GT
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 125:
    // 125. <relationalOp> -> GE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 126:
    // 126. <relationalOp> -> EQ
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 127:
    // 127. <relationalOp> -> NE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 128:
    // 128. <declareStmt> -> DECLARE <idList> COLON <dataType> SEMICOL
        root->type = 45;
        // Take care!! Child order is reversed.
        root->child_2 = generateAST(curr->firstchild->next->next->next, NULL);
        root->child_2 = generateAST(curr->firstchild->next, root->child_2);
        break;
    case 129:
    // 129. <conditionalStmt> -> SWITCH BO ID BC START <caseStmts> <default> END
        root->type = 46;
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // ID
        temp = temp->next->next->next;
        root->child_2 = generateAST(temp, NULL); // caseStmts
        temp = temp->next;
        root->child_3 = generateAST(temp, NULL); // default
        break;
    case 130:
    // 130. <caseStmts> -> CASE <value> COLON <statements> BREAK SEMICOL <N9>
        root->type = 47;
        temp = curr->firstchild->next; 
        root->child_1 = generateAST(temp, NULL); // value
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // statements
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // N9
        break;
    case 131:
    // 131. <N9> -> CASE <value> COLON <statements> BREAK SEMICOL <N9>
        root->type = 47;
        temp = curr->firstchild->next; 
        root->child_1 = generateAST(temp, NULL); // value
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // statements
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // N9
        break;
    case 132:
    // 132. <N9> -> @
        return_null = true;
        break;
    case 133:
    // 133. <value> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 134:
    // 134. <value> -> TRUE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 135:
    // 135. <value> -> FALSE
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 136:
    // 136. <default> -> DEFAULT COLON <statements> BREAK SEMICOL
        free(root);
        root = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 137:
    // 137. <default> -> @
        return_null = true;
        break;
    case 138:
    // 138. <iterativeStmt> -> FOR BO ID IN <range_for_loop> BC START <statements> END
        root->type = 48;
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // ID
        temp = temp->next->next;
        root->child_2 = generateAST(temp, NULL); // range_for_loop
        temp = temp->next->next->next;
        root->child_3 = generateAST(temp, NULL); // statements
        break;
    case 139:
    // 139. <iterativeStmt> -> WHILE BO <arithmeticOrBooleanExp> BC START <statements> END
        root->type = 49;
        temp = curr->firstchild->next->next; 
        root->child_1 = generateAST(temp, NULL); // arithmeticOrBooleanExp
        temp = temp->next->next->next;
        root->child_2 = generateAST(temp, NULL); // statements
        break;
    case 140:
    // 140. <range_for_loop> -> <index_for_loop> RANGEOP <index_for_loop>
        root->type = 50;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next->next, NULL);
        break;
    case 141:
    // 141. <index_for_loop> -> <sign_for_loop> <new_index_for_loop>
        root->type = 51;
        root->child_1 = generateAST(curr->firstchild, NULL);
        root->child_2 = generateAST(curr->firstchild->next, NULL);
        break;
    case 142:
    // 142. <new_index_for_loop> -> NUM
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 143:
    // 143. <sign_for_loop> -> PLUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 144:
    // 144. <sign_for_loop> -> MINUS
        free(root);
        root = generateAST(curr->firstchild, NULL);
        break;
    case 145:
    // 145. <sign_for_loop> -> @
        return_null = true;
        break;
    default:
        assert(false);
    }

    // printASTNode(root);

    // Remember to FREE treenodes!!!
    free(curr);
    if(return_null) {
        free(root);
        return NULL;
    }
    return root;
}

// Driver

ht* initialize () {
    readGrammarFile();
    computeFirstAndFollowSets();
    createParseTable();
    ht* lookup_table = init_ht();
    if(lookup_table == NULL) exit_nomem();
    fill_lookup_table(lookup_table);
    // print_lookup_table(lookup_table);
    return lookup_table;
}

void deallocateStructures (ht* lookup_table) {
    delete_ht(lookup_table);
    deallocateBuff();
    deallocateFromParser();
}

int main(int argc, char *argv[]) {
    printf("FIRST and FOLLOW set automated.\n");
    printf("Both lexical and syntax analysis modules implemented.\n");
    printf("Modules work with all testcases.\n");
    printf("Parse tree constructed.\n");
    printf("As far as our team is aware, everything is running properly.\n");

    ht* lookup_table = initialize();
    FILE* fp=fopen(argv[1],"r+");
    if (fp == NULL) {
        printf("Test file doesn't exist!\n");
        exit(0);
    }
    while(1)
    {
        printf("0. For exit\n");
        printf("1. For removal of comments\n");
        printf("2. For printing the token list (on the console) generated by the lexer\n");
        printf("3. For parsing to verify the syntactic correctness of the input source code and printing the parse tree appropriately\n");
        printf("4. For printing (on the console) the total time taken by your stage 1 code of lexer and parser to verify the syntactic correctness\n");
        int choice=0;

        scanf("%d",&choice);
        if(choice==0) break;
        else if(choice==1){
            //Remove Comments
            removeComments(fp);
        }
        else if(choice==2){
            //Print Token List (Only Lexer operates in this stage.)
            printLexemes(fp, atoi(argv[3]), lookup_table);
        }
        else if(choice==3){
            //Error Printing and Parse Tree Generation
            start_lexer(fp, atoi(argv[3]),true);
            treeNode* parseTreeRoot = parseInputSourceCode(lookup_table, fp, true);
            printParseTree(parseTreeRoot,argv[2]);
            // FILE* lmao = fopen("lmao.out", "w+");
            // printNodeInfo(parseTreeRoot->firstchild, lmao);
            FILE* lmao = fopen("ast.out", "w+");
            Ast_Node* ast_root = generateAST(parseTreeRoot->firstchild, NULL);
        }
        else if(choice==4)
        {
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();

            // Invoke your lexer and parser here
            start_lexer(fp, atoi(argv[3]),false); //Lexer Invoked
            parseInputSourceCode(lookup_table, fp, false); //Parser Invoked   

            end_time = clock();
            total_CPU_time = (double) (end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time /CLOCKS_PER_SEC;
            printf("Total CPU Time : %2.6f\n",total_CPU_time);
            printf("Total CPU Time in seconds: %2.6f\n",total_CPU_time_in_seconds);
        }
        else {
            printf("Invalid Input!\n");
        }
    }
    deallocateStructures(lookup_table);
    fclose(fp);
}
