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
	i(i(2, 5), 3);

	e(e(e(3, 3), 3), 3);


	e(idf, idf++);

	int e[10];

	e(e[0], e[1]);
	return 0;
}