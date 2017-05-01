#include <stdio.h> //printf
#include <stdlib.h> //malloc
#include <string.h>

//////////PARAM STACK STUFF/////////////

// EVERYTHING SHOULD BE ON THE STACK!
// WORD TAKES CHARACTERS UNTIL IT GETS TO WHATEVER IS ON THE STACK
// BL PUTS A SPACE CHARACTER (32) ON THE STACK

#define STACK_SIZE 20
#define INPUT_LEN 5

//stack pointer
long stack[STACK_SIZE];
long *stack_pointer = &stack[0]; //could call it SP, dont use int typedef a specified type size

//function to check the pointer is in the stack (not under or over flow)

void print_stack()
{
  printf("\n\n THE STACK: \n");
  long *current_slot = stack_pointer;
  while(current_slot!=&stack[0]){
    printf("%ld \n",*current_slot);
    current_slot--;
  }
}

void push(long val)
{
  stack_pointer = stack_pointer + 1;
  *stack_pointer = val;
  //*stack_pointer++ = val;???
}

long pop()
{
  long tos = *stack_pointer;
  stack_pointer = stack_pointer - 1;
  return tos;
}


////// return stack stuff ///////////
long return_stack[20];
long *RS_pointer = &return_stack[0]; //could call it SP, dont use long typedef a specified type size

void print_RS()
{
  printf("\n the stack: \n");
  long *current_slot = RS_pointer;
  while(current_slot!=&return_stack[0]){
    printf("%ldl\n",*current_slot);
    current_slot--;
  }
}

void pushRS(long* address) //IS THIS AN ADDRESS OF A DICTENTRY?
{
  RS_pointer = RS_pointer + 1;
  RS_pointer = address;
}

long popRS()
{
  long tos = *RS_pointer;
  RS_pointer = RS_pointer - 1;
  return tos;
}


////////dict stuff//////////////////
typedef void (*funcPtr)();

typedef struct DictEntry // DictEntry in struct namespace now
{
  char name[10];
  funcPtr code_pointer;
  //int* code_pointer;
  struct DictEntry* link_address;
  //immediate_flag;
  struct DictEntry* word_list[10];
  int word_list_ptr;
} DictEntry; //now DictEntry in the type namespace

DictEntry dict[50];

DictEntry* program_counter;

DictEntry* latest = NULL;

long dictLen = 0; ////////// USE HERE INSTEAD?!?!

DictEntry* here = NULL;

int adding_word = 0;

//////NATIVE DICT FUNCS ///////////////

void dup()
{
  long top = *stack_pointer;
  push(top);
}

void multiply()
{
  long tos = pop();
  long result = tos * tos;
  push(result);
}

void colon()
{
  adding_word = 1;
}

void colonend()
{
  printf("colonf called");
  adding_word = 0;
}


void enter()
{
  DictEntry* current_pc = program_counter;
  pushRS((long*) current_pc);
  program_counter = program_counter + 1;
}

/*void exit()
{

}*/


///////////  FUNCTIONS TO ADD TO DICT ///////////////////////

DictEntry* add_native_entry(char name[], funcPtr pointer)
{
  DictEntry* new_entry;
  if (latest == NULL) {
    new_entry = &dict[0];
  }
  else {
    new_entry = latest + 1;
  }

  strcpy(new_entry->name, name); //this should be in if to check there is enough space OR malloc
  //new_entry->name = *name;
  new_entry->code_pointer = pointer;
  new_entry->link_address = latest;
  latest = new_entry;

  dictLen = dictLen + 1;

  return latest;
}

/*void docolon()
{
  //DictEntry* word_list[] = {DUP, MUL};

}*/

