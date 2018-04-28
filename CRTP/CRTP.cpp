/* Implement non-virtual base class with
* Curiously Recurring Template Pattern (CRTP).
* Define Comparable template with implemented Derived class operator<.
* Implement operators ==, !=, >, <=, and >= using operator<.
*/

template<typename T>
class Comparable {
	friend bool operator>(T const& left, T const& right) {
		return right < left;
	}
	friend bool operator==(T const& left, T const& right) {
		return !(left < right) && !(right < left);
	}
	friend bool operator!=(T const& left, T const& right) {
		return left < right || right < left;
	}
	friend bool operator<=(T const& left, T const& right) {
		return !(right < left);
	}
	friend bool operator>=(T const& left, T const& right) {
		return !(left < right);
	}
};


class Derived : Comparable<Derived> {
	int value;
	friend bool operator< (Derived const& left, Derived const& right) {
		return left.value < right.value;
	}
public:
	Derived() = default;
	Derived(int val) {
		this->value = val;
	}
};
