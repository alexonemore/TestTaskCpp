// Source code for Test task
// Licensed after GNU GPL v3

#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <string>
#include <unordered_map>
#include <vector>
#include "huffmantree.h"

using namespace HuffmanTree;

class CompressedData
{
private:
	std::vector<bool> compressed_data;
	HTDistionary dictionary;
	HTNptr tree;

public:
	CompressedData() = default;

	template<typename It>
	void Compress(It first, It last)
	{
		tree = MakeHuffmanTree(first, last);
		dictionary = MakeHuffmanDictionary(tree);
		compressed_data = HuffmanCompress(first, last, dictionary);
	}

	template<typename It>
	void CompressParallel(It first, It last)
	{
		tree = MakeHuffmanTree(first, last);
		dictionary = MakeHuffmanDictionary(tree);
		compressed_data = HuffmanCompressParallel(first, last, dictionary);
	}

	std::vector<int> Decompress() const;
	size_t SizeOfData() const;
	void WriteToFile(const std::string& filename) const;
	void ReadFromFile(const std::string& filename);

};

#endif // COMPRESSOR_H
