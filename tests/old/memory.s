inc "internalStuff.s"

pub memcpy(*a, const *b, const max_t size) {
	for (max_t i = 0; i < size; i++) {
		*((byte*)a) = *((byte*)b);
	}
}

/*pub max_t sizeof(*a) {
	return os.query.size(a);
}*/