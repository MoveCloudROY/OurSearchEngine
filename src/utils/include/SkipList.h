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
#include <spdlog/spdlog.h>


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
        : m_height{0}, m_cnt{0}, m_rd{}, m_gen{m_rd()}, m_distr{1, 3} {

        head = new Node{nullptr, nullptr, nullptr, nullptr, 0, nullptr};
        createNewLevel(1);
    }
    ~SkipList() {
        auto p = head;
        while (p->down)
            p = p->down;

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
    Node* search(const TV* value) {
        auto p = head;
        while (p->down) {
            p = p->down;
            while (p->right && *(p->right->value) < *value) {
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

    void insert(const TV* value) {
        // 小于 value 的最大节点
        auto  p        = search(value);
        Node* posIndex = nullptr;

        auto nodeLevel = getLevel();
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

        ++m_cnt;
    }

    template <typename... Args>
    void emplace_insert(Args&&... args) {
        TV* value = new TV{std::forward<Args>(args)...};
        insert(value);
    }

    Json::Value dump(std::function<Json::Value(const TV& v)> func) {
        Json::Value sklRt;
        sklRt["height"] = m_height;
        sklRt["length"] = m_cnt;

        Json::Value nodes;

        auto p = head;
        while (p->down)
            p = p->down;
        p = p->right;

        uint32_t cnt = 0;
        while (p) {
            Json::Value node = func(*(p->value));
            node["rank"]     = ++cnt;
            node["level"]    = p->level;
            nodes.append(node);
            p = p->right;
        }
        sklRt["data"] = nodes;

        return sklRt;
    }

    void read() {}

    void print() {
        for (int i = 0; i < m_height; ++i) {
            std::cout << "   ****   ";
        }
        std::cout << std::endl;
        Node* p = head;
        while (p->down)
            p = p->down;

        while (p->right) {
            p      = p->right;
            auto r = p;
            while (r) {
                std::cout << *(r->value) << ' ';
                r = r->up;
            }
            std::cout << std::endl;
        }

        // while (p->down) {
        //     p      = p->down;
        //     auto q = p;
        //     std::cout << "* ->";
        //     while (q->right) {
        //         q = q->right;
        //         std::cout << *(q->value) << ' ';
        //     }
        //     std::cout << std::endl;
        // }
    }

    // static std::vector<TV> combine(const std::vector<SkipList<TK, TV>*>&) {
    // }

private:
    void createNewLevel(uint32_t level) {
        while (m_height < level) {
            Node* tmp = new Node{nullptr, nullptr, head, head->down, 0, nullptr};
            if (head->down)
                head->down->up = tmp;
            head->down = tmp;
            ++m_height;
        }
    }


    Node* insertNode(Node* posLeft, Node* posDown, const TV* value, uint32_t level) {
        Node* tmp = new Node{posLeft, posLeft->right, nullptr, posDown, level, value};

        if (posLeft->right)
            posLeft->right->left = tmp;
        posLeft->right = tmp;
        if (posDown)
            posDown->up = tmp;

        return tmp;
    }
    uint32_t getLevel() {
        return m_distr(m_gen);
    }

private:
    static constexpr unsigned int THRED = 20;
    struct Node {
        Node *    left, *right, *up, *down;
        uint32_t  level;
        TV const* value;
    };
    Node*                           head;
    uint32_t                        m_height;
    uint32_t                        m_cnt;
    std::random_device              m_rd;
    std::mt19937                    m_gen;
    std::uniform_int_distribution<> m_distr;
};

} // namespace Core
} // namespace SG