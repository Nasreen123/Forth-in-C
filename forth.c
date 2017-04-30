#include <stdio.h> //printf
#include <stdlib.h> //malloc
#include <string.h>

//////////PARAM STACK STUFF/////////////

#define STACK_SIZE 20
#define INPUT_LEN 5

//stack pointer
long stack[STACK_SIZE];
long *stack_pointer = &stack[0]; //could call it SP, dont use int typedef a specified type size

//function to check the pointer is in the stack (not under or over flow)

void print_stack()
{
  printf("\n the stack: \n");
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
  char name;
  funcPtr code_pointer;
  //int* code_pointer;
  struct DictEntry* link_address;
  //immediate_flag;
  struct DictEntry* word_list[10];
} DictEntry; //now DictEntry in the type namespace

DictEntry dict[50];

DictEntry* program_counter;

DictEntry* latest = NULL;

long dictLen = 0;

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

/*
void enter()
{

}

void exit()
{

}

void print()
{

}
*/

///////////  FUNCTIONS TO ADD TO DICT ///////////////////////

DictEntry* add_native_entry(char name, funcPtr pointer)
{
  DictEntry* new_entry;
  if (latest == NULL) {
    new_entry = &dict[0];
  }
  else {
    new_entry = latest + 1;
  }
  new_entry->name = name; //this only works in func - why?
  new_entry->code_pointer = pointer;
  new_entry->link_address = latest;
  latest = new_entry;

  dictLen = dictLen + 1;

  return latest;
}

void docolon()
{
  //DictEntry* word_list[] = {DUP, MUL};

}

DictEntry* add_composite_entry(char name)
{
  dictLen = dictLen + 1;
  return add_native_entry(name, &docolon);
}


/////////////// INTERPRETER //////////////////////

//LOOK FOR WORD IN DICT
void find(char input)
{
  printf("\n  now in find ");
  int i;
  for(i=0; i<dictLen; i++) {
    DictEntry* this_entry_address = latest-i;
    DictEntry this_entry= *this_entry_address;
    printf("this_entry.name : %c", this_entry.name);
    if (input == this_entry.name) { //use strchr?
      printf("\n ********** STRING FOUND IN DICT");
      push((long) this_entry_address);
      push(1);
      return;
    }
  }
  printf("\n ^^^^^^^^^^^^^^^^^ not found in dict");
  push(0);
  push(0);
  return;
}

//CONVERT CHAR TO NUMBER
void number(char input)  // FIX LATERRR
{
  printf("\n  now in number ");
  long num_code = (long) input; //////////// WHAT IS HAPPENING HERE????
  printf("\n  num_code is %ld \n", num_code);
  long num = num_code - 48;
  printf("\n  num is %ld \n", num);
  push(num);
}

//IF WORD IS IN DICT, EXECUTE, OTHERWISE SEND TO NUMBER
void interpret(char input)
{
    printf("\n  now in interpret input is %c", input);

    //look for current in dict
    find(input);
    long tos = pop();
    //if it's there execute
    if (tos == 1) {
      printf("\n EXECUTE THE WORD\n");
      DictEntry* address = (DictEntry*) pop();
      DictEntry word_we_want = *address;
      word_we_want.code_pointer();
    }
    else {
      pop();
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
  int i;
  for (i=0; i<INPUT_LEN; i++) { //change this to use len of input
    char input_char = input[i]; //*(input+i);
    printf("\n input_char is %c  \n", input_char);
    interpret(input_char);
  }
  //quit
}

/////////////////// START MAIN //////////////////////////////////
void main()
{
  //printf("\n size of dict: %d  size of dict entry at index 0: %d \n", sizeof(dict), sizeof(DictEntry));
  //int *tst;
  //printf("int: %d, long: %d, addr: %d", sizeof(int), sizeof(long), sizeof(tst));
  push(5);

  ///// ADD TO DICT
  DictEntry* DUP = add_native_entry('D', &dup);
  DictEntry* MUL = add_native_entry('M', &multiply);

  print_stack();

  char input[] = "6DM60";
  printf("\n input is %s \n", input);

  print_stack();
  printf("\n address of input: %p", &input);
  quit(input);

  print_stack();
  printf("\n at end of main");
}
