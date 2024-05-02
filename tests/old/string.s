inc "heap.s"
inc "primitiveStr.s"

bundle strm;

// groups are objects. by default members are public
pub group string {
friend strm;
	Constructor(char[] s);
	Destructor();
	max_t length : func {
		ret strlen(_str);
	}
	define = (char[] s) {
		strcpy(_str, s);
	}
prv:
	char[] _str = null;
}

string::Constructor(char[] s) {
	_str = malloc(strlen(s) + 1);
	strcpy(_str, s);
}