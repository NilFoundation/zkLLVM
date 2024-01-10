#include <stdlib.h>

int global = 5;
constexpr int num_elems = 3;

struct __attribute__((packed)) chain {
    int val;
    chain *next;
};

int sum(chain *elem) {
    int sum = 0;
    for (int i = 0; i < num_elems; ++i) {
        sum += elem->val;
        elem = elem->next;
    }
    return sum;
}

void free_list(chain *list) {
    for (int i = 0; i < num_elems; ++i) {
        chain *cur = list;
        list = list->next;
        free(cur);
    }
}

[[circuit]] int list_demo(__zkllvm_field_pallas_base unused) {
    chain *list = (chain *)malloc(sizeof(chain));
    chain *iter = list;
    for (int i = 0; i < num_elems; ++i) {
        iter->val = i;
        iter->next = (chain *)malloc(sizeof(chain));
        iter = iter->next;
    }
    iter->val = 0;
    iter->next = nullptr;

    int res = sum(list);
    free(list);
    return res + global;
}
