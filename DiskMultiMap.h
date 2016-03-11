#ifndef __HEADER_DISKMULTIMAP
#define __HEADER_DISKMULTIMAP

#include <list>
#include <string>
#include "BinaryFile.h"

typedef BinaryFile::Offset Location;
class DiskMultiMap {
public:
	struct MultiMapTuple {
		std::string key, value, context;
	};

	class Iterator {
	public:
		Iterator();
		Iterator(const std::list<MultiMapTuple>& nodeList);
		bool isValid() const;
		Iterator& operator++();
		MultiMapTuple operator*();
	private:
		std::list<MultiMapTuple> nodes;
	};

	DiskMultiMap();
	~DiskMultiMap();
	bool createNew(const std::string& filename, unsigned int numBuckets);
	bool openExisting(const std::string& filename);
	void close();
	bool insert(const std::string& key, const std::string& value, const std::string& context);
	Iterator search(const std::string& key);
	int erase(const std::string& key, const std::string& value, const std::string& context);
private:
	static const Location LIST_END = 2147483647;
	struct DiskMultiMapHeader {
		unsigned int numBuckets = 0;
		long numDeletedNodes = 0;
		Location hashTableStart = 0;
		Location nodeDataStart = 0;
		Location lastDeletedNode = LIST_END;
		Location firstDeletedNode = LIST_END;
	};

	struct DiskMultiMapBucket {
		long numNodes = 0;
		Location location = 0;
		Location lastNode = LIST_END;
		Location firstNode = LIST_END;
	};

	struct BucketNode {
		char key[121];
		char value[121];
		char context[121];
		Location location;
		Location nextNode;
	};

	bool createNode(const std::string& key, const std::string& value, const std::string& context, BucketNode& node) const;
	bool nodeEquals(const BucketNode& node, const std::string& key, const std::string& value, const std::string& context) const;
	MultiMapTuple convertToTuple(const BucketNode& node) const;

	unsigned long hash(const std::string& key) const;
	Location nextLocationToAddAt();
	void addDeletedNode(BucketNode node);

	DiskMultiMapBucket bucketAt(unsigned long index);
	BucketNode nodeAt(Location offset);
	bool updateBucket(DiskMultiMapBucket bucket);
	bool updateNode(BucketNode node);
	bool syncHeader();

	DiskMultiMapHeader header;
	BinaryFile bf;
};

#endif // __HEADER_DISKMULTIMAP
