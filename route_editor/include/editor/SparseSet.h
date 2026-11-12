#ifndef EDITOR_SPARSE_SET_H
#define EDITOR_SPARSE_SET_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

class SparseSet
{
public:
    explicit SparseSet(std::size_t max_size);

    void add(std::size_t id);
    void remove(std::size_t id);

    bool contains(std::size_t id) const;

    void for_each(std::function<void (std::size_t id)> func);

private:
    std::vector<std::size_t> ids;
    std::vector<std::int64_t> id_to_idx;
    std::size_t size;
};

#endif // EDITOR_SPARSE_SET_H
