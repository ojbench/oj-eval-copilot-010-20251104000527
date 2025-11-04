#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include "exceptions.hpp"
#include "algorithm.hpp"

#include <climits>
#include <cstddef>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a list.
 */
template<typename T>
class list {
protected:
    class node {
    public:
        node *prev, *next;
        T *val; // nullptr for sentinel
        // sentinel constructor
        node(): prev(this), next(this), val(nullptr) {}
        // data node
        explicit node(const T &value): prev(nullptr), next(nullptr), val(new T(value)) {}
        ~node(){ if (val) { delete val; val = nullptr; } }
    };

protected:
    node *s; // sentinel
    size_t n;

    node *insert(node *pos, node *cur) {
        // link cur before pos
        cur->next = pos;
        cur->prev = pos->prev;
        pos->prev->next = cur;
        pos->prev = cur;
        return cur;
    }
    node *erase(node *pos) {
        // unlink pos (do not delete)
        pos->prev->next = pos->next;
        pos->next->prev = pos->prev;
        return pos;
    }

public:
    class const_iterator;
    class iterator {
    private:
        friend class list;
        friend class const_iterator;
        list *owner;
        node *p;
        iterator(list *o, node *ptr): owner(o), p(ptr) {}
    public:
        iterator(): owner(nullptr), p(nullptr) {}
        iterator operator++(int) {
            if (owner == nullptr || p == nullptr || p == owner->s) throw invalid_iterator();
            iterator tmp = *this;
            p = p->next;
            return tmp;
        }
        iterator & operator++() {
            if (owner == nullptr || p == nullptr || p == owner->s) throw invalid_iterator();
            p = p->next;
            return *this;
        }
        iterator operator--(int) {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->s) { // --end() valid only when not empty
                if (owner->n == 0) throw invalid_iterator();
            }
            iterator tmp = *this;
            if (p == owner->s) p = owner->s->prev; else p = p->prev;
            return tmp;
        }
        iterator & operator--() {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->s) {
                if (owner->n == 0) throw invalid_iterator();
                p = owner->s->prev;
            } else {
                p = p->prev;
            }
            return *this;
        }
        T & operator *() const {
            if (owner == nullptr || p == nullptr || p == owner->s || p->val == nullptr) throw invalid_iterator();
            return *(p->val);
        }
        T * operator ->() const {
            if (owner == nullptr || p == nullptr || p == owner->s || p->val == nullptr) throw invalid_iterator();
            return p->val;
        }
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }
        bool operator==(const const_iterator &rhs) const;
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const const_iterator &rhs) const;
    };
    class const_iterator {
    private:
        friend class list;
        list const *owner;
        node *p;
        const_iterator(const list *o, node *ptr): owner(o), p(ptr) {}
    public:
        const_iterator(): owner(nullptr), p(nullptr) {}
        const_iterator(const iterator &it): owner(it.owner), p(it.p) {}
        const_iterator operator++(int) {
            if (owner == nullptr || p == nullptr || p == owner->s) throw invalid_iterator();
            const_iterator tmp = *this;
            p = p->next;
            return tmp;
        }
        const_iterator & operator++() {
            if (owner == nullptr || p == nullptr || p == owner->s) throw invalid_iterator();
            p = p->next;
            return *this;
        }
        const_iterator operator--(int) {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->s) {
                if (owner->n == 0) throw invalid_iterator();
            }
            const_iterator tmp = *this;
            if (p == owner->s) p = owner->s->prev; else p = p->prev;
            return tmp;
        }
        const_iterator & operator--() {
            if (owner == nullptr || p == nullptr) throw invalid_iterator();
            if (p == owner->s) {
                if (owner->n == 0) throw invalid_iterator();
                p = owner->s->prev;
            } else {
                p = p->prev;
            }
            return *this;
        }
        const T & operator *() const {
            if (owner == nullptr || p == nullptr || p == owner->s || p->val == nullptr) throw invalid_iterator();
            return *(p->val);
        }
        const T * operator ->() const {
            if (owner == nullptr || p == nullptr || p == owner->s || p->val == nullptr) throw invalid_iterator();
            return p->val;
        }
        bool operator==(const const_iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }
        bool operator==(const iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }
        bool operator!=(const const_iterator &rhs) const { return !(*this == rhs); }
        bool operator!=(const iterator &rhs) const { return !(*this == rhs); }
    };

    list(): s(new node()), n(0) {}
    list(const list &other): s(new node()), n(0) {
        for (node *x = other.s->next; x != other.s; x = x->next) push_back(*(x->val));
    }
    virtual ~list() {
        clear();
        delete s; s = nullptr;
    }
    list &operator=(const list &other) {
        if (this == &other) return *this;
        clear();
        for (node *x = other.s->next; x != other.s; x = x->next) push_back(*(x->val));
        return *this;
    }
    const T & front() const {
        if (n == 0) throw container_is_empty();
        return *(s->next->val);
    }
    const T & back() const {
        if (n == 0) throw container_is_empty();
        return *(s->prev->val);
    }
    iterator begin() { return iterator(this, s->next); }
    const_iterator cbegin() const { return const_iterator(this, s->next); }
    iterator end() { return iterator(this, s); }
    const_iterator cend() const { return const_iterator(this, s); }
    virtual bool empty() const { return n == 0; }
    virtual size_t size() const { return n; }

    virtual void clear() {
        if (n == 0) { s->next = s->prev = s; return; }
        node *cur = s->next;
        while (cur != s) {
            node *nx = cur->next;
            delete cur;
            cur = nx;
        }
        s->next = s->prev = s;
        n = 0;
    }
    virtual iterator insert(iterator pos, const T &value) {
        if (pos.owner != this || pos.p == nullptr) throw invalid_iterator();
        node *cur = new node(value);
        insert(pos.p, cur);
        ++n;
        return iterator(this, cur);
    }
    virtual iterator erase(iterator pos) {
        if (n == 0) throw container_is_empty();
        if (pos.owner != this || pos.p == nullptr || pos.p == s) throw invalid_iterator();
        node *nextp = pos.p->next;
        node *removed = erase(pos.p);
        delete removed;
        --n;
        return iterator(this, nextp);
    }
    void push_back(const T &value) { insert(iterator(this, s), value); }
    void push_front(const T &value) { insert(iterator(this, s->next), value); }
    void pop_back() {
        if (n == 0) throw container_is_empty();
        erase(iterator(this, s->prev));
    }
    void pop_front() {
        if (n == 0) throw container_is_empty();
        erase(iterator(this, s->next));
    }
    void sort() {
        if (n <= 1) return;
        // stable sort nodes by value using mergesort on array of pointers
        size_t m = n;
        node **arr = new node*[m];
        size_t idx = 0; for (node *x = s->next; x != s; x = x->next) arr[idx++] = x;
        node **tmp = new node*[m];
        // bottom-up mergesort
        for (size_t width = 1; width < m; width <<= 1) {
            for (size_t i = 0; i < m; i += (width << 1)) {
                size_t l = i, mid = (i + width < m ? i + width : m), r = (i + (width<<1) < m ? i + (width<<1) : m);
                size_t p = l, q = mid, t = l;
                while (p < mid && q < r) {
                    // stable: choose left when equal
                    if (!(*((arr[q])->val) < *((arr[p])->val))) {
                        tmp[t++] = arr[p++];
                    } else {
                        tmp[t++] = arr[q++];
                    }
                }
                while (p < mid) tmp[t++] = arr[p++];
                while (q < r) tmp[t++] = arr[q++];
                for (size_t k = l; k < r; ++k) arr[k] = tmp[k];
            }
        }
        // relink according to arr
        s->next = arr[0]; arr[0]->prev = s;
        for (size_t i = 1; i < m; ++i) { arr[i-1]->next = arr[i]; arr[i]->prev = arr[i-1]; }
        arr[m-1]->next = s; s->prev = arr[m-1];
        delete [] tmp; delete [] arr;
    }
    void merge(list &other) {
        if (this == &other || other.n == 0) return;
        if (n == 0) {
            // move all from other
            s->next = other.s->next; s->prev = other.s->prev;
            s->next->prev = s; s->prev->next = s;
            n = other.n;
            other.s->next = other.s->prev = other.s; other.n = 0;
            return;
        }
        node *a = s->next; node *b = other.s->next;
        while (b != other.s) {
            // find position in this where b should be inserted before 'a'
            while (a != s && !(*b->val < *a->val)) a = a->next;
            if (a == s) {
                // splice remaining [b .. other.s->prev] at end
                node *b_end = other.s->prev;
                // detach [b, b_end] from other
                b->prev->next = b_end->next; // which is other.s
                b_end->next->prev = b->prev; // set other.s->prev correctly
                other.s->next = other.s; other.s->prev = other.s;
                // insert before s (at end)
                s->prev->next = b; b->prev = s->prev; b_end->next = s; s->prev = b_end;
                n += other.n; other.n = 0;
                return;
            } else {
                // take a block from other starting at b with values < *a
                node *start = b; size_t cnt = 0;
                while (b != other.s && (*b->val < *a->val)) { b = b->next; ++cnt; }
                node *endblk = start; for (size_t i = 1; i < cnt; ++i) endblk = endblk->next; // or simply endblk = b->prev
                endblk = (b == other.s ? other.s->prev : b->prev);
                // detach [start, endblk] from other
                start->prev->next = endblk->next;
                endblk->next->prev = start->prev;
                // insert before a in this
                a->prev->next = start;
                start->prev = a->prev;
                endblk->next = a;
                a->prev = endblk;
                n += cnt; other.n -= cnt;
            }
        }
    }
    void reverse() {
        // swap next and prev for all nodes including sentinel
        node *cur = s;
        do { node *t = cur->next; cur->next = cur->prev; cur->prev = t; cur = t; } while (cur != s);
    }
    void unique() {
        if (n <= 1) return;
        node *cur = s->next;
        while (cur != s) {
            node *nx = cur->next;
            while (nx != s && (*(cur->val) == *(nx->val))) {
                node *to_del = nx; nx = nx->next; erase(to_del); delete to_del; --n;
            }
            cur = nx;
        }
    }
};

// cross definitions depending on const_iterator

template<typename T>
bool list<T>::iterator::operator==(const typename list<T>::const_iterator &rhs) const { return owner == rhs.owner && p == rhs.p; }

template<typename T>
bool list<T>::iterator::operator!=(const typename list<T>::const_iterator &rhs) const { return !(*this == rhs); }

}

#endif //SJTU_LIST_HPP
