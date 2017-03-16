void old_school_main() {
	while (1) {
		if (get_button(BTN0)) servo(0);
		if (get_button(BTN1)) servo(-150);
		if (get_button(BTN2)) servo(150);
	}
}