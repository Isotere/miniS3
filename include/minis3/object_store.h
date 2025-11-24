#pragma once

#include <optional>
#include <string>
#include <vector>

namespace minis3 {

struct ObjectID {
    std::string bucket;
    std::string key;
};

struct ObjectMetadata {
    std::string bucket;
    std::string key;
    std::size_t size{};
};

class ObjectStore {
public:
    virtual ~ObjectStore() = default;

    virtual bool create_bucket(const std::string& bucket) = 0;
    virtual bool delete_bucket(const std::string& bucket) = 0;
    [[nodiscard]] virtual std::vector<std::string> list_buckets() const = 0;

    virtual bool put_object(const std::string& bucket,
                            const std::string& key,
                            const std::string& data) = 0;

    [[nodiscard]] virtual std::optional<std::string> get_object(const std::string& bucket,
                                                                const std::string& key) const = 0;

    virtual bool delete_object(const std::string& bucket,
                               const std::string& key) = 0;

    [[nodiscard]] virtual std::vector<ObjectMetadata> list_objects(const std::string& bucket) const = 0;
};


} // namespace minis3