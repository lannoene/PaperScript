inc "string.s"
inc "stdfile.s"
inc "primitiveStr.s"

// bundles are namespaces
pub bundle strm {
	pub print(string str);
}

// pub to export function
pub strm::print(string str) {
	write(stdout, str._str, str.length);
	fflush(stdout);
}

pub strm::print(const char[] str) {
	write(stdout, str, strlen(str));
	fflush(stdout);
}