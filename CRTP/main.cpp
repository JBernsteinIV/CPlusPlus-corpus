//Driver program to test CRTP implementation.

#include <iostream>
#include <cctype>
#include <string>
#include "CRTP.cpp"

//Helper function for testing.
void menu() {
	std::cout << "1: Test <\n";
	std::cout << "2: Test <=\n";
	std::cout << "3: Test ==\n";
	std::cout << "4: Test >\n";
	std::cout << "5: Test >=\n";
	std::cout << "6: Test !=\n";
	std::cout << "Please select option to test: ";
}

int main() {
	int left, right;

	std::cout << "CRTP Test\nPlease enter two numbers:\n";
	std::cin >> left >> right;

	Derived L = Derived(left);
	Derived R = Derived(right);

	char cont = 'Y';

	int select;

	while (std::toupper(cont) == 'Y') {
		menu();
		
		std::cin >> select;

		switch (select) {
		case 1: L < R ? std::cout << "True\n" : std::cout << "False\n";
			break;
		case 2: L <= R ? std::cout << "True\n" : std::cout << "False\n";
			break;
		case 3: L == R ? std::cout << "True\n" : std::cout << "False\n";
			break;
		case 4: L > R ? std::cout << "True\n" : std::cout << "False\n";
			break;
		case 5: L >= R ? std::cout << "True\n" : std::cout << "False\n";
			break;
		case 6: L != R ? std::cout << "True\n" : std::cout << "False\n";
			break;
		default: std::cout << "Invalid option.\n";
			break;
		}
		std::cout << "Would you like to continue testing? (Y/N)";
		std::cin >> cont;
	}
	
	std::cin.get();
	return 0;
}
