inc "pair.s"

const max_t heapSize = 10000;
const max_t chunkSize = 512;
const max_t maxNodes = 100;
const max_t maxChunks = 10;

group chunk {
	
prv:
	byte[chunkSize] _heapSector;
}

group HeapPtr {
friend HeapManager;
	chunk* define[](max_t);
prv:
	AddNode(chunk *node, int idx);
	RemoveNode(int idx);
	pair<chunk, int>[maxChunks] allocedNodes(void, -1);
	int numNodes = 0;
}

chunk *HeapPtr::define[](max_t idx) {
	AddNode(heapMan.nodes[idx]);
}

HeapPtr::AddNode(chunk *node, int idx) {
	allocedNodes[numNodes++].Set(node, idx);
}

HeapPtr::RemoveNode(chunk *node) {
	allocedNodes[numNodes--].Set(nil, -1);
}

group HeapManager {
	*AllocateNew(max_t size);
	Reallocate(HeapPtr* ptr, max_t size);
prv:
	chunk[maxNodes] nodes;
	max_t numNodes = 0;
}

*HeapManager::AllocateNew(max_t size) {
	
}

HeapManager heapMan;

byte[heapSize] heap;
max_t allocatedSize = 0;

pub *malloc(max_t size) {
	return heapMan.AllocateNew(size);
}

pub realloc(*p, max_t newSize) {
	heapMan.Reallocate(p, newSize);
}

pub free(*p) {
	heapMan.Free(p);
}