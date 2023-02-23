#include <thread>
#include "nameserver.h"
#include "dataserver.h"

const int REPLICATE = 3;

int main() {
    NameServer ns(REPLICATE);

    DataServer ds1("node1");
    DataServer ds2("node2");
    DataServer ds3("node3");

    ns.add(&ds1);
    ns.add(&ds2);
    ns.add(&ds3);

    std::thread th1(std::ref(ds1));
    std::thread th2(std::ref(ds2));
    std::thread th3(std::ref(ds3));

    th1.detach();
    th2.detach();
    th3.detach();

    ns();
    return 0;
}
