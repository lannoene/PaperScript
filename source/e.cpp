int e(int a, int b) {
	return a*b;
}

int idf = 0;

int i(int a, int b) {
	int e = a + b;
	int fa = a - b;
	fa++;
	return e;
}

void a() {
	int j = 1;
	1 + j;
}

int main() {
	int asdf = 3, dse = 6;
	if (asdf == dse && asdf > 0 && dse < 10) {
		e(1, 4);
	}
	
	i(i(2, 5), 3);

	e(e(e(3, 3), 3), 3);


	e(idf, idf++);

	return 0;
}