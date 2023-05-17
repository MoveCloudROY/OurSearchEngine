#pragma once
#include <cstdint>
#include <json/value.h>
#include <memory>
#include <type_traits>
#include <utility>
#include <vector>
#include <random>
#include <functional>
#include <json/json.h>

#include <iostream>


namespace SG {
namespace Core {

template <typename T>
class IsEqualityComparable {
private:
    template <typename K>
    static constexpr auto Test(K*) -> decltype(std::declval<K>() == std::declval<K>());

    template <typename K>
    static constexpr int Test(...);

public:
    static constexpr bool Value = std::is_same_v<bool, decltype(Test<T>(nullptr))>;
};


/*
    FST -> SkipList

    SkipList should support `Insert`, `Search`,
    We create the Text-DocEntity and move it to the SkipList

    head0
    |
    head -> Data1 -------------> Data3 -------------------> Data7
    head -> Data1 -> Data2 -> Data3 -> Data4 -> Data5 -> Data7

*/
template <typename TK, typename TV, typename std::enable_if_t<IsEqualityComparable<TV>::Value, bool> = true>
class SkipList {
    struct Node;

public:
    SkipList()
        : m_maxLevel{1}, m_levels{0}, m_length{0}, m_gen{std::random_device{}()}, m_distr{} {

        head = new Node{nullptr, nullptr, nullptr, nullptr, 0, TV{}};
        createNewLevel(1);
        bottom = head->down;
    }
    ~SkipList() {
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
    Node* search(const TV& value) {
        auto p = head;
        while (p->down) {
            p = p->down;
            while (p->right && p->right->value < value) {
                p = p->right;
            }
        }
        return p;
    }

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

    void insert(const TV& value) {
        // 小于 value 的最大节点
        auto  p        = search(value);
        Node* posIndex = nullptr;

        auto nodeLevel = getRandomLevel();
        createNewLevel(nodeLevel);
        for (int i = 0; i < nodeLevel; ++i) {
            // 最底层时
            if (posIndex != nullptr) {
                while (!p->up)
                    p = p->left;
                p = p->up;
            }
            posIndex = insertNode(p, posIndex, value, nodeLevel);
            p        = posIndex;
        }
        ++m_length;
        expandLevel();
    }

    template <typename... Args>
    void emplace_insert(Args&&... args) {
        insert(TV{std::forward<Args>(args)...});
    }

    Json::Value dump(std::function<Json::Value(const TV& v)> func) {
        Json::Value sklRt;
        Json::Value nodes;

        auto p = bottom;
        p      = p->right;

        uint32_t cnt = 0;
        while (p) {
            Json::Value node = func(p->value);
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

    void read() {}

    void print() const {
        for (int i = 0; i < m_maxLevel; ++i) {
            std::cout << "   ****   ";
        }
        std::cout << std::endl;
        Node* p = bottom;

        while (p->right) {
            p      = p->right;
            auto r = p;
            while (r) {
                std::cout << r->value << ' ';
                r = r->up;
            }
            std::cout << std::endl;
        }
    }

private:
    void createNewLevel(uint32_t level) {
        while (m_levels < level) {
            Node* tmp = new Node{nullptr, nullptr, head, head->down, 0, TV{}};
            if (head->down)
                head->down->up = tmp;
            head->down = tmp;
            ++m_levels;
        }
    }

    Node* insertNode(Node* posLeft, Node* posDown, const TV& value, uint32_t level) {
        Node* tmp = new Node{posLeft, posLeft->right, nullptr, posDown, level, value};
        if (posLeft->right)
            posLeft->right->left = tmp;
        posLeft->right = tmp;
        if (posDown)
            posDown->up = tmp;

        return tmp;
    }
    void expandLevel() {
        std::cout << m_maxLevel << ' ' << m_length << '\n';
        if ((1 << m_maxLevel) < m_length)
            m_maxLevel = static_cast<uint32_t>(1.6 * m_maxLevel + 0.5);
    }

    uint32_t getRandomLevel() {
        uint32_t ret = 1;
        while (m_distr(m_gen) < THRED && ret < m_maxLevel) {
            ret++;
        }
        return ret;
    }

private:
    static constexpr double THRED = 0.5;

    struct Node {
        Node *   left, *right, *up, *down;
        uint32_t level;
        TV       value;
    };
    uint32_t                         m_maxLevel;
    Node *                           head, *bottom;
    uint32_t                         m_levels;
    uint32_t                         m_length;
    std::mt19937                     m_gen;
    std::uniform_real_distribution<> m_distr;
};

} // namespace Core
} // namespace SG