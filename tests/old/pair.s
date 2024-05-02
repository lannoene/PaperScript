pub group pair template<type A, type B> {
	Constructor(A, B);
	Destructor();
	define = (A, B);
	A& first : func {
		return a; 
	}
	B& second : func {
		return b;
	}
prv:
	A a;
	B b;
}

pair::Constructor(A newA, B newB) {
	a = newA;
	b = newB;
}

define=(A newA, B newB) {
	a = newA;
	b = newB;
}