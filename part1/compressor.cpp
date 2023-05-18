// Source code for Test task
// Licensed after GNU GPL v3

#include "compressor.h"
#include <bitset>
#include <numeric>
#include <iostream>
#include <fstream>

std::vector<int> CompressedData::Decompress() const
{
	return HuffmanDecompress(compressed_data.cbegin(), compressed_data.cend(), tree);
}

size_t CompressedData::SizeOfData() const
{
	// TODO calculate size of Huffman Tree
	size_t size = compressed_data.size() / CHAR_BIT;
	for (const auto& [key, value] : dictionary)
		size += sizeof(key) + value.size() / CHAR_BIT;
	return size;
}

void CompressedData::WriteToFile(const std::string& filename) const
{
	// TODO write dictionary
	std::ofstream output_file(filename, std::ios::binary);
	if (output_file.is_open())
	{
		constexpr size_t size = 32;
		std::bitset<size> bits;
		static_assert(bits.size() <= std::numeric_limits<unsigned long>::digits, "too many bits");
		int index = 0;
		for (auto i : compressed_data)
		{
			bits.set(index++, i);
			if (index == size)
			{
				unsigned long n = bits.to_ulong();
				output_file.write(reinterpret_cast<const char*>(&n), sizeof(n));
				index = 0;
			}
		}
		output_file.close();
	}
	else
	{
		std::cerr << "Failed to open the file for writing." << std::endl;
	}
}

void CompressedData::ReadFromFile(const std::string& filename)
{
	// TODO read dictionary
	std::ifstream  input_file(filename, std::ios::binary);
	if (input_file.is_open())
	{
		compressed_data.clear();
		char ch;
		while (input_file.get(ch))
		{
			std::bitset<CHAR_BIT> bits(ch);
			for (int i = 0; i < CHAR_BIT; ++i)
			{
				compressed_data.push_back(bits[i]);
			}
		}
		input_file.close();
	}
	else
	{
		std::cerr << "Failed to open the file for writing." << std::endl;
	}
}
