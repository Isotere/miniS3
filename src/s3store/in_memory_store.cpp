#include "in_memory_store.h"

namespace minis3 {
bool InMemoryObjectStore::create_bucket(const std::string& bucket) {
    // Пытается создать пару ключ-пустая мапа. Если ключ уже есть - ничего не будет создано
    buckets_.try_emplace(bucket);

    return true;
}

bool InMemoryObjectStore::delete_bucket(const std::string& bucket) {
    return buckets_.erase(bucket) > 0;
}

std::vector<std::string> InMemoryObjectStore::list_buckets() const {
    std::vector<std::string> buckets;
    buckets.reserve(buckets_.size());

    for (const auto& [k, _] : buckets_) {
        buckets.push_back(k);
    }

    // RVO, компилятор сам оптимизирует передачу локальной переменной того же типа, что возвращаемое значение
    // и копирования не будет
    // либо сделает (N)RVO и вообще ничего не будет копировать/перемещать;
    // либо, если по какой-то причине не сможет, сам вставит move.
    return buckets;
}

bool InMemoryObjectStore::put_object(const std::string& bucket, const std::string& key, const std::string& data) {
    // Размещаем новый объект, если ключа не было и перезаписываем старый, если ключ уже был
    buckets_[bucket].insert_or_assign(key, data);

    // В случае ниже объект будет размещен только, если ключа не существовало
    // buckets_[bucket].emplace(key, data);

    return true;
}

std::optional<std::string> InMemoryObjectStore::get_object(const std::string& bucket, const std::string& key) const {
    const auto it = buckets_.find(bucket);
    if (it == buckets_.end()) {
        return std::nullopt;
    }

    const auto result = it->second.find(key);
    if (result == it->second.end()) {
        return std::nullopt;
    }

    return result->second;
}

bool InMemoryObjectStore::delete_object(const std::string& bucket, const std::string& key) {
    return buckets_[bucket].erase(key) > 0;
}

std::vector<ObjectMetadata> InMemoryObjectStore::list_objects(const std::string& bucket) const {
    const auto it = buckets_.find(bucket);
    if (it == buckets_.end()) {
        return {};
    }

    std::vector<ObjectMetadata> objects;
    objects.reserve(it->second.size());
    for (const auto& [k, v] : it->second) {
        objects.push_back(ObjectMetadata{
            .bucket = bucket,
            .key = k,
            .size = v.length()
        });
    }

    return objects;
}

}