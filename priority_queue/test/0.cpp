#include <cstdlib>
#include <ctime>
#include <iostream>

#include "../include/priority_queue.hpp"

void Test() {
    std::cout << "My Test ..." << std::endl;
    sjtu::priority_queue<int> pq;
    pq.push(1);
    
    pq.push(2);
    pq.push(0);
    pq.push(3);
    std::cout << pq.top() << std::endl;std::cerr << "check" << std::endl;
    pq.pop();
    std::cout << pq.top() << std::endl;
    pq.pop();
    std::cout << pq.top() << std::endl;
}
int main(int argc, char* const argv[]) {
    Test();
    return 0;
}