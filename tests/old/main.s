inc "print.s"
inc "string.s"

int main {
	string myString("hello!");
	strm::print(myString);
	int i = 2;
	if (i == 2) {
		strm::print("i == 2!");
	} else {
		strm::print("i != 2");
	}
	while (true) {
		string myVar;
		strm::input("Prompt: ", myVar);
		strm::print("You entered %s", myVar);
	}
}