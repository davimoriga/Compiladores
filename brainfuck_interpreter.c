#include <stdio.h>

#define OP_END            0
#define OP_INC_POS        1
#define OP_DEC_POS        2
#define OP_INC_VAL        3
#define OP_DEC_VAL        4
#define OP_OUTPUT         5
#define OP_INPUT          6
#define OP_ADV            7
#define OP_BCK			  8

#define SUCESS            0
#define FAIL              1

#define PROGAM_SIZE       4096
#define STACK_SIZE        512
#define DATA_SIZE         65535

#define PUSH(A)   (PILHA[SP++] = A)
#define POP()     (PILHA[--SP])
#define EMPTY()   (SP == 0)
#define FULL()    (SP == STACK_SIZE)

struct instruction {
	unsigned short operator;
	unsigned short operand;
};

static struct instruction PROGRAM[PROGAM_SIZE];
static unsigned short PILHA[STACK_SIZE];
static unsigned int SP = 0;

int compile_brainfuck(FILE* fp) {
	unsigned short pc = 0, jmp_pc;
	int c;
	while ((c = getc(fp)) != EOF && pc < PROGAM_SIZE) {
		switch (c) {
		case '>': PROGRAM[pc].operator = OP_INC_POS; break;
		case '<': PROGRAM[pc].operator = OP_DEC_POS; break;
		case '+': PROGRAM[pc].operator = OP_INC_VAL; break;
		case '-': PROGRAM[pc].operator = OP_DEC_VAL; break;
		case '.': PROGRAM[pc].operator = OP_OUTPUT; break;
		case ',': PROGRAM[pc].operator = OP_INPUT; break;
		case '[':
			PROGRAM[pc].operator = OP_ADV;
			if (FULL()) {
				return FAIL;
			}
			PUSH(pc);
			break;
		case ']':
			if (EMPTY()) {
				return FAIL;
			}
			jmp_pc = POP();
			PROGRAM[pc].operator = OP_BCK;
			PROGRAM[pc].operand = jmp_pc;
			PROGRAM[jmp_pc].operand = pc;
			break;
		default: pc--; break;
		}
		pc++;
	}
	if (!EMPTY() || pc == PROGAM_SIZE) {
		return FAIL;
	}
	PROGRAM[pc].operator = OP_END;

	return SUCESS;
}

int execute_brainfuck() {
	unsigned short data[DATA_SIZE], pc = 0;
	unsigned int ptr = DATA_SIZE;

	while (--ptr) { data[ptr] = 0; }

	while (PROGRAM[pc].operator != OP_END && ptr < DATA_SIZE) {
		switch (PROGRAM[pc].operator) {
		case OP_INC_POS:
			ptr++;
			break;
		case OP_DEC_POS:
			ptr--;
			break;
		case OP_INC_VAL:
			data[ptr]++;
			break;
		case OP_DEC_VAL:
			data[ptr]--;
			break;
		case OP_OUTPUT:
			putchar(data[ptr]);
			break;
		case OP_INPUT:
			data[ptr] = (unsigned int)getchar();
			break;
		case OP_ADV:
			if (!data[ptr]) {
				pc = PROGRAM[pc].operand;
			}
			break;
		case OP_BCK:
			if (data[ptr]) {
				pc = PROGRAM[pc].operand;
			}
			break;
		default:
			return FAIL;
		}
		pc++;
	}

	return ptr != DATA_SIZE ? SUCESS : FAIL;
}

#pragma warning(disable : 4996)
int main(int argc, const char* argv[])
{
	int status;
	FILE* fp;

	if (argc != 2 || (fp = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "Uso: %s arquivo\n", argv[0]);
		return FAIL;
	}

	status = compile_brainfuck(fp);
	fclose(fp);

	if (status == SUCESS) {
		status = execute_brainfuck();
	}
	if (status == FAIL) {
		fprintf(stderr, "FALHOU!\n");
	}

	return status;
}