void add_composite_entry(char name[]) ////HERE
{
  if (here == NULL) { // IT CAN ONLY ADD ONE COMPOSITE ENTRY!!!!! HERE SHOULD BE UPDATED SO IT'S NOT NULL!!!
    dictLen = dictLen + 1;
    DictEntry* new_entry;
    new_entry = latest + 1; //composite word cannot be first dict entry
    strcpy(new_entry->name, name);
    printf( "\n new entry name is %s \n ", new_entry->name);
    new_entry->word_list_ptr;
    here = new_entry;
  } else {
    int index = here->word_list_ptr;
    find(name);
    pop(); //assuming its a word in the dict and not a num
    DictEntry* address = (DictEntry*) pop();
    here->word_list[index] = address;
  }

  //return add_native_entry(name, &docolon);
}



/////////////// INTERPRETER //////////////////////

//LOOK FOR WORD IN DICT
void find(char input[])
{
  printf("\n  now in find ");
  int i;
  for(i=0; i<dictLen; i++) {
    DictEntry* this_entry_address = latest-i;
    DictEntry this_entry= *this_entry_address;
    printf("this_entry.name : %s", this_entry.name);
    if (strcmp (input, this_entry.name) == 0) { //use strchr?
      printf("\n ********** STRING FOUND IN DICT");
      push((long) this_entry_address);
      push(1);
      return;
    }
  }
  printf("\n ^^^^^^^^^^^^^^^^^ not found in dict");
  push(0);
  return;
}

//CONVERT CHAR TO NUMBER
void number(char input[])  // FIX LATERRR
{
  printf("\n  now in number ");
  long num_code = (long) input[0]; ///iterate through each for numbers longer than one digit?
  printf("\n  num_code is %ld \n", num_code);
  long num = num_code - 48;
  printf("\n  num is %ld \n", num);
  push(num);
}

//execute the word!!!
void execute(DictEntry* address)
{
  DictEntry word_we_want = *address;
  word_we_want.code_pointer();
}


//IF WORD IS IN DICT, EXECUTE, OTHERWISE SEND TO NUMBER
void interpret(char input[])
{
    printf("\n  now in interpret input is %s", input);

    //look for current in dict
    find(input);
    long found = pop();
    //if it's there execute
    if (found == 1) {
      printf("\n EXECUTE THE WORD\n");
      DictEntry* address = (DictEntry*) pop();
      execute(address);
    }
    else {
      //else cast to long and push to stack
      number(input);
    }

}

// QUIT LOOP - SEND EACH WORD IN INPUT TO INTERPRET, THEN END
void quit(char input[])
{
  printf("\n now in quit input is: %s ", input);
  //reset return stack

  //call interpret
  const char space[2] = " ";
  char* token;

  token = strtok(input, space);

  while( token != NULL )
  {
    printf( "\n the token is: %s \n ", token);
    if (adding_word == 1) {
      printf( "sending to composite word");
      add_composite_entry(token);
    } else {
      interpret(token);
    }

    token = strtok(NULL, space);
  }


}

/////////////////// START MAIN //////////////////////////////////
void main()
{
  //printf("\n size of dict: %d  size of dict entry at index 0: %d \n", sizeof(dict), sizeof(DictEntry));
  //int *tst;
  //printf("int: %d, long: %d, addr: %d", sizeof(int), sizeof(long), sizeof(tst));
  push(5);

  ///// ADD TO DICT
  DictEntry* DUP = add_native_entry("DUP", &dup);
  DictEntry* MUL = add_native_entry("MUL", &multiply);
  DictEntry* PRINT = add_native_entry("PRINT", &print_stack);
  DictEntry* COLON = add_native_entry(":", &colon);
  DictEntry* COLONEND = add_native_entry(";", &colonend);
  //DictEntry* SQUARED = add_composite_entry("SQUARED", )
  //print_stack();

  //char input[] = "6DM60";
  char input[100] = " 9 DUP MUL PRINT : SQUARED DUP MUL ; 5 SQUARED PRINT";  //": SQUARED DUP MUL ;"
  printf("\n input is %s adding_word is %d\n", input, adding_word);

  //print_stack();
  printf("\n address of input: %p", &input);
  quit(input);
  printf("\n adding_word is %d\n", adding_word);
  //print_stack();
  printf("\n at end of main");






}
