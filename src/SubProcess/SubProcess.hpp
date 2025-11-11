#ifndef TABXX_SUBPROCESS_SUBPROCESS_HPP_
#define TABXX_SUBPROCESS_SUBPROCESS_HPP_

#include <string>
#include <iostream>

namespace tabxx {
using std::string;

class SubProcess {
public:
    SubProcess(const string& flow):
        flow_(flow) {
    }

    void run() {
        std::cout << "SubProcess mode is not supported yet." << std::endl;
    }

private:
    string flow_;

}; // class SubProcess

} // namespace tabxx

#endif // TABXX_SUBPROCESS_SUBPROCESS_HPP_