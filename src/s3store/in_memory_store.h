#pragma once

#include <unordered_map>
#include <vector>

#include "minis3/object_store.h"

namespace minis3 {
class InMemoryObjectStore : public ObjectStore {
public:
    bool create_bucket(const std::string& bucket) override;
    bool delete_bucket(const std::string& bucket) override;
    [[nodiscard]] std::vector<std::string> list_buckets() const override;

    bool put_object(const std::string& bucket,
                    const std::string& key,
                    const std::string& data) override;

    [[nodiscard]] std::optional<std::string> get_object(const std::string& bucket,
                                                        const std::string& key) const override;

    bool delete_object(const std::string& bucket, const std::string& key) override;

    [[nodiscard]] std::vector<ObjectMetadata> list_objects(const std::string& bucket) const override;

private:
    // bucket -> (key -> data)
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> buckets_;
};
}