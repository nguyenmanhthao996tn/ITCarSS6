#define max_size 512

uint8_t stack[max_size];
uint16_t stack_size = 0;

void push_stack(uint8_t val) {
	if (stack_size <= max_size - 1) {
		stack[stack_size] = val;
		stack_size += 1;
	} else {
		for(uint16_t i = 0; i < stack_size-1; i++) {
			stack[i] = stack[i+1];
		}
	}
	stack[stack_size-1] = val;
}

uint8_t get_stack(uint16_t idx) {
	if (idx < stack_size) return stack[idx];
	else return 0;
}