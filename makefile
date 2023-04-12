#Group No. 15
#Name: Shivam Abhay Pande	ID:2020A7PS0124P
#Name: Utkarsh Darolia		ID:2020A7PS0981P
#Name: Kshitij Garg			ID:2020A7PS0120P
#Name: Mithil Shah			ID:2020A7PS0980P
#Name: Tanveer Singh		ID:2020A7PS0084P

CC=gcc				#compiler
TARGET=a	#target file name

all: 
	$(CC) driver.c ht.c lexer.c linkedList.c grammarSymbol.c tree.c parser.c ast.c symbolTable.c typeCheck.c semanticError.c codegen.c -o $(TARGET)

clean:
	rm -rf $(TARGET)

#to clear previously formed executables run 'make clean'
#to execute the code run the following command:
#./stage1exe <testcaseFile>.txt parsetreeOutFile.txt <buffer_size>
