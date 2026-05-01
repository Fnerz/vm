#include <iostream>
#include <./pybind11/pybind11.h>
#include "./vm/vm.hpp"
#include "./vm/lexer.hpp"

namespace py = pybind11;


PYBIND11_MODULE(VM, handle) {
    handle.doc() = "Vm doc string";
    py::class_<VirtualMachine>(handle, "VirtualMachine")
    .def(py::init<>())
    .def("loadCode", VirtualMachine::loadCode);
}