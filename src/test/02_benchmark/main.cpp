#include <small_vector.h>

#include <iostream>
#include <chrono>
#include <set>
#include <unordered_map>

int main() {
	std::vector<double> rates;
	std::size_t cnt, index;
	std::size_t rst = 0;
	std::cin >> cnt >> index;
	assert(index < cnt);
	
	{
		for (std::size_t i = 0; i < 100; i++) {
			auto t0 = std::chrono::high_resolution_clock::now();
			for (std::size_t j = 0; j < 10000; j++) {
				Ubpa::small_vector<std::size_t, 16> vectors;
				for (std::size_t k = 0; k < cnt; k++)
					vectors.push_back(k + rst);
				rst += vectors[index];
			}
			auto t1 = std::chrono::high_resolution_clock::now();
			for (std::size_t j = 0; j < 10000; j++) {
				std::vector<std::size_t> vectors;
				for (std::size_t k = 0; k < cnt; k++)
					vectors.push_back(k + rst);
				rst += vectors[index];
			}
			auto t2 = std::chrono::high_resolution_clock::now();
			double rate = (t1 - t0).count() / (double)(t2 - t1).count();
			rates.push_back(rate);
		}

		double sum = 0;
		for (double r : rates)
			sum += r;
		sum /= rates.size();

		std::cout << sum << std::endl;
	}
	{
		for (std::size_t i = 0; i < 100; i++) {
			auto t0 = std::chrono::high_resolution_clock::now();
			{
				Ubpa::small_vector<std::size_t, 16> vectors;
				for (std::size_t k = 0; k < cnt; k++)
					vectors.push_back(k + rst);
				for (std::size_t j = 0; j < 10000; j++)
					rst += *std::find(vectors.begin(), vectors.end(), rst + (std::rand() % cnt));
			}
			auto t1 = std::chrono::high_resolution_clock::now();
			{
				std::set<std::size_t> vectors;
				for (std::size_t k = 0; k < cnt; k++)
					vectors.insert(k + rst);
				for (std::size_t j = 0; j < 10000; j++)
					rst += *vectors.find(rst + (std::rand() % cnt));
			}
			auto t2 = std::chrono::high_resolution_clock::now();
			double rate = (t1 - t0).count() / (double)(t2 - t1).count();
			rates.push_back(rate);
		}

		double sum = 0;
		for (double r : rates)
			sum += r;
		sum /= rates.size();

		std::cout << sum << std::endl;
	}
	{
		for (std::size_t i = 0; i < 100; i++) {
			auto t0 = std::chrono::high_resolution_clock::now();
			{
				Ubpa::small_vector<std::size_t, 16> vectors;
				for (std::size_t k = 0; k < cnt; k++)
					vectors.push_back(k + rst);
				for (std::size_t j = 0; j < 10000; j++) {
					std::size_t t = rst + (std::rand() % cnt);
					std::size_t left = 0, right = cnt;
					std::size_t mid;
					while (true) {
						if (left == right)
							break;
						mid = (left + right) / 2;
						if (vectors[mid] < t)
							left = mid + 1;
						else if(vectors[mid] > t)
							right = mid;
						else {
							rst += mid;
							break;
						}
					}
				}
			}
			auto t1 = std::chrono::high_resolution_clock::now();
			{
				std::set<std::size_t> vectors;
				for (std::size_t k = 0; k < cnt; k++)
					vectors.insert(k + rst);
				for (std::size_t j = 0; j < 10000; j++)
					rst += *vectors.find(rst + (std::rand() % cnt));
			}
			auto t2 = std::chrono::high_resolution_clock::now();
			double rate = (t1 - t0).count() / (double)(t2 - t1).count();
			rates.push_back(rate);
		}

		double sum = 0;
		for (double r : rates)
			sum += r;
		sum /= rates.size();

		std::cout << sum << std::endl;
	}
}
