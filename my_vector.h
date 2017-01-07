#ifndef MY_VECTOR_H
#define MY_VECTOR_H

#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include <algorithm>

template <typename T>
class my_vector {

private:
	size_t sizes = 0;
	T small = 0;
	std::shared_ptr<std::vector<T>> ptr;
	
	void copy_optimization() {
		if (sizes > 1 && !ptr.unique()) {
			auto temp = *ptr;
			ptr = std::make_shared<std::vector<T>>(temp);
		}
	}
public:
	my_vector() :sizes(0) {}

	my_vector(size_t sizes) {
		this->sizes = sizes;
		if (sizes > 1) {
			ptr = std::make_shared<std::vector<T>>(sizes);
		}
		this->sizes = sizes;
	}

	my_vector(const my_vector& rhs) {
		this->sizes = rhs.sizes;
		if (rhs.sizes <= 1) {
			this->small  = rhs.small;
		} else {
			ptr = rhs.ptr;
		}
	}

	my_vector<T>& operator =(const my_vector& rhs) {
		this->sizes = rhs.sizes;
		if (rhs.sizes <= 1) {
			this->small  = rhs.small;
		} else {
			ptr = rhs.ptr;
		}
		return *this;	
	}

	T& operator[](size_t num) {
		copy_optimization();
		// if (sizes <= num)
		 // std::cout << this->size() <<  ' ' <<sizes << ' ' << num << '\n';
		 assert(sizes > num);
		if (sizes > 1) {
			return (*ptr)[num];
		} else {
			return small;
		} 
	}

	T operator[](size_t num) const {
		// if (sizes <= num) std::cout << sizes << ' ' << num << '\n';
		assert(sizes > num);
		if (sizes > 1) {
			return (*ptr)[num];
		} else {
			return small;
		} 
	}
	size_t size() const{
		return sizes;
	}

	void resize(size_t sizes) {
		copy_optimization();
		if (this->sizes != sizes) {
			if (sizes > 1 && this->sizes > 1) {
				ptr->resize(sizes);
			} else {
				if (this->sizes <= 1 && sizes > 1) {
					ptr = std::make_shared<std::vector<T>>(sizes);
				} else {
					if(this->sizes > 1 && sizes <= 1) {
						T small = (*ptr)[0];
					}
				}
			}
		}
		this->sizes = sizes;
	}

	void push_back(T val) {
		copy_optimization();
		if (sizes == 0) {
			small = val;
		} else {
			if (sizes == 1) {
				ptr = std::make_shared<std::vector<T>>(2);
				(*ptr)[0] = small; 
				(*ptr)[1] = val; 
			} else {
				(*ptr).push_back(val);
			}
		}
		sizes++;
	}

	void pop_back() {
		copy_optimization();
		assert(sizes > 0);
		if (sizes == 2) {
			small = (*ptr)[0];
		} else {
				(*ptr).pop_back();
			}
		sizes--;
	}
	T back() const{
		assert(sizes > 0);
		if (sizes <= 1) {
			return small;
		} else {
			return (*ptr).back();
		}
	}
	bool empty() const{
		return (sizes == 0);
	}

 // it isn't from standarts of c++ containers, but i don't want to write iterators
	void reverse() {
		copy_optimization();
		if (sizes > 1) {
			std::reverse((*ptr).begin(), (*ptr).end());
		}
}
};
#endif
// int main() {
// 	int foo = 5;
// 	auto ptr = std::make_shared<int>(foo);
// 	auto nptr = *ptr;
// 	int&& t = 5;
// 	my_vector<int> v;
// 	// auto a =  std::make_shared<std::vector<int>>(4);
// 	// std::cout << (*a).size();
// 	for (int i = 0; i < 10; ++i) {
// 		v.push_back(i);
// 	}
// 	for (int i = 0; i < 10; ++i) {
// 		std::cout << v[i];
// 	}
// 	v[0] = 10;
// 	std::cout << v[0];
// 	v.resize(10);
// 	v[1] = 10;
// 	auto a = v;
// 	a.reverse();

// 	for (int i = 0; i < 10; ++i) {
// 		std::cout << a[i];
// 	}
// 	std::cout << a.size();
// }
