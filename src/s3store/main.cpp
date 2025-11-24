#include <iostream>

#include "in_memory_store.h"

int main() {
    std::cout << "Hello from s3store\n";

    minis3::InMemoryObjectStore store;

    store.put_object("photos", "cat.jpg", "meow");
    store.put_object("photos", "dog.jpg", "woof");

    if (const auto obj = store.get_object("photos", "cat.jpg")) {
        std::cout << "cat.jpg: " << *obj << "\n";
    }

    const auto objects = store.list_objects("photos");
    for (const auto& [bucket, key, size] : objects) {
        std::cout << bucket << "/" << key
            << " (" << size << " bytes)\n";
    }

    return 0;
}
