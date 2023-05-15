#pragma once
namespace SG {

namespace Core {

using SSIZE_T = unsigned long long;

/*


*/
template <typename TK, typename TV>
class SkipList {
public:
    SkipList();
    ~SkipList();
    void search(SSIZE_T docId);
};
} // namespace Core

} // namespace SG