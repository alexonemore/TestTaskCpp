// Source code for Test task
// Licensed after GNU GPL v3

#ifndef HUFFMANTREE_H
#define HUFFMANTREE_H

#include <unordered_map>
#include <memory>
#include <vector>
#include <queue>
#include <cassert>
#include <thread>
#include <future>

namespace HuffmanTree
{

struct HuffmanTreeNode
{
	int value{0};
	int frequency{0};
	std::vector<bool> code{};

	using NodePointer = std::shared_ptr<HuffmanTreeNode>;
	NodePointer left{nullptr}, right{nullptr};

	HuffmanTreeNode() = default;
	HuffmanTreeNode(int value, int frequency)
		: value{value}, frequency{frequency}
	{}
	bool IsLeaf() const
	{
		return left == nullptr && right == nullptr;
	}
};

using HTNptr = HuffmanTreeNode::NodePointer;
using HTDistionary = std::unordered_map<int, std::vector<bool>>;

bool operator <(const HTNptr& lhs, const HTNptr& rhs);
bool operator >(const HTNptr& lhs, const HTNptr& rhs);

void TraverseTree(HTNptr node, std::vector<bool> current_code, HTDistionary& codes);

HTDistionary MakeHuffmanDictionary(HTNptr root);

template<typename It>
std::unordered_map<int, int> MakeHuffmanFrequency(It first, It last)
{
	std::unordered_map<int, int> frequency_dictionary;
	for (It it = first; it != last; ++it)
		frequency_dictionary[*it]++;
	return frequency_dictionary;
}

template<typename It>
HTNptr MakeHuffmanTree(It first, It last)
{
	auto frequency = MakeHuffmanFrequency(first, last);
	std::priority_queue<HTNptr, std::vector<HTNptr>, std::greater<HTNptr>> queue;
	for (auto [value, frequency] : frequency)
	{
		if (frequency != 0)
			queue.push(std::make_shared<HuffmanTreeNode>(value, frequency));
	}
	while (queue.size() > 1)
	{
		auto l = queue.top();
		queue.pop();
		auto r = queue.top();
		queue.pop();
		auto p = std::make_shared<HuffmanTreeNode>(0, l->frequency + r->frequency);
		p->left = l;
		p->right = r;
		queue.push(p);
	}
	auto root = queue.top();
	queue.pop();
	assert(queue.empty());
	return root;
}

template<typename It>
std::vector<bool> HuffmanCompress(It first, It last, const HTDistionary& dictionary)
{
	std::vector<bool> result;
	result.reserve(std::distance(first, last));
	for (It it = first; it!= last; ++it)
	{
		const auto& code = dictionary.at(*it);
		result.insert(result.end(), code.cbegin(), code.cend());
	}
	result.shrink_to_fit();
	return result;
}

constexpr size_t MIN_LENGTH = 100;
size_t DetermineThreads(size_t length);

template<typename It>
std::vector<bool> HuffmanCompressParallel(It first, It last, const HTDistionary& dictionary)
{
	size_t length = std::distance(first, last);
	if (length < MIN_LENGTH)
		return HuffmanCompress(first, last, dictionary);

	size_t nthreads = DetermineThreads(length);
	size_t bsize = length / nthreads;

	std::vector<std::future<std::vector<bool>>> results(nthreads);

	auto compress_bloc = [&](auto first, auto last)
	{
		return HuffmanCompress(first, last, dictionary);
	};

	size_t tidx = 0;
	for (; length >= bsize * (tidx + 1); first += bsize, tidx += 1)
	{
		std::packaged_task<std::vector<bool>(It, It)> task{compress_bloc};
		results[tidx] = task.get_future();
		std::thread t{std::move(task), first, first + bsize};
		t.detach();
	}

	std::vector<bool> result;
	for (size_t i = 0; i < nthreads; ++i)
	{
		auto res = results[i].get();
		result.insert(result.end(), res.cbegin(), res.cend());
	}

	auto remainder = length - bsize * tidx;
	if (remainder > 0)
	{
		auto res = compress_bloc(first, first + remainder);
		result.insert(result.end(), res.cbegin(), res.cend());
	}
	return result;
}

template<typename It>
std::vector<int> HuffmanDecompress(It first, It last, HTNptr root)
{
	std::vector<int> result;
	auto node = root;
	for (It it = first; it != last; ++it)
	{
		if (*it)
		{
			if (node->right != nullptr)
			{
				node = node->right;
				if (node->IsLeaf())
				{
					result.push_back(node->value);
					node = root;
				}
			}
		}
		else
		{
			if (node->left != nullptr)
			{
				node = node->left;
				if (node->IsLeaf())
				{
					result.push_back(node->value);
					node = root;
				}
			}
		}
	}
	return result;
}

}

#endif // HUFFMANTREE_H
