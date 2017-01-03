#include "Control.h"

int main() {
    auto& driver = AccountBalancer::Control::getControl();
    driver.printWelcome();
    driver.control_main();
}
