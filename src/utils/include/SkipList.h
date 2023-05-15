#pragma once
namespace SG {

namespace Core {

using uint64 = unsigned long long;

template <typename TK, typename TV, uint64>
class SkipList {
public:
    SkipList();
    ~SkipList();
    void search(SSIZE_T docId);
    void insert()
};
} // namespace Core

} // namespace SG