#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <json/reader.h>
#include <memory>
#include <random>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <json/json.h>
#include <json/value.h>

namespace SG {
namespace Core {

template <typename, typename = void>
constexpr bool ValueCheck = false;

// clang-format off
template <typename T>
constexpr bool ValueCheck<T, std::void_t<decltype(
    std::declval<T>() < (std::declval<T>()),
    std::declval<T>() == (std::declval<T>()),
    (void)std::declval<T*>()->toJson(std::declval<const T&>()),
    (void)std::declval<T*>()->fromJson(std::declval<const Json::Value&>())
)>> = true;
// clang-format on

/*
    FST -> SkipList

    SkipList should support `Insert`, `Search`,
    We create the Text-DocEntity and move it to the SkipList

    head0
    |
    head -> Data1 -------------> Data3 -------------------> Data7
    head -> Data1 -> Data2 -> Data3 -> Data4 -> Data5 -> Data7

*/
template <typename TV, typename std::enable_if_t<ValueCheck<TV>, bool> = true>
class SkipList {
    struct Node;

public:
    SkipList()
        : m_maxLevel{1}
        , m_levels{0}
        , m_length{0}
        , m_gen{std::random_device{}()}
        , m_distr{} {

        head = new Node{nullptr, nullptr, nullptr, nullptr, 0, TV{}};
        createNewLevel(1);
        bottom = head->down;
    }

    /*
        从 Json 构建 SkipList
    */
    SkipList(const Json::Value &src)
        : m_maxLevel{src["levels"].asUInt64()}
        , m_levels{0}
        , m_length{src["length"].asUInt64()}
        , m_gen{std::random_device{}()}
        , m_distr{} {

        head = new Node{nullptr, nullptr, nullptr, nullptr, 0, TV{}};
        createNewLevel(1);
        bottom = head->down;
        createNewLevel(m_maxLevel);

        const Json::Value &data = src["data"];
        for (Json::Value::ArrayIndex i = 0; i != data.size(); ++i) {
            TV          tmp   = TV::fromJson(data[i]);
            std::size_t level = data[i]["level"].asUInt64();
            insertNodes(tmp, level);
        }
    }

    ~SkipList() noexcept {
        auto p = bottom;

        while (p) {
            auto tmp = p->right;
            while (p) {
                auto t = p->up;
                delete p;
                p = t;
            }
            p = tmp;
        }
    }

    /*
    返回小于 value 的最大的 Node
*/
    Node *search(const TV &value) const {
        auto p = head;
        while (p->down) {
            p = p->down;
            while (p->right && p->right->value < value) {
                p = p->right;
            }
        }
        return p;
    }

    Node *front() { return bottom->right; }

    /*
Firstly:

    head0     =>    head0           =>      head0
                      |                      |
                    head -> null

Typically:

    head0
      |
    head -> Data1 ---------> (Data3) ----xxxxx------------> (Data7) -> null
    head -> Data1 -> Data2 -> Data3 -> ([Data4]) -> Data5 -> Data7  -> null
                                <<<<<<<<<<
*/

    void insert(const TV &value) {
        // 小于 value 的最大节点
        auto nodeLevel = getRandomLevel();
        createNewLevel(nodeLevel);

        insertNodes(value, nodeLevel);

        ++m_length;
        expandLevel();
    }

    template <typename... Args>
    void emplace_insert(Args &&...args) {
        insert(TV{std::forward<Args>(args)...});
    }
    /*
    序列化为 Json
    */
    Json::Value dump() const {
        Json::Value sklRt;
        Json::Value nodes;

        auto p = bottom;
        p      = p->right;

        std::size_t cnt = 0;
        while (p) {
            Json::Value node = TV::toJson(p->value);
            node["rank"]     = ++cnt;
            node["level"]    = p->level;
            nodes.append(node);
            p = p->right;
        }
        sklRt["data"]   = nodes;
        sklRt["levels"] = m_maxLevel;
        sklRt["length"] = m_length;

        return sklRt;
    }

    void print() const {
        for (int i = 0; i < m_maxLevel; ++i) {
            std::cout << "   ****   ";
        }
        std::cout << std::endl;
        Node *p = bottom;

        while (p->right) {
            p      = p->right;
            auto r = p;
            while (r) {
                std::cout << r->value << ' ';
                r = r->up;
            }
            std::cout << std::endl;
        }
        std::cout << "==================" << std::endl;
    }

    static std::vector<TV> combine(std::vector<SkipList<TV> *> &v) {
        std::vector<TV> ret;
        if (v.empty())
            return ret;
        std::size_t okCnt       = 0;
        Node       *currentNode = v[0]->front();

        auto i = v.begin();
        while (true) {
            if (currentNode == nullptr)
                break;
            Node *prevNode       = (*i)->search(currentNode->value);
            Node *bigOrEqualNode = prevNode->right;
            if (bigOrEqualNode == nullptr)
                break;
            if (bigOrEqualNode->value == currentNode->value) {
                okCnt++;
                if (okCnt == v.size()) {
                    okCnt = 0;
                    ret.emplace_back(currentNode->value);
                    currentNode = bigOrEqualNode->right;
                }
            } else {
                okCnt       = 0;
                currentNode = bigOrEqualNode;
            }

            i++;
            if (i == v.end())
                i = v.begin();
        }
        return ret;
    }

private:
    void createNewLevel(std::size_t level) {
        while (m_levels < level) {
            Node *tmp = new Node{nullptr, nullptr, head, head->down, 0, TV{}};
            if (head->down)
                head->down->up = tmp;
            head->down = tmp;
            ++m_levels;
        }
    }
    void insertNodes(const TV &value, std::size_t level) {
        auto  p        = search(value);
        Node *posIndex = nullptr;

        for (int i = 0; i < level; ++i) {
            // 中间层时
            if (posIndex != nullptr) {
                while (!p->up)
                    p = p->left;
                p = p->up;
            }
            posIndex = insertNode(p, posIndex, value, level);
            p        = posIndex;
        }
    }
    Node *insertNode(Node *posLeft, Node *posDown, const TV &value, std::size_t level) {
        Node *tmp =
            new Node{posLeft, posLeft->right, nullptr, posDown, level, value};
        if (posLeft->right)
            posLeft->right->left = tmp;
        posLeft->right = tmp;
        if (posDown)
            posDown->up = tmp;

        return tmp;
    }
    void expandLevel() {
        if (static_cast<std::size_t>(1.5 * m_maxLevel + 0.5) < m_length)
            m_maxLevel = static_cast<std::size_t>(1.5 * m_maxLevel + 0.5);
    }

    std::size_t getRandomLevel() {
        std::size_t ret = 1;
        while (m_distr(m_gen) < THRED && ret < m_maxLevel) {
            ret++;
        }
        return ret;
    }

private:
    static constexpr double THRED = 0.5;

    struct Node {
        Node       *left, *right, *up, *down;
        std::size_t level;
        TV          value;
    };
    std::size_t                      m_maxLevel;
    Node                            *head, *bottom;
    std::size_t                      m_levels;
    std::size_t                      m_length;
    std::mt19937                     m_gen;
    std::uniform_real_distribution<> m_distr;
};

} // namespace Core
} // namespace SG