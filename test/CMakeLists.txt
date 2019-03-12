#
# Author: Kevin XU <kevin.xu.1982.02.06@gmail.com>
#

add_executable(test-auto-ptr "test-auto-ptr.cpp")

add_executable(test-ref-counting "test-ref-counting.cpp")

add_executable(test-signaling "test-signaling.cpp")

add_test(NAME "test-auto-ptr" COMMAND test-auto-ptr)

add_test(NAME "test-ref-counting" COMMAND test-ref-counting)

add_test(NAME "test-signaling" COMMAND test-signaling)

