// Source code for Test task
// Licensed after GNU GPL v3

#include "huffmantree.h"
#include <iostream>

namespace HuffmanTree
{

bool operator <(const HTNptr& lhs, const HTNptr& rhs)
{
	if (lhs->frequency == rhs->frequency)
		return lhs->value < rhs->value;
	return lhs->frequency < rhs->frequency;
}

bool operator >(const HTNptr& lhs, const HTNptr& rhs)
{
	return !(lhs < rhs);
}

void TraverseTree(HTNptr node, std::vector<bool> current_code, HTDistionary& codes)
{
	if (node->left != nullptr)
	{
		auto newcode = current_code;
		newcode.push_back(false);
		TraverseTree(node->left, newcode, codes);
	}
	if (node->right != nullptr)
	{
		auto newcode = current_code;
		newcode.push_back(true);
		TraverseTree(node->right, newcode, codes);
	}
	if (node->left == nullptr && node->right == nullptr)
	{
		node->code = current_code;
		codes[node->value] = current_code;
#ifdef VERBOSE_DEBUG
		std::string code_string;
		for(auto i : current_code)
			code_string += i ? "1" : "0";
		std::cout << node->value << " " << node->frequency << " " << code_string << std::endl;
#endif
	}
}

HTDistionary MakeHuffmanDictionary(HTNptr root)
{
	HTDistionary codes;
	TraverseTree(root, std::vector<bool>{}, codes);
	return codes;
}

size_t DetermineThreads(size_t length)
{
	const size_t min_per_thread = MIN_LENGTH;
	size_t max_threads = length / min_per_thread;
	size_t hardware_conc = std::thread::hardware_concurrency();
	return std::min(hardware_conc != 0 ? hardware_conc : 1, max_threads);
}

}
